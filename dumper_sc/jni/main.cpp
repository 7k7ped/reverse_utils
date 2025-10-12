/**
 * Runtime Memory monitoring tool (known as Dumper tool)
 * This tool is used to monitor any changes made to Android application's native library
 * Initially is was written in C then I re wrote it for efficiency
 * Though it was built for Android it works perfectly on linux too.
 * To make it work on linux just take direct PID input instead of using getPID() function
 * 
 * 
 * Author : @Rhythm113
 */

 

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>

// to do : add win support
#include <dirent.h>
#include <unistd.h>

using namespace std;

int getPID(const char *pkg){
    DIR *dir = opendir("/proc");
    if(!dir){
        perror("opendir");
        return 0;
    }

    struct dirent *entry;
    while((entry = readdir(dir)) != nullptr){
        if(entry->d_type != DT_DIR)
            continue;

        int pid = atoi(entry->d_name);
        if(pid == 0)
            continue;

        string cmdlinePath = "/proc/" + string(entry->d_name) + "/cmdline";
        ifstream cmdlineFile(cmdlinePath);
        string processName;
        getline(cmdlineFile, processName, '\0');

        if(processName == pkg){
            closedir(dir);
            return pid;
        }
    }

    closedir(dir);
    return 0;
}

pair<uintptr_t, uintptr_t> getModuleRange(const char *moduleName, int pid){
    string mapsPath = "/proc/" + to_string(pid) + "/maps";
    ifstream file(mapsPath);
    string line;
    uintptr_t base = 0, end = 0;

    while(getline(file, line)){
        if(line.find(moduleName) != string::npos){
            sscanf(line.c_str(), "%lx-%lx", &base, &end);
            break;
        }
    }
    
    return {base, end};
}

void monitorMemory(int pid, uintptr_t stAddr, uintptr_t endAddr){
    size_t sizeToMonitor = endAddr - stAddr;
    vector<uint8_t> origMem(sizeToMonitor);

    string memPath = "/proc/" + to_string(pid) + "/mem";  
    int fd = open(memPath.c_str(), O_RDONLY);
    if(fd == -1){
        perror("open");
        return;
    }

    if(pread(fd, origMem.data(), sizeToMonitor, stAddr) == -1){  //USING RAM AS STORAGE 
        perror("pread");
        close(fd);
        return;
    }

    cout << "Monitoring memory changes...\n";

    while(true){
        vector<uint8_t> currMem(sizeToMonitor);
        if(pread(fd, currMem.data(), sizeToMonitor, stAddr) == -1){
            perror("pread");
            break;
        }

        bool detected = false;
        size_t modStart = 0, modEnd = 0;

        for (size_t i = 0; i < sizeToMonitor; ++i){
            if(currMem[i] != origMem[i]){
                if(!detected){
                    modStart = i;
                    detected = true;
                }
                modEnd = i;
                origMem[i] = currMem[i];
            }else if(detected){
                printf("\n0x%lx : ",  modStart);
                for (size_t j = modStart; j <= modEnd; ++j)
                    printf(" %02x", currMem[j]);
                detected = false;
            }
        }

        if(detected){
            printf("\n0x%lx : ",  modStart);
            for (size_t j = modStart; j <= modEnd; ++j)
                printf(" %02x", currMem[j]);
        }

        usleep(1000);  // 1 ms
    }

    close(fd);
}

int main(int argc, char *argv[]){
    if(argc < 3){
        cerr << "Usage: " << argv[0] << " <process> <module>\n";
        return -1;
    }

    cout << "Waiting for the process to start...\n";
    int pid;
    while(!(pid = getPID(argv[1]))){ usleep(500000); }  // Check every 500ms
    cout << "Process started! PID: " << pid << endl;

    pair<uintptr_t, uintptr_t> moduleRange;
    while((moduleRange = getModuleRange(argv[2], pid)).first == 0){ usleep(500000); }
    
    cout << "Module " << argv[2] << " loaded at: 0x" << hex << moduleRange.first << " - 0x" << moduleRange.second << dec << endl;
    
    monitorMemory(pid, moduleRange.first, moduleRange.second);
    return 0;
}
