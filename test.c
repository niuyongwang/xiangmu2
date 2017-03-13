#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "make_log.h"
#define FDFS_TEST_MODULE "test"
#define FDFS_TEST_PROC  "fdfs"

int main(int argc, const char* argv[])
{
		//创建匿名管道(读端和写端)
    int pipefd[2];
    int pip=pipe(pipefd);
    
    //创建子进程
    pid_t pid=fork();
   
    if(pid==0)
    {
    	 //第一个子进程关闭"管道读端"
        close(pipefd[0]);
        //重定向"标准输出"到"管道写端"
        dup2(pipefd[1],STDOUT_FILENO);
        //执行exec函数
        execlp("fdfs_upload_file","fdfs_upload_file","./conf/client.conf",argv[1],NULL);
				//关闭"管道写端"
        close(pipefd[1]);
    }
    else
    {
    		//父进程关闭"管道写端"
        close(pipefd[1]);
        //父进程读取"管道读端"
        char fileid[1024]={0};
        read(pipefd[0],fileid,sizeof(fileid));
				fileid[strlen(fileid)-1]='\0';
        printf("%s\n",fileid);
        //父进程阻塞等待回收死亡的子进程
        int status;
        while((wait(&status))!=-1)
        {
            if(WIFEXITED(status))
            {
                WEXITSTATUS(status);
            }
        }
        //存入日志文件ID和文件名的对应关系
        LOG(FDFS_TEST_MODULE,"setup.log","filename = [%s], fileid=[%s]", argv[1], fileid);
    }
    return 0;
}
