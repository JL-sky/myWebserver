#include<iostream>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
//#include<errno.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include"util.h"

using namespace std;

#define MAX_EVENTS 1024

//设置非阻塞IO
/*
文件描述符（File Descriptor，fd）的文件状态标志（File Status Flags）设置为非阻塞模式（O_NONBLOCK）。

fcntl(fd, F_GETFL)：获取文件描述符 fd 的当前文件状态标志。
fcntl(fd, F_GETFL) | O_NONBLOCK：通过按位或运算，将获取到的文件状态标志与 O_NONBLOCK 进行组合，以设置非阻塞模式。这是通过将O_NONBLOCK标志添加到文件状态标志中来实现的。
fcntl(fd, F_SETFL, ...)：将新的文件状态标志设置回文件描述符 fd。
*/
void setnonblocking(int fd){
	fcntl(fd,F_SETFL,fcntl(fd,F_GETFL) | O_NONBLOCK);
}

void serverTest()
{
	int lfd=socket(AF_INET,SOCK_STREAM,0);
	errif(lfd<0,"socket create error");

	sockaddr_in saddr;
	saddr.sin_family=AF_INET;
	saddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	saddr.sin_port=htons(8888);

	int ret=bind(lfd,(sockaddr*)&saddr,sizeof(saddr));
	errif(ret<0,"bind error!");
	
	ret=listen(lfd,128);
	errif(ret<0,"listen error");
	
	int epfd=epoll_create1(0);
	errif(epfd==-1,"epoll create error");

	struct epoll_event events[MAX_EVENTS],ev;
	bzero(events,sizeof(events));
	bzero(&ev,sizeof(ev));

	ev.data.fd=lfd;
	//EPOLLIN用于检测socket的读事件，EPOLLET表示边缘触发模式
	ev.events=EPOLLIN | EPOLLET;
	setnonblocking(lfd);
	epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&ev);

	while(1)
	{
		int nfds=epoll_wait(epfd,events,MAX_EVENTS,-1);
		errif(nfds==-1,"epoll_wait error");

		for(int i=0;i<nfds;i++)
		{
			int curfd=events[i].data.fd;
			if(curfd==lfd)
			{
				struct sockaddr_in caddr;
				bzero(&caddr,sizeof(caddr));
				socklen_t caddr_len=sizeof(caddr);

				int cfd=accept(lfd,(sockaddr*)&caddr,&caddr_len);
				errif(cfd==-1,"accept error");

				cout<<"client IP:"<<inet_ntoa(caddr.sin_addr)<<"\t port:"
				<<ntohs(caddr.sin_port)<<endl;

				bzero(&ev,sizeof(ev));
				ev.data.fd=cfd;
				ev.events=EPOLLIN | EPOLLET;
				setnonblocking(cfd);
				epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&ev);
				
			}
			else if(curfd & EPOLLIN)
			{
				char buf[1024];
				while(1){
					bzero(buf,sizeof(buf));
					ssize_t len=read(curfd,buf,sizeof(buf));
					if(len>0)
					{
						cout<<"client fd "<<curfd<<":"<<buf<<endl;
					}
					else if(len<0)
					{
						if(errno==EINTR)
						{
							cout<<"continue reading...."<<endl;
							continue;
						}
						if(errno==EAGAIN || errno==EWOULDBLOCK)
						{
							cout<<"finish reading once,errno:"<<errno<<endl;
							break;
							//continue;
						}
					}
					else if(len==0)
					{
						cout<<"EOF,client fd "<<curfd<<" is disconnect..."<<endl;
						close(curfd);
						break;
					}
					else
					{
						cout<<"something else happending..."<<endl;
					}
				}
			}
		}
	}
	close(lfd);
}

int main()
{
	serverTest();
	return 0;
}
