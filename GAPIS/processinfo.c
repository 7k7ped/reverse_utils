/*
Author : @Rhythm113


*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 8192

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

//#define __NR_android_get_sysinfo 383 //not implemented

//#define __NR_android_get_sysinfo 24

struct android_sysinfo {
    long long firstInstallTime;
    long long lastUpdateTime;
    // Add more fields as needed
};


void printField(char *field, char *value) {
    printf("%-25s: %s\n", field, value);
}

void printProcessInfo(int pid) {
    char path[MAX_BUF];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_BUF];
    while (fgets(line, sizeof(line), file) != NULL) {
        // Customize the fields you want to extract here
        if (strstr(line, "Name:") || strstr(line, "Uid:") || strstr(line, "VmRSS:")) {
            char *field = strtok(line, ":");
            char *value = strtok(NULL, "\n");
            printField(field, value);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pid>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int pid = atoi(argv[1]);
    if (pid <= 0) {
        fprintf(stderr, "Invalid process ID\n");
        return EXIT_FAILURE;
    }

    printProcessInfo(pid);
    

    struct android_sysinfo info;
    ret = syscall(__NR_android_get_sysinfo, );

    if (ret == 0) {
       // printf("firstInstallTime: %lld\n", info);
       printf("lastUpdateTime: %lld\n", info.lastUpdateTime);
        // Add more fields as needed
    } else {
        perror("Error retrieving Android system info");
        return 1;
    }

    return 0;

    return EXIT_SUCCESS;
}
