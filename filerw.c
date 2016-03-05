/*
 * author:lkb
 */
#include "filesym.h"

extern struct user usernode[USERLEN];
extern struct group groupnode[GROUPLEN];
extern struct fcb fcbs[LEN];
extern struct index indexs[LEN];
extern struct diretory *curdir;
extern struct diretory dirchain;
extern char   filenode[2049];
extern struct command commandchain;

int read_file(FILE *fp){
  char ch;
  int count=0;
  // ch = fgetc(fp);
  while(!feof(fp)){
    ch = fgetc(fp);
    if(ch == 32 || ch == 44 || ch == 10 || ch == ':'){
      break;
    }
    count++;
  }
  // if(feof(fp)){
  //   printf("文件读取错误！");
  //   exit(1);
  // }
  fseek(fp,-count-1,SEEK_CUR);
  return count;
}

void read_group(){
  FILE *group = fopen("./group.ini","r");
  if(group == 0){
    printf("文件打开失败");
    exit(1);
  }

  char ch;
  char str[10];
  int index = 0;
  ch = fgetc(group);
  while (ch != -1 && ch != 10) {
    fseek(group,-1,SEEK_CUR);

    fgets(str, read_file(group)+2, group);
    groupnode[index].gid = atoi(str);

    fgets(str, read_file(group)+1, group);
    fseek(group,1,SEEK_CUR);
    bzero(groupnode[index].g_name,strlen(groupnode[index].g_name));
    strcpy(groupnode[index].g_name,str);

    index++;
    ch = fgetc(group);
  }
  fclose(group);

}

void read_user(){
  FILE *user = fopen("./user.ini","r");
  if(user == 0){
    printf("文件打开失败");
    exit(1);
  }
  char ch;
  char str[10];
  int index = 0;
  ch = fgetc(user);
  while (ch != -1 && ch != 10) {
    fseek(user,-1,SEEK_CUR);

    fgets(str, read_file(user)+2, user);
    usernode[index].uid = atoi(str);

    fgets(str, read_file(user)+2, user);
    usernode[index].gid = atoi(str);

    fgets(str, read_file(user)+1, user);
    fseek(user,1,SEEK_CUR);
    bzero(usernode[index].username,strlen(usernode[index].username));
    strcpy(usernode[index].username,str);

    fgets(str, read_file(user)+1, user);
    fseek(user,1,SEEK_CUR);
    bzero(usernode[index].passwd,strlen(usernode[index].passwd));
    strcpy(usernode[index].passwd,str);

    index++;
    ch = fgetc(user);
  }
  fclose(user);
}


void read_fcb(){
  FILE *fcb = fopen("./fcb","rb");
  if(fcb == 0){
    printf("文件打开失败");
    exit(1);
  }
  char ch;
  char str[10];
  int index = 0;
  int i;
  ch = fgetc(fcb);
  while (ch != -1 && ch != 10) {
    fseek(fcb,-1,SEEK_CUR);

    fgets(str, read_file(fcb)+2, fcb);
    index = atoi(str);
    fcbs[index].inode = index;

    fgets(str, read_file(fcb)+2, fcb);
    fcbs[index].f_type = *str;

    fgets(str, read_file(fcb)+2, fcb);
    fcbs[index].uid = atoi(str);

    fgets(str, read_file(fcb)+2, fcb);
    fcbs[index].gid = atoi(str);

    fgets(str, read_file(fcb)+2, fcb);
    fcbs[index].f_size = atoi(str);

    for (i= 0;  i< 10; i++) {
      fgets(str, read_file(fcb)+2, fcb);
      fcbs[index].ext[i] = atoi(str);
    }

    fgets(str, read_file(fcb)+1, fcb);
    bzero(fcbs[index].access,strlen(fcbs[index].access));
    strcpy(fcbs[index].access,str);
    fseek(fcb,1,SEEK_CUR);

    fgets(str, read_file(fcb)+2, fcb);
    fcbs[index].tm.tm_year = atoi(str);
    fgets(str, read_file(fcb)+2, fcb);
    fcbs[index].tm.tm_mon = atoi(str);
    fgets(str, read_file(fcb)+2, fcb);
    fcbs[index].tm.tm_mday = atoi(str);
    fgets(str, read_file(fcb)+2, fcb);
    fcbs[index].tm.tm_hour = atoi(str);
    fgets(str, read_file(fcb)+2, fcb);
    fcbs[index].tm.tm_min = atoi(str);
    fgets(str, read_file(fcb)+2, fcb);
    fcbs[index].tm.tm_sec = atoi(str);

    fcbs[index].flag = 1;

    ch = fgetc(fcb);
  }
  fclose(fcb);
}


