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

bool BerkeleyEnvironment::verify(const std::string &filename) {
  Db db(dbEnv.get(), 0);
  return db.verify(filename.c_str(), nullptr, nullptr, 0);
}

void BerkeleyEnvironment::open() {
  if (_fDbEnvInit)
    return;
  std::string errorMsg;

  createDirectories(_path);
  lockDirectory(_path, ".dbEnvLock");

  QDir pathLogDir = _path;
  errorMsg = "Cannot create database log directory";
  if (!pathLogDir.mkdir("database") || !pathLogDir.cd("database"))
    throw std::runtime_error(errorMsg +
                             QString2StdString(pathLogDir.absolutePath()));

  unsigned int envFlags = 0;
  envFlags |= DB_CREATE;
  envFlags |= DB_INIT_LOCK;
  envFlags |= DB_INIT_LOG;
  envFlags |= DB_INIT_MPOOL;
  envFlags |= DB_INIT_TXN;
  envFlags |= DB_THREAD;
  envFlags |= DB_RECOVER;
  envFlags |= DB_PRIVATE;

  dbEnv->set_cachesize(0, DEFAULT_DB_CACHESIZE, 1);
  dbEnv->set_lg_dir(QString2StdString(pathLogDir.absolutePath()).c_str());
  dbEnv->set_lg_bsize(DEFAULT_DB_LOGSIZE);
  dbEnv->set_lg_max(DEFAULT_DB_LOGMAX);
  dbEnv->set_errfile(
      fopen(QString2StdString(_path.filePath("db.log")).c_str(), "a+"));
  dbEnv->set_flags(DB_AUTO_COMMIT, 1);

  int ret =
      dbEnv->open(QString2StdString(_path.absolutePath()).c_str(), envFlags, 0);
  errorMsg = "Cannot create database environment: ";
  if (ret)
    throw std::runtime_error(errorMsg + DbEnv::strerror(ret));

  _fDbEnvInit = true;
}

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

void BerkeleyEnvironment::flush(bool fShutdown) {
  if (!_fDbEnvInit)
    return;

  {
    const std::lock_guard<std::recursive_mutex> lock(mutexDb);
    for (auto &db : mapDatabases) {
      auto it = mapFileUseCount.find(db.first);
      if (it == mapFileUseCount.end() || it->second != 0)
        continue;
      closeDb(db.first);
      mapFileUseCount.erase(db.first);
    }
    dbEnv->txn_checkpoint(0, 0, 0);

    if (fShutdown && mapFileUseCount.empty()) {
      int ret;
      std::string errorMsg;
      char **begin, **list;

      errorMsg = "Cannot get log archive list: ";
      if ((ret = dbEnv->log_archive(&list, DB_ARCH_ABS)) != 0)
        throw std::runtime_error(errorMsg + DbEnv::strerror(ret));

      errorMsg = "Cannot remove log: ";
      if (list != NULL) {
        int listlen = 0, i;
        for (begin = list; *begin != NULL; begin++)
          listlen++;
        int minlog = listlen - 3; // Keep the most recent 3 logs
        for (begin = list, i = 0; i < minlog; list++, i++) {
          if ((ret = unlink(*list)) != 0)
            throw std::runtime_error(errorMsg + *list);
        }
        free(begin);
      }

      close();
    }
  }
}

void BerkeleyEnvironment::closeDb(const std::string &filename) {
  auto db = mapDatabases.find(filename);
  if (db != mapDatabases.end())
    db->second.get().close();
}

void BerkeleyEnvironment::reloadDbEnv() {
  std::unique_lock<std::recursive_mutex> lock(mutexDb);
  cvDbInUse.wait(lock, [this]() {
    for (auto &count : mapFileUseCount) {
      if (count.second > 0)
        return false;
    }
    return true;
  });
  std::vector<std::string> filenames;
  for (auto &it : mapDatabases) {
    filenames.push_back(it.first);
  }
  for (auto &it : filenames)
    closeDb(it);
  flush(true);
  reset();
  open();
}

DbTxn *BerkeleyEnvironment::TxnBegin() {
  DbTxn *pTxn = nullptr;
  int ret = dbEnv->txn_begin(nullptr, &pTxn, 0);
  if (!pTxn || ret != 0)
    return nullptr;
  return pTxn;
}

BerkeleyDatabase::BerkeleyDatabase(
    const std::shared_ptr<BerkeleyEnvironment> &dbEnv,
    const std::string &filename) {
  env = dbEnv;
  _filename = filename;
  env->mapDatabases.emplace(_filename, std::ref(*this));
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

std::string BerkeleyDatabase::getFileName() const { return _filename; }

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

void BerkeleyDatabase::backup(const std::string &pathDest) {
  std::string errorMsg = "Cannot backup database: ";
  if (!env || !db)
    throw std::runtime_error(errorMsg + "Null pointer");

  std::unique_lock<std::recursive_mutex> lock(mutexDb);
  env->cvDbInUse.wait(lock, [this]() {
    if ((this->env->mapFileUseCount.count(this->_filename) == 0) ||
        (this->env->mapFileUseCount[this->_filename] == 0))
      return true;
    return false;
  });

  close();
  env->dbEnv->txn_checkpoint(0, 0, 0);
  env->mapFileUseCount.erase(_filename);

  QString fileSrc = env->getDirectory().filePath(StdString2QString(_filename));
  QString fileDest = StdString2QString(pathDest);
  if (QDir(fileDest).exists())
    fileDest = QDir(fileDest).filePath(StdString2QString(_filename));
  if (!QFile::copy(fileSrc, fileDest))
    throw std::runtime_error(errorMsg + "Error when copy from " +
                             QString2StdString(fileSrc) + " to " +
                             QString2StdString(fileDest));
}

BerkeleyBatch::BerkeleyBatch(BerkeleyDatabase &database, bool isReadOnly,
                             bool isCreate) {
  std::string errorMsg;
  _fReadOnly = isReadOnly;
  _env = database.env.get();
  _filename = database.getFileName();

  unsigned int flags = DB_THREAD;
  if (isCreate)
    flags |= DB_CREATE;

  {
    const std::unique_lock<std::recursive_mutex> lock(mutexDb);
    _env->open();

    _pDb = database.db.get();
    if (_pDb == nullptr) {
      int ret;
      std::unique_ptr<Db> pDb_temp(new Db(_env->dbEnv.get(), 0));
      errorMsg = "Cannot open database: ";
      if ((ret = pDb_temp->open(nullptr, _filename.c_str(), nullptr, DB_BTREE,
                                flags, 0)) != 0)
        throw std::runtime_error(errorMsg + DbEnv::strerror(ret));

      _pDb = pDb_temp.release();
      database.db.reset(_pDb);
    }

    ++_env->mapFileUseCount[_filename];
  }
}

BerkeleyBatch::~BerkeleyBatch() { close(); }

void BerkeleyBatch::flush() {
  if (_activeTxn)
    return;

  uint32_t min = _fReadOnly ? 1 : 0;
  uint32_t kbyte = (_fReadOnly ? DEFAULT_DB_LOGSIZE : 0) / 1024;

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
