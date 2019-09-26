#ifndef _CHATROOM_H_
#define _CHATROOM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <unistd.h>  
#include <signal.h>  
#include <pthread.h>  
#include <semaphore.h>  
#include <termios.h>  
#include <time.h>  
#include <sys/stat.h>  
#include <fcntl.h>  

/*********************注册登录**********************************/  
#define REG             1         //注册  
#define LOG             2         //登录  
#define FORGET          3         //忘记密码  
#define EXIT            4         //退出  
#define EXISTING_ACC    5         //账号已存在  
#define LOGGED_ACC      6         //账号已登录  
#define ERROR           7         //账号或密码错误  
#define LOG_SUCCESS     8         //登录成功  
#define REG_SUCCESS     9         //注册成功  
#define offline         10        //下线 
#define CANCELLATION    42        //注销
#define CANCEL_SUCCESS  43        //注销成功
#define CANCEL_FALIED   44        //注销失败
/***************************************************************/  

/******************聊天室功能***********************************/  
#define private_chat    11        //私聊  
#define group_chat      12        //群聊  
#define group_result    13        //群聊接受  
#define change_password 14        //更改密码  
#define online          15        //查看在线人数  
#define expression      16        //表情  
#define unlike          17        //常用语  
#define motto           18        //个性签名  
#define motto_change    19        //更改个性签名  
#define like            20        //点赞  
#define Vip             21        //开会员  
#define Shutup          22        //禁言  
#define lifted          23        //解禁  
#define kick            24        //踢人  
#define file_transfer   25
#define phrases			40

/***************************************************************/  


/****************服务器返回结果*********************************/  
#define VIP_SUCCESS         26          //开会员成功  
#define SHUTUP_SUCCESS      27          //禁言成功  
#define SEND_SUCCESS        28          //发送成功  
#define SEND_FAILED         29          //操作失败
#define KICK_SUCCESS        30          //踢人成功  
#define LIKE_SUCCESS        31          //点赞成功  
#define CHANGE_SUCCESS      32          //更改个性签名成功
#define PSW_SUCCESS         33          //更改密码成功
#define UNLIKE_SUCCESS      34          //unlike成功 

/***************************************************************/  

              
/*****************账户链表操作*********************************/ 

#define ONLINE              40           //上线
#define OFFLINE             41           //下线

#endif

