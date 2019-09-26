#include "UDP_chatroom.h"
//#include <sqlite3.h>

#define PORT 8888


typedef struct info
{
    char username[20];              //用户名
    char account[20];               //账号
    char passwd[20];                //密码
    char from_name[20];             //发信人
    char to_name[20];               //收信人
    char moto[30];                  //个性签名
    char online_name[20][20];       //在线人名
    int  online_num;                //在线人数
    int  cmd;                       //提取操作符  
    int  vip;                       //会员标志   
    int  likes;                     //点赞数
    int  unlikes;  					//unlike 
    int  result;                    //返回操作结果 
    char msg[1024];                 //发送、接收消息    
    char e_s;                       //确认发送的表情
    char p_s;                       //确认发送的常用语  
    char file_name[50];             //文件名
}Info;

struct sockaddr_in server_addr;

Info SendBuf;
Info RecvBuf;
sqlite3 *ppdb = NULL;           //数据库

int  sockfd;    
int  State;         
int  LIKES;             //点赞标志
int  UNLIKES;
int  LIKE_UNLIKE;
int  VIP;
int  OUT = 0;           //踢人标志
char Name[20];
int  ret;
char password[20];
/****************************************************************/

//时间函数
void time_show()
{
     time_t rawtime;
     struct tm *timeinfo;
     time (&rawtime);
     timeinfo = localtime(&rawtime);
     //printf("\n\t\tTime: %d year %d month %d day %d time %d minute %d second\n", timeinfo->tm_year + 1900, timeinfo->tm_mon + 1,
                //timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	printf("\n\t\t actual time is: %s",asctime(timeinfo));
}

//防止空格影响
void SCAN_N()  
{  
    char ch;  
    while((getchar()) != '\n' && ch != EOF);  
}  
//上框
void frame()
{
	printf("\n");
	printf("** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **\n");
	printf("** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **\n");
	printf("\n");
}


//欢迎界面
void Welcome(void)
{
    system("clear");  
    frame();
    printf("\t\twelcome to the chat room\n\n");
    printf("\t\twelcome use chat app\n");
	frame();
    sleep(2);
}

//退出界面
void Bye(void)
{
    system("clear");  
    frame();
    printf("\t\tGOODBYE\n");
    printf("\t\tSee  you  lala.\n");
    sleep(2);   
}

//开机界面
void Display()
{
    system("clear");  
    frame();
    printf("\t\twelcome to the chat room\n");
    printf("\t\t1.(REG)\n");
    printf("\t\t2.(LOG)\n");
    printf("\t\t3.(EXIT)\n");   
	printf("\t\t4.(CANCELLATION)\n"); 
    printf("\t\t choose num please\n");
	frame();
}

//注册界面
void RegFrame_ok()
{
    system("clear");
	frame();
    printf("\t\twelcome to registration\n"); 
	sleep(1);
    system("clear");
	frame();
    printf("\t\tbeing registered.\n");
    printf("\t\tplease wait a little later\n");
	frame();
    sleep(1);

}   
void RegFrame_fail()
{
    system("clear");
	frame();
    printf("\t\twelcome to registration\n"); 
	sleep(1);
    system("clear");
	frame();
    printf("\t\tbeing registered.\n\n");
	printf("\t\tSorry,\n");
	printf("\t\tthe account is registered!\n\n");
    printf("\t\tPlease register again.\n\n");
	frame();
    sleep(1);

}  


//登入界面
void LogFrame()
{

    system("clear");
	frame();
    printf("\t\twelcome to enter\n\n"); 
    printf("\t\tplease wait a little later\n");
	frame();
    sleep(1);
    system("clear");
	frame();
    printf("\t\twelcome to enter\n\n"); 
    printf("\t\tSuccessful entry\n\n");
	frame();
    sleep(2);
}

