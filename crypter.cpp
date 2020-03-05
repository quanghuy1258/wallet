#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>
#include <cryptopp/sha.h>

#include "crypter.h"

void Crypter::renew() {
  _keyPtr.reset(new SecureBytes());
  _ivPtr.reset(new SecureBytes());

  _keyPtr->resize(KEY_SIZE);
  _ivPtr->resize(IV_SIZE);
}

Crypter::Crypter() {
  renew();
  _fKeySet = false;
}

Crypter::~Crypter() {
  renew();
  _fKeySet = false;
}

bool Crypter::setKey(const SecureBytes &key, const SecureBytes &iv) {
  if (key.size() != KEY_SIZE || iv.size() != IV_SIZE)
    return false;

  std::memcpy(_keyPtr->data(), key.data(), KEY_SIZE);
  std::memcpy(_ivPtr->data(), iv.data(), IV_SIZE);
  _fKeySet = true;

  return true;
}

bool Crypter::setKeyFromPassphrase(const SecureString &passpharse,
                                   const std::vector<unsigned char> &salt,
                                   const int nRounds) {
  if (nRounds < 1 || salt.size() != SALT_SIZE)
    return false;

  SecureBytes buf;
  buf.resize(CryptoPP::SHA512::DIGESTSIZE);
  CryptoPP::SHA512 hash;
  hash.Update((unsigned char *)passpharse.data(), passpharse.size());
  hash.Update(salt.data(), salt.size());
  hash.Final(buf.data());

  for (int i = 0; i < nRounds - 1; i++) {
    hash.Restart();
    hash.Update(buf.data(), CryptoPP::SHA512::DIGESTSIZE);
    hash.Final(buf.data());
  }

  std::memcpy(_keyPtr->data(), buf.data(), KEY_SIZE);
  std::memcpy(_ivPtr->data(), buf.data() + KEY_SIZE, IV_SIZE);
  _fKeySet = true;

  return true;
}

bool Crypter::encrypt(const SecureBytes &plaintext,
                      std::vector<unsigned char> &ciphertext) {
  if (!_fKeySet)
    return false;

  CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption enc(_keyPtr->data(), KEY_SIZE,
                                                    _ivPtr->data());
  CryptoPP::StreamTransformationFilter filter(enc);
  filter.PutMessageEnd(plaintext.data(), plaintext.size());
  ciphertext.resize(filter.MaxRetrievable());
  filter.Get(ciphertext.data(), ciphertext.size());

  return true;
}

bool Crypter::decrypt(const std::vector<unsigned char> &ciphertext,
                      SecureBytes &plaintext) {
  if (!_fKeySet)
    return false;

  CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption dec(_keyPtr->data(), KEY_SIZE,
                                                    _ivPtr->data());
  CryptoPP::StreamTransformationFilter filter(dec);
  filter.PutMessageEnd(ciphertext.data(), ciphertext.size());
  plaintext.resize(filter.MaxRetrievable());
  filter.Get(plaintext.data(), plaintext.size());

  return true;
}
