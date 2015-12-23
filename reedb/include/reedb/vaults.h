
#ifndef VAULT_H
#define VAULT_H

#include "reedb/utils/uuid.h"

/* Struct that */
struct vault_head {
  rdb_uuid *id;
  string *name, *path;
  unsigned int size;
} vault_head;

class rdb_vaults
{
public:
  
  rdb_vaults(reedb *core);
  ~rdb_vaults();
  
  vector<vault_head> *list_vaults();
  
  void scope_vault(vault_head *head);
  void unscope_vault(rdb_uuid uuid);
  
  vault_head *create(string name, string path, string passphrase);
  
// ree_err_t rdb_vault_create(ree_token **token, char **uuid, char *name, char *path, char *passphrase);
// 
// ree_err_t rdb_vault_authenticate(ree_token *token, char *uuid);
// 
// ree_err_t rdb_vault_headers(char *uuid, ree_token *token, char *search);
// 
// ree_err_t rdb_vault_headers(char *uuid, ree_token *token);
// 
// ree_err_t rdb_vault_file(char *uuid, ree_token *token);
// 
// ree_err_t rdb_vault_insert(char *uuid, ree_token *token, char *data);
// 
// ree_err_t rdb_vault_delete(char *uuid, ree_token *token, char *file);
// 
// ree_err_t rdb_vault_close(char *uuid, ree_token *token);
};

#endif // REEDB_H
