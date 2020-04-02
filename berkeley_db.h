#ifndef BERKELEY_DB_H
#define BERKELEY_DB_H

#include <condition_variable>
#include <functional>
#include <map>
#include <memory>
#include <string>

#include <QDataStream>
#include <QDir>

#define HAVE_CXX_STDHEADERS
#include <db_cxx.h>

static const unsigned int DEFAULT_DB_CACHESIZE = 0x100000;
static const unsigned int DEFAULT_DB_LOGSIZE = 0x10000;
static const unsigned int DEFAULT_DB_LOGMAX = 0x100000;

class BerkeleyEnvironment;
class BerkeleyDatabase;
class BerkeleyBatch;
class SafeDbt;

class SafeDbt {
public:
  Dbt dbt;

  SafeDbt();
  SafeDbt(char *data, int size);
  ~SafeDbt();
};

class BerkeleyEnvironment {
private:
  bool _fDbEnvInit;
  QDir _path;

public:
  std::unique_ptr<DbEnv> dbEnv;
  std::map<std::string, int> mapFileUseCount;
  std::map<std::string, std::reference_wrapper<BerkeleyDatabase>> mapDatabases;
  std::condition_variable_any cvDbInUse;

  BerkeleyEnvironment(const QDir &env_directory);
  ~BerkeleyEnvironment();
  void reset();

  bool verify(const std::string &filename);

  bool isInitialized() const;
  bool isDatabaseLoaded(const std::string &dbFilename) const;
  QDir getDirectory() const;

  void open();
  void close();
  void flush(bool fShutdown);

  void closeDb(const std::string &filename);
  void reloadDbEnv();

  DbTxn *TxnBegin();
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

class BerkeleyBatch {
private:
  BerkeleyEnvironment *_env;
  std::string _filename;
  Db *_pDb;
  DbTxn *_activeTxn;
  bool _fReadOnly;

public:
  BerkeleyBatch(BerkeleyDatabase &database, bool isReadOnly = false,
                bool isCreate = false);
  ~BerkeleyBatch();

  BerkeleyBatch(const BerkeleyBatch &) = delete;
  BerkeleyBatch &operator=(const BerkeleyBatch &) = delete;

  void flush();
  void close();

  bool TxnBegin();
  bool TxnCommit();
  bool TxnAbort();

  Dbc *getCursor();
  bool readAtCursor(Dbc *pCursor, QDataStream &keyStream,
                    QDataStream &valueStream);

  template <typename K, typename T> bool read(const K &key, T &value) {
    if (!_pDb)
      return false;

    QByteArray keyArray;
    QDataStream keyStream(&keyArray, QIODevice::ReadWrite);
    keyStream << key;
    SafeDbt keyData(keyArray.data(), keyArray.size());

    SafeDbt valueData;
    int ret = _pDb->get(_activeTxn, &keyData.dbt, &valueData.dbt, 0);
    if (valueData.dbt.get_data() != nullptr) {
      try {
        QByteArray valueArray(
            reinterpret_cast<const char *>(valueData.dbt.get_data()),
            valueData.dbt.get_size());
        QDataStream valueStream(&valueArray, QIODevice::ReadWrite);
        valueArray >> value;
      } catch (...) {
        return false;
      }
    }

    return (ret == 0);
  }

  template <typename K, typename T>
  bool write(const K &key, const T &value, bool fOverwrite = true) {
    if (!_pDb || _fReadOnly)
      return false;

    QByteArray keyArray;
    QDataStream keyStream(&keyArray, QIODevice::ReadWrite);
    keyStream << key;
    SafeDbt keyData(keyArray.data(), keyArray.size());

    QByteArray valueArray;
    QDataStream valueStream(&valueArray, QIODevice::ReadWrite);
    valueStream << value;
    SafeDbt valueData(valueArray.data(), valueArray.size());

    int ret = _pDb->put(_activeTxn, &keyData.dbt, &valueData.dbt,
                        (fOverwrite ? 0 : DB_NOOVERWRITE));
    return (ret == 00);
  }

  template <typename K> bool erase(const K &key) {
    if (!_pDb || _fReadOnly)
      return false;

    QByteArray keyArray;
    QDataStream keyStream(&keyArray, QIODevice::ReadWrite);
    keyStream << key;
    SafeDbt keyData(keyArray.data(), keyArray.size());

    int ret = _pDb->del(_activeTxn, &keyData.dbt, 0);
    return (ret == 0 || ret == DB_NOTFOUND);
  }

  template <typename K> bool exists(const K &key) {
    if (!_pDb)
      return false;

    QByteArray keyArray;
    QDataStream keyStream(&keyArray, QIODevice::ReadWrite);
    keyStream << key;
    SafeDbt keyData(keyArray.data(), keyArray.size());

    int ret = _pDb->exists(_activeTxn, &keyData.dbt, 0);
    return (ret == 0);
  }
};

#endif // BERKELEY_DB_H
