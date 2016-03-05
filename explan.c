
#include "filesym.h"

char op_arr[OPLEN][LEN];
extern struct diretory *curdir;
extern struct user curuser;
extern struct index indexs[LEN];
extern struct command commandchain;

int get_param(){
	char temp[LEN];
  memset(temp,0,sizeof(temp));
  int j = 0;
  int k = 0;
  // fgets(temp,sizeof(temp),stdin);

	// unsigned short ch;
  //   while (1) {
  //       ch=getch();
  //       if (27==ch) continue;//按Esc键退出
  //       if (0==ch||0xe0==ch) ch|=getch()<<8;//非字符键
  //       printf("%04x pressed.\n",ch);
  //   }

	struct command *p = &commandchain;
	int a = 0;	//arr's index
	char c;	//recevice form stdin
	int flag = 0;//tab
	while (1) {
		c = getch();
		if(flag && c != 9) flag = 0;
		if(c == 10){
			temp[a] = 10;
			break;
		}else if(c == 127){
			if(a == 0){
				continue;
			}
			temp[--a] = '\0';
			printf("\b \b");
		}else if(c == 27){
			getch();
			c = getch();
			if(c == 65){	//arrow up
				if(p->next == NULL) continue;
				p = p->next;
				while(a--){
					printf("\b \b");
				}
				strcpy(temp,p->str);
				printf("%s",p->str);
				a = strlen(temp);
			}else if(c == 66){	//arrow down
				if(p->pre ==NULL) continue;
					p = p->pre;
					while(a--){
						printf("\b \b");
					}
					strcpy(temp,p->str);
					printf("%s",p->str);
					a = strlen(temp);
			}
			continue;
		}else if(c == 9){	//tab
			int offset = 0;
			if(flag){
				get_tab(temp,1);
				flag = 0;
				continue;
			}else{
				offset = get_tab(temp,0);
			}
			if(offset > 0){
				a += offset;
				flag = 0;
			}else if(offset == 0){
				flag = 1;
			}else{
				continue;
			}
		}else{
			temp[a++] = c;
			printf("%c",c);
		}

	}

	printf("\n");

	if( temp[0] != 10 ){
		p = &commandchain;
		struct command *new = getpch(struct command);

		strncpy(new->str,temp,strlen(temp)-1);
		new->pre = p;
		new->next = p->next;
		if(p->next != NULL) 	p->next->pre = new;

		p->next = new;
	}

  for(int i=0;temp[i] != 0;i++){
    if(temp[i] == 32 || temp[i] == 10 ){
      if(temp[i-1] != 32 && i != 0){
        strncpy(op_arr[k],&temp[j],i-j);
        k++;
      }
      j = i+1;
    }else{
      if(k == OPLEN){
				k++; break;
			}
		}
  }
	return k;
}


void file_start(struct fcb *file){
	while (1) {
		memset(op_arr,0,sizeof(op_arr));
		int op_len = 0;
		printf("%s>", indexs[file->inode].i_name);

		op_len = get_param();
		if(op_len == 0){
			printf("please input ur command!\n");
			continue;
		}
		if(op_len > OPLEN){
			printf("wrong command len!\n");
			continue;
		}

		if(op_len == 1){
			if(!strcmp(op_arr[0],"read")){
				if(auth('r',file)){
					read_block(file);
				}else{
					printf("you have no access to read this file!\n");
				}

			}else if(!strcmp(op_arr[0],"close")){
				file = NULL;
				return;
			}else{
				printf("unkown command!\n");
			}
		}

		if(op_len == 2){
			if(!strcmp(op_arr[0],"write")){
				if(auth('w',file)){
					write_block(file,op_arr[1]);
				}else{
					printf("you have no access to write this file!\n");
				}
			}else{
				printf("unkown command!\n");
			}
		}
	}
}

void start(){
	printf("welcome to DJM's filesystem!\n");
	while(1){
		memset(op_arr,0,sizeof(op_arr));
		int op_len = 0;
		printf("[%s@filesystem %s]%s",curuser.username,curdir->d_name,curuser.uid == 1?"#":"$");
		op_len = get_param();
		if(op_len == 0){
			printf("please input ur command!\n");
			continue;
		}
		if(op_len > OPLEN){
			printf("wrong command len!\n");
			continue;
		}

		if(op_len == 1){
			if(!strcmp(op_arr[0],"ls")){
				ls('n');
			}else if(!strcmp(op_arr[0],"ll")){
				ls('d');
			}else if(!strcmp(op_arr[0],"logout")){
				printf("bye!\n");
				return;
			}else if(!strcmp(op_arr[0],"pwd")){
				pwd();
			}else if(!strcmp(op_arr[0],"exit")){
				printf("bye!\n");
				return;
			}else{
				printf("unkown command!\n");
			}
		}

		if(op_len == 2){
			if(!strcmp(op_arr[0],"cd")){
				cd(op_arr[1]);
			}else if(!strcmp(op_arr[0],"open")){
				struct fcb *f;
				if((f = open_file(op_arr[1])) != NULL){
					file_start(f);
				}else{
					printf("%s can not find!\n",op_arr[1]);
				}
			}else if(!strcmp(op_arr[0],"delete")){
					delete_file(op_arr[1]);
			}else{
				printf("unkown command!\n");
			}
		}
		if(op_len == 3){
			if(!strcmp(op_arr[0],"create")){
				create(op_arr[1],op_arr[2]);
			}else if(!strcmp(op_arr[0],"mkdir")){
				mkdir(op_arr[1],op_arr[2],'d');
			}else{
				printf("unkown command!\n");
			}
		}
	}
}

void running(){
	while(1){
		int i;
		char op[10];
		scanf("%s",op);
		// gets(op,10,stdin);
		if(!strcmp(op,"login")){
			char username[10];
			char passwd[10];
			memset(username,0,sizeof(username));
			memset(passwd,0,sizeof(passwd));
			printf("username:");
			scanf("%s",username);
			getchar();
			printf("passwd:");
			i = 0;
		  while (1) {
		   	passwd[i]=getch();
		   if(passwd[i]=='\n' || i == sizeof(passwd)){
		    	passwd[i]='\0';
		    	break;
		   }
			 if(passwd[i]==127){
				 if( i != 0 ){
					 printf("\b \b");
					 passwd[i--]='\0';
				 }
					continue;
		   }
			 i++;
		  //  else printf("*");
		  //  if(i<0) passwd[0]='\0';
		  }
			printf("\n");
			if(!login(username,passwd)){
				system("clear");
				start();
			}else{
				printf("login failed!\n");
			}
		}else if(!strcmp(op,"exit")){
			save_config(1);
			exit(0);
		}else{
			printf("unkown command!\n");
		}
	}
}
