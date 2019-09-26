#include "UDP_chatroom.h"

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
    int  unlikes;
    int  result;                    //返回操作结果 
    char msg[1024];                 //发送、接收消息    
    char e_s;                       //确认发送的表情
    char p_s;                       //确认发送的常用语  
    char file_name[50];             //文件名
}Info;

typedef struct node
{
    struct sockaddr_in client_addr;
    char name[20];
    char account[20];
	//int  offline_flag;//已下线等于1
    struct node *next;
}Node, *LinkList;
struct sockaddr_in client_addr;

Info SendBuf;
Info RecvBuf;
sqlite3 *ppdb = NULL;           //数据库
LinkList head = NULL;           //在线用户
//LinkList head;
//head->account = 0000;
//head->next    = head;

int sockfd;         
int ret;

/*void register(char tmp_name[20],char tmp_account[20],struct sockaddr_in tmp_client_addr)
{
	LinkList p =(LinkList)malloc(sizeof(Node));
	if (NULL == p)
    {
        printf("Malloc p Failure!\n");
        return;
    }
	p->account = tmp_account;
	p->name    = tmp_name;
	p->client_addr = tmp_client_addr;
	head->next = p;
	p->next    = head;
}*/

//处理注销
void deal_cancellation(struct sockaddr_in temp_addr)
{
	char sql[200] = {0};
	char sql1[200] = {0};
	char **Result = NULL;
    int nRow; //行数
    int nColumn; //列数
    int ret;
	char *errmsg = NULL;
    
    sprintf(sql, "select username, passwd ,account from chatroom where account = '%s' and username = '%s'", RecvBuf.account, RecvBuf.username);
    ret = sqlite3_get_table(ppdb, sql, &Result, &nRow, &nColumn, NULL);
    if (ret != SQLITE_OK)
    {
        perror("sqlite3_get_table_deal_log");
        exit(1);
    }
	if(nRow == 1)
		{
			if(!strcmp(RecvBuf.passwd,Result[4]))
				{				
					sprintf(sql1, "delete from chatroom where account = '%s' and username = '%s'", RecvBuf.account, RecvBuf.username);					
					ret = sqlite3_exec(ppdb, sql1, NULL, NULL, &errmsg);
					if (ret != SQLITE_OK)
                     {
                         perror("sqlite3_delete_error");
                         exit(1);
                     }
					printf("Delete account: %s success.\n",RecvBuf.account);
					SendBuf.result = CANCEL_SUCCESS;
				}
			else 
				{
					printf("Incorrect password.\n");
					SendBuf.result = CANCEL_FALIED;
					//exit(1);
				}
		}
	ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
	if (ret < 0)
	{
		perror("sendto_server_log");
		exit(1);
	}  


}


//处理注册
void deal_reg(struct sockaddr_in temp_addr)                                     
{  
    char sql[200] = {0};     //sql要足够大，不然出现stack smashing detected

	/*char tmp_name[20];
	char tmp_account[20];
	struct sockaddr_in tmp_client_addr;
	tmp_name = RecvBuf.username;
	tmp_account = RecvBuf.account;*/
	//tmp_client_addr = RecvBuf.

    sprintf(sql, "insert into chatroom(username, account, passwd, likes,vip,unlikes) values('%s','%s','%s','%d','%d','%d')",RecvBuf.username, RecvBuf.account, RecvBuf.passwd, RecvBuf.likes,RecvBuf.vip,RecvBuf.unlikes);     
    char *errmsg = NULL;
    ret = sqlite3_exec(ppdb, sql, NULL, NULL, &errmsg);  
    if(ret != SQLITE_OK)  
    {  
        perror("sqlite3_exec_error");
		perror("register_error");
        SendBuf.result = EXISTING_ACC;

        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if (ret < 0)
        {
            perror("sendto_server_log");
            exit(1);
        }
        return;  
    }  

    SendBuf.result = REG_SUCCESS;
    printf("registeation information :\nusername: %s \t account: %s \t password: %s \t likes: %d \t unlikes: %d\n", RecvBuf.username, RecvBuf.account, RecvBuf.passwd, RecvBuf.likes, RecvBuf.unlikes);

    ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
    if (ret < 0)
    {
        perror("sendto_server_register");
        exit(1);
    }              
    bzero(&SendBuf, sizeof(SendBuf));
}

