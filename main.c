/*
 * author DJM
 */
#include "filesym.h"
struct user usernode[USERLEN];
struct fcb fcbs[LEN];
struct index indexs[LEN];
struct diretory *curdir;
struct user curuser;
struct diretory dirchain;
struct group groupnode[GROUPLEN];
char   filenode[2049];
struct command commandchain;
char command_tab[COMMANDLEN][10] = {"ls","ll","open","mkdir","create","delete","cd","pwd","logout","exit"};

int main(){
  signal(SIGINT,save_config);
  printf("##############################\n");
  printf("##                          ##\n");
  printf("##    filesystem v1.0       ##\n");
  printf("##     DJM from GDUT        ##\n");
  printf("##   op87960@gmail.com      ##\n");
  printf("##                          ##\n");
  printf("##############################\n");

  init();
  running();

  return 0;
}
