// MT25XXX

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define PORT 9000
#define FIELDS 8

typedef struct {
    int size;
    int duration;
} args_t;

void* worker(void *a){
    args_t *arg=a;

    int sock=socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in serv={0};
    serv.sin_family=AF_INET;
    serv.sin_port=htons(PORT);
    serv.sin_addr.s_addr=inet_addr("127.0.0.1");

    connect(sock,(void*)&serv,sizeof(serv));

    send(sock,&arg->size,sizeof(int),0);

    char *buf=malloc(arg->size);

    time_t start=time(NULL);

    while(time(NULL)-start < arg->duration){
        for(int i=0;i<FIELDS;i++)
            recv(sock,buf,arg->size,0);
    }

    close(sock);
    return NULL;
}

int main(int c,char **v){
    int threads=atoi(v[1]);
    int size=atoi(v[2]);
    int duration=atoi(v[3]);

    pthread_t t[threads];
    args_t arg={size,duration};

    for(int i=0;i<threads;i++)
        pthread_create(&t[i],NULL,worker,&arg);

    for(int i=0;i<threads;i++)
        pthread_join(t[i],NULL);
}