//登入
void deal_log(struct sockaddr_in temp_addr)
{
    char **Result = NULL;
    int nRow; //行数
    int nColumn; //列数
    int ret;
    char sql[500] = {0};

    sprintf(sql, "select username, passwd, likes, vip, moto ,unlikes from chatroom where account = '%s' and passwd = '%s'", RecvBuf.account, RecvBuf.passwd);
    ret = sqlite3_get_table(ppdb, sql, &Result, &nRow, &nColumn, NULL);
    if (ret != SQLITE_OK)
    {
        perror("sqlite3_get_table_deal_log");
        exit(1);
    }

    if(1 == nRow)
    {
        LinkList tmp = head->next;
        while(tmp != head)
        {  
            if(!strcmp(tmp->account,RecvBuf.account))//找到了账户并且已经登陆
            {    
                SendBuf.result = LOGGED_ACC;        //已登入
                printf("%s already login!\n",RecvBuf.username);
				//strcpy(SendBuf.passwd,RecvBuf.passwd);
                ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
                if (ret < 0)
                {
                    perror("sendto_server_log");
                    exit(1);
                }
                return;  
            } 
			//if(tmp->next == NULL)tmp->next = head;
            tmp = tmp->next;  
        }

        SendBuf.result = LOG_SUCCESS;  //登入成功
		printf("%s login success\n",Result[6]);
        LinkList p =(LinkList)malloc(sizeof(Node));
        if(p == NULL)
        {
            printf("malloc error!\n");
            return;
        }  
       // LinkList p;
        strcpy(p->account,RecvBuf.account);
        strcpy(p->name, Result[6]); 
		//p->offline_flag = ONLINE;
        strcpy(SendBuf.username,Result[6]); 
        SendBuf.vip = *(Result[9]) - 48;
        SendBuf.likes = *(Result[8]) - 48 ;
//      strcpy(SendBuf.moto, Result[10]);
		SendBuf.unlikes = *(Result[11]) -48;

        p->client_addr.sin_family = temp_addr.sin_family;
        p->client_addr.sin_port = temp_addr.sin_port;
        p->client_addr.sin_addr.s_addr = temp_addr.sin_addr.s_addr;

        printf("%s online!\n",Result[6]);   
        //printf("Login information:\nusername: %s \t account: %s \t password: %s \t port number:%d\nlikes: %d \t unlikes: %d \n\n",SendBuf.username, p->account, RecvBuf.passwd, p->client_addr.sin_port, SendBuf.likes, SendBuf.unlikes);
		if(SendBuf.vip == 0)
			{
				
				printf("Login information:\nOrdinary username: %s \t account: %s \t password: %s \t port number:%d\nlikes: %d \t unlikes: %d \n\n",SendBuf.username, p->account, RecvBuf.passwd, p->client_addr.sin_port, SendBuf.likes, SendBuf.unlikes);
			}
		else
			{
				printf("Login information:\nVIP username: %s \t account: %s \t password: %s \t port number:%d\nlikes: %d \t unlikes: %d \n\n",SendBuf.username, p->account, RecvBuf.passwd, p->client_addr.sin_port, SendBuf.likes, SendBuf.unlikes);
			}
        //p->next = head->next;
        //head->next = p;    
        tmp = p;
		//tmp->next = head;
		tmp = tmp->next;
    }   

    else
    {
        SendBuf.result = ERROR;
    }
	strcpy(SendBuf.passwd,RecvBuf.passwd);
    ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
    if (ret < 0)
    {
        perror("sendto_server_log");
        exit(1);
    }   
}

