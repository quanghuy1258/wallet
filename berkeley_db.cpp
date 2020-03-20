#include "berkeley_db.h"
#include "util.h"

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
