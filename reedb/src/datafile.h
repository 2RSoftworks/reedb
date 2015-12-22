#ifndef RDBDATAFILE_H
#define RDBDATAFILE_H

#include <string>
#include <vector>
#include "RdbVault.h"

extern "C" {
#include "reedb/utils/hashmap.h"
}

typedef enum CacheMode {
  /* Drop from cache after every action */
  HOTDROP, 
  
  /* Keep in cache for limited time (defined in config) */
  TIMEOUT, 
  
  /* Keep in cache until daemon shuts down */
  ENDLESS
} CacheMode;

typedef struct FilePath {
  string *path;
  int opMode;
} FilePath;

class RdbDatafile
{
public:
  
  /** 
   * Default constructor that creates a datfile. It goes through
   * the entire file creation process and optionally also creates
   * new keys for zones it should be long to.
   * 
   * @param name Name of the file (in cleartext)
   * @param *parent Struct that contain parent path + permissions
   */
  RdbDatafile(string name, FilePath *parent);
  
  /** 
   * Use this constructor to initiate a file that has previously 
   * existed.
   * 
   * @param name Name of the file (in cleartext)
   * @param oldFile Contents on the old file as binary buffer
   */
  RdbDatafile(string name, string oldFile);
  
  /**
   * 
   */
  ree_err_t insertData(string data);
  
  /**
   * 
   */
  ree_err_t readFile(map_t *(*data));
  
  /**
   * @param mode: The mode that should be used for caching. See @"CacheMode"
   */
  ree_err_t cache(const CacheMode mode);
  
  /**
   * Closes the file and uncaches it.
   */
  ree_err_t close();
  
  /**
   * Deletes this datafile, scrubs it from disk and removes the encryption
   * keys from the keystore.
   */
  
private:
  
  /* Some global metadata */
  string name;
  string path;
  int version;
  bool locked;
  
  /* Database metadata for queries */
  string category;
  string tags[];
  
  /* Vector of data maps. Each version creates a new map */
  std::vector<map_t*> data;
  
  /* @return If file is locked */
  bool isLocked();
  
  /** 
   * Populate a new file with basic data so there are no nullpointers
   * that can get thrown.
   */
  void populate();
  
  /**
   * Writes the data from 
   */
  void write();
  
};

#endif // RDBDATAFILE_H
