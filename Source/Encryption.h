#pragma once

#include <string>

// If you change preHashSize and saltSize, make sure to update the
// sql table accordingly:

// IE:
//    `password` varchar(outHashSize)
//	  `salt` varchar(saltSize)

#define preHashSize 128
#define saltSize 128

// A string needs to be converted once hashed,
// the converted value is twice as big
#define outHashSize preHashSize*2

// Hashpassword uses argon2i
std::string argon2iHash(std::string password, std::string salt);
std::string sha512Hash(std::string password);
std::string _generateSalt();

#define useArgonHashing
#ifdef useArgonHashing
#define hashPassword(password, salt) argon2iHash(password, salt)
#else
#define hashPassword(password, salt) sha512Hash(password + salt)
#endif

#define generateSalt() sha512Hash(_generateSalt())
//bool altGenSalt(uint8_t salt[saltSize]);