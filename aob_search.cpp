/**
 * simple AOB search tool
 * inspired from Kuroyma Tools utility 
 * 
 * author: Rhythm113
 */


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <sys/uio.h>
#include <unistd.h>
#include <cstring>
#include <cctype>
#include <iomanip>

using namespace std;

#define INRANGE(x, a, b) ((x) >= (a) && (x) <= (b))
#define getBits(x) (INRANGE(x,'0','9') ? ((x) - '0') : ((x & (~0x20)) - 'A' + 0xA))
#define getByte(x) (getBits((x)[0]) << 4 | getBits((x)[1]))


// parser
struct PatternByte {
    uint8_t value;
    bool wildcard;
};

vector<PatternByte> parsePattern(const string& pattern){
    vector<PatternByte> bytes;
    istringstream iss(pattern);
    string byteStr;

    while(iss >> byteStr){
        if (byteStr == "??" || byteStr == "?"){
            bytes.push_back({0x00, true});
        }else{
            bytes.push_back({(uint8_t)getByte(byteStr.c_str()), false});
        }
    }
    return bytes;
}


// combined 
bool getLibraryRange(pid_t pid, const string& libName, uintptr_t& base, uintptr_t& end){
    string mapsPath = "/proc/" + to_string(pid) + "/maps";
    ifstream maps(mapsPath);
    if (!maps.is_open()){
        cerr << "[-] Failed to open " << mapsPath << "\n";
        return false;
    }

    string line;
    while(getline(maps, line)){
        if (line.find(libName) != string::npos){
            istringstream iss(line);
            string addrRange;
            iss >> addrRange;
            size_t dash = addrRange.find('-');
            if (dash == string::npos) continue;
            base = stoull(addrRange.substr(0, dash), nullptr, 16);
            end = stoull(addrRange.substr(dash + 1), nullptr, 16);
            return true;
        }
    }
    return false;
}


//  process_vm_readv
bool readMemory(pid_t pid, uintptr_t address, void* buffer, size_t size){
    struct iovec local_iov = { buffer, size };
    struct iovec remote_iov = { (void*)address, size };
    ssize_t nread = process_vm_readv(pid, &local_iov, 1, &remote_iov, 1, 0);
    return nread == (ssize_t)size;
}


// pattern scan
uintptr_t findPattern(pid_t pid, const string& libName, const string& pattern){
    uintptr_t base = 0, end = 0;
    if (!getLibraryRange(pid, libName, base, end)){
        cerr << "[-] Failed to find " << libName << " in maps.\n";
        return 0;
    }

    size_t regionSize = end - base;
    vector<uint8_t> buffer(regionSize);

    if (!readMemory(pid, base, buffer.data(), regionSize)){
        cerr << "[-] Failed to read memory from target process.\n";
        return 0;
    }

    auto parsedPattern = parsePattern(pattern);
    size_t patSize = parsedPattern.size();

    for (size_t i = 0; i < buffer.size() - patSize; ++i){
        bool found = true;
        for (size_t j = 0; j < patSize; ++j){
            if (!parsedPattern[j].wildcard && buffer[i + j] != parsedPattern[j].value){
                found = false;
                break;
            }
        }
        if (found){
           // cout << base+1 << endl;
            return base + i; 
        }
    }

    return 0;
}


// main Entry
int main(int argc, char* argv[]){
    if (argc < 4){
        cout << "usage: " << argv[0] << " <pid> <libname.so> <pattern>\n";
        cout << "example: " << argv[0] << " 6969 libtersafe.so \"?? 00 AA ?? 00\"\n";
        return 1;
    }

    pid_t pid = atoi(argv[1]);
    string lib = argv[2];
    string pattern = argv[3];

    uintptr_t result = findPattern(pid, lib, pattern);
    if (result){
        cout << "[+] Pattern found at: 0x" << hex << result << "\n";
        return 0;
    }else{
        cout << "[-] Pattern not found.\n";
        return -1;
    }
    
}
