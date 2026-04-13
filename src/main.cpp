#include <iostream>
#include <fstream>
#include <filesystem>
#include "object_store.h"
using namespace std;

namespace fs = filesystem;

// ----------------------------
// init 
// Creats the .mgit directory structure from scratch..
// just like "git init" - sets up the skeleton mgit need to work..

void init(){
    if(fs::exists(".mgit")){
        cout << "already Initialized\n";
        return;
    }

    fs::create_directories(".mgit/objects");
    fs::create_directories(".mgit/refs/heads");

    // HEAD Points to the current branch
    // On init , no commits yet - just points to main by default
    ofstream head(".mgit/HEAD");
    head << "ref: refs/heads/main\n";

    cout << "Initialized empty mgit repository\n";
}

// ____________________________
// hash Object
// Reads a file , hashes its content and stores it as a blob object .
// print 40 char SHA1 so you can reference it later.

void hashObject(const string& filepath){
    ifstream in(filepath);
    if(!in){
        cerr<< "Error: file not found: " << filepath << "\n";
        return;
    }
    // read entire file into string
    string content((istreambuf_iterator<char>(in)),{});

    string hash = ObjectStore::hashObject(content, "blob");
    ObjectStore::storeObject(hash, content, "blob");

    cout << hash << "\n";
}

// ________________________
// Given a hash , reads the object back from disk and prints content.
// This is how git cat-file works - just content-addressed lookup.
// __________________________________

void catFile(const string& hash){
    string content = ObjectStore::readObject(hash);
    if(!content.empty()){
        cout << content;
    }
}

// _______________________________________
// Simple command dispatcher - reads argv and calls the right function
// We'll keep adding commands here as we build each phase.
// __________________________________________

int main(int argc , char* argv[]){
    if(argc < 2){
        cout << "usage : mgit<command>[args]\n";
        cout << "Commands: init , hash-object , cat-file\n";
        return 1;
    }
    string command = argv[1];
    if(command == "init"){
        init();
    }
    else if(command == "hash-object"){
        if(argc < 3){
            cerr << "usage: mgit hash-object <file>\n";
            return 1;
        }
        hashObject(argv[2]);
    }
    else if (command == "cat-file"){
        if(argc < 3){
            cerr << "usage: mgit cat-file <hash> \n";
            return 1;
        }
        catFile(argv[2]);
    }
    else{
        cerr << "unknown command: " << command << "\n";
        return 1;
    }
    return 0;
}