#include "FileReader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <iomanip>
#include "Base64Wrapper.h"
#include "Utils.h"



MyInfoFileDetails::MyInfoFileDetails(std::string username, unsigned char* clientId, unsigned char* privateKey)
{
	username_ = username;
	clientId_ = clientId;
	privateKey_ = privateKey;
}

MyInfoFileDetails::~MyInfoFileDetails()
{
	//delete username_;
	//delete clientId_;
	//delete privateKey_;
}

std::string MyInfoFileDetails::getUsername()
{
	return username_;
}

unsigned char* MyInfoFileDetails::getClientId()
{
	return clientId_;
}

unsigned char* MyInfoFileDetails::getPrivateKey()
{
	return privateKey_;
}

std::vector<std::string> TransferFileDetails::split(std::string str)
{    
    std::string token(":");
    std::vector<std::string>result;
    while (str.size()) {
        int index = str.find(token);
        if (index != std::string::npos) {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0)result.push_back(str);
        }
        else {
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

TransferFileDetails::TransferFileDetails(std::string address, std::string username, std::string filename)
{
    //split host and port
    std::vector<std::string> tokens = split(address);
    if (tokens.size() != 2) {
        //raise exception
        std::cout << "OOPS!!!" << std::endl;
    }
    host_ = tokens.front();
    port_ = tokens.at(1);
    username_ = username;
    filename_ = filename;
}

TransferFileDetails::~TransferFileDetails()
{
}

std::string TransferFileDetails::getUsername()
{
	return username_;
}

std::string TransferFileDetails::getFilename()
{
	return filename_;
}

std::string TransferFileDetails::getHost()
{
	return host_;
}

std::string TransferFileDetails::getPort()
{
	return port_;
}


void FileReader::readHex(char* buf, char* txt)
{
    char b[3] = "00";
    for (unsigned int i = 0; i < strlen(txt); i += 2) {
        b[0] = *(txt + i);
        b[1] = *(txt + i + 1);
        *(buf + (i >> 1)) = strtoul(b, NULL, 16);
    }
}

TransferFileDetails* FileReader::readTransferFile()
{
    std::string address;
    std::string username;
    std::string filename;
    std::ifstream myReadFile("transfer.info");
    getline(myReadFile, address);
    getline(myReadFile, username);
    getline(myReadFile, filename);
    myReadFile.close();
    TransferFileDetails* transferFileDetails = new TransferFileDetails(address, username, filename);
    return transferFileDetails;
}

MyInfoFileDetails* FileReader::readMyInfoFile()
{
    std::string username;
    std::string clientIdRaw;
    std::string clientId;
    std::string privateKey;
    std::ifstream myReadFile("me.info");
    getline(myReadFile, username);
    getline(myReadFile, clientIdRaw);
    //parse hex to chars
    //getline(myReadFile, privateKey);
    myReadFile.close();
    //MyInfoFileDetails* myInfoFileDetails = new MyInfoFileDetails(username, (unsigned char*) clientId.c_str(),
    //    (unsigned char*) Base64Wrapper::decode(privateKey).c_str());
    const char* temp = clientIdRaw.c_str();
    char* temp2 = (char*) temp;
    std::cout << "const chars: " << temp << std::endl;
    std::cout << "chars: " << temp2 << std::endl;
    std::cout << "string: " << clientIdRaw << std::endl;
    char* decodedCidBuf = new char[17];
    readHex(decodedCidBuf, temp2);
    std::cout << "decoded: " << std::endl;
    Utils::hexify((unsigned char*)decodedCidBuf, 16);

    MyInfoFileDetails* myInfoFileDetails = new MyInfoFileDetails(username, (unsigned char *)decodedCidBuf, nullptr);

    return myInfoFileDetails;
}

void FileReader::createMyInfoFile(MyInfoFileDetails* fileInfo)
{
    std::ofstream t("me.info");
    //std::string* cidStr = new std::string( (char *) fileInfo->getClientId());
    //unsigned char * key = fileInfo->getPrivateKey();
    //std::string temp((char*) fileInfo->getPrivateKey());
    //std::string key = Base64Wrapper::encode(temp);
    const unsigned char* cid = fileInfo->getClientId();
    t << fileInfo->getUsername() << "\n";
    std::ios::fmtflags f(std::cout.flags());
    t << std::hex;
    for (size_t i = 0; i < 16; i++)
        t << std::setfill('0') << std::setw(2) << (0xFF & cid[i]);
    //t << "\n" << key << std::feof;
    t << std::endl;
    t.close();

}

std::string FileReader::readFile(std::string filename)
{
    std::ifstream t;
    t.open(filename, std::ios_base::in | std::ios_base::binary);
    std::string data;
    t.seekg(0, std::ios::end);
    data.reserve(t.tellg());
    t.seekg(0, std::ios::beg);
    data.assign((std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>());
    t.close();
    //std::cout << "read file data str: \n";
    //Utils::hexify(data.length(), data);
    //std::cout << "read file data chars: \n";
    //Utils::hexify((unsigned char*)data.c_str(), data.length());


    return data;
    //return slurp(filename);
}



bool FileReader::infoFileExists()
{
    std::fstream fs;
    fs.open("me.info");
    return !fs.fail();
}

inline std::string FileReader::slurp(const std::string& path)
{
    std::ostringstream buf;
    std::ifstream input(path.c_str());
    buf << input.rdbuf();
    return buf.str();

}