// 登入后的界面 
void InterFrame()
{
    system("clear");
    //printf("\t\t\t\t");
    time_show();
    if(VIP == 1)  
    {  
		frame();
        printf("\t\tDear VIP User: %s                     \n", Name);
        printf("\t\tPersonality signature: %s     \n", RecvBuf.moto);
        printf("\t\tlike: %d                             \n", LIKES);
        printf("\t\tWelcome to Chatroom \n\n");
        printf("\t\t 1. Private chat                \n");//私聊
        printf("\t\t 2. Group chat                  \n");//群发
        printf("\t\t 3. View online number          \n");//查看在线人数
        printf("\t\t 4. Thumbs-up                   \n");//点赞
        printf("\t\t 5. Change signature            \n");//改个性签名
        printf("\t\t 6. Change password             \n");//改密码
        printf("\t\t 7. Group expression            \n");//群发表情
        printf("\t\t 8. unlike                      \n");//unlike
        printf("\t\t 9. View chat logs              \n");//查看聊天记录
        printf("\t\t10. Registered VIP              \n");//注册VIP
        printf("\t\t11. kicking(VIP)                \n");//踢人
        printf("\t\t12. Prohibit speaking(VIP)      \n");//禁言
        printf("\t\t13. Relieve(VIP)                \n");//解禁
        printf("\t\t14. Downline                    \n\n");//下线
        printf("\t\tplease make you choice \n\n");
		frame();
    }  
    else  
    {  
			frame();
			printf("\t\tDear Ordinary User: %s			     \n", Name);
			printf("\t\tPersonality signature: %s	 \n", RecvBuf.moto);
			printf("\t\tlike: %d 							\n", LIKES);
			printf("\t\tWelcome to Chatroom  \n\n"); 
			printf("\t\t 1. Private chat				\n");
			printf("\t\t 2. Group chat				    \n");
			printf("\t\t 3. View online number		    \n");
			printf("\t\t 4. Thumbs-up 				    \n");
			printf("\t\t 5. Change signature		    \n");
			printf("\t\t 6. Change password			    \n");
			printf("\t\t 7. Group expression		    \n");
			printf("\t\t 8. unlike                      \n");
			printf("\t\t 9. View chat logs 			    \n");
			printf("\t\t10. Registered VIP			    \n");
			printf("\t\t11. kicking(VIP)			    \n");
			printf("\t\t12. Prohibit speaking(VIP)	    \n");
			printf("\t\t13. Relieve(VIP)			    \n");
			printf("\t\t14. Downline				    \n\n");
			printf("\t\t please make you choice \n\n");
			frame();
		}  

    
}
//注销
int Cancellation()
{
	system("clear");
	frame();
	printf("\t\tWelcome to close your account.\n");
	frame();
	bzero(&SendBuf, sizeof(SendBuf));
	//char close_name[20];
	//char close_password[20];
	//char close_account[20];
	printf("\t\tPlease enter your account.\n");
	scanf("\t\t%s",SendBuf.account);
	SCAN_N();
	printf("\t\tPlease enter your username.\n");
	scanf("\t\t%s",SendBuf.username);
	SCAN_N();
	printf("\t\tPlease enter your password.\n");
	scanf("\t\t%s",SendBuf.passwd);
	SCAN_N();

	SendBuf.cmd = CANCELLATION;
	//strcpy(SendBuf.username,close_name);
	//strcpy(SendBuf.account ,close_account);
	//strcpy(SendBuf.passwd  ,close_password);
	ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
          if (ret < 0)
                {
                    perror("sendto_Login_error");
                    exit(1);
                }
	//return ;
}

//注册
int RegAcc() 
{
    system("clear");
    frame();
    printf("\t\twelcome to registration\n");   
	frame();

    SendBuf.cmd = REG;
    SendBuf.vip = 0;
	VIP = SendBuf.vip;
    SendBuf.likes = 0;
	SendBuf.unlikes = 0;
    strcpy(SendBuf.moto, "say something about yourself");

    char psd1[20] = {0};
    char psd2[20] = {0};

    printf("\n\n");
    printf("\t\tplease enter username:\n\n");
    scanf("\t\t%s", SendBuf.username);
    //SCAN_N();

    printf("\t\tplesase enter account:\n\n");
    scanf("\t\t%s", SendBuf.account);
    //SCAN_N();

    printf("\t\tplease enter password:\n\n");
    scanf("\t\t%s", psd1);
    //SCAN_N();

    printf("\t\tplease confirm password:\n\n");
    scanf("\t\t%s", psd2);
    //SCAN_N();

    if(strcmp(psd1, psd2) != 0)  
    {  
        system("clear");
		frame();
        printf("\t\tWelcome to registration.\n\n");   
        printf("\t\tPassword inconsistent.\n\n");
        printf("\t\tEnter password again!\n");
        sleep(2);
        scanf("%s",psd2);
    }
    else
    {
        strcpy(SendBuf.passwd, psd1);
    }
}

//登入
int Login()
{
    system("clear");
	frame();
    printf("\t\tWelcome to enter\n\n"); 
    printf("\t\tacount:\n\n");
    printf("\t\tpassword:\n\n");
    SendBuf.cmd = LOG;
    printf("\n\n");
    printf("\t\tplease enter account:\n\n");
    scanf("%s", SendBuf.account);
    SCAN_N();
    printf("\t\tplease enter password\n");
    scanf("%s", password);
    SCAN_N();
	strcpy(SendBuf.passwd,password);
	//printf("password is : %s\n\n",password);
}


