#include "sha256.h"
#include "OTPGenerator.h"
#include <iostream>
#include <string>
#include <vector>
#include <set>

using std::cout;
using std::endl;

int main()
{
	std::string key = "810770FF00FF07012"; //hard-coded key
	std::string currHash;
	std::string currOTP;
	int otpCollisionCount = 0;

	//We use a set here because a set can only have allows unique
	//elements. We can then subtract unique elements from total elements
	//generated to determine the total number of collisions that occured.
	set<string> otp;


	//Generate the first OTP based on the hard-coded key
	currHash = GenerateHash(key);
	currOTP = TruncateHash(currHash);
	otp.insert(currOTP);


	//Loop to generate 1,000,000 OTP's
	for (int i = 1; i < 1000001; i++)
	{
		currHash = GenerateHash(currHash);
		currOTP = TruncateHash(currHash);

		//Insert the OTP into the set, if there is a match
		//nothing is inserted
		otp.insert(currOTP);

		//Retrieve how many collisions there were every 50,000
		//OTP's generated. Done by subtracting the number of items
		//in the set, from the number of OTP's generated
		if (i % 50000 == 0)
		{
			cout << i - otp.size() << endl;
		}
	}

}