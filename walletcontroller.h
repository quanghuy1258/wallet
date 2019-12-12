#ifndef WALLETCONTROLLER_H
#define WALLETCONTROLLER_H

#include <QObject>

class WalletController : public QObject {
  Q_OBJECT
public:
  explicit WalletController(QObject *parent = nullptr);

signals:

public slots:
};

#endif // WALLETCONTROLLER_H
