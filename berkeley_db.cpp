#include <mutex>

#include <cryptopp/misc.h>

#include "berkeley_db.h"
#include "util.h"

std::recursive_mutex mutexDb;

BerkeleyEnvironment::BerkeleyEnvironment(const QDir &env_directory) {
  _path = env_directory;
  reset();
}

BerkeleyEnvironment::~BerkeleyEnvironment() {
  close();
  for (auto &db : mapDatabases)
    db.second.get().reset();
}

void BerkeleyEnvironment::reset() {
  dbEnv.reset(new DbEnv(DB_CXX_NO_EXCEPTIONS));
  _fDbEnvInit = false;
}

bool BerkeleyEnvironment::isInitialized() const { return _fDbEnvInit; }

bool BerkeleyEnvironment::isDatabaseLoaded(
    const std::string &dbFilename) const {
  return mapDatabases.count(dbFilename);
}

QDir BerkeleyEnvironment::getDirectory() const { return _path; }

void BerkeleyEnvironment::close() {
  if (!_fDbEnvInit)
    return;

  _fDbEnvInit = false;

  for (auto &db : mapDatabases)
    db.second.get().close();

  FILE *errorFile = nullptr;
  dbEnv->get_errfile(&errorFile);

  int ret = dbEnv->close(0);
  std::string errorMsg = "Cannot close database environment: ";
  if (ret)
    throw std::runtime_error(errorMsg + DbEnv::strerror(ret));

  if (errorFile)
    fclose(errorFile);

  unlockDirectory(_path, ".dbEnvLock");
}

void BerkeleyEnvironment::closeDb(const std::string &filename) {
  auto db = mapDatabases.find(filename);
  if (db != mapDatabases.end())
    db->second.get().close();
}

DbTxn *BerkeleyEnvironment::TxnBegin() {
  DbTxn *pTxn = nullptr;
  int ret = dbEnv->txn_begin(nullptr, &pTxn, 0);
  if (!pTxn || ret != 0)
    return nullptr;
  return pTxn;
}

BerkeleyDatabase::~BerkeleyDatabase() {
  close();
  if (env) {
    env->mapFileUseCount.erase(_filename);
    env->mapDatabases.erase(_filename);
  }
}

void BerkeleyDatabase::reset() {
  _filename.clear();
  env.reset();
  db.reset();
}

void BerkeleyDatabase::close() {
  std::string errorMsg;

  if (env) {
    auto count = env->mapFileUseCount.find(_filename);
    errorMsg = "Database in use cannot be closed: ";
    if (count != env->mapFileUseCount.end() && count->second)
      throw std::runtime_error(errorMsg + _filename);
  }

  if (db) {
    int ret = db->close(0);
    errorMsg = "Cannot close database: ";
    if (ret)
      throw std::runtime_error(errorMsg + DbEnv::strerror(ret));
    db.reset();
  }
}

BerkeleyBatch::~BerkeleyBatch() { close(); }

void BerkeleyBatch::flush() {
  if (_activeTxn)
    return;

  uint32_t min = _fReadOnly ? 1 : 0;
  uint32_t kbyte = (_fReadOnly ? DEFAULT_WALLET_DBLOGSIZE : 0) * 1024;

  if (_env)
    _env->dbEnv->txn_checkpoint(kbyte, min, 0);
}

void BerkeleyBatch::close() {
  if (!_pDb)
    return;
  if (_activeTxn)
    _activeTxn->abort();
  _activeTxn = nullptr;
  _pDb = nullptr;

  flush();

  {
    const std::lock_guard<std::recursive_mutex> lock(mutexDb);
    --_env->mapFileUseCount[_filename];
  }
  _env->cvDbInUse.notify_all();
}

bool BerkeleyBatch::TxnBegin() {
  if (!_pDb || _activeTxn)
    return false;
  DbTxn *pTxn = _env->TxnBegin();
  if (!pTxn)
    return false;
  _activeTxn = pTxn;
  return true;
}

bool BerkeleyBatch::TxnCommit() {
  if (!_pDb || !_activeTxn)
    return false;
  int ret = _activeTxn->commit(0);
  _activeTxn = nullptr;
  return (ret == 0);
}

bool BerkeleyBatch::TxnAbort() {
  if (!_pDb || !_activeTxn)
    return false;
  int ret = _activeTxn->abort();
  _activeTxn = nullptr;
  return (ret == 0);
}

Dbc *BerkeleyBatch::getCursor() {
  if (!_pDb)
    return nullptr;
  Dbc *pCursor = nullptr;
  int ret = _pDb->cursor(nullptr, &pCursor, 0);
  if (ret != 0)
    return nullptr;
  return pCursor;
}

bool BerkeleyBatch::readAtCursor(Dbc *pCursor, QDataStream &keyStream,
                                 QDataStream &valueStream) {
  SafeDbt keyData;
  SafeDbt valueData;
  int ret = pCursor->get(&keyData.dbt, &valueData.dbt, DB_NEXT);
  if (ret != 0 || keyData.dbt.get_data() == nullptr ||
      valueData.dbt.get_data() == nullptr)
    return false;

  keyStream.writeBytes(reinterpret_cast<const char *>(keyData.dbt.get_data()),
                       keyData.dbt.get_size());
  valueStream.writeBytes(
      reinterpret_cast<const char *>(valueData.dbt.get_data()),
      valueData.dbt.get_size());
  return true;
}

SafeDbt::SafeDbt() { dbt.set_flags(DB_DBT_MALLOC); }

SafeDbt::SafeDbt(char *data, int size) {
  dbt.set_data(data);
  dbt.set_size(size);
}

SafeDbt::~SafeDbt() {
  if (dbt.get_data() != nullptr) {
    CryptoPP::memset_z(dbt.get_data(), 0, dbt.get_size());
    if (dbt.get_flags() & DB_DBT_MALLOC)
      free(dbt.get_data());
  }
}
