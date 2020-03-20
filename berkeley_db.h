#ifndef BERKELEY_DB_H
#define BERKELEY_DB_H

#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <string>

#include <QDir>

#define HAVE_CXX_STDHEADERS
#include <db_cxx.h>

class BerkeleyEnvironment;
class BerkeleyDatabase;
class BerkeleyBatch;

class BerkeleyEnvironment {
private:
  bool _fDbEnvInit;
  QDir _path;

public:
  std::unique_ptr<DbEnv> dbEnv;
  std::map<std::string, int> mapFileUseCount;
  std::map<std::string, std::reference_wrapper<BerkeleyDatabase>> mapDatabases;
  std::condition_variable cvDbInUse;

  BerkeleyEnvironment(const QDir &env_directory);
  ~BerkeleyEnvironment();
  void reset();

  bool isInitialized() const;
  bool isDatabaseLoaded(const std::string &dbFilename) const;
  QDir getDirectory() const;

  void close();

  void closeDb(const std::string &filename);
};

class BerkeleyDatabase {
private:
  std::string _filename;

public:
  std::shared_ptr<BerkeleyEnvironment> env;
  std::unique_ptr<Db> db;

  ~BerkeleyDatabase();
  void reset();

  void close();
};

class BerkeleyBatch {};

#endif // BERKELEY_DB_H
