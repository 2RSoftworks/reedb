#include <reedb/reedb.h>

#include <hashtable.h>
#include <stdlib.h>
#include <stdio.h>
#include <list.h>
#include <datastore/hashmap.h>
#include <err.h>
#include <gcrypt.h>

#include "utils/uuid.h"
#include "defines.h"
#include "config.h"

#define VAULT_SANE \
    if(vault == NULL) return INVALID_VAULT; \
    if(vault->name == NULL) return INVALID_VAULT; \
    if(vault->path == NULL) return INVALID_VAULT; \
    if(vault->combined == NULL) return INVALID_VAULT; \

#define MEMCHECK \
{ \
    if(vault->inner == NULL) { \
        vault->inner = (ree_vault*) malloc(sizeof(ree_vault)); \
        if(vault->inner == NULL) return MALLOC_FAILED; \
        memset(vault->inner, 0, sizeof(ree_vault)); \
    } \
};

#define ASSIGN(field, fl) \
    if(flags & (fl)) field = fl;

#define FIELD_CHECK(field) \
    if(field == 0) { \
        printf("Failed to provide value for field %s\n", #field); \
        return INVALID_PARAMS; \
    }

struct ree_vault {
    unsigned long       type;
    unsigned long       cache;
    unsigned long       write_m;
    unsigned long       scale_t;
    unsigned long       log_t;
    unsigned long       rqlsyn_t;
    unsigned long       ftr_single, ftr_multi, ftr_tree;
    unsigned long       ftr_lut_dis, ftr_rql_dis, ftr_head_dis;
    unsigned long       ftr_perm;

    /* Vault contents */
    Hashtable           *headers, *files;
    List                *tags, *categories;
    struct threadpool   *thpool;

    /** Add metadata **/
    map_t               *users;
    struct vault_zone   **zones;
    size_t              zone_s, usd_zones;
};

/**********************************************************************/

rdb_err_t check_flagmask(unsigned long flags, unsigned long mask);

rdb_err_t rdb_vlts_setflags(rdb_vault *vault, unsigned long flags)
{
    VAULT_SANE MEMCHECK

    rdb_err_t err;

    /**
     *
     * This function is a bit insane. Three MACROs are invoked over and over again and this should
     * quickly explain what they do for future contributers.
     *
     * SINGLE_BIT checks if a number is power of 2 (aka only has 1 bit set) in a range. The range is
     *      determined by OR-ing all valid flags together. Thus we make sure we don't have conflicts.
     *
     * ASSIGNs (badum-tss) a field in the inner vault to a flag (if that flag is set)
     *      No further checking is done by this MACRO.
     *
     * FIELD_CHECK makes sure that the value in the field isn't 0 therefore making sure that a flag was
     *      actually provided in the end.
     *
     * The order the fields are assigned at makes a difference in case of errors during the single bit
     * checking or future bugs. It makes sure that certain values override others (e.g. verbose logging
     * overrides silent logging)
     *
     **/

    /****** Make sure the VAULT TYPE field is unique ******/
    err = check_flagmask(flags, RDB_FLG_ROOT | RDB_FLG_MULTI_USER | RDB_FLG_MANAGED | RDB_FLG_ANARCHY);
    if(err) {
        printf("Failed to provide a vault type!\n");
        return err;
    }

    /** Set fields with precidence order **/
    ASSIGN(vault->inner->type, RDB_FLG_ROOT);
    ASSIGN(vault->inner->type, RDB_FLG_MULTI_USER);
    ASSIGN(vault->inner->type, RDB_FLG_MANAGED);
    ASSIGN(vault->inner->type, RDB_FLG_ANARCHY);

    /** Check that the field is actually set **/
    FIELD_CHECK(vault->inner->type)

    /****** Make sure the CACHE type field is unique ******/
    err = check_flagmask(flags, RDB_FLG_CACHE_SECURE | RDB_FLG_CACHE_QUICK | RDB_FLG_CACHE_BUFFERED);
    if(err == 0) {

        /** Set fields with precidence order **/
        ASSIGN(vault->inner->cache, RDB_FLG_CACHE_QUICK);
        ASSIGN(vault->inner->cache, RDB_FLG_CACHE_SECURE);
        ASSIGN(vault->inner->cache, RDB_FLG_CACHE_BUFFERED);
    } else {

        vault->inner->cache = RDB_FLAG_DEFAULT_CACHING;
    }

    /** Check that the field is actually set **/
    FIELD_CHECK(vault->inner->cache)

    /****** Make sure the WRITE type field is unique ******/
    err = check_flagmask(flags, RDB_FLG_WRITE_BLOCK | RDB_FLG_WRITE_TREE);
    if(err == 0) {

        /** Set fields with precidence order **/
        ASSIGN(vault->inner->write_m, RDB_FLG_WRITE_BLOCK);
        ASSIGN(vault->inner->write_m, RDB_FLG_WRITE_TREE);
    } else {

        /* If none was provided --> set default value */
        vault->inner->write_m = RDB_FLG_DEFAULT_WRITE_MODE;
    }

    /** Check that the field is actually set **/
    FIELD_CHECK(vault->inner->write_m)

    /****** Make sure the scaling mode field is unique ******/
    err = check_flagmask(flags, RDB_FLG_SCALE_DYNAMIC | RDB_FLG_SCALE_ENDLESS | RDB_FLG_SCALE_FIXED);
    if(err == 0) {

        /** Set fields with precidence order **/
        ASSIGN(vault->inner->scale_t, RDB_FLG_SCALE_DYNAMIC);
        ASSIGN(vault->inner->scale_t, RDB_FLG_SCALE_ENDLESS);
        ASSIGN(vault->inner->scale_t, RDB_FLG_SCALE_FIXED);
    } else {

        /* If none was provided --> set default value */
        vault->inner->scale_t = RDB_FLG_DEFAULT_SCALE;
    }


    /** Check that the field is actually set **/
    FIELD_CHECK(vault->inner->scale_t)

    /****** Make sure the logging level is unique ******/
    err = check_flagmask(flags, RDB_FLG_LOG_VERBOSE | RDB_FLG_LOG_INFO |
                                RDB_FLG_LOG_WARN | RDB_FLG_LOG_ERROR | RDB_FLG_LOG_SILENT);
    if(err == 0) {

        /** Set fields with precidence order **/
        ASSIGN(vault->inner->log_t, RDB_FLG_LOG_SILENT);
        ASSIGN(vault->inner->log_t, RDB_FLG_LOG_ERROR);
        ASSIGN(vault->inner->log_t, RDB_FLG_LOG_WARN);
        ASSIGN(vault->inner->log_t, RDB_FLG_LOG_INFO);
        ASSIGN(vault->inner->log_t, RDB_FLG_LOG_VERBOSE);
    } else {

        /* If none was provided --> set default value */
        vault->inner->log_t = RDB_FLG_DEFAULT_LOGLEVEL;
    }

    FIELD_CHECK(vault->inner->log_t)


    /****** Record mode doesn't need uniqueness - thus we don't check ******/
    ASSIGN(vault->inner->ftr_single, RDB_FLG_FTR_SINGLE_RECORD)
    ASSIGN(vault->inner->ftr_multi, RDB_FLG_FTR_MULTI_RECORD)
    ASSIGN(vault->inner->ftr_tree, RDB_FLG_FTR_RECORD_TREE)


    /****** Optionally switch off some features like LUT, RQL and headers ******/
    ASSIGN(vault->inner->ftr_head_dis, RDB_FLG_FTR_DISABLE_HEADERS)
    ASSIGN(vault->inner->ftr_lut_dis, RDB_FLG_FTR_DISABLE_LUT)
    ASSIGN(vault->inner->ftr_rql_dis, RDB_FLG_FTR_DISABLE_RQL)
    ASSIGN(vault->inner->ftr_perm, RDB_FLG_FTR_PERMISSIVE)


    /****** Choose a RQL syntax version - uniqueness is key again ******/
    err =  check_flagmask(flags, RDB_FLG_RQLSYNTAX_SIMPLE | RDB_FLG_RQLSYNTAX_COMPLEX);
    if(err == 0) {

        ASSIGN(vault->inner->rqlsyn_t, RDB_FLG_RQLSYNTAX_SIMPLE)
        ASSIGN(vault->inner->rqlsyn_t, RDB_FLG_RQLSYNTAX_COMPLEX)
    } else {

        /* If none was provided --> set default value */
        vault->inner->rqlsyn_t = RDB_FLG_DEFAULT_RQLSYNTAX;
    }

    /* Return with success :) */
    return HUGE_SUCCESS;
}

/**
 * A zone is a collection of files that a collection of users have
 * access to. Zones can be created during runtime. Each zone has a numeric
 * ID that makes it unique inside it's own vault.
 *
 * Users can then get bound to zone IDs. By default the ID 0 is taken for
 * the root zone.
 *
 * @param vault
 * @param name
 * @return
 */
rdb_err_t rdb_vlts_addzone(rdb_vault *vault, const char *name)
{
    VAULT_SANE MEMCHECK

    /* If our zones fields are still empty */
    if(vault->inner->zones == NULL) {
        vault->inner->zones = (struct vault_zone**) malloc(sizeof(struct vault_zone) * MINIMUM_BUFFER_SIZE);
        if(vault->inner->zones == NULL) return MALLOC_FAILED;
        vault->inner->zone_s = MINIMUM_BUFFER_SIZE;
        vault->inner->usd_zones = 0;
    }

    /* Potentially increase buffer size for zones */
    if(vault->inner->usd_zones >= vault->inner->zone_s) {
        vault->inner->zone_s += 2;
        vault->inner->zones = realloc(vault->inner->zones, vault->inner->zone_s);
        if(vault->inner->zones == NULL) return MALLOC_FAILED;
    }

    struct vault_zone *zone = malloc(sizeof(struct vault_zone) * 1);
    memset(zone, 0, sizeof(struct vault_zone));

    /* Add the provided name for the zone */
    zone->name = (char*) malloc(sizeof(char) * REAL_STRLEN(name));
    strcpy(zone->name, name);

    /* Add the new zone */
    vault->inner->zones[vault->inner->usd_zones++] = zone;

    return SUCCESS;
}

/**
 * Adds a new user to the vault. The name in the vault must be unique.
 *
 * @param name Unique username
 * @param zones A list of zones to add the user to
 * @return The vault user ID
 */
rdb_err_t rdb_vlts_adduser(rdb_vault *vault, const char *name, long zones)
{
    VAULT_SANE MEMCHECK

    if(vault->inner->users == NULL) {
        vault->inner->users = hashmap_new();
        if(vault->inner->users == NULL) return MALLOC_FAILED;
    }

    // TODO: Change the user index to the UUID
    struct vault_user user;
    int exists = hashmap_get(vault->inner->users, name, (any_t*) &user);

    /* Check if the user already existed */
    if(exists == MAP_OK) {
        printf("User with name %s already exists!\n", name);
        return USER_EXISTS;
    }

    /* We know the user is new - Create a new one */
    struct vault_user *new = malloc(sizeof(struct vault_user));
    memset(new, 0, sizeof(struct vault_user));

    /* Copy over the name */
    new->name = malloc(sizeof(char) * REAL_STRLEN(name));
    strcpy(new->name, name);

    /* Generate a unique-id for our user */
    rdb_uuid_alloc(&new->id);

    /* Set creation time - leave last login empty */
    new->created = time(0);

    return SUCCESS;
}

/**
 * Get the user id for a user in this vault. Will return -1 if the user
 * doesn't exist
 *
 * @param username
 * @return
 */
rdb_uuid rdb_vlts_getuser(rdb_vault *vault, const char *username)
{
    struct vault_user user;

    /* Attempt to retrieve a user from the map and return if it's OK */
    int ret = hashmap_get(vault->inner->users, username, (any_t*) &user);
    return (ret == MAP_OK) ? user : NULL;
}

/**
 * Sets the passphrase for a specific user ID. ID 0 is by default always
 * the ROOT user. This function can only be used to do initial passphrase
 * setup
 *
 * @param user Unique user ID
 * @param passphrase
 */
rdb_err_t rdb_vlts_setlogin(rdb_vault *vault, rdb_uuid *user, const char *passphrase)
{
    VAULT_SANE MEMCHECK

    if(user == NULL) return USER_DOESNT_EXIST;
    if(passphrase == NULL) return SHORT_PASSPHRASE;

    /* Define some fields we may need */
    gcry_md_hd_t md_ctx;
    char *username;

    gcry_error_t err;
    rdb_err_t ret;

    /* Get the username for the salt :) */
    ret = rdb_vlts_getplainuser(vault, user, &username);
    if(ret) {
        printf("Failed to retrieve username from vault!\n");
        return ret;
    }

    /* Get a reference to our user struct in the table */
    struct vault_user ref_user;
    hashmap_get(vault->inner->users, username, (any_t*) &ref_user);
    if(ref_user.id == NULL) return USER_DOESNT_EXIST;

    /* Open a hashing context to secure the passphrase */
    err = gcry_md_open(&md_ctx, PASSWD_HASH_FUNCT, GCRY_MD_FLAG_SECURE);
    if(err) {
        printf("Failed to open digest context!\n");
        return HASHING_FAILED;
    }

    /* Add the data into our context */
    gcry_md_write(md_ctx, username, strlen(username));
    gcry_md_write(md_ctx, SALT_SEPERATOR, strlen(SALT_SEPERATOR)); // Add ":::"
    gcry_md_write(md_ctx, passphrase, strlen(passphrase));

    /* Compute passwrod digest (Tiger2) */
    gcry_md_final(md_ctx);
    unsigned char *pw_digest = gcry_md_read(md_ctx, PASSWD_HASH_FUNCT);
    memcpy(ref_user.passwd, pw_digest, PASSWD_HASH_LEN);

    /* Clean up */
    gcry_md_close(md_ctx);
    free(username);

    return SUCCESS;
}

/**
 * Take currently loaded settings, finalise them and finish setup. Only
 * past this point are changes to the vault final and persistent!
 *
 * This function will throw detailed errors about wrong setup parameters
 */
rdb_err_t rdb_vlts_finalise(rdb_vault *vault)
{
    VAULT_SANE

    /* Make sure that the inner vault actually exists (and might have been configured!) */
    if(vault->inner == NULL) {
        printf("Can't initialise empty inner context! Provide configuration parameters first!\n");
        return INVALID_VAULT;
    }

    return SUCCESS;
}


/*****************************************************************/

rdb_err_t check_flagmask(unsigned long flags, unsigned long mask)
{
    unsigned long x = flags & mask;
    if(x == 0) return MISSING_PARAMETERS;

    if(x != 1) {
        while(((x % 2) == 0) && x > 1) x /= 2;
        if(x != 1) return INVALID_PARAMS;
    }

    return SUCCESS;
}