//私聊
int deal_private(struct sockaddr_in temp_addr)
{
    int flag = 0;
    LinkList tmp = head->next;
    while(tmp != head)
    {
        if(!strcmp(tmp->name,RecvBuf.to_name))
        {
            flag = 1;
            strcpy(SendBuf.msg, RecvBuf.msg);
            strcpy(SendBuf.from_name,RecvBuf.username);

            SendBuf.result = private_chat;

            ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&tmp->client_addr, sizeof(tmp->client_addr));
            if(ret == -1)
            {
                perror("sendto_pchat");
                exit(1);
            }
            break;
        }
        tmp=tmp->next;
    }

     if(flag)
    {
        SendBuf.result = SEND_SUCCESS;
        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if(ret == -1)
        {
            perror("sendto_success");
            exit(1);
        }       
    }
    else
    {
        SendBuf.result = SEND_FAILED;
        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if(ret == -1)
        {
            perror("sendto_failure");
            exit(1);
        }
    }
}   

//群聊
int deal_group(struct sockaddr_in temp_addr)
{
    int flag = 0;
    LinkList tmp = head->next;

    while (tmp != head)
    {
        if (tmp->client_addr.sin_port != temp_addr.sin_port)
        {
            flag = 1;

            SendBuf.result = group_chat;


            strcpy(SendBuf.from_name, RecvBuf.username);
//          printf("%s\n",SendBuf.from_name);
            strcpy(SendBuf.msg, RecvBuf.msg);

            ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&tmp->client_addr, sizeof(tmp->client_addr));
            if (ret < 0)
            {
                perror("sendto_group_chat");
                exit(1);
            }
        }
        tmp = tmp->next;
    }
    if (flag)
    {
        SendBuf.result = SEND_SUCCESS;
        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if (ret < 0)
        {
            perror("sendto_group_chat_success");
            exit(1);
        }
    }
    else 
    {
        SendBuf.result = SEND_FAILED;
        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if (ret < 0)
        {
            perror("sendto_group_chat_failure");
            exit(1);
        }
    }
}

//查看在线人数
int deal_online(struct sockaddr_in temp_addr)
{
    int i = 0;
    SendBuf.online_num = 0;
    LinkList tmp = head->next;
    while(tmp != head)
    {
        SendBuf.online_num++;
        strcpy(SendBuf.online_name[i],tmp->name);
        i++;
        tmp = tmp->next;
    }
    SendBuf.result = online;

    ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
    if(ret == -1)
    {
        perror("sendto_online");
    }
}

//点赞
int deal_like(struct sockaddr_in temp_addr)
{
    char *errmsg = NULL;  
    char **Result = NULL;  
    int  nRow;  
    int  nColumn;  
    char sql[200];  

    sprintf(sql, "select likes ,unlikes from chatroom where username = '%s'", RecvBuf.to_name);
    ret = sqlite3_get_table(ppdb, sql, &Result, &nRow, &nColumn, &errmsg);  
    if(ret != SQLITE_OK)  
    {  
        printf("select fail:%d(%s)\n", ret, errmsg);  
        return -1;  
    }  

    if(nRow == 1)  //行不算行名
    {   
        sprintf(sql, "update chatroom set likes = %d where username = '%s'", *(Result[2]) - 47, RecvBuf.to_name);  
        ret = sqlite3_exec(ppdb, sql, NULL, NULL, &errmsg);  
        if(ret != SQLITE_OK)  
        {  
            printf("update fail:%d(%s)\n", ret, errmsg);  
            return ;  
        }  

        LinkList tmp = head->next;

        while(tmp != head)              //遍历数据库，找到用户
        {   
            if(strcmp(tmp->name, RecvBuf.to_name) == 0)  
            {  
                SendBuf.likes = *(Result[2]) - 47;  
                SendBuf.result = like;
				SendBuf.unlikes = *(Result[3]) - 48;
                strcpy(SendBuf.from_name,RecvBuf.username);  

                ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&tmp->client_addr, sizeof(tmp->client_addr));
                if(ret == -1)
                {
                    perror("sendto_like");
                }
				printf("update %s likes = %d and unlike = %d\n\n",RecvBuf.to_name, SendBuf.likes, SendBuf.unlikes);
                break;
            }
            tmp = tmp->next;
        }

        SendBuf.result = LIKE_SUCCESS;          
        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if(ret == -1)
        {
            perror("sendto_like_suuess");
        }   
    }
    else  
    {  
        SendBuf.result = SEND_FAILED;  
        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if(ret == -1)
        {
            perror("sendto_like_failed");
        }       
    }
}

