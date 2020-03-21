#ifndef UTIL_H
#define UTIL_H

#include <chrono>
#include <string>

#include <QDir>

std::string QString2StdString(const QString &s);
QString StdString2QString(const std::string &s);

void createDirectories(const QDir &pathDir);

void lockDirectory(const QDir &pathDir, const std::string &lockfileName);
void unlockDirectory(const QDir &pathDir, const std::string &lockfileName);

int64_t getTime();

#endif // UTIL_H
