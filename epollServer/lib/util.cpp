#ifndef UTIL_CPP
#define UTIL_CPP
#include"util.h"
#include<stdio.h>
#include<stdlib.h>
void errif(bool flag,const char*errmsg)
{
	if(flag){
		perror(errmsg);
		exit(-1);
	}
}

#endif

