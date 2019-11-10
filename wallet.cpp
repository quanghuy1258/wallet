#include "wallet.h"

Wallet::Wallet() {}

Wallet::~Wallet() {}

bool Wallet::encryptWallet(const std::string &wallet_passphrase) {
  return true;
}

bool Wallet::isCrypted() { return true; }

bool Wallet::lock() { return true; }

bool Wallet::unlock(const std::string &wallet_passphrase) { return true; }

bool Wallet::isLocked() { return true; }

bool Wallet::changeWalletPassphrase(const std::string &old_wallet_passphrase,
                                    const std::string &new_wallet_passphrase) {
  return true;
}
