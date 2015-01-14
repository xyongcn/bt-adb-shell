#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//文件操作类头文件
#include <sys/types.h>  /*提供类型pid_t,size_t的定义*/
#include <sys/stat.h>
#include <fcntl.h>

//网络相关头文件
#include <sys/socket.h>  
#include <linux/in.h>  

//bluetooth include files
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

//多线程相关头文件
#include <pthread.h>

void * read_thread(void *p);
void *read_shell_result_thread(void *p);
int readsocket=-1;

typedef struct socketAndStream
{
	int *socket;
	FILE *readstream;
	FILE *logstream;
}socket_and_stream;


int main(int argc,char **argv)
{
	pid_t pid;
	pid_t mpid;
	pid_t fpid;

	/*pid=fork();
	if(pid<0)
	{
		printf("create child proc failed\n");
		return -1;
	}
	else if(pid>0)
	{
		return -1;
	}*/
	
	
	//debug file ,redirect input and output
/*	int file_in,file_out,file_error;
	file_in=open("./log/in.txt",O_RDWR|O_APPEND|O_CREAT,S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);
	file_out=open("./log/out.txt",O_RDWR|O_APPEND|O_CREAT,S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);
	file_error=open("./log/error.txt",O_RDWR|O_APPEND|O_CREAT,S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH);

	if(file_in ==-1 || file_out==-1 || file_error==-1)
	{
		printf("create file failed\n");
		return -1;
	}*/

//只需要父进程执行即可
/*	dup2(file_in,STDIN_FILENO);//重定向标准输入流
	dup2(file_out,STDOUT_FILENO);//重定向标准输出流
	dup2(file_error,STDERR_FILENO);//重定向错误输出流*/

	/*fputs("in",stdin);
	fflush(stdin);*/
	/*fputs("out",stdout);
	fflush(stdout);
	fputs("error",stderr);
	fflush(stderr);*/
	//测试写入日志文件
	/*char msg[16]="hello\n";
	write(STDOUT_FILENO,msg,strlen(msg));
	write(STDERR_FILENO,msg,strlen(msg));*/
	// sync();

	// int fds[2]; 
	int input[2],output[2];
	// pid_t pid; 

	 // 创建管道
	pipe (input);
	pipe(output); 
	pthread_t child;

	pid = fork (); 
	if (pid == 0) 
	{ 
		setvbuf(stdout, NULL, _IONBF, 0);
		close(input[1]);
		close(output[0]);

		/*char debug1[12]="debug=1\n";
		char debug2[12]="debug=2\n";
		write(file_out,debug1,sizeof(debug1));*/

		dup2(input[0],STDIN_FILENO);
		dup2(output[1],STDOUT_FILENO);

		// write(file_out,debug2,sizeof(debug2));

		// system("/bin/sh");//linux is /bin/sh
		system("/system/bin/sh");//android is /system/bin/sh
		// execl("/bin/sh",NULL);

		close(input[0]);
		close(output[1]);
		// close(file);
		return -1;
	} 
	else 
	{ 
		/*dup2(file_in,STDIN_FILENO);//重定向标准输入流
		dup2(file_out,STDOUT_FILENO);//重定向标准输出流
		dup2(file_error,STDERR_FILENO);//重定向错误输出流*/

		close(input[0]);//father write input
		close(output[1]);//father read output

		FILE* writestream; 
		writestream = fdopen (input[1], "w"); 

		FILE *readstream;
		readstream=fdopen(output[0],"r");

		FILE *logstream;
		logstream=fopen("./data/BluetoothShellLog.txt","a+");//the log file

		socket_and_stream *ss=(socket_and_stream *)malloc(sizeof(socket_and_stream));
		ss->readstream=readstream;
		ss->logstream=logstream;

		int createthread=pthread_create(&child,NULL,read_shell_result_thread,ss);
        if(createthread!=0)
        {
            fputs("create read thread error\n",stderr);
            fflush(stdout);
            return ;
        }
        else
        {
            fputs("create new child thread\n",stdout);
            fflush(stdout);
            pthread_detach(child);
        }

		char buffer[512];
		// char result[512];
		// int len=sizeof(buffer);
		fflush(stdout);

		//----------------
		//从bluetooth读取操作命令
		int temp;//the value return from function
		int server_socket,client_socket;
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
		server_addr.l2_psm = htobs(0x1001);//psm类似于tcp中的端口，一个psm只能绑定一个server socket
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
	    //循环等待客户端连接
		while(1)  
	    {
			printf("wait for BLE connection\n");
			memset(&remote_addr, 0, len);
			//蓝牙的server socket接受到一个终端的链接，client_socket就是这个链接的socket标识
			client_socket = accept(server_socket, (struct sockaddr*)(&remote_addr), &len);
			if(client_socket<0)
			{
				printf("accept BLE connection fail\n");
				continue;
			}
			printf("accept BLE connection\n");
			readsocket=client_socket;//share socket num to read thread
			printf("clientsocket:%d\nreadsocket:%d\n",client_socket,readsocket);

	    	//循环读取连接的数据
	    	while(1)
		    {
		    	memset(buffer,'\0',sizeof(buffer));
		        int readnum=read(client_socket,buffer,sizeof(buffer));
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
		            printf("there is problem in read buffer\n");
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

		//从控制台读取操作命令
		/*do
		{
			// printf("input:");
			fprintf(stdout,"input:");
			fflush(stdout);
			memset(buff,'\0',len);
			if((fgets(buff,sizeof(buff),stdin))!=NULL)
			{
				fputs(buff,writestream);
				// fwrite(stream,buff,strlen(buff)+1);
				fflush(writestream);
				//文件中获取的可能没有足够的换行符
				fputs("\n",writestream);
				fflush(writestream);
			}
			
		}while(strcmp(buff,"exit\n")!=0);*/

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
	}
	return ; 

}

void *read_shell_result_thread(void *p)
{
	// FILE *readstream=(FILE *)p;
	socket_and_stream *ss=(socket_and_stream *)p;

	FILE *readstream=ss->readstream;
	FILE *logfile=ss->logstream;
	// logfile=fopen("./log/data.txt","a+");

	char buff[1024];
	while(1)
	{
		/*if(fread(buff,sizeof(char),sizeof(buff),readstream)>0)
		{
			printf("%s",buff);
			memset(buff,0,sizeof(buff));
		}*/
		// printf("read_shell_result_thread prepare to read\n");
		fflush(stdout);
		if(fgets(buff,sizeof(buff),readstream)>0)
		{

			if(readsocket>0)
			{
				write(readsocket,buff,strlen(buff)+1);
			}

			printf("%s",buff);
			fflush(stdout);

			fputs(buff,logfile);
			fflush(logfile);

		}
			
		memset(buff,0,sizeof(buff));

		// pthread_testcancel();
	}
}

/*void * read_thread(void *p)
{
    int socket=(int)(*(int *)p);
    char buffer[512];
    int readnum;

    while(1)
    {
        readnum=read(socket,buffer,sizeof(buffer));
        if(readnum>0)
        {
            fprintf(stdout,"accept:%s\n",buffer);
            fflush(stdout);
        }
        else if(readnum<0)
        {
            fprintf(stdout,"there is problem in read buffer\n");
            fflush(stdout);
            break;
        }
        else
        {
            fprintf(stdout,"the socket is out\n");
            fflush(stdout);
            break;
        }
        
    }
}

void *read_shell_result_thread(void *p)
{
	socket_and_stream *ss;
	ss=(socket_and_stream *)p;

	//读取子进程的shell执行结果流
	FILE *readstream=ss->readstream;
	int socket=*ss->socket;

	//日志的保存文件
	FILE *logfile;
	logfile=fopen("./log/data.txt","a+");

	//读取执行结果缓存
	char buff[1024];
	while(1)
	{
		
		if(fgets(buff,sizeof(buff),readstream)>0)
		{
			fprintf(stdout,"%s",buff);
			fflush(stdout);
			//写入到日志文件
			fputs(buff,logfile);

			//返回给socket
			write(socket,buff,strlen(buff)+1);

			//清空缓存
			memset(buff,0,sizeof(buff));
		}
		
		//设置线程取消点
		// pthread_testcancel();
	}
}*/