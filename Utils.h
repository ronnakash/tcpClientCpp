#pragma once
#include <string>



class Utils {

public:
	static void hexify(const unsigned char* chars, int length);
	static void hexify(int length, std::string str);

};