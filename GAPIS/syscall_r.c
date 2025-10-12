#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <pid>\n", argv[0]);
    return 1;
  }

  // Get process ID from argument
  pid_t pid = atoi(argv[1]);

  // Open syscall file
  char filename[50];
  snprintf(filename, sizeof(filename), "/proc/%d/syscall", pid);
  FILE *file = fopen(filename, "r");
  if (!file) {
    printf("Error opening syscall file for PID %d: %s\n", pid, strerror(errno));
    return 1;
  }

  // Read system call number
  long syscall_nr;
  if (fscanf(file, "%ld", &syscall_nr) != 1) {
    printf("Error reading system call number for PID %d\n", pid);
    fclose(file);
    return 1;
  }

  // Print system call number
  printf("System call number: %ld\n", syscall_nr);

  // Read argument registers and stack pointer (6 registers + SP)
  long registers[7];
  for (int i = 0; i < 7; i++) {
    if (fscanf(file, "%lx", &registers[i]) != 1) {
      printf("Error reading register %d for PID %d\n", i, pid);
      fclose(file);
      return 1;
    }
  }

  // Close file
  fclose(file);

  // Print register addresses
  for (int i = 0; i < 7; i++) {
    printf("Register %d address: 0x%lx\n", i, registers[i]);
  }

  return 0;
}
