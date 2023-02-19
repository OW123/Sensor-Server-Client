This repository contains two programs: "server.c" and "client.c".

Note: You can use the makefile to comiple both C files. You need to install the libjson-c-dev header

## Dependencies

This program has the following dependencies:

* `stdio.h`
* `unistd.h`
* `stdlib.h`
* `sys/types.h`
* `sys/socket.h`
* `netinet/in.h`
* `fcntl.h`
* `arpa/inet.h`
* `string.h`
* `pthread.h`
* `json-c/json.h`
* `frame.h`

## multipleServer.c

The "multipleServer.c" program simulates a sensor device that responds to requests from a client. The server receives requests from a client containing the desired sensor, axis and other parameters, and returns a response containing the requested data. The server uses a socket to communicate with the client, and the response is sent in the form of a frame. The response frame takes the data form a JSON file.

To compile and run the program, use the following commands:

```
gcc -pthread -ljson-c -o multipleServer multipleServer.c
./server <PORT>
```


Note: The server must be running before the client can connect.

## client.c

The "client.c" program sends requests to the server and displays the response. The client receives user input to determine which sensor, axis and other parameters to request from the server. The client also uses a socket to communicate with the server, and the request and response are sent in the form of frames.

To compile and run the program, use the following commands:

```
gcc -pthread -o client client.c
./client

```

Both programs use the "frame.c" library, which contains the frame structure definitions used in communication between the server and client. The library is included in the compile commands for both programs.