//接收服务器反馈
void *Recv_from_server(void *arg)           
{
    int i;
    int length = sizeof(server_addr);

    while(1)
    {
        ret = recvfrom(*(int *)arg, &RecvBuf, sizeof(RecvBuf), 0, (struct sockaddr *)&server_addr, &length);
        if (ret < 0)
        {
            perror("recvfrom");
            exit(1);
        }

        switch(RecvBuf.result)
        {
            case(private_chat)://私聊
            {
                printf("\n\t\tfriend:%s send you amessage:%s\n",RecvBuf.from_name,RecvBuf.msg);
                memset(&RecvBuf.result,0,sizeof(RecvBuf.result));
                break;
            }   

            case(group_chat)://群聊
            {
                printf("\n\t\t%s group chat a message:%s\n",RecvBuf.from_name,RecvBuf.msg);
                memset(&RecvBuf.result,0,sizeof(RecvBuf.result));
                break;
            }

            case(SEND_SUCCESS)://发送成功
            {
                sleep(1);
                printf("\n\n\n\n");
                printf("\t\tSend success\n");
                sleep(1);
                memset(&RecvBuf.result,0,sizeof(RecvBuf.result));
                break;
            }

            case(SEND_FAILED)://发送失败
            {
                sleep(2);
                printf("\n\n\n\n");
                printf("\t\toperation failed...\n");
                sleep(2);
                memset(&RecvBuf.result,0,sizeof(RecvBuf.result));
                break;
            }

            case(online)://在线
            {
                system("clear");
                printf("\n\n\t\tCurrent number of online friends: %d\n",RecvBuf.online_num);
                for(i = 0 ; i < RecvBuf.online_num ; i++)
                {
                    printf("\n\t\t%s\n",RecvBuf.online_name[i]);
                }
                memset(&RecvBuf.result,0,sizeof(RecvBuf.result));
                break;
            }

            case(like)://点赞
            {
                LIKES = RecvBuf.likes;
				LIKE_UNLIKE = LIKES - UNLIKES;
                printf("\n\t\tfriend%s give you a like!\n", RecvBuf.from_name);
				printf("\t\tyou total likes is %d.\n\n",LIKE_UNLIKE);
                sleep(1.5);
                memset(&RecvBuf.result,0,sizeof(RecvBuf.result));
                break;          
            }

			case(unlike)://unlike
            {
                UNLIKES = RecvBuf.unlikes;
				LIKE_UNLIKE = LIKES - UNLIKES;
                printf("\n\t\t\t\tfriend%s give you a unlike!\n", RecvBuf.from_name);
				printf("\t\tyou total likes is %d.\n\n",LIKE_UNLIKE);
                sleep(1.5);
                memset(&RecvBuf.result,0,sizeof(RecvBuf.result));
                break;          
            }

            case(LIKE_SUCCESS)://点赞成功
            {
                sleep(1);
                printf("\n\n\n\n");
                printf("\t\tlike success\n");
                sleep(1);
                memset(&RecvBuf.result,0,sizeof(RecvBuf.result));
                break;
            }

			case(UNLIKE_SUCCESS)://点赞成功
            {
                sleep(1);
                printf("\n\n\n\n");
                printf("\t\tunlike success\n");
                sleep(1);
                memset(&RecvBuf.result,0,sizeof(RecvBuf.result));
                break;
            }

            case(VIP_SUCCESS):
            {
                sleep(1);
                printf("\n\n\n");
                printf("\t\tCongratulations, you have successfully opened VIP!\n");
				VIP = 1;
                sleep(1);
                memset(&RecvBuf.result,0,sizeof(RecvBuf.result));
                break;
            }

            case(Shutup):
            {
                State = Shutup;
                printf("\t\t%sYou are forbidden to speak!\n",RecvBuf.from_name);
                sleep(2.5);
                break;
            }

            case(SHUTUP_SUCCESS):
            {
                sleep(1);
                printf("\n\n\t\tThe success of the ban!\n");
                sleep(2);
                break;
            }   

            case(kick):
            {
                OUT = 1;
                printf("\n\n\t\tYou have been kicked out of the chat room by %s.\n",RecvBuf.from_name);
				printf("\t\tPlease login again!\n");
                sleep(1);
            //  exit(1);
            //    return (void *)ERROR;
                //pthread_exit("Sorry,offline!");
             //   break;
             /*   SendBuf.cmd = offline;
                strcpy(SendBuf.username,RecvBuf.username);
				pringtf("You are offline now.");
                ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
                if (ret < 0)
                {
                    perror("sendto_quit");
                    exit(1);
                }
                sleep(0.5);
               // return offline;
                //return ;
            // return ;*/
            break;
            }   

            case(KICK_SUCCESS):
            {
                sleep(1);
                printf("\n\n\t\tCongratulations! The other side has been successfully kicked off the line!\n");
                sleep(3);
                break;
            }           

            case(expression):
            {
                char er[20];  

                switch (RecvBuf.e_s)  
                {  
                    case '1' :  
                    {  
                        strcpy(er, ":-D");  
                        printf("\t\t\t\t%sThe group made an expression：%s\n", RecvBuf.from_name, er);  
                        break;  
                    }  
                    case '2' :  
                    {  
                        strcpy(er, "＝　＝＃");  
                        printf("\t\t\t\t%sThe group made an expression：%s\n", RecvBuf.from_name, er);  
                        break;  
                    }  
                    case '3' :  
                    {  
                        strcpy(er, "*\(^_^)/*");  
                        printf("\t\t\t\t%sgroup made an expression：%s\n", RecvBuf.from_name, er);  
                        break;  
                    }  
                    case '4' :  
                    {  
                        strcpy(er, "╮(￣▽ ￣)╭");  
                        printf("\t\t\t\t%sThe group made an expression：%s\n", RecvBuf.from_name, er);  
                        break;  
                    }  
                    case '5' :  
                    {  
                        strcpy(er, "づ￣ 3￣)づ");  
                        printf("\t\t\t\t%sThe group made an expression：%s\n", RecvBuf.from_name, er);  
                        break;  
                    }  
                    case '6' :  
                    {  
                        strcpy(er, "(T_T)");  
                        printf("\t\t\t\t%sThe group made an expression：%s\n", RecvBuf.from_name, er);  
                        break;  
                    }  
                    case '7' :  
                    {  
                        strcpy(er, "b（￣▽￣）d");  
                        printf("\t\t\t\t%sThe group made an expression：%s\n", RecvBuf.from_name, er);  
                        break;  
                    }  
                    case '8' :  
                    {  
                        strcpy(er, "(︶︿︶)");   
                        printf("\t\t\t\t%sThe group made an expression：%s\n", RecvBuf.from_name, er);   
                        break;  
                    }  
                    case '9' :  
                    {  
                        strcpy(er, ">3<");   
                        printf("\t\t\t\t%sThe group made an expression：%s\n", RecvBuf.from_name, er);  
                        break;  
                    }  
                    default :  
                    {  
                        system("clear");  
                        printf("\n\n\n\n\t\t\t\tSorry, no such operation！/n");  
                        break;  
                    }  
                }  
                break;
            }

            case(phrases):
            {
                char ph[60];  

                switch (RecvBuf.p_s)  
                {  
                    case '1' :  
                    {     
                        strcpy(ph, " I promise");  
                        printf("\t\t\t\t%sSend you a message:%s\n", RecvBuf.from_name, ph);  
                        break;  
                    }  
                    case '2' :  
                    {  
                        strcpy(ph, "Take care!");   
                        printf("\t\t\t\t%sSend you a message:%s\n", RecvBuf.from_name, ph);  
                        break;  
                    }  
                    case '3' :  
                    {  
                        strcpy(ph, "Be careful!");  
                        printf("\t\t\t\t%sSend you a message:%s\n", RecvBuf.from_name, ph);   
                        break;  
                    }  
                    case '4' :  
                    {  
                        strcpy(ph, "Cheer up!");  
                        printf("\t\t\t\t%sSend you a message:%s\n", RecvBuf.from_name, ph);  
                        break;  
                    }  
                    case '5' :  
                    {  
                        strcpy(ph, "I doubt it");   
                        printf("\t\t\t\t%sSend you a message:%s\n", RecvBuf.from_name, ph);  
                        break;  
                    }  
                    case '6' :  
                    {  
                        strcpy(ph, "Bless you!");  
                        printf("\t\t\t\t%sSend you a message:%s\n", RecvBuf.from_name, ph);  
                        break;  
                    }  
                    case '7' :  
                    {  
                        strcpy(ph, "Keep it up!");  
                        printf("\t\t\t\t%sSend you a message:%s\n", RecvBuf.from_name, ph);   
                        break;  
                    }  
                    case '8' :  
                    {  
                        strcpy(ph, "I'm single!");  
                        printf("\t\t\t\t%sSend you a message:%s\n", RecvBuf.from_name, ph);   
                        break;  
                    }  
                    case '9' :  
                    {  
                        strcpy(ph, "That's all!");  
                        printf("\t\t\t\t%sSend you a message:%s\n", RecvBuf.from_name, ph);  
                        break;  
                    }  
                    default :  
                    {  
                        system("clear");  
                        printf("\n\n\n\n\t\t\t\tSorry, no such operation！\n");  
                        break;  
                    }  
                }  
                break;
            }           
        }
    }
}

