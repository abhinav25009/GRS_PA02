// MT25009

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/uio.h>

#define PORT 9000
#define FIELDS 8

void* worker(void *arg){
    int sock=*(int*)arg;
    free(arg);

    int size;
    recv(sock,&size,sizeof(int),0);

    struct iovec iov[FIELDS];

    for(int i=0;i<FIELDS;i++){
        iov[i].iov_base=malloc(size);
        memset(iov[i].iov_base,'A'+i,size);
        iov[i].iov_len=size;
    }

    struct msghdr msg={0};
    msg.msg_iov=iov;
    msg.msg_iovlen=FIELDS;

    while(1)
        sendmsg(sock,&msg,0);
}

int main(){
    int s=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in serv={0};
    serv.sin_family=AF_INET;
    serv.sin_port=htons(PORT);
    serv.sin_addr.s_addr=INADDR_ANY;

    bind(s,(void*)&serv,sizeof(serv));
    listen(s,128);

    while(1){
        int *c=malloc(sizeof(int));
        *c=accept(s,NULL,NULL);

        pthread_t t;
        pthread_create(&t,NULL,worker,c);
        pthread_detach(t);
    }
}
