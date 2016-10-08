// It's important to include <stdlib.h> and define _CRT_RAND_S before including "Encryption.h"
#define _CRT_RAND_S
#include <stdlib.h>

#include "Encryption.h"
#include "sha512.h"
#include "argon2.h"

bool altGenSalt(uint8_t salt[saltSize]){

	errno_t err;
	unsigned int number;
	int i;

	for (i = 0; i < saltSize; i++){
		err = rand_s(&number);

		if (err != 0) puts("rand_s function fail . . I don't know what went wrong, do you?");

		salt[i] = (uint8_t) ((double) number / ((double) UINT_MAX + 1) * 255.0);
	}
	return true;
}

std::string _generateSalt(){

	errno_t err;
	unsigned int number;
	int i;
	uint8_t salt[saltSize];

	for (i = 0; i < saltSize; i++){
		err = rand_s(&number);

		if (err != 0) puts("rand_s function fail . . I don't know what went wrong, do you?");

		salt[i] = (uint8_t) ((double) number / ((double) UINT_MAX + 1) * 255.0);
	}

	return std::string(salt, salt + saltSize);
}

std::string argon2iHash(std::string password, std::string salt){
	unsigned char hash[preHashSize];

	uint8_t * _salt = (uint8_t *)strdup(salt.c_str());

	uint8_t * _password = (uint8_t *)strdup(password.c_str());
	uint32_t _passwordLength = strlen((char *)password.c_str());

	uint32_t computations = 2;
	uint32_t memoryCost = (2<<14);
	uint32_t threads = 1;

	argon2i_hash_raw(computations, memoryCost, threads, _password, _passwordLength, _salt, saltSize, hash, preHashSize);

	int i;
	unsigned char converted[outHashSize + 1];
	for (i = 0; i < preHashSize; i++){
		sprintf((char *)(converted + i * 2), "%02x", hash[i]);
	}

	return std::string(converted, converted + outHashSize);
}

std::string sha512Hash(std::string password){

	return sha512(password);
}