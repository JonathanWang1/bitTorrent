#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

#define BUFFERT 512

int duration (struct timeval *start,struct timeval *stop, struct timeval *delta);
int create_client_socket (int port, char* ipaddr);

struct sockaddr_in sock_serv;

int main (int argc, char**argv){
	struct timeval start, stop, delta;
    int sfd,fd;
    char buf[BUFFERT];
    off_t count=0, m,sz;//long
	long int n;
    int l=sizeof(struct sockaddr_in);
	struct stat buffer;
	FILE *fptr;
    
    //check the command line
	if (argc != 3){
		printf("Error usage : %s <ip_serv> <port_serv> <filename>\n",argv[0]);
		return EXIT_FAILURE;
	}
	//create socket
    sfd=create_client_socket(atoi(argv[2]), argv[1]); 

    //store the message in buffer
	strcpy(buf, "hello.txt");
	//create the file
	//fptr = fopen("hello.txt", "w");

    if(connect(sfd,(struct sockaddr*)&sock_serv,l)==-1){
        perror("connection error\n");
        exit (3);
    }

	gettimeofday(&start,NULL);

	m=sendto(sfd,buf,sizeof(buf),0,(struct sockaddr*)&sock_serv,l);
	gettimeofday(&stop,NULL);
    duration(&start,&stop,&delta);  

    fd = open("hello.txt", O_CREAT|O_WRONLY, 0777);
    n=recv(sfd,buf,BUFFERT,0);
    printf("this is the message: %s\n",buf );
        while(n) {
            if(n==-1){
                perror("recv fail");
                exit(5);
            }
            if((m=write(fd,buf,n))==-1){
                perror("write fail");
                exit (6);
            }
            count=count+m;
            bzero(buf,BUFFERT);
            n=recv(sfd,buf,BUFFERT,0);
            printf("this is the message: %s\n",buf );
        };
    close(sfd);
	return EXIT_SUCCESS;
}

/* Fonction permettant le calcul de la durée de l'envoie */
int duration (struct timeval *start,struct timeval *stop,struct timeval *delta)
{
    suseconds_t microstart, microstop, microdelta;
    
    microstart = (suseconds_t) (100000*(start->tv_sec))+ start->tv_usec;
    microstop = (suseconds_t) (100000*(stop->tv_sec))+ stop->tv_usec;
    microdelta = microstop - microstart;
    
    delta->tv_usec = microdelta%100000;
    delta->tv_sec = (time_t)(microdelta/100000);
    
    if((*delta).tv_sec < 0 || (*delta).tv_usec < 0)
        return -1;
    else
        return 0;
}

/* Fonction permettant la creation d'un socket
 * Renvoie un descripteur de fichier
 */
int create_client_socket (int port, char* ipaddr){
    int l;
	int sfd;
    
	sfd = socket(PF_INET,SOCK_STREAM,0);
	if (sfd == -1){
        perror("socket fail");
        return EXIT_FAILURE;
	}
    
    //preparation de l'adresse de la socket destination
	l=sizeof(struct sockaddr_in);
	bzero(&sock_serv,l);
	
	sock_serv.sin_family=AF_INET;
	sock_serv.sin_port=htons(port);
    if (inet_pton(AF_INET,ipaddr,&sock_serv.sin_addr)==0){
		printf("Invalid IP adress\n");
		return EXIT_FAILURE;
	}
    
    return sfd;
}