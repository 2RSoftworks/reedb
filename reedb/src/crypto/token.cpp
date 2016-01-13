/*
 * token.c
 *
 *  Created on: 10 Aug 2015
 *      Author: spacekookie
 */

// Reedb internal includes
#include "reedb/crypto/token.h"
#include "crypto/rcry_engine.h"

// Runtime includes
#include <string>
#include <iostream>
#include <iomanip>

// Crypto++ includes
#include <cryptopp/base64.h>
#include <cryptopp/osrng.h>
#include <cryptopp/des.h>
#include <string>

#include <stdio.h>
#include <malloc.h>

using namespace CryptoPP;

rdb_token *rdb_tokens_create(unsigned int timed)
{
  rdb_token *token = (rdb_token*) malloc(sizeof(rdb_token));
  token->timed = timed;
  token->created = 0;
  
  /** Temporary string for our random sink */
  std::string encoded;
  
  /** Create a generation scratch */
  const unsigned int BLOCKSIZE = 95;
  byte pcbScratch[BLOCKSIZE];

  /** Using a ANSI approved Cipher */
  CryptoPP::AutoSeededX917RNG<CryptoPP::DES_EDE3> rng;
  rng.GenerateBlock(pcbScratch, BLOCKSIZE);

  /** Base64 encode and strip newlines from random block */
  StringSource ss(pcbScratch, sizeof(pcbScratch) , true, new Base64Encoder(new StringSink(encoded)));
  std::string::iterator end_pos = std::remove(encoded.begin(), encoded.end(), '\n');
  encoded.erase(end_pos, encoded.end());
 
  /** Save our token in the struct */
  token->contents = &encoded->c_str();
  return token;
}