//发送表情(群发)
int deal_expression(struct sockaddr_in temp_addr)
{
    int flag = 0;
    LinkList tmp = head->next;
    while (tmp != head)
    {
        if (tmp->client_addr.sin_port != temp_addr.sin_port)
        {
            flag = 1;

            SendBuf.result = expression;
            strcpy(SendBuf.from_name, RecvBuf.username);
            SendBuf.e_s = RecvBuf.e_s;
            ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&tmp->client_addr, sizeof(tmp->client_addr));
            if (ret < 0)
            {
                perror("sendto_face");
                exit(1);
            }
        }
        tmp = tmp->next;
    }
    if (flag)
    {
        SendBuf.result = SEND_SUCCESS;
        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if (ret < 0)
        {
            perror("sendto_face_success");
            exit(1);
        }
    }
    else
    {
        SendBuf.result = SEND_FAILED;
        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if (ret < 0)
        {
            perror("sendto_face_failure");
            exit(1);
        }
    }
}

//unlike
int deal_unlike(struct sockaddr_in temp_addr)
{
   
    char *errmsg = NULL;  
    char **Result = NULL;  
    int  nRow;  
    int  nColumn;  
    char sql[200];  

    sprintf(sql, "select unlikes ,likes from chatroom where username = '%s'", RecvBuf.to_name);
    ret = sqlite3_get_table(ppdb, sql, &Result, &nRow, &nColumn, &errmsg);  
    if(ret != SQLITE_OK)  
    {  
        printf("select fail:%d(%s)\n", ret, errmsg);  
        return -1;  
    }  

    if(nRow == 1)  
    {   
        sprintf(sql, "update chatroom set unlikes = %d where username = '%s'", *(Result[2]) - 47, RecvBuf.to_name);  
        ret = sqlite3_exec(ppdb, sql, NULL, NULL, &errmsg);  
        if(ret != SQLITE_OK)  
        {  
            printf("update fail:%d(%s)\n", ret, errmsg);  
            return ;  
        }

        LinkList tmp = head->next;

        while(tmp != head)              //遍历数据库，找到用户
        {   
            if(strcmp(tmp->name, RecvBuf.to_name) == 0)  
            {  
                SendBuf.unlikes = *(Result[2]) - 48 + 1;  
                SendBuf.result = unlike;
				SendBuf.likes  = *(Result[3]) -48;
                strcpy(SendBuf.from_name,RecvBuf.username);  

                ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&tmp->client_addr, sizeof(tmp->client_addr));
                if(ret == -1)
                {
                    perror("sendto_like");
                }
				printf("update %s likes = %d and unlike = %d\n\n",RecvBuf.to_name, SendBuf.likes, SendBuf.unlikes);
                break;
            }
            tmp = tmp->next;
        }

        SendBuf.result = UNLIKE_SUCCESS;          
        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if(ret == -1)
        {
            perror("sendto_like_suuess");
        }   
    }
    else  
    {  
        SendBuf.result = SEND_FAILED;  
        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if(ret == -1)
        {
            perror("sendto_unlike_failed");
        }       
    }
}   

//注册会员
void deal_vip(struct sockaddr_in temp_addr)
{
    char sql[200] = {0};

    sprintf(sql,"update chatroom set vip= %d where username = '%s';",RecvBuf.vip,RecvBuf.username);

    printf("%s become VIP\n",RecvBuf.username);

    ret = sqlite3_exec(ppdb,sql,NULL,NULL,NULL);
    if(ret!=SQLITE_OK)
    {
        perror("sqlite3_exec_vip");
        return;
    }

    SendBuf.vip = 1;
    SendBuf.result = VIP_SUCCESS;
    strcpy(SendBuf.username , RecvBuf.username);
    ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
    if(ret == -1)
    {
        perror("sendto_vip_failure");
        exit(1);
    }   
}

