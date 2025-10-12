#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>  

pid_t change_gpid_to = 17657;

int main() {

  pid_t pid = syscall(SYS_getpid);
  int result = syscall(SYS_setpgid, pid, change_gpid_to); 

  if (result == 0) {
    printf("gPID successfully changed to %d\n", change_gpid_to);
  } else {
    perror("Error changing gPID");  
  }

  return 0;
}
