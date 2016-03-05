#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>

#define getpch(type) (type*)malloc(sizeof(type))
#define getsize(arr) sizeof(arr)/sizeof(&arr[0])

#define LEN 100 //数组长度
#define MAXBLOCK 2048 //最大的物理块数
#define BLOCKSIZE 512 //块大小
#define USERLEN 20  //用户数量
#define GROUPLEN 20 // 用户组数量
#define OPLEN 3 //指令长度
#define COMMANDLEN 10 //命令数目

struct command{
  char str[100];  //命令内容
  struct command *pre;
  struct command *next;
};

struct diretory{
  int inode;
  char d_name[10];  //目录名
  struct diretory *next;
};

struct group{
  char  g_name[10];  //用户组名
  int   gid;  //用户组id
};

struct user{
  int   gid;  //文件所属组id
  int   uid;  //文件主id
  char  username[10]; //用户名
  char  passwd[8]; //密码
};

struct index{ //索引目录
  int   inode; //inode节点号
  char  i_name[10];  //文件名（最大10个字符）
  int   flag;
};

struct fcb{ //文件控制块
  int   inode; //inode节点号
  char  f_type;  //文件类型（f／d）d：目录，f：文件
  int   uid; //文件主id
  int   gid; //文件所属组id
  int   f_size; //文件大小 单位B
  int   ext[10]; //物理块号数组 文件最大为5120B
  char  access[3];  //权限数组集 eg: 775
  struct tm tm; //修改时间
  int   flag; //启用标志
};

/******************************
        filerw.c
******************************/
void read_inode();
void write_inode();
int read_file(FILE *fp);
void read_fcb();
void write_fcb();
void read_user();
void write_user();
void init();
void save_config(int i);
void read_group();

/******************************
        fileblockrw.c
******************************/
void read_block(struct fcb *file);
void write_block(struct fcb *file,char *contents);
void read_real(int num);
void write_filenode();
void read_filenode();
void init_filenode();

/******************************
        explan.c
******************************/
int login(char *username,char *passwd);
void start();
void running();
int get_param();
void file_start(struct fcb *file);

/******************************
        command.c
******************************/
struct fcb* open_file(char *f_name);
void write_file(char *str);
void close_file();
void delete_file(char *f_name);
void mkdir(char *param1,char *param2,char type);
void create(char *param1,char *param2);
void cd(char *f_name);
void ls(char type);
void logout();
void pwd();

/******************************
        tools.c
******************************/
int str_screah(char *str1,char *str2);
int get_inode_by_name(char *name);
int is_file_in_curdir(int inode);
int is_emtpy_file(char *f_name);
int auth(char op,struct fcb *file);
struct tm get_cur_time();
void fcb_disp(struct fcb *file,int access,int more);
void auth_disp(char *access);
char* user_disp(char *temp,int uid);
char* group_disp(char *temp,int gid);
int getch();
int get_command_tab(char *str,int all);
int get_file_tab(char *str,int all);
int get_tab(char *str,int all);
