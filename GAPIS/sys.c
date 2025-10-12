#include <unistd.h>
#include <sys/syscall.h>
#include "calls.h"
#include <stdio.h>

int main(void) {

 long result = syscall(__NR_getuid);
  if (result == -1) {
    return -1;
  }
  printf("%ld", result);
  return 0;
}