//改密码
void deal_change_psw(struct sockaddr_in temp_addr)
{
    char sql[200] = {0};

    sprintf(sql,"update chatroom set passwd = '%s' where username = '%s';",RecvBuf.passwd,RecvBuf.username);

    printf("%s changes password,the new password is :%s\n",RecvBuf.username,RecvBuf.passwd);

    ret = sqlite3_exec(ppdb,sql,NULL,NULL,NULL);
    if(ret!=SQLITE_OK)
    {
        perror("sqlite3_exec_change_password_error");
        return ;
    }

    SendBuf.result = PSW_SUCCESS;
	strcpy(SendBuf.passwd, RecvBuf.passwd);
    strcpy(SendBuf.username , RecvBuf.username);
    ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
    if(ret == -1)
    {
        perror("sendto_password_failure");
        exit(1);
    }

   
}

//禁言
int deal_shutup(struct sockaddr_in temp_addr)
{
    LinkList tmp = head->next;
    int flag = 0;
    while(tmp != head)
    {
        if(strcmp(tmp->name,RecvBuf.to_name)==0)
        {
            flag = 1;
            SendBuf.result = Shutup;
            strcpy(SendBuf.from_name,RecvBuf.username);
            ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&tmp->client_addr, sizeof(tmp->client_addr));
            if (ret < 0)
            {
                perror("sendto_deal_shutup");
                return;
            }
        }
        tmp = tmp->next;
    }
    if(flag)
    {
        SendBuf.result = SHUTUP_SUCCESS;
        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if (ret < 0)
        {
            perror("sendto_shutup_success");
            return;
        }
    }
    else
    {
        SendBuf.result = SEND_FAILED;
        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if (ret < 0)
        {
            perror("sendto_shutup_failure");
            return;
        }
    }
  
    
    SendBuf.result = PSW_SUCCESS;
	strcpy(SendBuf.passwd, RecvBuf.passwd);
    strcpy(SendBuf.username , RecvBuf.username);
    ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
    if(ret == -1)
    {
        perror("sendto_password_failure");
        exit(1);
    }
}

//踢人
int deal_kick(struct sockaddr_in temp_addr)//踢人的
{   
    int flag = 0;

    LinkList tmp = head->next;
	//tmp = tmp->next

    while(tmp != head)
    {
        if(strcmp(tmp->next->name, RecvBuf.to_name ) == 0)
        {
            flag = 1;

            SendBuf.result = kick;
            strcpy(SendBuf.from_name,RecvBuf.username);
			strcpy(SendBuf.username,RecvBuf.to_name);
            ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&(tmp->next->client_addr), sizeof(tmp->next->client_addr));
            if (ret < 0)
            {
                perror("sendto_kick");
                return;
            }
          
            LinkList q = tmp->next;
           // LinkList q = tmp;
            tmp->next = q->next;
		   tmp = tmp->next;
            free(q);
			//while(tmp)
        
            printf("%s offline!\n", RecvBuf.to_name );//被踢的

            break;
        }
		//LinkList q = tmp;
        //tmp->next = q->next->next;
        //free(q);
        tmp = tmp->next;
        
    }

    if(flag == 1)
    {
        SendBuf.result = KICK_SUCCESS;
        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if (ret < 0)
        {
            perror("sendto_kick_success");
            exit(1);
        }
    }
    else
    {
        SendBuf.result = SEND_FAILED;
        ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
        if (ret < 0)
        {
            perror("sendto_kick_failed");
            exit(1);
        }
    }
}

