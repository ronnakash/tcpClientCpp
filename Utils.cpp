#include "Utils.h"
#include <iostream>
#include <iomanip>

using namespace std;

void Utils::hexify(int length, std::string str)
{
	const unsigned char* chars = (unsigned char*) str.c_str();
	hexify(chars, length);
}

void Utils::hexify(const unsigned char* chars, int length)
{
	//std::cout << "Payload:" << endl;
	//std::cout << "Client ID: ";
	std::ios::fmtflags f(std::cout.flags());
	std::cout << std::hex;
	for (size_t i = 0; i < length; i++)
		std::cout << std::setfill('0') << std::setw(2) << (0xFF & chars[i]) << (((i + 1) % 16 == 0) ? "\n" : " ");
	std::cout << std::endl;
	std::cout.flags(f);
}
