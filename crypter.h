#ifndef CRYPTER_H
#define CRYPTER_H

#include <memory>
#include <vector>

#include <cryptopp/aes.h>

#include "sec_block.h"

const int KEY_SIZE = CryptoPP::AES::DEFAULT_KEYLENGTH;
const int IV_SIZE = CryptoPP::AES::BLOCKSIZE;
const int SALT_SIZE = 8;

class Crypter {
private:
  std::unique_ptr<SecureBytes> _keyPtr;
  std::unique_ptr<SecureBytes> _ivPtr;
  bool _fKeySet;

  void renew();

public:
  Crypter();
  ~Crypter();

  bool setKey(const SecureBytes &key, const SecureBytes &iv);
  bool setKeyFromPassphrase(const SecureString &passpharse,
                            const std::vector<unsigned char> &salt,
                            const int nRounds);
  bool encrypt(const SecureBytes &plaintext,
               std::vector<unsigned char> &ciphertext);
  bool decrypt(const std::vector<unsigned char> &ciphertext,
               SecureBytes &plaintext);
};

#endif // CRYPTER_H
