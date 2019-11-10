#ifndef WALLET_H
#define WALLET_H

#include <string>

class Wallet {
public:
  Wallet();
  ~Wallet();

  // Encrypt wallet.
  bool encryptWallet(const std::string &wallet_passphrase);

  // Return whether wallet is encrypted.
  bool isCrypted();

  // Lock wallet.
  bool lock();

  // Unlock wallet.
  bool unlock(const std::string &wallet_passphrase);

  // Return whether wallet is locked.
  bool isLocked();

  // Change wallet passphrase.
  bool changeWalletPassphrase(const std::string &old_wallet_passphrase,
                              const std::string &new_wallet_passphrase);
};

#endif // WALLET_H
