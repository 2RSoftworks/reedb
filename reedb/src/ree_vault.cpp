#include "ree_vault.h"

#include "reedb/utils/errors.h"
#include <iostream>
#include <cstring>

// Advanced path handling
#include <boost/filesystem.hpp>
#include <wordexp.h>
#include <stddef.h>

// Time, machines and username
#include <unistd.h>
#include <chrono>

// Config handling
#include <libconfig.h++>

// Cryptography includes
#include "crypto/rcry_engine.h"
#include "crypto/rcry_utils.h"
#include <cryptopp/aes.h>

#include <cryptopp/osrng.h>

using CryptoPP::AutoSeededRandomPool;

extern "C" {
#include "reedb/utils/helper.h"
}

using namespace std;
using namespace boost;
using namespace libconfig;

ree_vault::ree_vault(rdb_token *(*token), rdb_uuid *(*uuid), rcry_engine *engine, string name, string path,
                     string passphrase) {
    /* Generate a UUID and initialise the crypto engine with said token */
    uuid_helper uh;
    *uuid = uh.generate();
    this->uuid = **uuid;

    /* Then generate and asign token */
    rdb_tokens_create(token, 0);
    this->token = **(token);

    this->name = name;
    this->path = path;
    this->file_count = 0;

    cout << "Creating a new vault. From scratch" << endl;

    this->headers = new map<string, datafile_h *>();
    this->files = new map<string, datafile *>();
    this->locks = new map<string, bool>();

    /* Now things required for quick header caching */
    this->h_fields = new map<string, void *>();

    /** CREATE DIRECTORIES **/

    wordexp_t expantion;
    wordexp(path.c_str(), &expantion, 0);

    bool created = true;
    string combined = name + ".vault";

    /* Expand our path, add the name as a folder and data dir */
    filesystem::path target(expantion.we_wordv[0]);
    target /= combined.c_str();
    target /= "datastore";
    created = filesystem::create_directories(target);

    if (created) {
        target.remove_leaf() /= "keystore";
        filesystem::create_directories(target);

        target.remove_leaf() /= "checksum";
        filesystem::create_directories(target);

        target.remove_leaf() /= "metadata";
        filesystem::create_directories(target);
        target.remove_leaf();
    } else {
        cout << "Creating vault path went wront!" << endl;
        wordfree(&expantion);
        throw 1;
    }

    target /= "master.cfg";
    string config_path = target.c_str();
    wordfree(&expantion);

    /* Now write our default configuration to disk */
    Config cfg;

    /* Get root setting and add sub-settings */
    Setting &root = cfg.getRoot();

    auto time = std::chrono::system_clock::now();

    char *hostname = new char[128];
    gethostname(hostname, 128);

    char *username = new char[128];
    getlogin_r(username, 128);

    root.add("creation_date", Setting::TypeInt64) = time.time_since_epoch().count();
    root.add("last_updated", Setting::TypeInt64) = time.time_since_epoch().count();
    root.add("creation_machine", Setting::TypeString) = hostname;
    root.add("updating_machine", Setting::TypeString) = hostname;
    root.add("creation_user", Setting::TypeString) = username;
    root.add("updating_user", Setting::TypeString) = username;

    cout << "Writing config to file..." << endl;

    /** Then write our configuration */
    cfg.writeFile(config_path.c_str());

    delete (hostname);
    delete (username);

    /* Initialise the crypto engine for this vault */
    engine->init(*token);
    engine->switch_context(*token);

    cout << "Now encrypting key.." << endl;

    /* Then retrieve the key in encrypted form to write it to disk */
    char *salt;
    string *encrypted_key = engine->get_encrypted_key(salt, *token, &passphrase);
    // get_encrypted_key(char *salt, rdb_token *token, string passphrase)
    // rcry_engine::get_encrypted_key(unsigned char**, std::__cxx11::string&)

    /* Release the crypto engine again */
    engine->switch_context(nullptr);

    /* Write things to disk. Make it official! */

}

ree_vault::ree_vault(rdb_uuid uuid, string path, string passphrase) {

}
