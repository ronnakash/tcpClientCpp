#pragma once
#include <string>
#include <vector>


class TransferFileDetails {

private:

	std::string host_;
	std::string port_;
	std::string username_;
	std::string filename_;

	std::vector<std::string> split(std::string str);


public:

	TransferFileDetails(std::string address, std::string username, std::string filename);
	~TransferFileDetails();

	std::string getUsername();
	std::string getFilename();
	std::string getHost();
	std::string getPort();
};

class MyInfoFileDetails {

private:

	std::string username_;
	unsigned char* clientId_;
	unsigned char* privateKey_;

public:

	MyInfoFileDetails(std::string username, unsigned char* clientId, unsigned char* privateKey);
	~MyInfoFileDetails();

	std::string getUsername();
	unsigned char* getClientId();
	unsigned char* getPrivateKey();
};

class FileReader {

private:
	static void readHex(char* buf, char* txt);


public:

	//FileReader();
	//~FileReader();

	static TransferFileDetails* readTransferFile();
	static MyInfoFileDetails* readMyInfoFile();
	static void createMyInfoFile(MyInfoFileDetails* fileInfo);
	static std::string readFile(std::string filename);
	static bool infoFileExists();
	static inline std::string slurp(const std::string& path);
};