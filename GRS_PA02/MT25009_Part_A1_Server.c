// MT25009

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 9000
#define FIELDS 8

typedef struct {
    char *f[FIELDS];
} message_t;

void create_msg(message_t *m, int size){
    for(int i=0;i<FIELDS;i++){
        m->f[i] = malloc(size);
        memset(m->f[i],'A'+i,size);
    }
}

void* worker(void *arg){
    int sock = *(int*)arg;
    free(arg);

    int size;
    recv(sock,&size,sizeof(int),0);

    char *buf = malloc(size);

    while(1){
        for(int i=0;i<FIELDS;i++)
            recv(sock, buf, size, 0);   // ✅ RECEIVE, not send
    }

    return NULL;
}



int main(){
    int s = socket(AF_INET,SOCK_STREAM,0);

    struct sockaddr_in serv={0};
    serv.sin_family=AF_INET;
    serv.sin_port=htons(PORT);
    serv.sin_addr.s_addr=INADDR_ANY;

    bind(s,(void*)&serv,sizeof(serv));
    listen(s,128);

    while(1){
        int *c = malloc(sizeof(int));
        *c = accept(s,NULL,NULL);

        pthread_t t;
        pthread_create(&t,NULL,worker,c);
        pthread_detach(t);
    }
}
