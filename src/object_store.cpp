#include "object_store.h"
#include "sha1.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

namespace fs = filesystem;

// Git object format on disk:
// "<type> <size>\0<content>"
// example : "blob 13\0hello, world!"
// header isn't just meta data .. it's a part of what gets hashed
// That's why a blob and a commit with the same content have different hashes..

// hash Object
// Takes raw content + type , builds the full git-style store string,
// returns the sha1 hex of it
// this is hash function of our hash map .
// content -> key . No manual Id needed . Ever.

string ObjectStore::hashObject(const string& content , const string& type){
    string header = type + " " + to_string(content.size()) + '\0';
    string store = header + content;
    return SHA1::hash(store);
}


// store Object
// write the object ot .mgit/objects/xx/remaining38chars
// first 2 chars of hash = directory name (spread load across 256 dirs)
// remaining 38chars file name..

// this is exactly how real git stores objects on disk..

bool ObjectStore:: storeObject(const string& hash , const string& content , const string& type){
    // don't restore what already exists.. same hash same element.
    if(objectExists(hash)) return true;

    string dir = ".mgit/objects/" + hash.substr(0,2);
    string file = dir + "/" + hash.substr(2);

    fs::create_directories(dir);
    ofstream out(file);

    if(!out){
        cerr<< "error: could not write object" << hash << "\n";
        return false;
    }

    // store with header , exactly like git does
    string header = type + " " + to_string(content.size()) + '\0';
    out << header << content;
    return true;
}

// read Object
// reads raw stored content back from disk by hash.
// strips the header and returns just the original content.

string ObjectStore:: readObject(const string& hash){
    string file = ".mgit/objects" + hash.substr(0,2) + "/" + hash.substr(2);
    ifstream in(file);
    if(!in){
        cerr << "error: Object not found: " << hash << "\n";
        return "";
    }

    // read full stored content
    string stored((istreambuf_iterator<char>(in)), {});

    // strip the header - find the null byte that seperate header from content
    size_t nullPos = stored.find('\0');
    if(nullPos == string::npos) return stored;

    return stored.substr(nullPos+1);
}

bool ObjectStore:: objectExists(const string& hash){
    string file = ".mgit/objects/" + hash.substr(0,2) + "/" + hash.substr(2);
    return fs::exists(file);
}