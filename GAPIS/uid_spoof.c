#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <PID>\n", argv[0]);
    return 1;
  }

  pid_t pid = atoi(argv[1]);
  struct passwd* pwd_entry = getpwuid(getuid(pid));

  if (!pwd_entry) {
    perror("getpwuid");
    return 1;
  }

  printf("UID of process %d: %d\n", pid, pwd_entry->pw_uid);
  printf("Username: %s\n", pwd_entry->pw_name);

  return 0;
}