//处理登陆后的函数
int after_login()                   
{
    if(OUT == 1) //被踢了 
    {
    	
        OUT--;  
        return offline; //下线
    }   

    int  fd;
    int  choice;
    char MSG[2048] = {0};
    char filename[100] ={0};

    while(OUT != 1)
    {
        InterFrame();

        scanf("%d",&choice);

        switch(choice)
        {
            case 1:                                 //私聊  
            {
                system("clear");  
				if(LIKE_UNLIKE < 0)//当likes比unlikes少
					{
						printf("\n\t\tSorry,you cannot chat with others unless your likes are more than unlikes\n");
						sleep(2);
						break;
					}
                if(State == Shutup)
                {
                    printf("\n\t\tSorry,you have been forbidden to speak!\n");
                    sleep(2);
                    break;
                }

                printf("\n\n");
                printf("\t\tPlease enter a privte chat\n");
                printf("\t\t");
                scanf("%s", SendBuf.to_name);  
                SCAN_N();  

                printf("\t\tPlease enter the message to send\n");
                printf("\t\t");
                scanf("%s", SendBuf.msg);  
                SCAN_N();  

                SendBuf.cmd = private_chat;  

                strcpy(SendBuf.username,Name);

                if(strcmp(SendBuf.to_name, Name) ==0)  //不能给自己发送消息
                {  
                    sleep(1);  
                    system("clear");  
                    printf("\t\tYou cannot send a message to yourself!\n");  
                    sleep(2);
                    break;  
                } 

                ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
                if (ret < 0)
                {
                    perror("sendto_pchat");
                    exit(1);
                }  

                sprintf(filename, "%s chat with %s.txt", Name, SendBuf.to_name);  
                fd = open(filename, O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR);  
                if(fd == -1)  
                {  
                    perror("open");  
                    exit(1);  
                }  

                sprintf(MSG, "%s send s message to %s：%s", Name, SendBuf.to_name, SendBuf.msg);  
                ret = write(fd, MSG, strlen(MSG));  
                if(ret == -1)  
                {  
                    perror("write");  
                    exit(1);  
                }
                printf("\n\n\t\tsending please wait.\n");  
                sleep(2);  

                break;  
            }

            case 2:                                 //群聊
            {
                system("clear");

                if(State == Shutup)
                {
                    printf("\n\n\t\tSorry,you have been forbidden to speak!\n");
                    sleep(2);
                    break;
                }

                printf("\n\n\t\tPlease send a group chat message: \n");
                printf("\t\t");
                scanf("%s", SendBuf.msg);

                SendBuf.cmd = group_chat;

                strcpy(SendBuf.username,Name);
                ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
                if (ret < 0)
                {
                    perror("sendto_groupchat");
                    exit(1);
                }
                printf("\n\n\t\tSending,please wait.\n");
                sleep(2);
                break;
            }

            case 3:                                 //查看在线人数
            {
                system("clear");
                SendBuf.cmd = online;
                ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
                if (ret < 0)
                {
                    perror("sendto_online_num");
                    exit(1);
                }
                sleep(3.5);
                break;
            }

            case 4:                                 //点赞
            {
                system("clear");
                SendBuf.cmd = like; 
                printf("\n\n\n\t\t\t\tWho do you want to like\n");  
                scanf("%s", SendBuf.to_name);  
                SCAN_N();  
                strcpy(SendBuf.username, Name);  

                if(strcmp(Name, SendBuf.to_name) == 0)  
                {  
                    sleep(1);  
                    printf("\n\n\n\t\tCannot like yourself!\n");  
                    sleep(2);
                    break; 
                }  

                ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr,sizeof(server_addr));
                if (ret < 0)
                {
                    perror("sendto_like_error");
                    exit(1);
                }

                printf("\n\n\t\t\t\tPlease wait.\n");
                sleep(2);
                break;
            }

            case 5:                                 //修改个签
            {
                SendBuf.cmd = motto_change;
                printf("\n\n\n\n\t\t\t\tPlease change.\n\n\t\t\t\t");  
                scanf("%s", SendBuf.moto);  
                SCAN_N();       

                strcpy(SendBuf.username, Name);  

                ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr,sizeof(server_addr));
                if (ret < 0)
                {
                    perror("sendto_motto_change");
                    exit(1);
                }   
                printf("\n\n\t\t\t\tPlease wait.\n");  
                sleep(2);  
                break;  
            }               

            case 6:                                 //改密码
            {
					SendBuf.cmd = change_password;
					char yes_no[20];
					char yes[]={"yes"};
					char new_psw[20];
					char old_psw[20];
					system("clear");
					frame();
					printf("\t\tDo you want to change your passwork? yes/no \n\n");
					frame();
					scanf("%s",yes_no);
					if(strcmp(yes_no,yes)==0)
						{
							strcpy(SendBuf.username, Name);
							printf("your name is %s\n",Name);
							SendBuf.cmd = change_password;
							printf("\t\tPlease enter your old password.\n\n");
							scanf("%s",old_psw);
							printf("old password : %s\n",password);
							printf("old password the you enter : %s\n",old_psw);
							if(strcmp(old_psw,password)==0)
								{
									printf("\t\tPlease enter your new password.\n\n");
									scanf("%s",new_psw);
									strcpy(SendBuf.passwd,new_psw);
									ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr,sizeof(server_addr));
									if (ret < 0)
									{
										 perror("sendto_password_change_error");
										 exit(1);
									}	
									printf("\n\n\t\tPlease wait.\n"); 
								}
							else
								{
									system("clear");
									frame();
									printf("Sorry, incorrect old password entered.");
									frame();
									sleep(2);
									break;
								}
						}
					else 
						{
							break;
						}
               
              
            }  

            case 7:                                //发送表情（群发）  
            {  
                system("clear");

                if(State == Shutup)
                {
                    printf("\n\n\t\t\t\tSorry,you are forbidden to speak.Become VIP.\n");//开通会员解禁。  
                    sleep(2);
                    break;
                }

                char e[10]={0};                     //输入表情选项

                SendBuf.cmd = expression;

                printf("\n\n\n\n\t\t\t\t");   
                printf("**************Please choose**************\n\n");//选择表情
                printf("\t\t\t\t1 HAPPY       :-D\n");  
                printf("\t\t\t\t2 ANGRY       ＝　＝＃\n");  
                printf("\t\t\t\t3 FIGHTING   *\(^_^)/*\n");  
                printf("\t\t\t\t4 HELPLESS   ╮(￣▽ ￣)╭\n");  
                printf("\t\t\t\t5 THREW A KISS   づ￣ 3￣)づ\n");  
                printf("\t\t\t\t6 CRY       (T_T)\n");  
                printf("\t\t\t\t7 GREAT b（￣▽￣）d\n");  
                printf("\t\t\t\t8 DISCONTENT       (︶︿︶)\n");  
                printf("\t\t\t\t9 KISS       >3<\n\n");  

                scanf("%s", e);  
                SCAN_N();       

                SendBuf.e_s = e[0];     

                strcpy(SendBuf.username,Name);
                ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
                if (ret < 0)
                {
                    perror("sendto_expression");
                    exit(1);
                }
                printf("\n\n\t\t\t\tSending...\n");
                sleep(2);               
                break; 
            }   

            case 8:                                 //unlike
            { 
                system("clear");
                SendBuf.cmd = unlike; 
                printf("\n\n\n\t\tWho do you want to unlike?\n");  
                scanf("\t\t%s", SendBuf.to_name);  
                SCAN_N();  
                strcpy(SendBuf.username, Name);  

                if(strcmp(Name, SendBuf.to_name) == 0)  
                {  
                    sleep(1);  
                    printf("\n\n\n\t\tCannot unlike yourself!\n");  
                    sleep(2);
                    break; 
                }  

                ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr,sizeof(server_addr));
                if (ret < 0)
                {
                    perror("sendto_unlike_name_error");
                    exit(1);
                }

                printf("\n\n\t\tPlease wait.\n");
                sleep(2);
                break;
            }

            case 9:                                 //查看聊天记录  
            {  
                system("clear");  
                char b[20];  
                char file_name[50];  
                char File[2048];  
                char kl;  
                int fd1;  
                int i = 0;  
                printf("\n\n\n\n\t\tWhich chat records do you want to see?\n"); //聊天记录
                printf("\n\t\t");
                scanf("%s",b);  
                SCAN_N();  
                sprintf(file_name, "%s chat with %s.txt", Name, b);  
                fd1 = open(file_name,O_RDONLY,S_IRUSR | S_IWUSR);  
                if(fd1 == -1)  
                {  
                    system("clear");  
                    printf("\n\n\t\tyou and %s still have not chat records.\n",b);  
                    sleep(2);
                    break;  
                }  

                while(1)  
                {  
                    memset(&kl, 0, sizeof(kl));  
                    ssize_t read_bytes = read(fd1, &kl, sizeof(kl));  
                    if(read_bytes == -1)  
                    {  
                        perror("read_chat_records_error");  
                        return -1;  
                    }  
                    if(read_bytes == 0)  
                    {  
                        break;  
                    }  
                    File[i] = kl;   
                    i++;  
                }  
                File[i] = '\0';  

                printf("\n\t\t%s\n", File);
                sleep (4);
                break;  
            }  

            case 10:                                //开通会员
            {
                system("clear");
				if(VIP == 1)
					{
						printf("\t\tyou are already VIP.\n\n");
						sleep(3);
						break;
					}
                if(VIP == 0)  
                {  
                    char a[2];
                    system("clear");  
                    printf("\n\n\n\t\tPlease pay 200 to become VIP.\n");  
                    printf("\n\t\tSure?(y/n)\n");  
                    printf("\n\t\t");
                    scanf("%s",a);   

                    if(a[0] == 'y')  
                    {  
                        strcpy(SendBuf.username,Name);  
                        SendBuf.vip = 1;  
                        SendBuf.cmd = Vip;  

                        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
                        if (ret < 0)
                        {
                            perror("sendto_vip");
                            exit(1);
                        }  
                        printf("\n\n\t\tPlease wait.\n");  
                        sleep(2);  
                    }
                    else if(a[0] == 'n')
                    {
                        break;
                    }
                    else
                    {
                        printf("\n\n\t\tInput eror.\n\n");
                        sleep(2);
                    }
                }  
                else if(SendBuf.vip == 1)
                {
                    printf("\n\n\t\tYou are VIP now.\n");  
                    sleep(2);
                }
                break;  
            }

            case 11:                                //踢人
            {
                if(VIP == 1)
                {
                    system("clear");
                    printf("\n\n\t\tPlease enter who you want to kick.\n");
                    printf("\n\t\t");
                    scanf("%s",SendBuf.to_name);

                    SendBuf.cmd = kick;

                    strcpy(SendBuf.username, Name);

                     if(strcmp(SendBuf.to_name, Name) ==0)  
                    {          
                        printf("\n\t\tCannot be yourself.\n");  
                        sleep(2);
                        break;  
                    } 

                    ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
                    if (ret < 0)
                    {
                        perror("sendto_kick1");
                        exit(1);
                    } 
                    printf("\n\n\t\tPlease wait.\n");  
                    sleep(2);
                }

                else 
                {
                    printf("\n\n\t\tYou are not VIP,cannot kick.");
                    sleep(2);
                }

                break;
            }

            case 12:                                //禁言
            {
                if(SendBuf.vip == 1)
                {
                    system("clear");
                    printf("\n\n\n\n\t\tPlease enter the username you want to forbid talking.\n");
                    printf("\t\t");
                    scanf("%s",SendBuf.to_name);

                    SendBuf.cmd = Shutup;

                    strcpy(SendBuf.username,Name);

                    if(strcmp(SendBuf.to_name, Name) ==0)  
                    {  
                        sleep(1);  
                        system("clear");  
                        printf("\n\n\n\n\t\tSorry,cannot forbid yourself.\n");  
                        sleep(2);
                        break;  
                    } 

                    ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
                    if (ret < 0)
                    {
                        perror("sendto_shutup");
                        exit(1);
                    } 
                    printf("\n\t\t\t\tPlease wait.\n");  
                    sleep(2);
                }

                else if(SendBuf.vip == 0)
                {
                    printf("Sorry,you are not VIP,cannot forbid other users talking.\n");
                    sleep(2);
                }

                break;
            }

            case 13:                                //解禁  
            { 
                char x[2];
                if(State == 0)  
                {  
                    system("clear");  
                    printf("\n\n\n\t\tYou are not be forbidden.\n"); //您没有被禁言, 无需解禁！
                    sleep(2);
                }  
                else if(State == Shutup)  
                {  
                    if(SendBuf.vip == 0)  
                    {  
                        system("clear");  
                        printf("\n\n\n\n\t\tPlease become VIP,before lift the ban.\n");//您还没有开通会员，解禁请先开通会员!
                        sleep(2);                       
                    }  
                    else if(SendBuf.vip == 1)  
                    {  
                        system("clear");  
                        printf("\n\n\n\n\t\tDo you want to lift the ban now?(y/n)\n");  
                        scanf("%s",x);  
                        SCAN_N();  

                        if(x[0] == 'y')  
                        {  
                            State = 0;  
                            system("clear");  
                            printf("\n\n\n\t\tCongratulations,it is a relief.\n");//解禁成功。  
                            sleep(2);
                        }  
                    }  
                }  
                break;  
            }  

            case 14:                                //下线
            {
                SendBuf.cmd = offline;
                strcpy(SendBuf.username,Name);
                ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
                if (ret < 0)
                {
                    perror("sendto_quit");
                    exit(1);
                }
                sleep(0.5);
                //return offline;
                return ;
            }

            default:
            {
                system("clear");  
                printf("\n\n\n\n");  
                printf("\n\n\t\tPlease enter again.\n");  
                sleep(1.5);  
                break;
            }
        }
    }
}


