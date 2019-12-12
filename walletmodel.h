#ifndef WALLETMODEL_H
#define WALLETMODEL_H

#include <QObject>

class WalletModel : public QObject {
  Q_OBJECT
public:
  explicit WalletModel(QObject *parent = nullptr);

signals:

public slots:
};

#endif // WALLETMODEL_H
