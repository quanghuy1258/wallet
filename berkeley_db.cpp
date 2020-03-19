#include "berkeley_db.h"
#include "util.h"

BerkeleyEnvironment::BerkeleyEnvironment(const QDir &env_directory) {
  _path = env_directory;
  reset();
}

BerkeleyEnvironment::~BerkeleyEnvironment() { close(); }

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

void BerkeleyEnvironment::close() {}
