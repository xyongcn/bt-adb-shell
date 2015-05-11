#include <stdlib.h>  
#include <sys/types.h>  
#include <stdio.h>  
#include <sys/socket.h>  
#include <netinet/in.h>
// #include <linux/in.h>  
#include <string.h>  
#include <pthread.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

void * solve_socketread(void *p);//对已建立的蓝牙socket进行读取数据
int connection=-1;

int main(int argc,char **arg)
{
	pthread_t readthread;

    //处理终端的输入命令
    char inputbuff[128];
    while(1)
    {
	    
	    //wait for connection
	    if(connection<0)
	    {
			memset(&inputbuff, 0, sizeof(inputbuff));
		    printf("input:");
		    fgets(inputbuff,sizeof(inputbuff),stdin);


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
					continue;
				}

				//通过ble_addr的蓝牙地址，建立相应的蓝牙sockaddr_l2
				struct sockaddr_l2 client_addr;
				memset(&client_addr, 0, sizeof(struct sockaddr_l2));
				client_addr.l2_family = PF_BLUETOOTH;
				str2ba(ble_addr, &client_addr.l2_bdaddr);
				client_addr.l2_psm = htobs(0x1003);
				//client_addr.l2_psm = htobs(0x1001);//这里的0x1003类似于tcp通信的端口，应该是末尾为基数，可设置为0x1005.一个psm只能帮顶一个server socket

				//连接远程的蓝牙服务
				int temp=connect(socketclient, (struct sockaddr*)&client_addr, sizeof(client_addr));
				if(temp<0)
				{
					printf("connect to the BLE server fail");
					continue;
				}
				printf("connect to the BLE server\n");

				//create read thread
				temp=pthread_create(&readthread,NULL,solve_socketread,&socketclient);
				if(temp!=0)
				{
					printf("create thread for BLE read thread fail\n");
					continue ;
				}
				printf("crate thread for BLE read thread\n");
				pthread_detach(readthread);

				connection=socketclient;
			}
			else
			{
				if(!strcmp(cmd,"exit\n"))
				{
					// connection=-1;
					break;
				}
				else
					printf("please write the right cmd: bleconnect (device address)\n");
				/*while(cmd=strtok(NULL," "))
				{
					printf("%s ",cmd);
				}
				printf("\n");*/
			}
    	}
    	//send shell cmd to remote device
		else
		{
			memset(&inputbuff, 0, sizeof(inputbuff));
		    // printf("$ ");
		    fgets(inputbuff,sizeof(inputbuff),stdin);

		    if(!strcmp(inputbuff,"exit\n"))
			{
				connection=-1;
			}
			else
			{
				printf("excute cmd:%s\n",inputbuff);
			    //send shell cmd to remote
			    int t=write(connection,inputbuff,sizeof(inputbuff));
			    if(t<=0)
			    {
			    	printf("send shell cmd failed\n$ ");
			    }
			}
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
			printf("%s",buffer);
			// printf("$ ");
		}
		else if(readnum<0)
		{
			printf("there is something during reading\n");
			connection=-1;
			return ;
		}
		else
		{
			printf("connection is shut down!\n");
			connection=-1;
			return ;
		}
	}
}

