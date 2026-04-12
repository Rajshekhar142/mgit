#pragma once 
#include<string>

using namespace std;

// the content addressable storage..
// entity is stored as an objec-identified purely
// by its sha1 hash..

class ObjectStore{

    public:
    // Hashes content and returns its SHA1 hex string
    // type is "blob" for files , "trees" for dirs , "commit " for commits

    static string hashObject(const string& content, const string& type = "blob");

    // stoes the object in .mgit/objects/xx/remaining38chars
    static bool storeObject(const string& hash , const string& content , const string& type = "blob");
    
    // read and returns the raw content of an object by hash
    static string readObject(const string& hash);

    // just check if an object already exists (no- restoring needed)
    static bool objectExists(const string& hash);

};