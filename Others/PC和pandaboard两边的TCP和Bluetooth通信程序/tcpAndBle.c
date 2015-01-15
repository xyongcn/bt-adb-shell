#include <stdlib.h>  
#include <sys/types.h>  
#include <stdio.h>  
#include <sys/socket.h>  
#include <netinet/in.h>
//如果是编译pandaboard端执行程序，需要加上下面的头文件
//#include <linux/in.h>  
#include <string.h>  
#include <pthread.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

void * solve_socketread(void *p);
void * solve_server(void *p);
void * solve_bleServerThread(void *p);//the bluetooth server thread 

int main(int argc,char **arg)
{
	if(argc<2)
	{
		printf("no port num\n");
		return -1;
	}
	
	//tcp socket server prot num
	int portnum=atoi(*(arg+1));

	// pthread_t socketreader;
	pthread_t serverthread;
	// bluetooth server thread;
	pthread_t blethread;

	int socketserver,socketclient=-1;//server num and accept socket num

	struct sockaddr_in server_addr,remote_addr;

	if(-1==(socketserver=socket(AF_INET, SOCK_STREAM, 0)))
	{
		printf("server socket fail\n");
		return -1;
	}

	bzero(&server_addr,sizeof(struct sockaddr_in));  
    server_addr.sin_family=AF_INET;  
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY); /* 这里地址使用全0，即所有 */  
    server_addr.sin_port=htons(portnum); 
    /* 使用bind进行绑定端口 */  
    if(-1 == bind(socketserver,(struct sockaddr *)(&server_addr), sizeof(struct sockaddr)))  
    {  
        printf("bind fail !\n");  
        return -1;  
    } 
    //listen
    if(-1 == listen(socketserver,5))  
    {  
        printf("listen fail !\r\n");  
        return -1;  
    }  

    printf("socketserver is ready!port num:%d\n",portnum);

    	/*struct sockaddr_in accept_addr;
    	int size=sizeof(struct sockaddr_in);
    	int socketaccept=accept(socketserver, (struct sockaddr *)(&accept_addr), &size);
    	if(-1==socketaccept)
    	{
    		printf("accept socket fail\n");
    		return ;
    	}*/

//create thread for tcp server
    if(0!=pthread_create(&serverthread,NULL,solve_server,&socketserver))
    {
    	printf("create thread for accepting cmd input fail\n");
    	return -1;
    }
    pthread_detach(serverthread);

//create thread for bluetooth server
    if(0!=pthread_create(&blethread,NULL,solve_bleServerThread,NULL))
    {
    	printf("create thread for BLE server thread fail!\n");
    	return -1;
    }
    printf("create thread for accepting BLE connection!\n");


    //solve cmd input
    char inputbuff[128];
    while(1)
    {
	memset(&inputbuff, 0, sizeof(inputbuff));
    	printf("input:");
    	gets(inputbuff);
    	// scanf("%[^\n]",inputbuff);
    	// fflush(stdin);
    	if(socketclient>0)
    	{
			if(!strcmp(inputbuff,"exit"))
	    	{
	    		close(socketclient);
	    		socketclient=-1;
	    	}
	    	write(socketclient,inputbuff,strlen(inputbuff));
    	}
    	else
    	{
	    	if(!strcmp(inputbuff,"exit"))
	    	{
	    		close(socketserver);
	    		break;
	    	}

	    	char *cmd=strtok(inputbuff," ");
	    	//TCP connection 
	    	if(!strcmp(cmd,"connect"))
	    	{
	    		bzero(&remote_addr,sizeof(struct sockaddr_in));  
				remote_addr.sin_family=AF_INET;  
				cmd=strtok(NULL," ");
				printf("connect to the remote device ,ipaddress:%s",cmd);
				remote_addr.sin_addr.s_addr= inet_addr(cmd); /* ip转换为4字节整形，使用时需要根据服务端ip进行更改 */  
				cmd=strtok(NULL," ");
				printf("port num:%s\n",cmd);
				int num=atoi(cmd);
				remote_addr.sin_port=htons(num); 

				// printf("s_addr = %#x ,port : %#x\r\n",remote_addr.sin_addr.s_addr,remote_addr.sin_port);
				if(-1==(socketclient=socket(AF_INET, SOCK_STREAM, 0)))
				{
					printf("create client socket fail!\n");
					return -1;
				}

				if(-1 == connect(socketclient,(struct sockaddr *)(&remote_addr), sizeof(struct sockaddr)))  
			    {  
			        printf("connect to the remote device fail !\n");  
			        return -1;  
			    } 
			    printf("connect to the remote TCP device\n") ;
	    	}
	    	else if(!strcmp(cmd,"bleconnect"))
	    	{
	    		char *ble_addr=strtok(NULL," ");
	    		if(!ble_addr)
	    		{
	    			printf("input correct bluetooth address\n");
	    			continue;
	    		}

				socketclient= socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
				if(socketclient < 0)
				{
					printf("create BLE client socket fail\n");
					return -1;
				}

				struct sockaddr_l2 client_addr;
				memset(&client_addr, 0, sizeof(struct sockaddr_l2));
				client_addr.l2_family = PF_BLUETOOTH;
				str2ba(ble_addr, &client_addr.l2_bdaddr);
				client_addr.l2_psm = htobs(0x1003);

				int temp=connect(socketclient, (struct sockaddr*)&client_addr, sizeof(client_addr));
				if(temp<0)
				{
					printf("connect to the BLE server fail");
					return -1;
				}
				printf("connect to the BLE server\n");

	    	}
	    	else
	    	{
	    		printf("%s ",cmd);
	    		while(cmd=strtok(NULL," "))
	    		{
	    			printf("%s ",cmd);
	    		}
	    		printf("\n");
	    	}
	    }
	    

    	// printf("%s\n",inputbuff);

    }

}

