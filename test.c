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
		//���������ܵ�(���˺�д��)
    int pipefd[2];
    int pip=pipe(pipefd);
    
    //�����ӽ���
    pid_t pid=fork();
   
    if(pid==0)
    {
    	 //��һ���ӽ��̹ر�"�ܵ�����"
        close(pipefd[0]);
        //�ض���"��׼���"��"�ܵ�д��"
        dup2(pipefd[1],STDOUT_FILENO);
        //ִ��exec����
        execlp("fdfs_upload_file","fdfs_upload_file","./conf/client.conf",argv[1],NULL);
				//�ر�"�ܵ�д��"
        close(pipefd[1]);
    }
    else
    {
    		//�����̹ر�"�ܵ�д��"
        close(pipefd[1]);
        //�����̶�ȡ"�ܵ�����"
        char fileid[1024]={0};
        read(pipefd[0],fileid,sizeof(fileid));
				fileid[strlen(fileid)-1]='\0';
        printf("%s\n",fileid);
        //�����������ȴ������������ӽ���
        int status;
        while((wait(&status))!=-1)
        {
            if(WIFEXITED(status))
            {
                WEXITSTATUS(status);
            }
        }
        //������־�ļ�ID���ļ����Ķ�Ӧ��ϵ
        LOG(FDFS_TEST_MODULE,"setup.log","filename = [%s], fileid=[%s]", argv[1], fileid);
    }
    return 0;
}
