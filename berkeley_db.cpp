#include "berkeley_db.h"

BerkeleyEnvironment::BerkeleyEnvironment(const std::string &env_directory) {
  _path = env_directory;
}
