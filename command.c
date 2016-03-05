#include "filesym.h"

extern struct diretory *curdir;
extern struct user curuser;
extern struct fcb fcbs[LEN];
extern struct index indexs[LEN];
extern struct user usernode[USERLEN];
extern struct diretory dirchain;

int login(char *username,char *passwd){
	int i;
	for(i=0; i < USERLEN; i++){
		if(!strcmp(usernode[i].username,username)){
			if(!strcmp(usernode[i].passwd,passwd)){
				strcpy(curuser.username,usernode[i].username);
				strcpy(curuser.passwd,"******");
				curuser.uid = usernode[i].uid;
				curuser.gid = usernode[i].gid;
        curdir = &dirchain;
        curdir->next = NULL;
				return 0;
			}
		}
	}
	return 1;
}

void cd(char *f_name){
  struct diretory *p = &dirchain;
  if(!strcmp(f_name,"..")){
    if(strcmp(curdir->d_name,"/")){
      while(p->next->next != NULL) p = p->next;
        p->next = NULL;
        curdir = p;
    }else{
      printf("you are in the root dir!\n");
    }
    return;
  }

  int index = get_inode_by_name(f_name);
  if(index == -1){
    printf("diretory did not find!\n");
    return;
  }

  if(!is_file_in_curdir(index)){
    printf("diretory can not find in curdir!\n");
    return;
  }

  if(fcbs[index].f_type != 'd'){
    printf("%s is not a diretory\n",f_name);
    return;
  }
  if(auth('x',&fcbs[index])){
    struct diretory *b;
    // b = (struct diretory*)malloc(sizeof(struct diretory));
    b = getpch(struct diretory);
    strcpy(b->d_name,f_name);
    b->inode = fcbs[index].inode;
    b->next = NULL;
    curdir->next = b;
    curdir = curdir->next;
  }else{
    printf("you have no access to change diretory into this dir!\n");
  }
  return;

}

void pwd(){
  struct diretory *p = &dirchain;
  while (p != NULL) {
    printf("%s",p->d_name);
		if(!strcmp(p->d_name,"/")){

		}else{
			printf("/");
		}
    p = p->next;
  }
  printf("\n");
}

/*************************************************
  Function:       ls
  Description:    ls -- list directory contents
  Calls:          fcb_disp
  Called By:      strat
  Input:          char type
                  value:(n)ormal or (d)etail
  Output:
  Return:
  Others:
*************************************************/

void ls(char type){
  int index= curdir->inode;
  int access = 0;
  if(auth('r',&fcbs[index])){
    access = 1;
  }
  switch (type) {
    case 'n':fcb_disp(&fcbs[index],access,0); break;
    case 'd':fcb_disp(&fcbs[index],access,1); break;
    default: break;
  }
}

void delete_file(char *f_name){
  int k;
  int index = get_inode_by_name(f_name);
  if(index == -1){
    printf("file did not find!\n");
    return;
  }

  if(!is_file_in_curdir(index)){
    printf("file can not find in curdir!\n");
    return;
  }
	if(auth('w',&fcbs[index])){
		fcbs[index].flag = 0;
	  indexs[index].flag = 0;
	  for(k=0; k<getsize(fcbs[index].ext) ;k++){
	    if(fcbs[curdir->inode].ext[k] == index){
	      fcbs[curdir->inode].ext[k] = 0;
	      return;
	    }
	  }
	}else{
		printf("you have no access to write this directory!\n");
	}
}

struct fcb* open_file(char *f_name){
  int index = get_inode_by_name(f_name);
  if(index == -1 ){
    printf("file did not find!\n");
    return NULL;
  }

  if(!is_file_in_curdir(index)){
    printf("file can not find in curdir!\n");
    return NULL;
  }
  return &fcbs[index];
}


void create(char *param1,char *param2){
  mkdir(param1,param2,'f');
}

void mkdir(char *param1,char *param2,char type){
  char name[10];
  char access[4];
  int flag = 0;
  if(atoi(param1)){
    strcpy(access,param1);
    flag++;
  }else{
    strcpy(name,param1);
  }

  if(atoi(param2)){
    strcpy(access,param2);
    flag++;
  }else{
    strcpy(name,param2);
  }

  if(flag != 1){
    printf("param unkown!");
    return;
  }

  int i,k;
  for(i=0; i < LEN; i++){
    if(indexs[i].flag == 0){
      indexs[i].inode = i;
      strcpy(indexs[i].i_name,name);
      indexs[i].flag = 1;

      fcbs[i].inode = i;
      fcbs[i].f_type = type;
      fcbs[i].uid = curuser.uid;
      fcbs[i].gid = curuser.gid;
      for(k=0;k < getsize(fcbs[i].ext);k++){
        fcbs[i].ext[k] = 0;
      }
      strcpy(fcbs[i].access,access);
      fcbs[i].tm = get_cur_time();
      fcbs[i].flag = 1;
      break;
    }
  }
  for(k=0; k < sizeof(fcbs[curdir->inode].ext)/sizeof(&fcbs[curdir->inode].ext[0]) ;k++){
      if(fcbs[curdir->inode].ext[k] == 0){
        fcbs[curdir->inode].ext[k] = i;
        break;
      }
    }
}
