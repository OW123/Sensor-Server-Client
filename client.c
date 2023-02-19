/*Needed headers*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include "frame.h"

/*The port used for this program*/
#define PORT 5040




pthread_mutex_t lock;


int main(int argc, char *argv[])
{
   int fd, numbytes;

   /*IP addres used (localhost)*/
   struct hostent *he;

   struct sockaddr_in server;

   /*Frame structs*/
   reqFrame clietnFrame;
   resFrame serverFrame;

   clietnFrame.preamb = PREAMB;

   /*Missing the IP address*/
   if (argc != 2) {
      if(argv[1] == NULL){
       printf("Utilice: %s  <host>\n",argv[0]);
      }
      exit(1);
   }

   if ((he=gethostbyname(argv[1]))==NULL){
      printf("gethostbyname() error\n");
      exit(-1);
   }

   if ((fd = socket(AF_INET, SOCK_STREAM, 0))==-1){
      printf("socket() error\n");
      exit(-1);
   }

   server.sin_family = AF_INET;
   server.sin_port = htons(PORT);
   server.sin_addr = *((struct in_addr *)he->h_addr);
   bzero(&(server.sin_zero),8);

   if(connect(fd, (struct sockaddr *)&server,  sizeof(struct sockaddr))==-1){
      printf("connect() error\n");
      exit(-1);
   }
   printf("Connection Established\n");
   while(1){
         printf("For each selection, type the number that corresponds with your petition\n");
         printf("Available sensors:\n 1.-Accelerometer\n 2.-Magnetometer\n 3.-Gyroscope\n 4.-All sensors\n");
         scanf("%d",(int *)&clietnFrame.sensor);

         printf("Available axis:\n 1.- X\n 2.- Y\n 3.- Z\n 4.- XYX\n");
         scanf("%d",(int *)&clietnFrame.axis);

         clietnFrame.checksum = 0xFB;

         pthread_mutex_lock(&lock);

         /*Sending the request frame*/
         send(fd,(char *)&clietnFrame,sizeof(reqFrame),0);

      /*Receiving the response frame*/
      if ((numbytes=recv(fd,(char *)&serverFrame,sizeof(resFrame),0)) == -1){
         printf("Error en recv() \n");
         continue;
      }

      /*Validation of the frame*/
      if(serverFrame.sensor < 4 && serverFrame.sensor > 0){
         switch(serverFrame.size){
            case 5:
               if(serverFrame.checksum != 0xFA){
                  perror("Invalid checksum\n");
                  continue;
               }
               break;
            case 7:
               if(serverFrame.checksum != 0xF8){
                  perror("Invalid checksum\n");
                  continue;
               }
               break;
         }
      }else if(serverFrame.sensor == 4){
         switch(serverFrame.size){
            case 7:
               if(serverFrame.checksum!= 0xF8){
                  perror("Invalid checksum\n");
                  continue;
               }
               break;
            case 13:
               if(serverFrame.checksum != 0xF2){
                  perror("Invalid checksum\n");
                  continue;
               }
               break;
         }
      }

      system("clear");
      printf("The received frame is:\n");
   
      if(serverFrame.sensor != 4){
         if(serverFrame.size != 7){
            printf("%X-%X-%X-%X-%X\n",serverFrame.preamb,serverFrame.sensor,serverFrame.size, serverFrame.data[0],serverFrame.checksum);
         }else{
            printf("%X-%X-%X",serverFrame.preamb,serverFrame.sensor,serverFrame.size);
            for(int i=0;i<3;i++){
               printf("-%X",serverFrame.data[i]);
            }
            printf("-%X\n", serverFrame.checksum);
         }
      }else{
         if(serverFrame.size != 13){
            printf("%X-%X-%X",serverFrame.preamb,serverFrame.sensor,serverFrame.size);
            for(int i=0;i<3;i++){
               printf("-%X",serverFrame.data[i]);
            }
            printf("-%X\n", serverFrame.checksum);
         }else{
            printf("%X-%X-%X", serverFrame.preamb,serverFrame.sensor,serverFrame.size);
            for(int i=0;i<13;i++){
               printf("-%X",serverFrame.data[i]);
            }
            printf("-%X\n", serverFrame.checksum);
         }
      }
      
      pthread_mutex_unlock(&lock);
   
   }
   return 0;
}
