#include<iostream>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include"util.h"

using namespace std;

#define MAXLISTEN 128
void serverTest()
{
	int lfd=socket(AF_INET,SOCK_STREAM,0);
	if(lfd<0)
		errif(1,"socket create error");
	
	sockaddr_in saddr;
	bzero(&saddr,sizeof(saddr));

	saddr.sin_family=AF_INET;
	saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	saddr.sin_port=htons(8888);

	int ret=bind(lfd,(sockaddr*)&saddr,sizeof(saddr));
	if(ret<0)
		errif(1,"bind error");

	ret=listen(lfd,MAXLISTEN);
	if(ret<0)
		errif(1,"listen error");

	sockaddr_in caddr;
	bzero(&caddr,sizeof(caddr));
	socklen_t caddr_len=sizeof(caddr);

	int cfd=accept(lfd,(sockaddr*)&caddr,&caddr_len);
	if(cfd<0)
		errif(1,"accept error");

	cout<<"IP:"<<inet_ntoa(caddr.sin_addr)
	<<"\nport:"<<ntohs(caddr.sin_port)<<endl;

	//char* data=nullptr;
	while(1){
		char data[1024];
		bzero(data,sizeof(data));

		ssize_t len=read(cfd,data,sizeof(data));
		if(len<0){
			close(cfd);
			errif(1,"read error");
		}else if(len==0){
			cout<<"client is disconnected..."<<endl;
			close(cfd);
		}else{
			cout<<data<<endl;
			char buf[1024]="你好，我是服务端...";
			write(cfd,buf,strlen(buf));
		}

	}
	close(lfd);
}

int main()
{
	serverTest();
	return 0;
}
