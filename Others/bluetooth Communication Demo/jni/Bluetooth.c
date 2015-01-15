#include <stdlib.h>  
#include <sys/types.h>  
#include <stdio.h>  
#include <sys/socket.h>  
#include <netinet/in.h>
#include <linux/in.h>  
#include <string.h>  
#include <pthread.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

void * solve_socketread(void *p);//对已建立的蓝牙socket进行读取数据
void * solve_bleServerThread(void *p);//建立蓝牙的服务端

int main(int argc,char **arg)
{
	pthread_t blethread;

	//建立蓝牙服务端，循环等待客户端的连接
    if(0!=pthread_create(&blethread,NULL,solve_bleServerThread,NULL))
    {
    	printf("create thread for BLE server thread fail!\n");
    	return -1;
    }
    printf("create thread for accepting BLE connection!\n");


    //处理终端的输入命令
    char inputbuff[128];
    while(1)
    {
		memset(&inputbuff, 0, sizeof(inputbuff));
	    printf("input:");
	    gets(inputbuff);
	    
		char *cmd=strtok(inputbuff," ");
		if(!strcmp(cmd,"bleconnect"))
		{
			char *ble_addr=strtok(NULL," ");
			if(!ble_addr)
			{
				printf("input correct bluetooth address\n");
				continue;
			}

			//建立连接的蓝牙socket
			int socketclient= socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
			if(socketclient < 0)
			{
				printf("create BLE client socket fail\n");
				return -1;
			}

			//通过ble_addr的蓝牙地址，建立相应的蓝牙sockaddr_l2
			struct sockaddr_l2 client_addr;
			memset(&client_addr, 0, sizeof(struct sockaddr_l2));
			client_addr.l2_family = PF_BLUETOOTH;
			str2ba(ble_addr, &client_addr.l2_bdaddr);
			client_addr.l2_psm = htobs(0x1003);//这里的0x1003类似于tcp通信的端口，应该是末尾为基数，可设置为0x1005.一个psm只能帮顶一个server socket

			//连接远程的蓝牙服务
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

}

//对已建立的蓝牙socket进行读取数据
void * solve_socketread(void *p)
{
	int socket=(int)(*(int *)p);
	char buffer[1024];
	int readnum;

	while(1)
	{
		//对蓝牙socket的读写与tcp的类似，也和文件读写方式相同。
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


//建立蓝牙的服务端
void * solve_bleServerThread(void *p)
{
	int temp;//the value return from function
	int server_socket,remote_socket;
	struct sockaddr_l2 server_addr;
	struct sockaddr_l2 remote_addr;
	pthread_t readthread;//BLE read thread

	//创建蓝牙的服务socket
	server_socket = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);  //发送数据，使用SOCK_SEQPACKET为好
	if(server_socket < 0)
	{
		printf("create BLE socket fail\n");
		return ;
	}

	//将server socket绑定到相应的psm
	server_addr.l2_family = PF_BLUETOOTH;
	server_addr.l2_psm = htobs(0x1003);//psm类似于tcp中的端口，一个psm只能绑定一个server socket
	bacpy(&server_addr.l2_bdaddr, BDADDR_ANY);
	temp = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if(temp < 0)
	{
		printf("bind BLE server socket fail!\n");
		return ;
	}

	//设定server socket监听
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
		//蓝牙的server socket接受到一个终端的链接，remote_socket就是这个链接的socket标识
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
