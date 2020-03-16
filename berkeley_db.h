#ifndef BERKELEY_DB_H
#define BERKELEY_DB_H

#include <string>

#define HAVE_CXX_STDHEADERS
#include <db_cxx.h>

class BerkeleyEnvironment {
private:
  bool _fDbEnvInit;
  std::string _path;

public:
  BerkeleyEnvironment(const std::string &env_directory);
  void reset();

  void reloadDbEnv();
};

class BerkeleyDatabase {};

class BerkeleyBatch {};

#endif // BERKELEY_DB_H
