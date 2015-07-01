/*
 * tester.cpp
 *
 *  Created on: 1 Jul 2015
 *      Author: spacekookie
 */

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <cryptopp/osrng.h>
using CryptoPP::AutoSeededRandomPool;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <string>
using std::string;

#include <cstdlib>
using std::exit;

#include <cryptopp/cryptlib.h>
using CryptoPP::Exception;

#include <cryptopp/hex.h>
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include <cryptopp/filters.h>
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::StreamTransformationFilter;

#include <cryptopp/aes.h>
using CryptoPP::AES;

#include <cryptopp/ccm.h>
using CryptoPP::CTR_Mode;

#include <assert.h>

#include "rcry_wrapper.hpp"

int main(int argc, char* argv[])
{
	byte *key;

	generateKey(RCRY_TWOFISH, &key);

	string encoded;
	encoded.clear();
	StringSource(key, sizeof(key), true, new HexEncoder(new StringSink(encoded)) // HexEncoder
			);// StringSource
	cout << "key: " << encoded << endl;

//	AutoSeededRandomPool prng;
//
//	byte key[AES::DEFAULT_KEYLENGTH];
//	prng.GenerateBlock(key, sizeof(key));
//
//	byte iv[AES::BLOCKSIZE];
//	prng.GenerateBlock(iv, sizeof(iv));
//
//	string plain = "This is something really awesome in CTR mode! YAy!";
//	string cipher, encoded, recovered;
//
//	/*********************************\
//	\*********************************/
//
//	// Pretty print key
//	encoded.clear();
//	StringSource(key, sizeof(key), true, new HexEncoder(new StringSink(encoded)) // HexEncoder
//			);// StringSource
//	cout << "key: " << encoded << endl;
//
//	// Pretty print iv
//	encoded.clear();
//	StringSource(iv, sizeof(iv), true, new HexEncoder(new StringSink(encoded)) // HexEncoder
//			);// StringSource
//	cout << "iv: " << encoded << endl;
//
//	/*********************************\
//	\*********************************/
//
//	try
//	{
//		cout << "plain text: " << plain << endl;
//
//		CTR_Mode<AES>::Encryption e;
//		e.SetKeyWithIV(key, sizeof(key), iv);
//
//		// The StreamTransformationFilter adds padding
//		//  as required. ECB and CBC Mode must be padded
//		//  to the block size of the cipher.
//		StringSource(plain, true,
//				new StreamTransformationFilter(e, new StringSink(cipher)) // StreamTransformationFilter
//						);// StringSource
//	} catch (const CryptoPP::Exception& e)
//	{
//		cerr << e.what() << endl;
//		exit(1);
//	}
//
//	/*********************************\
//	\*********************************/
//
//	// Pretty print
//	encoded.clear();
//	StringSource(cipher, true, new HexEncoder(new StringSink(encoded)) // HexEncoder
//			);// StringSource
//	cout << "cipher text: " << encoded << endl;
//
//	/*********************************\
//	\*********************************/
//
//	try
//	{
//		CTR_Mode<AES>::Decryption d;
//		d.SetKeyWithIV(key, sizeof(key), iv);
//
//		// The StreamTransformationFilter removes
//		//  padding as required.
//		StringSource s(cipher, true,
//				new StreamTransformationFilter(d, new StringSink(recovered)) // StreamTransformationFilter
//						);// StringSource
//
//		cout << "recovered text: " << recovered << endl;
//	} catch (const CryptoPP::Exception& e)
//	{
//		cerr << e.what() << endl;
//		exit(1);
//	}
//
//	/*********************************\
//	\*********************************/

	return 0;
}
