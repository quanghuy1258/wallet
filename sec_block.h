#ifndef SEC_BLOCK_H
#define SEC_BLOCK_H

#include <string>

#include <cryptopp/secblock.h>

typedef std::basic_string<char, std::char_traits<char>,
                          CryptoPP::AllocatorWithCleanup<char>>
    SecureString;

#endif // SEC_BLOCK_H
