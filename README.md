# filesystem
    《多目录多用户文件系统》(C语言)

项目功能
1、模拟linux系统shell，实现了用户命令解析、补全、历史
2、可以进行文件目录权限控制以及位示图的读写。

执行手册
1.编译与执行
./make
./filesystem
./make clean 卸载

2.数据文件注意事项
用户名密码配置：user.ini
eg：1:1:root:123456 用户id:组id:用户名:密码
用户组配置：group.ini
eg：1:root	组id:组名

