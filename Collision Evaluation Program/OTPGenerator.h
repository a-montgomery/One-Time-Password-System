#include "sha256.h"
#include <iostream>
#include <string>
#pragma once
using namespace std;


std::string GenerateHash(std::string key);
std::string TruncateHash(std::string hash);

/*
Function that generates the sha256 hash based on the hardcoded key.
@param key: The key used to generate the hash
@return a string representing the sha256 hash generated from the key
*/
std::string GenerateHash(std::string key)
{
	std::string newHash;
	newHash = sha256(key);
	return newHash;
}

/*
Function that truncates the hash to represent an OTP. The function takes
the 6 least significant digits from the hash
@para hash: A string representing the sha256 hash
@return a string representing the 6 digit OTP
*/
std::string TruncateHash(std::string hash)
{
	std::string newOTP;
	int length = hash.length();
	newOTP = hash.substr(length - 6, length);
	return newOTP;
}



