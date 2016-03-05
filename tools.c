#include "filesym.h"

extern struct fcb fcbs[LEN];
extern struct index indexs[LEN];
extern struct diretory *curdir;
extern struct user curuser;
extern struct user usernode[USERLEN];
extern struct group groupnode[GROUPLEN];
extern char   filenode[2049];
extern char command_tab[COMMANDLEN][10];

int str_screah(char *str1,char *str2){ //在str1中从头匹配str2，找到返回1，否则0
  char *p1 = str1;
  char *p2 = str2;

  while(*p2){
    if(*p1++ != *p2++){
      return 0;
    }
  }
  return 1;
}

int get_command_tab(char *str,int all){
  int i,j;
  if(*str == 0 && all){
    printf("\n");
    for(i=0;i<COMMANDLEN;i++){
      printf("%s\t",command_tab[i]);
    }
    printf("\n");
    printf("[%s@filesystem %s]%s",curuser.username,curdir->d_name,curuser.uid == 1?"#":"$");
    return 0;
  }

  if(*str == 0 && all == 0) return 0;

  
  char *cur = str;
  while(!islower(*cur)) cur++;

  int arrlen = strlen(cur);
  int node = -1;
  int count = 0;
  if(all) printf("\n");
  for(j=0;j<COMMANDLEN;j++){
    if(str_screah(command_tab[j],cur)){
      if(all){
        printf("%s\t",command_tab[j]);
      }else{
        node = j;
      }
      count++;
    }
  }

  if(count == 1){
    int offset = strlen(command_tab[node]) - arrlen;
    while (arrlen--) {
      printf("\b \b");
    }
    strcpy(str,command_tab[node]);
    printf("%s",command_tab[node]);
    return offset;
  }else if(count > 1){
    if(all){
      printf("\n");
      printf("[%s@filesystem %s]%s",curuser.username,curdir->d_name,curuser.uid == 1?"#":"$");
      printf("%s",str);
    }
    return 0;
  }else{
    return -1;
  }
}

int get_file_tab(char *str,int all){
  char *cur = str;
  int i;
  while(!islower(*cur)) cur++;

  while(!(*cur == 32 && (*(cur+1) == 0 || islower(*(cur+1))))) cur++;

  cur++;

  if(*cur == 0 && all){
    printf("\n");
    for(i=0;i<getsize(fcbs[curdir->inode].ext);i++){
      if(fcbs[curdir->inode].ext[i] && indexs[fcbs[curdir->inode].ext[i]].flag)
        printf("%s\t",indexs[fcbs[curdir->inode].ext[i]].i_name);
    }
    printf("\n");
    printf("[%s@filesystem %s]%s",curuser.username,curdir->d_name,curuser.uid == 1?"#":"$");
    printf("%s",str);
    return 0;
  }

  if(*cur == 0 && all == 0) return 0;

  int j;


  int arrlen = strlen(cur);
  int node = -1;
  int count = 0;
  if(all) printf("\n");
  for(j=0;j<getsize(fcbs[curdir->inode].ext);j++){
    if(fcbs[curdir->inode].ext[j] && indexs[fcbs[curdir->inode].ext[j]].flag){
      if(str_screah(indexs[fcbs[curdir->inode].ext[j]].i_name,cur)){
        if(all){
          printf("%s\t",indexs[fcbs[curdir->inode].ext[j]].i_name);
        }else{
          node = j;
        }
        count++;
      }
    }
  }

  if(count == 1){
    int offset = strlen(indexs[fcbs[curdir->inode].ext[node]].i_name) - arrlen;
    while (arrlen--) {
      printf("\b \b");
    }
    strcpy(cur,indexs[fcbs[curdir->inode].ext[node]].i_name);
    printf("%s",indexs[fcbs[curdir->inode].ext[node]].i_name);
    return offset;
  }else if(count > 1){
    if(all){
      printf("\n");
      printf("[%s@filesystem %s]%s",curuser.username,curdir->d_name,curuser.uid == 1?"#":"$");
      printf("%s",str);
    }
    return 0;
  }else{
    return -1;
  }
}

int get_tab(char *str,int all){
  int is_file = 0;

  if(*str == 0){
    return get_command_tab(str,all);
  }

  char *cur = str;

  for(;*cur != 0;cur++){
    if(islower(*cur) && *(cur+1) == 32){
      is_file = 1;
      break;
    }
  }

  if(!is_file){
    return get_command_tab(str,all);
  }else{
    return get_file_tab(str,all);
  }

}