int main()
{
    int choice;
    int length = sizeof(server_addr);
    pthread_t tid;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);//创建套接字
    if (-1 == sockfd)
    {
        perror("sockt_error");
        exit(1);
    }

    bzero(&server_addr, sizeof(server_addr));//清空地址结构
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr =htonl(INADDR_ANY);
    Welcome();  

    while(1)  
    {  
        Display(); //开机画面 

        scanf("%d",&choice);

        switch(choice)
        {  
            case 1:               //注册  
            {  
                RegAcc();

                ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
                if (ret < 0)
                {
                    perror("sendto_register_error");
                    exit(1);
                }
                bzero(&SendBuf, sizeof(SendBuf));

                ret = recvfrom(sockfd, &RecvBuf, sizeof(RecvBuf), 0, (struct sockaddr *)&server_addr, &length);                 
                if (ret < 0)
                {
                    perror("recvfrom_register_error");
                    exit(1);
                }

                if(RecvBuf.result == REG_SUCCESS)  //注册成功
                {  
                    RegFrame_ok();
                }  

                else if(RecvBuf.result == EXISTING_ACC)  //注册失败
                {  
                 
                   RegFrame_fail();
                } 
                sleep(2);
                break;               
            }

            case 2:               //登录
            {
                Login();
				//OUT = 0;

                ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
                if (ret < 0)
                {
                    perror("sendto_Login_error");
                    exit(1);
                }


                bzero(&SendBuf, sizeof(SendBuf));

                ret = recvfrom(sockfd, &RecvBuf, sizeof(RecvBuf), 0, (struct sockaddr *)&server_addr, &length);
                if (ret < 0)
                {
                    perror("recvfrom_login_error");
                    exit(1);
                }
				VIP = RecvBuf.vip;
                strcpy(Name,RecvBuf.username);
				strcpy(password,RecvBuf.passwd);
				printf("login password is : %s\n\n",RecvBuf.passwd);
                if(RecvBuf.result == LOG_SUCCESS)//登录成功
                {
                	printf("login success");
                    LIKES = RecvBuf.likes; 
					VIP = RecvBuf.vip;
                    LogFrame();              

                    ret = pthread_create(&tid, NULL, (void *)Recv_from_server, (void *)&sockfd);//创建线程 
                    if (ret < 0)
                    {
                        perror("pthread_create_error");
                        exit(1);
                    }
                    pthread_detach(tid);//设为分离，线程结束释放资源

                    ret = after_login();
                    if(ret == offline)
                    {
                    	printf("Sorry,you can't login.");
                        break;
                    }   
                }

                else if(RecvBuf.result == LOGGED_ACC)
                {
                    system("clear");
                    frame();
                    printf("\t\tWelcome to register\n\n");   
                    printf("\t\tSorry,\n");
                    printf("\t\tthe account is logged in\n\n");
                    printf("\t\tPlease login again.\n");   
                    sleep(2);                       
                }

                else if(RecvBuf.result == ERROR)
                {
                    system("clear");
                    frame();
                    printf("\t\tWelcome to log in\n"); 
                    printf("\t\tSorry,\n");
                    printf("\t\tthe account or password you entered is incorrect.\n\n");
                    printf("\t\tPlease enter again.\n");       
                    sleep(2);
                }               
                break;  
            }

            case 3:               //退出
            {   
                Bye();  
                system("clear");  
                exit(1);  
                break;
            }

			case 4:
			{
				system("clear");
				char yes_no[20];
				char yes[]={"yes"};
				printf("\t\tAre you sure to close your account? yes/no \n");
				scanf("%s",yes_no);
				if(strcmp(yes_no,yes) == 0)
					{
						Cancellation();
						ret = recvfrom(sockfd, &RecvBuf, sizeof(RecvBuf), 0, (struct sockaddr *)&server_addr, &length);
          				if (ret < 0)
                		{
                   			 perror("recvfrom_login_error");
                   			 exit(1);
                		}
						if(RecvBuf.result ==CANCEL_SUCCESS)
							{
								printf("\t\tclose account success.");
								sleep(2);
							}
						else
							{
								printf("\t\tclose account failed.");
								sleep(2);
							}
						break;
					}
				else 
					{
						printf("\t\tYou will return to the previous step.\n");
						sleep(2);
						break;
					}
			}

            default : 
            {       
                system("clear");  
                printf("\n\n\n\n");  
                printf("\n\n\t\tSorry,enter again.\n");  
                sleep(2);  
                break;
            }
        }
    }       
    return 0;
}