void write_fcb() {
  FILE *fcb = fopen("./fcb","wb");
  char str[10];
  int index,i;
  for (index = 0; index < LEN; index++) {
    if(fcbs[index].flag == 1){
      sprintf(str,"%d",fcbs[index].inode);
      fputs(str,fcb);
      fputs(" ",fcb);

      fputs(&fcbs[index].f_type,fcb);
      fputs(" ",fcb);

      sprintf(str,"%d",fcbs[index].uid);
      fputs(str,fcb);
      fputs(" ",fcb);

      sprintf(str,"%d",fcbs[index].gid);
      fputs(str,fcb);
      fputs(" ",fcb);

      sprintf(str,"%d",fcbs[index].f_size);
      fputs(str,fcb);
      fputs(" ",fcb);

      for (i= 0;  i< 10; i++) {
        sprintf(str,"%d",fcbs[index].ext[i]);
        fputs(str,fcb);
        if(i == 9){
          fputs(" ",fcb);
        }else{
          fputs(",",fcb);
        }
      }

      fputs(fcbs[index].access,fcb);
      fputs(" ",fcb);

      sprintf(str,"%d",fcbs[index].tm.tm_year);
      fputs(str,fcb);
      fputs(" ",fcb);
      sprintf(str,"%d",fcbs[index].tm.tm_mon);
      fputs(str,fcb);
      fputs(" ",fcb);
      sprintf(str,"%d",fcbs[index].tm.tm_mday);
      fputs(str,fcb);
      fputs(" ",fcb);
      sprintf(str,"%d",fcbs[index].tm.tm_hour);
      fputs(str,fcb);
      fputs(" ",fcb);
      sprintf(str,"%d",fcbs[index].tm.tm_min);
      fputs(str,fcb);
      fputs(" ",fcb);
      sprintf(str,"%d",fcbs[index].tm.tm_sec);
      fputs(str,fcb);
      fputs(" ",fcb);
    }
  }
  fclose(fcb);
}

void read_inode() {

  FILE *inode = fopen("./inode","rb");
  if(inode == 0){
    printf("文件打开失败");
    exit(1);
  }
  char ch;
  char str[10];
  int index = 0;
  ch = fgetc(inode);
  while (ch != -1 && ch != 10) {
    fseek(inode,-1,SEEK_CUR);

    fgets(str, read_file(inode)+2, inode);
    index = atoi(str);
    indexs[index].inode = index;

    fgets(str, read_file(inode)+1, inode);
    fseek(inode,1,SEEK_CUR);
    bzero(indexs[index].i_name,strlen(indexs[index].i_name));
    strcpy(indexs[index].i_name,str);

    indexs[index].flag = 1;

    ch = fgetc(inode);
  }
  fclose(inode);
}

void write_inode(){
  FILE *inode = fopen("./inode","wb");
  int index;
  char str[10];
  for(index=0 ; index < LEN ; index++){
    if(indexs[index].flag == 1){
      sprintf(str,"%d",indexs[index].inode);
      fputs(str,inode);
      fputs(" ",inode);
      fputs(indexs[index].i_name,inode);
      fputs(" ",inode);
    }
  }
  fclose(inode);
}

void init(){
  memset(filenode,'0',sizeof(filenode));
  if(is_emtpy_file("real")){
    init_filenode();
  }else{
    read_filenode();
  }

  if(!is_emtpy_file("./user.ini")){
    read_user();
  }else{
    printf("user.ini can not open!\nplease check you conf!\n");
    exit(1);
  }

  if(!is_emtpy_file("./group.ini")){
    read_group();
  }else{
    printf("group.ini can not open!\nplease check you conf!\n");
    exit(1);
  }

  if(!is_emtpy_file("./inode")){
    read_inode();
  }else{
    strcpy(indexs[0].i_name,"/");
    indexs[0].inode = 0;
    indexs[0].flag = 1;
  }

  if(!is_emtpy_file("./fcb")){
    read_fcb();
  }else{
    fcbs[0].inode = 0;
    fcbs[0].f_type = 'd';
    fcbs[0].uid = 1;
    fcbs[0].gid = 1;
    // fcbs[0].ext[0] = 0;
    strcpy(fcbs[0].access,"777");
    fcbs[0].tm = get_cur_time();
    fcbs[0].flag = 1;
  }

  strcpy(dirchain.d_name,"/");
  dirchain.next = NULL;
  dirchain.inode = 0;

  memset(commandchain.str,0,sizeof(commandchain.str));
  commandchain.next = NULL;
  commandchain.pre = NULL;

  curdir = &dirchain;
}

void save_config(int i){
  printf("\nsaving configure^^^!\n");
  write_inode();
  write_fcb();
  write_filenode();
  printf("success\nthx!\n");
  exit(0);
}
