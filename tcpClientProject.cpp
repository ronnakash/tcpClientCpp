// tcpClientProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Base64Wrapper.h"
#include "RSAWrapper.h"
#include "AESWrapper.h"
#include "crc32.h"

#include <iostream>
#include <iomanip>
#include <iostream>

#include <cstdlib>
#include <cstring>

#include "Request.h"
#include "Response.h"

#include <boost/asio.hpp>
#include "SocketWrapper.h"
#include "FileReader.h"
#include "Utils.h"
#include "User.h"

using boost::asio::ip::tcp;

enum { max_length = 1024 };

std::string* random_string(size_t length)
{
	auto randchar = []() -> char
	{
		srand(time(0));
		const char charset[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof(charset) - 1);
		return charset[rand() % max_index];
	};
	std::string* str= new std::string(length, 0);
	std::generate_n(str->begin(), length, randchar);
	return str;
}

void hexify(const unsigned char* buffer, unsigned int length)
{
	std::ios::fmtflags f(std::cout.flags());
	std::cout << std::hex;
	for (size_t i = 0; i < length; i++)
		std::cout << std::setfill('0') << std::setw(2) << (0xFF & buffer[i]) << (((i + 1) % 16 == 0) ? "\n" : " ");
	std::cout << std::endl;
	std::cout.flags(f);
}


