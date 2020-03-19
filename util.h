#ifndef UTIL_H
#define UTIL_H

#include <string>

#include <QDir>

std::string QString2StdString(const QString &s);
QString StdString2QString(const std::string &s);

void tryCreateDirectories(const QDir &pathDir);

void tryLockDirectory(const QDir &pathDir, const std::string &lockfileName);
void unlockDirectory(const QDir &pathDir, const std::string &lockfileName);

#endif // UTIL_H