int getch(){
 int c=0;
 struct termios org_opts, new_opts;
 int res=0;
    //-----  store old settings -----------
     res=tcgetattr(STDIN_FILENO, &org_opts);
     assert(res==0);
   //---- set new terminal parms --------
  memcpy(&new_opts, &org_opts, sizeof(new_opts));
  new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
  tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
  c=getchar();
   //------  restore old settings ---------
  res=tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
  assert(res==0);
  return c;
}

void auth_disp(char *access){
  int i;
  for(i = 0; i<strlen(access);i++){
      switch (*(access+i)) {
      case '1': printf("--x");break;
      case '2': printf("-w-");break;
      case '3': printf("-wx");break;
      case '4': printf("r--");break;
      case '5': printf("r-x");break;
      case '6': printf("rw-");break;
      case '7': printf("rwx");break;
    }
  }
  printf("\t");
}

char* user_disp(char *temp,int uid){
  int i;
  for(i=0;i < USERLEN;i++){
    if(usernode[i].uid == uid){
      strcpy(temp,usernode[i].username);
      return temp;
    }
  }
  return "NULL";
}

char* group_disp(char *temp,int gid){
  int i;
  for(i=0;i < GROUPLEN;i++){
    if(groupnode[i].gid == gid){
      strcpy(temp,groupnode[i].g_name);
      return temp;
    }
  }
  return "NULL";
}

void fcb_disp(struct fcb *file,int access,int more){
  int k;
  char temp[10];
  for(k=0; k<sizeof(file->ext)/sizeof(&file->ext[0]) ;k++){
      if(file->ext[k]){
        if(more){
          memset(temp,0,sizeof(temp));
          printf("%c", fcbs[file->ext[k]].f_type == 'f'?'-':fcbs[file->ext[k]].f_type );
          auth_disp(fcbs[file->ext[k]].access);
          // printf("%s%s%s\t",auth_disp(temp,fcbs[file->ext[k]].access[0]),auth_disp(temp,fcbs[file->ext[k]].access[1]),auth_disp(temp,fcbs[file->ext[k]].access[2]));
          printf("%s\t", user_disp(temp,fcbs[file->ext[k]].uid));
          memset(temp,0,sizeof(temp));
          printf("%s\t", group_disp(temp,fcbs[file->ext[k]].gid));
          printf("%d\t", fcbs[file->ext[k]].f_size);
          printf("%d:%d:%d\t", fcbs[file->ext[k]].tm.tm_hour,fcbs[file->ext[k]].tm.tm_min,fcbs[file->ext[k]].tm.tm_sec);
          printf("%s\n", access?indexs[file->ext[k]].i_name:"??????");
        }else{
          printf("%s\t", access?indexs[file->ext[k]].i_name:"??????");
        }
      }
    }
  printf("\n");
}

int is_file_in_curdir(int inode){
  int k;
	for(k=0; k<sizeof(fcbs[0].ext)/sizeof(&fcbs[0].ext[0]) ;k++){
			if(fcbs[curdir->inode].ext[k] == inode){
				return 1;
			}
		}
	return 0;
}

int get_inode_by_name(char *name){
	int index = -1;
  int j;
  for(j=0; j<LEN ;j++){
    if(indexs[j].flag == 0) continue;
    if(!strcmp(indexs[j].i_name,name)){
      index = j;
      break;
    }
  }

	return index;
}

int auth(char op,struct fcb *file){
	if(curuser.uid == 1){
		return 1;
	}
	int acc;
	if(file->uid == curuser.uid){
		acc = 0;
	}else if(file->gid == curuser.gid){
		acc = 1;
	}else acc = 2;
	switch(op){
		case 'r':
						if(file->access[acc] != '1' && file->access[acc] != '2' && file->access[acc] != '3'){
							return 1;
						}else return 0;
		case 'w':
						if(file->access[acc] != '1' && file->access[acc] != '4' && file->access[acc] != '5'){
							return 1;
						}else return 0;
		case 'x':
						if(file->access[acc] != '2' && file->access[acc] != '4' && file->access[acc] != '6'){
							return 1;
						}else return 0;
		default : return 0;break;
	}

}

struct tm get_cur_time(){
  time_t t = time(NULL);
  struct tm *tm;
  tm = localtime(&t);
  tm->tm_year += 1900;
  tm->tm_mon++;
  return *tm;
}

int is_emtpy_file(char *f_name){
  FILE *fp;
  char ch;
  if((fp=fopen(f_name,"r"))==NULL)
  {
    return 1;
  }
  ch=fgetc(fp);
  fp = NULL;
  if(ch==EOF){
    return 1;
  }else{
    return 0;
  }
}
