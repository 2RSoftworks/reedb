#include <reedb/reedb.h>
#include <reedb/data.h>

#include <stdio.h>

int main(void)
{
    rdb_err_t err;
    
    rdb_context ctx;
    err = rdb_ctx_init(&ctx);
    printf("Context init...%s!\n", (err == 0) ? "OK" : "FAILED"); if(err) goto exit;

    rdb_vault *vault;
    err = rdb_ctx_vaultcreate(&ctx, &vault, "default", "~/Documents/");
    printf("Vault creation...%s!\n", (err == 0) ? "OK" : "FAILED"); if(err) goto exit;

//    /* Setup the root login passphrase */
//    err = rdb_vlts_setlogin(vault, RDB_USER_ROOT, "SetTheRootPassphraseToThis!");
//    printf("Set login...%s!\n", (err == 0) ? "OK" : "FAILED"); if(err) goto exit;

    /* Setup some flags */
    err = rdb_vlts_setflags(vault, RDB_FLG_ROOT);
    printf("Set flags...%s!\n", (err == 0) ? "OK" : "FAILED"); if(err) goto exit;

    rdb_vlts_adduser(vault, "spacekookie", 0);
    rdb_uuid spacekookie = rdb_vlts_getuser(vault, "spacekookie");

    rdb_vlts_setlogin(vault, &spacekookie, "mega_passphrase!");

    err = rdb_ctx_free(&ctx);
    printf("Context free...%s!\n", (err == 0) ? "OK" : "FAILED"); if(err) goto exit;

    exit:
    return err;
}
