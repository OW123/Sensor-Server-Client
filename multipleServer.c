/*All the headers needed for this program*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <json-c/json.h>
#include "frame.h"

/*The BACKLOG is the maximun number of allowed connections*/
#define BACKLOG 5

/*Array of thread for each client*/
pthread_t threads[BACKLOG];
pthread_mutex_t lock;

/*Arrays to store the JSON data*/
int accele_data[3];
int magne_data[3];
int gyro_data[3];

/*Declaring the function prototype*/
void *handle_client(void *socket_desc);



int main(int argc, char **argv) {
   int sockfd, newfd;
   struct sockaddr_in host_addr, client_addr;
   socklen_t sin_size;

   /*The used port is recived in the execution*/
   if(argc != 2) {
      perror("Usage: server <port>");
      exit(1);
   }

   if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("Socket failed");
      exit(1);
   }

   host_addr.sin_family = AF_INET;
   host_addr.sin_port = htons(atoi(argv[1]));
   host_addr.sin_addr.s_addr = INADDR_ANY;
   memset(&(host_addr.sin_zero), '\0', 8);

   if(bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1) {
      perror("Bind failed");
      exit(1);
   }

   if(listen(sockfd, BACKLOG) == -1) {
      perror("Listen failed");
      exit(1);
   }

   printf("Server ready, listening...\n");

   while(1) {
      sin_size = sizeof(struct sockaddr_in);
      if((newfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size)) == -1) {
         perror("Accept failed");
         continue;
      }

      printf("Connection from %s received\n", inet_ntoa(client_addr.sin_addr));

      /*A thread is created with the ID connection*/
      if(pthread_create(&threads[newfd], NULL, handle_client, &newfd) < 0) {
         perror("Thread creation failed");
         continue;
      }
   }

   if(close(sockfd) < 0) {
      perror("Close socket failed\n");
      exit(1);
   }

   return 0;
}


void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    int read_size;

    /*Frame structs*/
    reqFrame clientFrame;
    resFrame serverFrame;

    serverFrame.preamb = PREAMB;

   /*Always receiving messages form the clients on each thread*/
    while((read_size = recv(sock ,(char *)&clientFrame , sizeof(reqFrame) , 0)) > 0) 
    {
         /*Checking the client request frame*/
        if(clientFrame.checksum != 0xFB) {
            perror("Invalid checksum\n");
            continue;
        }
        printf("Received message sensor: %i\t axis:%i\n", clientFrame.sensor, clientFrame.axis);

        /*Making the response frame*/
        if(clientFrame.sensor == 1){
            serverFrame.sensor = clientFrame.sensor;
            switch(clientFrame.axis){
                case 1: 
                    serverFrame.data[0] = accele_data[0];
                    serverFrame.size = 5;
                    serverFrame.checksum = 0xFA;
                    break;
                case 2: 
                    serverFrame.data[0] = accele_data[1];
                    serverFrame.size = 5;
                    serverFrame.checksum = 0xFA;
                    break;
                case 3:
                    serverFrame.data[0] = accele_data[2];
                    serverFrame.size = 5;
                    serverFrame.checksum = 0xFA;
                    break;
                case 4:
                    serverFrame.data[0] = accele_data[0];
                    serverFrame.data[1] = accele_data[1];
                    serverFrame.data[2] = accele_data[2];
                    serverFrame.size = 7;
                    serverFrame.checksum = 0xF8;                  
            }
        }else if(clientFrame.sensor == 2){
            serverFrame.sensor = clientFrame.sensor;
            switch(clientFrame.axis){
                case 1: 
                    serverFrame.data[0] = magne_data[0];
                    serverFrame.size = 5;
                    serverFrame.checksum = 0xFA;
                    break;
                case 2:
                    serverFrame.data[1] = magne_data[1];
                    serverFrame.size = 5;
                    serverFrame.checksum = 0xFA;
                    break;
                case 3:
                    serverFrame.data[2] = magne_data[2];
                    serverFrame.size = 5;
                    serverFrame.checksum = 0xFA;
                    break;
                case 4:
                    serverFrame.data[0] = magne_data[0];
                    serverFrame.data[1] = magne_data[1];
                    serverFrame.data[2] = magne_data[2];
                    serverFrame.size = 7;     
                    serverFrame.checksum = 0xF8;             
                  break;               
            }
         }else if(clientFrame.sensor  == 3){
            serverFrame.sensor = clientFrame.sensor;
            switch(clientFrame.axis){
               case 1:
                  serverFrame.data[0] = gyro_data[0];
                  serverFrame.size = 5;
                  serverFrame.checksum = 0xFA;
                  break;
               case 2:
                  serverFrame.data[0] = gyro_data[1];
                  serverFrame.size = 5;
                  serverFrame.checksum = 0xFA;
                  break;
               case 3:
                  serverFrame.data[0] = gyro_data[2];
                  serverFrame.size = 5;
                  serverFrame.checksum = 0xFA;
                  break;
               case 4:
                  serverFrame.data[0] = gyro_data[0];
                  serverFrame.data[1] = gyro_data[1];
                  serverFrame.data[2] = gyro_data[2];
                  serverFrame.size = 7;
                  serverFrame.checksum = 0xF8;
                  break;
            }
         }else if(clientFrame.sensor  == 4){
            serverFrame.sensor = clientFrame.sensor;
            switch(clientFrame.axis){
               case 1:
                  serverFrame.data[0] = accele_data[0];
                  serverFrame.data[1] = magne_data[0];
                  serverFrame.data[2] = gyro_data[0];
                  serverFrame.size = 7;
                  serverFrame.checksum = 0xF8;
                  break;
               case 2:
                  serverFrame.data[0] = accele_data[1];
                  serverFrame.data[1] = magne_data[1];
                  serverFrame.data[2] = gyro_data[1];
                  serverFrame.size = 7;
                  serverFrame.checksum = 0xF8;
                  break;
               case 3:
                  serverFrame.data[0] = accele_data[2];
                  serverFrame.data[1] = magne_data[2];
                  serverFrame.data[2] = gyro_data[2];
                  serverFrame.size = 7;
                  serverFrame.checksum = 0xF8;
                  break;
               case 4:
                  serverFrame.data[0] = accele_data[0];
                  serverFrame.data[1] = accele_data[1];
                  serverFrame.data[2] = accele_data[2];
                  serverFrame.data[3] = magne_data[0];
                  serverFrame.data[4] = magne_data[1];
                  serverFrame.data[5] = magne_data[2];
                  serverFrame.data[6] = gyro_data[0];
                  serverFrame.data[7] = gyro_data[1];
                  serverFrame.data[8] = gyro_data[2];
                  serverFrame.size = 13;
                  serverFrame.checksum = 0xF2;
                  break;
            }
         }else{
            /*An error frame is created*/
            serverFrame.sensor = SENSOR_FAIL;
            serverFrame.size = SIZE_FAIL;;
            serverFrame.checksum = CRC_FAIL;
            perror("Invalid axis\n");
            pthread_mutex_unlock(&lock);
            continue;
         }


        if(send(sock, &serverFrame, sizeof(resFrame), 0) < 0) {
            perror("Send failed\n");
        }
    }

      if(read_size == 0) {
         perror("Client disconnected\n");
      } else if(read_size == -1) {
         perror("Receive failed\n");
      }

      pthread_mutex_unlock(&lock);

   if(close(sock) < 0) {
      perror("Close socket failed\n");
   }

   pthread_exit(NULL);
}