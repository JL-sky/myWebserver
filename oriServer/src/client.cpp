#include<iostream>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include"util.h"
using namespace std;

void cliTest()
{
	int cfd=socket(AF_INET,SOCK_STREAM,0);
	errif(cfd<0,"socket create error");

	sockaddr_in caddr;
	caddr.sin_family=AF_INET;
	caddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	caddr.sin_port=htons(8888);

	int ret=connect(cfd,(sockaddr*)&caddr,sizeof(caddr));
	errif(ret<0,"conn error");

	while(1){
		char data[1024]="你好，我是客户端";
		write(cfd,data,strlen(data));
		sleep(1);		
		
		char buf[1024];
		bzero(buf,sizeof(buf));
		ssize_t len=read(cfd,buf,sizeof(data));
		if(len<0){
			close(cfd);
			errif(1,"read error");
		}
		if(len==0){
			cout<<"server is closed..."<<endl;
			break;
		}else if (len>0){
			cout<<buf<<endl;
		}
	}
	close(cfd);
}

int main()
{
	cliTest();
	return 0;
}
