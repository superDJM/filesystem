#include "filesym.h"

extern char filenode[2049];

void read_real(int num){
	FILE *f = fopen("real","r");
	char ch,*str;
	str = (char *)malloc(512);
	memset(str,0,512);
	int i = 0;
// 	if(f == NULL){
// 		printf("open file fail!");
// //		exit(1);//文件打开出错
// 	}
	fseek(f,0,SEEK_SET);  //将文件指针置在文件开头
	fseek(f,(num-1)*512,SEEK_CUR);//将文件指针偏转到对应物理块开头
	ch = fgetc(f);
	while(ch != 0){
		str[i] = ch;
		ch = fgetc(f);
		i++;
	}
	printf("%s",str);
}

int write_real(int num,char *str){  //写物理块，最多2048块，每块512B，前4块为位示图
	int size = strlen(str);
	if(size>511){
		printf("more than 511B!");
		return -1;//内容过大
	}
	FILE *f = fopen("real","r+"); //一定不能用a,用a的话fseek会失效
// 	if(f == NULL){
// 		printf("open file fail!");
// //		exit(1);//文件打开出错
// 	}
	fseek(f,0,SEEK_SET);  //将文件指针置在文件开头
	fseek(f,(num-1)*512,SEEK_CUR);//将文件指针偏转到对应物理块开头
	fwrite(str,size,1,f);  //写入文件
	fputc(0,f);  //写入结束符
	fclose(f);
	return 1;
}

void init_filenode(){
  FILE *f = fopen("real","w");

  filenode[0] = '1';
  filenode[1] = '1';
  filenode[2] = '1';
  filenode[3] = '1';
  fseek(f,0,SEEK_SET);
  fwrite(filenode,sizeof(filenode),1,f);
  fclose(f);

}

void read_filenode(){
	FILE *f = fopen("real","rb");
// 	if(f == NULL){
// 		printf("open file fail!");
// //		exit(1);
// 		return;
// 	}
	fseek(f,0,SEEK_SET);  //将文件指针置在文件开头
	fread(filenode,2048,1,f);//读出位示图
	fclose(f);
}

void write_filenode(){
	FILE *f = fopen("real","r+");
// 	if(f == NULL){
// 		printf("open file fail!");
// //		exit(1);
// 		return;
// 	}
	fseek(f,0,SEEK_SET);  //将文件指针置在文件开头
	fwrite(filenode,2048,1,f);  //写入位示图内容
	fclose(f);
}

void read_block(struct fcb *file){
  	int i;
  	for(i = 0;i<getsize(file->ext);i++){
	    if(file->ext[i]){
	      read_real(file->ext[i]);
	    }else{
	      break;
	    }
	  }
	printf("\n");
}

/*****
void write_block(struct fcb *file,char *contents){
  for(int i = 0;i<getsize(filenode);i++){
    if(filenode[i] == '0'){
      filenode[i] = '1';
      write_real(i+1,contents);
      for(int j=0;j<getsize(file->ext);j++){
        if(file->ext[j] == 0){
          file->ext[j] = i+1;
          break;
        }
      }
      file->f_size += strlen(contents);
      break;
    }
  }
}
*****/


void write_block(struct fcb *file,char *contents){
	int i,k;
	int end,count=0,content_size = strlen(contents),j;
	char ch,temp[300] = {0};
	file->f_size += strlen(contents);
	if(file->ext[0] == 0){                   //文件原来就没内容
		for(i = 0;i<getsize(filenode);i++){
			if(filenode[i] == '0'){
				filenode[i] = '1';
				write_real(i+1,contents);
				file->ext[0] = i+1;
				break;
			}
		}
	}else{
		for(i=1;i<getsize(file->ext);i++){
			if(file->ext[i] == 0){
				end = file->ext[i-1];
				FILE *f = fopen("real","r+");
				fseek(f,0,SEEK_SET);  //将文件指针置在文件开头
				fseek(f,(end-1)*512,SEEK_CUR);
				ch = fgetc(f);
				while(ch){
					count++;    //count为这个物理块已经写了的长度,总长511B
					ch = fgetc(f);
				}

				if(count == 511){         //这个块已经写满了
					fclose(f);
					for(j = 0;j<getsize(filenode);j++){
						if(filenode[j] == '0'){
							filenode[j] = '1';
							write_real(j+1,contents);
							file->ext[i] = j+1;
							break;
						}
					}
					break;
				}
				if((511-count)>=content_size){         //这个物理块剩下的位置够写
					fseek(f,-1,SEEK_CUR);
					fwrite(contents,content_size,1,f);  //写入文件
					fputc(0,f);  //写入结束符
					fclose(f);
				}else{
					fseek(f,-1,SEEK_CUR);
					fwrite(contents,511-count,1,f);  //写入文件
					fputc(0,f);  //写入结束符
					fclose(f);
					for(k =0;k<content_size-(511-count);k++){
						temp[k] = contents[k+(511-count)];
					}
					temp[k] = 0;   //temp为剩下的字符串
					for(j = 0;j<getsize(filenode);j++){
						if(filenode[j] == '0'){
							filenode[j] = '1';
							write_real(j+1,temp);
							file->ext[i] = j+1;
							break;
						}
					}
				}
				write_filenode();
				break;
			}
		}
	}
}
