#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int mask;
  int i;

  if(argc < 3){
    fprintf(2, "usage: trace <mask> <cmd> [args...]\n");
    exit(1);
  }

  mask = atoi(argv[1]);

  if(trace(mask) < 0){
    fprintf(2, "trace(%d) failed\n", mask);
    exit(1);
  }

  // Shift argv so argv[0] is the command, argv[1..] are its args
  for(i = 2; i < argc; i++)
    argv[i-2] = argv[i];
  argv[argc-2] = 0;  // Null terminate

  exec(argv[0], argv);  // argv[0] should now be "grep", not "trace"
  fprintf(2, "exec %s failed\n", argv[0]);
  exit(1);
}