//下线
int deal_quit(struct sockaddr_in temp_addr)
{
    LinkList tmp = head;

/*  if(tmp->next->next == head)
    {
        if(strcmp(tmp->next->name, RecvBuf.username ) == 0)
        {
            LinkList p = tmp->next;
            tmp->next = p->next;
            free(p);
            printf("%s 下线！\n", RecvBuf.username );
            return;
        }
        tmp = tmp->next;
    }*/
    printf("name : %s\n", RecvBuf.username);
    while(tmp->next != head)
    {
        if(strcmp(tmp->next->name, RecvBuf.username ) == 0)
        {
            LinkList q = tmp->next;
            tmp->next = q->next;
            free(q);
            printf("%s offline!\n", RecvBuf.username );
            break;
        }
		//while(tmp->next != NULL)
		tmp = tmp->next;
		//tmp->next = head;
    }
	/*
	printf("name : %s offline.\n", RecvBuf.username);
    while(tmp->next != head)
    	{
    		if(strcmp(tmp->next->name, RecvBuf.username ) == 0)
				{
					tmp=tmp->next;
					//tmp->offline_flag = OFFLINE;
					printf("name : %s offline.\n", RecvBuf.username);
					break;
    			}
			tmp = tmp->next;
    	}
	*/

    ret = sendto(sockfd, &SendBuf, sizeof(SendBuf), 0, (struct sockaddr *)&temp_addr, sizeof(temp_addr));
    if(ret == -1)
    {
        perror("sendto_quit");
    }

    return 0;
}


int main()
{
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int length;
    int flag;
    char sql[200] = {0};

    head = (LinkList)malloc(sizeof(Node));
    if (NULL == head)
    {
        printf("Malloc Failure!\n");
        return;
    }
    head->next = head;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);//创建套接字
    if (-1 == sockfd)
    {
        perror("socket");
        exit(1);
    }

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr =htonl(INADDR_ANY);//任意本机地址
    ret = bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        perror("bind_error");
        exit(1);
    }

    ret = sqlite3_open("chatroom.db", &ppdb);//打开数据库
    if (ret != SQLITE_OK)
    {
        perror("sqlite3_open_error");
        exit(1);
    }
    //sprintf(sql, "create table if not exists chatroom (username text, account text primary key, passwd text ,likes integer, vip text, moto text);");

    sprintf(sql, "create table if not exists chatroom(username text, account text primary key, passwd text ,likes char,vip text, moto text,unlikes char);");
    ret = sqlite3_exec(ppdb, sql, NULL, NULL, NULL);//用户名，账号(关键字)，密码，点赞，vip，个性签名,unlike
    if (ret != SQLITE_OK)
    {
        perror("sqlite3_exec1_error");//创建表格1,chatroom
        exit(1);
    }

    while (1)
    {
        bzero(&SendBuf, sizeof(SendBuf));

        length = sizeof(client_addr);
        ret = recvfrom(sockfd, &RecvBuf, sizeof(RecvBuf), 0, (struct sockaddr *)&client_addr, &length);
        if (ret < 0)
        {
            perror("recvfrom_error");
            exit(1);
        }


        switch (RecvBuf.cmd)  
        {  
            case (REG):                          //注册  
            {   
                deal_reg(client_addr);
                break;
            }

            case (LOG):                         //登录
            {
                deal_log(client_addr);
                break;
            }

            case (EXIT):                        //退出
            {
                exit(1);
                break;
            }

            case (private_chat):                //私聊
            {
                deal_private(client_addr);
                break;
            }

            case(group_chat):                  //群聊
            {
                deal_group(client_addr);
                break;
            }

            case(online):                      //查看在线人数
            {
                deal_online(client_addr);
                break;
            }

            case (like):                        //点赞
            {
                deal_like(client_addr);
                break;
            }

            case(expression):                   //群发表情
            {
                deal_expression(client_addr);
                break;
            }

            case(unlike):                      //发送常用语
            {
                deal_unlike(client_addr);
                break;
            }   

            case(Vip):                          //开通会员
            {
                deal_vip(client_addr);
                break;
            }

            case(Shutup):                       //禁言
            {
                deal_shutup(client_addr);
                break;
            }

            case(kick):                         //踢人
            {
                deal_kick(client_addr);
                break;
            }

            case (change_password):               //改密码
            {
                deal_change_psw(client_addr);
                break;
            }

            case(offline):                         //下线
            {
                printf("NAME : %s\n", RecvBuf.username);
                deal_quit(client_addr);
                break;
            }
			case(CANCELLATION):
				printf("%s wants to cancel account.\n",RecvBuf.username);
				deal_cancellation(client_addr);
				break;
        }           
    } 
    sqlite3_close(ppdb);        

    return 0;
}

