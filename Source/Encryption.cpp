#include "Encryption.h"
#include "sha512.h"

std::string hashPassword(std::string password){
	return sha512(password);
}