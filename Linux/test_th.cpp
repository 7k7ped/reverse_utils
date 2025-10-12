#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/uio.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <dlfcn.h>


using namespace std;



int getPID(const char *PackageName) {
    DIR *dir = opendir("/proc");
    if (dir == nullptr) {
        perror("opendir");
        return 0;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type != DT_DIR)
            continue;
        
        int pid = atoi(entry->d_name);
        if (pid == 0)
            continue;
        
        char cmdline_path[256];
        snprintf(cmdline_path, sizeof(cmdline_path), "/proc/%d/cmdline", pid);

        ifstream cmdline_file(cmdline_path);
        string process_name;
        getline(cmdline_file, process_name, '\0');

        if (strcmp(process_name.c_str(), PackageName) == 0) {
            closedir(dir);
            return pid;
        }
    }

    closedir(dir);
    return 0;
}



uintptr_t get_module_base(const char *module_name, int target_pid) {
    uintptr_t addr = 0;
    char filename[32], buffer[1024];
    snprintf(filename, sizeof(filename), "/proc/%d/maps", target_pid);
    ifstream file(filename);
    if (file.is_open()) {
        while (file.getline(buffer, sizeof(buffer))) {
            if (strstr(buffer, module_name)) {
                sscanf(buffer, "%lx-", &addr);
                break;
            }
        }
        file.close();
    }
    return addr;
}

uintptr_t get_module_end(const char *module_name, int target_pid) {

    uintptr_t temp = 0, addr = 0;
    char filename[32], buffer[1024];
    snprintf(filename, sizeof(filename), "/proc/%d/maps", target_pid);
    ifstream file(filename);
    if (file.is_open()) {
        while (file.getline(buffer, sizeof(buffer))) {
            if (strstr(buffer, module_name)) {
                sscanf(buffer, "%*x-%lx", &addr);
                break;
            }
        }
        file.close();
    }
    return addr;
}




void find_thread_pointers(pid_t pid, unsigned long base, unsigned long end) {
    char mem_path[256];
    snprintf(mem_path, sizeof(mem_path), "/proc/%d/mem", pid);

    FILE* mem_file = fopen(mem_path, "rb");

    if (mem_file == NULL) {
        perror("Error opening /proc/pid/mem");
        return;
    }

    fseek(mem_file, base, SEEK_SET);

    const size_t buffer_size = 1024;
    unsigned char buffer[buffer_size];

    while (base < end) {
        size_t bytes_read = fread(buffer, 1, buffer_size, mem_file);

        for (size_t i = 0; i < bytes_read - sizeof(void*); ++i) {
            void* potential_pointer;
            memcpy(&potential_pointer, buffer + i, sizeof(void*));

            if (potential_pointer >= (void*)base && potential_pointer <= (void*)end) {
                printf("Thread pointer found at address: %p\n", potential_pointer);
            }
        }

        base += buffer_size;
    }

    fclose(mem_file);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <pkg> <lib>\n", argv[0]);
        return 1;
    }

    pid_t pid = getPID(argv[1]);
    unsigned long base = get_module_base(argv[2], pid);
    unsigned long end = get_module_end(argv[3], pid);

    find_thread_pointers(pid, base, end);

    return 0;
}
