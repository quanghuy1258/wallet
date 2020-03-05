#ifndef SEC_BLOCK_H
#define SEC_BLOCK_H

#include <string>

#include <cryptopp/secblock.h>

typedef std::basic_string<char, std::char_traits<char>,
                          CryptoPP::AllocatorWithCleanup<char>>
    SecureString;
typedef std::vector<unsigned char,
                    CryptoPP::AllocatorWithCleanup<unsigned char>>
    SecureBytes;

#endif // SEC_BLOCK_H