int rsa_example()
{
	std::cout << std::endl << std::endl << "----- RSA EXAMPLE -----" << std::endl << std::endl;

	// plain text (could be binary data as well)
	unsigned char plain[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
	std::cout << "plain:" << std::endl;
	hexify(plain, sizeof(plain));		// print binary data nicely

	// 1. Create an RSA decryptor. this is done here to generate a new private/public key pair
	RSAPrivateWrapper rsapriv;

	// 2. get the public key
	std::string pubkey = rsapriv.getPublicKey();	// you can get it as std::string ...

	char pubkeybuff[RSAPublicWrapper::KEYSIZE];
	rsapriv.getPublicKey(pubkeybuff, RSAPublicWrapper::KEYSIZE);	// ...or as a char* buffer

	// 3. create an RSA encryptor
	RSAPublicWrapper rsapub(pubkey);
	std::string cipher = rsapub.encrypt((const char*)plain, sizeof(plain));	// you can encrypt a const char* or an std::string 
	std::cout << "key:" << std::endl;
	//hexify((unsigned char*)cipher.c_str(), cipher.length());	// print binary data nicely
	hexify((unsigned char*)pubkeybuff, 160);	// print binary data nicely


	// 4. get the private key and encode it as base64 (base64 in not necessary for an RSA decryptor.)
	std::string base64key = Base64Wrapper::encode(rsapriv.getPrivateKey());

	// 5. create another RSA decryptor using an existing private key (decode the base64 key to an std::string first)
	RSAPrivateWrapper rsapriv_other(Base64Wrapper::decode(base64key));

	std::string decrypted = rsapriv_other.decrypt(cipher);		// 6. you can decrypt an std::string or a const char* buffer
	std::cout << "decrypted:" << std::endl;
	hexify((unsigned char*)decrypted.c_str(), decrypted.length());	// print binary data nicely

	return 0;
}

//User* registerUser(SocketWrapper* sw, std::string nameString) {
//	std::string clientId("ignored");
//	RegisterRequestPayload* registerReqPayload = new RegisterRequestPayload(nameString);
//	Request* registerRequest = new Request(&clientId, RequestHeader::REGISTER_REQUEST_CODE, 255, (RequestPayload*)registerReqPayload);
//	Response* response1 = sw->sendAndReceive(registerRequest);
//	response1->print();
//	std::string* newClientId = new std::string(((RegisterResponsePayload*)response1->getPayload())->getClientId());
//	return new User(*newClientId, nameString);
//
//}
//
//User* registerIfNeeded(SocketWrapper* sw, std::string nameString) {
//	if (FileReader::infoFileExists()) {
//		MyInfoFileDetails* myInfo = FileReader::readMyInfoFile();
//		std::string clientIdStr = (char*)myInfo->getClientId();
//		std::string username = myInfo->getUsername();
//		return new User(clientIdStr, username);
//	}
//	else {
//		User* user = registerUser(sw, nameString);
//		//create MyInfo file
//		std::string clientIdStr = user->getId();
//		std::string username = user->getUsername();
//		MyInfoFileDetails* myInfo = new MyInfoFileDetails(username, (unsigned char*)clientIdStr.c_str(), nullptr);
//		FileReader::createMyInfoFile(myInfo);
//		return user;
//	}
//}


int main(int argc, char* argv[])
{
	try
	{
		//read transfer file
		TransferFileDetails* tfd = FileReader::readTransferFile();
		std::string host = tfd->getHost();
		std::string port = tfd->getPort();
		SocketWrapper* sw = new SocketWrapper(host, port);
		std::string nameString = tfd->getUsername();

		//register if needed and get clientId
		User* user = User::registerIfNeeded(sw, nameString);
		std::string* clientId = new std::string(user->getId());

		//generage rsa keys
		RSAPrivateWrapper rsapriv;
		std::string pubkeyfromPriv = rsapriv.getPublicKey();
		RSAPublicWrapper rsapub(pubkeyfromPriv);
		std::string pubKey = rsapub.getPublicKey();

		//key exchange
		KeyExchangeRequestPayload* keyReqPayload = new KeyExchangeRequestPayload(nameString, pubKey);
		Request* keyExchangeRequest = new Request(clientId, RequestHeader::SEND_PUBLIC_KEY_REQUEST_CODE, 415, (RequestPayload*)keyReqPayload);
		Response* response2 = sw->sendAndReceive(keyExchangeRequest);
		//response2->print();

		//decrypt aes key from server and create aes wrapper
		KeyExchangeResponsePayload* kerp = (KeyExchangeResponsePayload*)response2->getPayload();
		const unsigned char* encryptedKey = kerp->getEncryptedKey();
		std::string encKeyStr = (char*)encryptedKey;
		std::string decryptedStr = rsapriv.decrypt((char *)encryptedKey, 144-16);
		unsigned char* decryptedChars = (unsigned char*) decryptedStr.c_str();
		AESWrapper aes(decryptedChars, AESWrapper::DEFAULT_KEYLENGTH);

		//read and encrypt file to transfer
		std::string filename = tfd->getFilename();
		std::string fileStr = FileReader::readFile(filename);
		const char* transferFile = fileStr.c_str();
		std::string* encryptedFile = new std::string (aes.encrypt(transferFile, fileStr.length()));
		const char* encryptedFileChars = encryptedFile->c_str();

		//log fileStr transferFile encryptedFile+len
		//std::cout << "read file str: ";
		//Utils::hexify(fileStr.length(), fileStr);
		//std::cout << "read file chars: ";
		//Utils::hexify((unsigned char*)transferFile, fileStr.length());
		//std::cout << "encrypted file content str: " << encryptedFile->length() << std::endl;
		//Utils::hexify(encryptedFile->length(), *encryptedFile);
		//std::cout << "encryptedFileChars: " << encryptedFile->length() << std::endl;
		//Utils::hexify((unsigned char*)transferFile, encryptedFile->length());

		//calc checksum
		unsigned long crc;
		crc = crc_init();
		crc = crc_update(crc, (unsigned char*)transferFile, fileStr.length());
		crc = crc_finalize(crc);
		std::cout << "crc checksum: " << crc << std::endl;


		//send file
		unsigned short retries = 0;
		for (; retries < 4; retries++) {
			FileExchangeRequestPayload* fileExchangeRequestPayload = new FileExchangeRequestPayload(*clientId, encryptedFile->length(), filename, *encryptedFile);
			Request* fileExchangeRequest = new Request(clientId, RequestHeader::SEND_FILE_REQUEST_CODE, fileExchangeRequestPayload->getSize(), (RequestPayload*)fileExchangeRequestPayload);
			Response* response3 = sw->sendAndReceive(fileExchangeRequest);
			//compare checksums and respond accordingly
			FileExchangeResponsePayload* fileExchangeResponsePayload = (FileExchangeResponsePayload*)response3->getPayload();
			unsigned int responseChecksum = fileExchangeResponsePayload->getChecksum();
			crcRequestPayload* crcrp = new crcRequestPayload(*clientId, filename);
			if (crc == responseChecksum) {
				//send success
				//crcRequestPayload* crcrp = new crcRequestPayload(*clientId, filename);
				Request* successRequest = new Request(clientId, RequestHeader::CRC_OK_REQUEST_CODE, 255+16, (RequestPayload*)crcrp);
				Response* response4 = sw->sendAndReceive(successRequest);
				std::cout << "File sent!" << std::endl;
				break;
			}
			//send fail and retry
			else if (retries != 3) {
				Request* retryRequest = new Request(clientId, RequestHeader::CRC_RETRY_REQUEST_CODE, 255 + 16, (RequestPayload*)crcrp);
				Response* response4 = sw->sendAndReceive(retryRequest);
			}
		}


		if (retries == 4) {
			//send failed and abort message
			crcRequestPayload* crcfp = new crcRequestPayload(*clientId, filename);
			Request* retryRequest = new Request(clientId, RequestHeader::CRC_FAIL_REQUEST_CODE, 255 + 16, (RequestPayload*)crcfp);
			Response* response4 = sw->sendAndReceive(retryRequest);
			std::cout << "Error!" << std::endl;
		}

	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}





int aes_example()
{
	std::cout << std::endl << std::endl << "----- AES EXAMPLE -----" << std::endl << std::endl;

	std::string plaintext = "Once upon a time, a plain text dreamed to become a cipher";
	std::cout << "Plain:" << std::endl << plaintext << std::endl;

	// 1. Generate a key and initialize an AESWrapper. You can also create AESWrapper with default constructor which will automatically generates a random key.
	unsigned char key[AESWrapper::DEFAULT_KEYLENGTH];
	AESWrapper aes(AESWrapper::GenerateKey(key, AESWrapper::DEFAULT_KEYLENGTH), AESWrapper::DEFAULT_KEYLENGTH);

	// 2. encrypt a message (plain text)
	std::string ciphertext = aes.encrypt(plaintext.c_str(), plaintext.length());
	std::cout << "Cipher:" << std::endl;
	hexify(reinterpret_cast<const unsigned char*>(ciphertext.c_str()), ciphertext.length());	// print binary data nicely

	// 3. decrypt a message (cipher text)
	std::string decrypttext = aes.decrypt(ciphertext.c_str(), ciphertext.length());
	std::cout << "Decrypted:" << std::endl << decrypttext << std::endl;

	return 0;
}



//int main()
//{
//	aes_example();
//
//	rsa_example();
//
//	return 0;
//}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file


