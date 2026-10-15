// xargs.c
#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  char buf[512];
  char *words[64];
  char *batch_argv[128];
  int maxwords = 64;
  int bufsiz = 512;
  int batch_size = 64;
  int nw = 0;
  int pos = 0;
  int nread;
  int i;

  i = 1;
  if(argc > 1 && strcmp(argv[1], "-n") == 0){
    if(argc < 4){
      fprintf(2, "usage: xargs [-n N] command [arg...]\n");
      exit(1);
    }
    batch_size = atoi(argv[2]);
    if(batch_size < 1 || batch_size > maxwords){
      fprintf(2, "xargs: bad -n value\n");
      exit(1);
    }
    i = 3;
  }
  if(argc <= i){
    fprintf(2, "usage: xargs [-n N] command [arg...]\n");
    exit(1);
  }

  // Store template argument pointers directly (we won't modify them)
  char *template_args[64];
  int template_count = 0;
  for(; i < argc && template_count < 64; i++){
    template_args[template_count++] = argv[i];
  }

  // Read stdin and tokenize in place
  while(pos < bufsiz - 1){
    nread = read(0, &buf[pos], 1);
    if(nread <= 0)
      break;
    if(buf[pos] == ' ' || buf[pos] == '\t' || buf[pos] == '\n')
      buf[pos] = '\0';
    pos++;
  }
  buf[pos] = '\0';

  // Extract word pointers
  for(i = 0; i < pos && nw < maxwords; i++){
    if(buf[i] != '\0' && (i == 0 || buf[i-1] == '\0'))
      words[nw++] = &buf[i];
  }

  // Spawn one child per batch, rebuilding argv fresh each time
  for(i = 0; i < nw; i += batch_size){
    int k;
    int count = nw - i;
    if(count > batch_size)
      count = batch_size;

    // Build argv from scratch for this batch
    for(k = 0; k < template_count; k++)
      batch_argv[k] = template_args[k];
    for(k = 0; k < count; k++)
      batch_argv[template_count + k] = words[i + k];
    batch_argv[template_count + k] = 0;

    int pid = fork();
    if(pid < 0){
      fprintf(2, "xargs: fork failed\n");
      exit(1);
    }
    if(pid == 0){
      exec(batch_argv[0], batch_argv);
      fprintf(2, "exec %s failed\n", batch_argv[0]);
      exit(1);
    }
    wait(0);
  }

  exit(0);
}
