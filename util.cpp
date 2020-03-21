#include <mutex>
#include <set>

#include "util.h"

static std::set<std::string> dirLocks;
static std::mutex mDirLocks;

std::string QString2StdString(const QString &s) { return s.toStdString(); }

QString StdString2QString(const std::string &s) {
  return QString::fromStdString(s);
}

void createDirectories(const QDir &pathDir) {
  std::string errorMsg = "Cannot create directories: ";
  if (!QDir::root().mkpath(pathDir.absolutePath()))
    throw std::runtime_error(errorMsg +
                             QString2StdString(pathDir.absolutePath()));
}

void lockDirectory(const QDir &pathDir, const std::string &lockfileName) {
  std::lock_guard<std::mutex> lock(mDirLocks);
  std::string errorMsg;
  QString pathQString = pathDir.filePath(StdString2QString(lockfileName));
  std::string pathStdString = QString2StdString(pathQString);

  if (dirLocks.count(pathStdString))
    return;
  QFile lockfile(pathQString);
  errorMsg = "The lock file exists --> Cannot lock directory: ";
  if (lockfile.exists())
    throw std::runtime_error(errorMsg + pathStdString);
  errorMsg = "Cannot create the lock file: ";
  if (!lockfile.open(QIODevice::ReadWrite))
    throw std::runtime_error(errorMsg + pathStdString);
  lockfile.close();
  dirLocks.insert(pathStdString);
}

void unlockDirectory(const QDir &pathDir, const std::string &lockfileName) {
  std::lock_guard<std::mutex> lock(mDirLocks);
  std::string errorMsg;
  QString pathQString = pathDir.filePath(StdString2QString(lockfileName));
  std::string pathStdString = QString2StdString(pathQString);

  if (!dirLocks.count(pathStdString))
    return;
  QFile lockfile(pathQString);
  errorMsg = "The lock file is not found: ";
  if (!lockfile.exists())
    throw std::runtime_error(errorMsg + pathStdString);
  errorMsg = "Cannot remove the lock file: ";
  if (!lockfile.remove())
    throw std::runtime_error(errorMsg + pathStdString);
  dirLocks.erase(pathStdString);
}

int64_t getTime() {
  auto now = std::chrono::system_clock::now();
  return now.time_since_epoch().count();
}
