#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <unistd.h>

//#include <linux/in.h>
#include <string.h>
#include <pthread.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>


typedef struct socketAndStream
{
	int *socket;
	FILE *readstream;
	FILE *logstream;
}socket_and_stream;

void * read_shell_result_thread(socket_and_stream *p);

void * solve_socketread(void *p);
void * solve_server(void *p);
void * solve_bleServerThread(void *p); //the bluetooth server thread

void * socket_handler(void *p);

//global
int input[2], output[2];
int readsocket=-1;
pid_t pid;
pid_t mpid;
pid_t fpid;
FILE* writestream;
FILE *readstream;
FILE *logstream;

int main(int argc, char **arg) {

	// bluetooth server thread;
	pthread_t blethread;
	pthread_t sh_thread;
	pthread_t child;

	pipe(input);
	pipe(output);	

//create thread for bluetooth server

	pid=fork();
	if(pid<0)
	{
		printf("create child proc failed\n");
		return -1;
	}
	else if(pid==0)
	{
		//child
		setvbuf(stdout, NULL, _IONBF, 0);
		close(input[1]);
		close(output[0]);

		dup2(input[0],STDIN_FILENO);
		dup2(output[1],STDOUT_FILENO);

		system("/bin/sh");//android is /system/bin/sh
		// execl("/bin/sh",NULL);

		//close(input[0]);
		//close(output[1]);
		// close(file);
		
		return 0;
	} else { 
		//father
		close(input[0]);//father write input
		close(output[1]);//father read output
		writestream = fdopen (input[1], "w");
		readstream=fdopen(output[0],"r");
		logstream=fopen("BluetoothShellLog.txt","a+");//the log file

		if(readstream==NULL) printf("error1111111\n");
		if(logstream==NULL) printf("error2222222\n");
		
		socket_and_stream *ss=(socket_and_stream *)malloc((unsigned int)sizeof(socket_and_stream));
		ss->readstream=readstream;
		ss->logstream=logstream;

		if (0 != pthread_create(&blethread, NULL, solve_bleServerThread, NULL)) {
			printf("create thread for BLE server thread fail!\n");
			return -1;
		}
		printf("create thread for accepting BLE connection!\n");
		

		read_shell_result_thread(ss);

	}
	

}

void * solve_socketread(void *p) {
	int socket = (int) (*(int *) p);
	char buffer[1024];
	int readnum;

	while (1) {
		readnum = read(socket, buffer, sizeof(buffer));
		if (readnum > 0) {
			printf("\nreaceive msg:%s\n", buffer);
			printf("input:");
		} else if (readnum < 0) {
			printf("there is something during reading\n");
			continue;
		} else {
			printf("connection is shut down!\n");
			return;
		}
	}
}

void * solve_server(void *p) {
	int socketserver, socketaccept;
	int size;
	struct sockaddr_in accept_addr;
	pthread_t socketreader;

	socketserver = (int) (*(int *) p);

	printf("wait for TCP client connect!\n");
	// wait for socket accept
	while (1) {
		size = sizeof(struct sockaddr_in);
		socketaccept = accept(socketserver, (struct sockaddr *) (&accept_addr),
				&size);
		if (-1 == socketaccept) {
			printf("accept socket fail\n");
			return;
		}
		printf("accept an TCP connection from %#x : %#x\n",
				ntohl(accept_addr.sin_addr.s_addr),
				ntohs(accept_addr.sin_port));
		printf("accept an TCP connection ");

		//create thread to solve accepted socket
		if (0 != pthread_create(&socketreader, NULL, solve_socketread,
						&socketaccept)) {
			printf("create thread to solve accepted socket fail\n");
			continue;
		}
		pthread_detach(socketreader);
	}

}

void * solve_bleServerThread(void *p) {

	
	close(input[0]);//father write input
	close(output[1]);//father read output


	char buffer[512];
	// char result[512];
	// int len=sizeof(buffer);
	fflush(stdout);

	//----------------
	//从bluetooth读取操作命令
	int temp;//the value return from function
	int server_socket,remote_socket,client_socket;
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
			printf("accept BLE connection fail, errno: %d\n", remote_socket);
			sleep(3);
			continue;
		}
		printf("accept BLE connection\n");
		readsocket= remote_socket;//client_socket;//share socket num to read thread
		printf("clientsocket:%d\nreadsocket:%d\n",client_socket,readsocket);
		
		temp=pthread_create(&readthread,NULL,socket_handler,&remote_socket);
		if(temp!=0)
		{
			printf("create thread for BLE read thread fail\n");
			continue ;
		}
		printf("crate thread for BLE read thread\n");
		pthread_detach(readthread);

	}	
/*
	//关闭子进程
	fprintf(writestream,"exit\n");
	fflush(writestream);

	//关闭我们的读取端描述符。
	close (input[1]);
	close(output[0]);

	//等待子进程结束
	// waitpid (pid, NULL, 0);
	printf("make sure child thread exit\n");
	waitpid(pid, NULL, 0);
	// pthread_cancel(child);
	printf("exit\n");
	fflush(stdout);
*/
}

void *read_shell_result_thread(socket_and_stream *ss)
{
	// FILE *readstream=(FILE *)p;
	//socket_and_stream *ss=(socket_and_stream *)p;

	FILE *readstream=ss->readstream;
	FILE *logfile=ss->logstream;
	// logfile=fopen("./log/data.txt","a+");


	char buff[1024];

	struct msghdr msg;
	struct iovec io;
	msg.msg_name = NULL;
	io.iov_base = buff;
	io.iov_len = sizeof(buff);
	msg.msg_iov = &io;
	msg.msg_iovlen = 1;

	while(1)
	{
		fflush(stdout);
		if( NULL != fgets(buff, (unsigned int)sizeof(buff),readstream))
		{
			if(readsocket>0)
			{
				write(readsocket,buff,strlen(buff)+1);
				//sendmsg(readsocket, &msg, strlen(buff)+1);
			}

			printf("%s",buff);
			fflush(stdout);

			fputs(buff,logfile);
			fflush(logfile);

		}

		memset(buff,0, (unsigned int)sizeof(buff));

		// pthread_testcancel();
	}
}

void * socket_handler(void *p) {
	int tmpsocket = (int) (*(int *) p);
	char buffer[1024];
	int readnum=0;

	struct msghdr msg;
	struct iovec io;
	msg.msg_name = NULL;
	io.iov_base = buffer;
	io.iov_len = sizeof(buffer);
	msg.msg_iov = &io;
	msg.msg_iovlen = 1;

    	//循环读取连接的数据
    	while(1)
	{

		memset(buffer,'\0', (unsigned int)sizeof(buffer));
	        readnum = read(tmpsocket, buffer, sizeof(buffer));
	    	//int readnum = recvmsg(tmpsocket, &msg, (int) sizeof(buffer));
	        if(readnum>0)
	        {
	            printf("accept:%s\n",buffer);

	            //把命令传给子进程
	            fputs(buffer,writestream);
	            fflush(writestream);
	            fputs("\n",writestream);
	            fflush(writestream);

	            //把命令写日志
	            fputs("$ ",logstream);
	            fputs(buffer,logstream);
	            fputs("\n",logstream);
	            fflush(logstream);
	        }
	        else if(readnum<0)
	        {
	            printf("there is problem in read buffer, errno: %d\n", readnum);
	            readsocket=-1;
	            break;
	        }
	        else
	        {
	            printf("the socket is out\n");
	            readsocket=-1;
	            break;
	        }
	}//循环读取数据结束
}