void * solve_socketread(void *p)
{
	int socket=(int)(*(int *)p);
	char buffer[1024];
	int readnum;

	while(1)
	{
		readnum=read(socket,buffer,sizeof(buffer));
		if(readnum>0)
		{
			printf("\nreaceive msg:%s\n",buffer);
			printf("input:");
		}
		else if(readnum<0)
		{
			printf("there is something during reading\n");
			continue;
		}
		else
		{
			printf("connection is shut down!\n");
			return ;
		}
	}
}

void * solve_server(void *p)
{
	int socketserver,socketaccept;
	int size;
	struct sockaddr_in accept_addr;
	pthread_t socketreader;

	socketserver=(int)(*(int *)p);

	printf("wait for TCP client connect!\n");
	// wait for socket accept
    while(1)
    {
    	size=sizeof(struct sockaddr_in);
    	socketaccept=accept(socketserver, (struct sockaddr *)(&accept_addr), &size);
    	if(-1==socketaccept)
    	{
    		printf("accept socket fail\n");
    		return ;
    	}
    	printf("accept an TCP connection from %#x : %#x\n",ntohl(accept_addr.sin_addr.s_addr),ntohs(accept_addr.sin_port));
    	printf("accept an TCP connection ");

    	//create thread to solve accept socket
    	if(0!=pthread_create(&socketreader,NULL,solve_socketread,&socketaccept))
    	{
    		printf("create thread to solve accept socket fail\n");
    		continue;
    	}
    	pthread_detach(socketreader);
    }

}

void * solve_bleServerThread(void *p)
{
	int temp;//the value return from function
	int server_socket,remote_socket;
	struct sockaddr_l2 server_addr;
	struct sockaddr_l2 remote_addr;
	pthread_t readthread;//BLE read thread

	server_socket = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);  //发送数据，使用SOCK_SEQPACKET为好
	if(server_socket < 0)
	{
		printf("create BLE socket fail\n");
		return ;
	}

	//bind
	server_addr.l2_family = PF_BLUETOOTH;
	server_addr.l2_psm = htobs(0x1003);
	bacpy(&server_addr.l2_bdaddr, BDADDR_ANY);
	temp = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if(temp < 0)
	{
		printf("bind BLE server socket fail!\n");
		return ;
	}

	//listen
	temp = listen(server_socket, 10);
	if(temp < 0)
	{
		printf("regesiter BLE socket listen fail\n");
		return ;
	}

	int len = sizeof(struct sockaddr_l2);
	printf("wait for BLE connection\n");
	while(1)
	{
		memset(&remote_addr, 0, len);
		remote_socket = accept(server_socket, (struct sockaddr*)(&remote_addr), &len);
		if(remote_socket<0)
		{
			printf("accept BLE connection fail\n");
			continue;
		}
		printf("accept BLE connection\n");

		temp=pthread_create(&readthread,NULL,solve_socketread,&remote_socket);
		if(temp!=0)
		{
			printf("create thread for BLE read thread fail\n");
			continue ;
		}
		printf("crate thread for BLE read thread\n");
		pthread_detach(readthread);
	}
}
