/*
 * Copyright (c) 2014 Putilov Andrey
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <math.h>
#include "websocket.h"
#include "oneline.h"
#define PORT 9000
#define BUF_LEN 5000000
#define PACKET_DUMP

uint8_t rBuffer[BUF_LEN];
uint8_t wBuffer[BUF_LEN];
fd_set rFd;
fd_set wFd;
fd_set eFd;
struct timeval tv;
#undef max
#define max(x,y) ((x) > (y) ? (x) : (y))


void error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int safeSend(int clientSocket, const uint8_t *buffer, size_t bufferSize)
{
    #ifdef PACKET_DUMP
    printf("out packet:\n");
    fwrite(buffer, 1, bufferSize, stdout);
    printf("\n");
    #endif
    ssize_t written = send(clientSocket, buffer, bufferSize, 0);
    if (written == -1) {
        close(clientSocket);
        perror("send failed");
        return EXIT_FAILURE;
    }
    if (written != bufferSize) {
        close(clientSocket);
        perror("written not all bytes");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}


void clientReset(int clientSocket)
{
    FD_ZERO(&rFd);
    FD_ZERO(&wFd);
    FD_ZERO(&eFd);
    FD_SET(clientSocket, &rFd);
    FD_SET(clientSocket, &wFd);
    tv.tv_sec=5;
    tv.tv_usec=0;
}

void clientWorker(int clientSocket)
{
    memset(rBuffer, 0, BUF_LEN);
    memset(wBuffer, 0, BUF_LEN); 
    clientReset( clientSocket );
    int nfds;
    size_t readedLength = 0;
    size_t frameGSize = BUF_LEN;
    size_t frameWSize = BUF_LEN;
  
     
    enum wsState state = WS_STATE_OPENING;
    uint8_t *data = NULL;
    size_t dataSize = 0;
    enum wsFrameType frameType = WS_INCOMPLETE_FRAME;
    struct handshake hs;
    // keep things at one module per all clientWorkers
    // using this module
    oneline_module_t_ptr module;
    int exit = 0; 
    nullHandshake(&hs);
     
    
    #define prepareGBuffer frameGSize = BUF_LEN; memset(rBuffer, 0, BUF_LEN);
    #define initNewGFrame frameType = WS_INCOMPLETE_FRAME; readedLength = 0; memset(rBuffer, 0, BUF_LEN);
    #define prepareWBuffer frameWSize = BUF_LEN; memset(wBuffer, 0, BUF_LEN);
    #define initNewWFrame memset(wBuffer, 0, BUF_LEN);
    
   
    
       nfds=clientSocket+1;
    while ( TRUE ) {
       clientReset( clientSocket );
	//oneline_log("Trying to receive", oneline_log_msg_init("EventLoop()", __LINE__, "Trying to receive", "INFO"));
       int result  = select(nfds,&rFd,&wFd,NULL,NULL);
       if ( result == -1 ) {
	    close(clientSocket);
	    perror("recv failed");
	} else {
	 	//oneline_log("Received buffer", oneline_log_msg_init("EventLoop()", __LINE__, "Received Message", "INFO"));
	       if ( state == WS_STATE_NORMAL && FD_ISSET(clientSocket, &wFd)  ) {
		 //oneline_log("Sending Listener Message", oneline_log_msg_init("EventLoop()", __LINE__, "Sending Listener Message", "INFO"));
		  char* oneline_char_listener = oneline_invoke_object_callback( module, "listener", "");
		 prepareWBuffer;
		 wsMakeFrame(oneline_char_listener, strlen(oneline_char_listener), wBuffer, &frameWSize, WS_TEXT_FRAME);
		 if (safeSend(clientSocket,wBuffer,frameWSize)== EXIT_FAILURE) {
		     break;
		 }
		 initNewWFrame;
		}
		if (FD_ISSET(clientSocket, &rFd)) {
			oneline_log("Trying to receive Message", oneline_log_msg_init("EventLoop()", __LINE__, "Trying to receive", "INFO"));	
			ssize_t readed = recv(clientSocket, rBuffer+readedLength, BUF_LEN-readedLength, 0);
			if (!readed || exit == 1) {
			    close(clientSocket);
			    perror("recv failed");
			    return;
			}
			//oneline_log("event loop: reading", oneline_log_msg_init("event loop: reading", rBuffer,__LINE__,"INFO"));
			#ifdef PACKET_DUMP
			printf("in packet:\n");

			fwrite(rBuffer, readedLength, BUF_LEN-readedLength, stdout);
			printf("\n");
			#endif
			readedLength+= readed;
			assert(readedLength <= BUF_LEN);
			
			if (state == WS_STATE_OPENING) {
			    frameType = wsParseHandshake(rBuffer, readedLength, &hs);
			} else {
			    frameType = wsParseInputFrame(rBuffer, readedLength, &data, &dataSize);
			}

			// run our listener loop if we are in normal state
			if ( state == WS_STATE_NORMAL ) {
			 char* oneline_char_listener = oneline_invoke_object_callback( module, "listener", "");
			 prepareGBuffer;
			 wsMakeFrame(oneline_char_listener, strlen(oneline_char_listener), rBuffer, &frameGSize, WS_TEXT_FRAME);
			 if (safeSend(clientSocket,rBuffer,frameGSize)== EXIT_FAILURE) {
			     break;
			 }
			initNewGFrame;
			}

			
			if ((frameType == WS_INCOMPLETE_FRAME && readedLength == BUF_LEN) || frameType == WS_ERROR_FRAME) {
			    if (frameType == WS_INCOMPLETE_FRAME)
				printf("buffer too small");
			    else
				printf("error in incoming frame\n");
			    
			    if (state == WS_STATE_OPENING) {
				prepareGBuffer;
				frameGSize = sprintf((char *)rBuffer,
						    "HTTP/1.1 400 Bad Request\r\n"
						    "%s%s\r\n\r\n",
						    versionField,
						    version);
				safeSend(clientSocket, rBuffer, frameGSize);
				break;
			    } else {
				prepareGBuffer;
				wsMakeFrame(NULL, 0, rBuffer, &frameGSize, WS_CLOSING_FRAME);
				if (safeSend(clientSocket, rBuffer, frameGSize) == EXIT_FAILURE)
				    break;
				state = WS_STATE_CLOSING;
				initNewGFrame;
			    }
			}
			
			if (state == WS_STATE_OPENING) {
			    assert(frameType == WS_OPENING_FRAME);
			    if (frameType == WS_OPENING_FRAME) {
				 
				// if resource is right, generate answer handshake and send it

				 // updated 2/22/2016 Nadir Hamid
				// get the absolute module name 

				char* module_name=(char*) malloc(sizeof(hs.resource));
				strncpy(module_name,hs.resource+1,strlen(hs.resource));
				module_name[strlen(hs.resource)]='\0';
				 oneline_log("clientWorker()", oneline_log_msg_init("clientWorker()",  __LINE__, module_name, "INFO"));
				if ((module=oneline_find_module( module_name ) ) == NULL) {
				    frameGSize = sprintf((char *)rBuffer, "HTTP/1.1 404 Not Found\r\n\r\n");
				    safeSend(clientSocket, rBuffer, frameGSize);
				     
				    exit=1;
				    break;
				} else {
				   oneline_invoke_object_callback(module, "start", "");
				 }
				prepareGBuffer;
				wsGetHandshakeAnswer(&hs, rBuffer, &frameGSize);
				freeHandshake(&hs);
				if (safeSend(clientSocket, rBuffer, frameGSize) == EXIT_FAILURE)
				    break;
				state = WS_STATE_NORMAL;
				initNewGFrame;
			    }
			} else {
			    if (frameType == WS_CLOSING_FRAME) {
				if (state == WS_STATE_CLOSING) {
				    break;
				} else {
				    prepareGBuffer;
				    wsMakeFrame(NULL, 0, rBuffer, &frameGSize, WS_CLOSING_FRAME);
				    safeSend(clientSocket, rBuffer, frameGSize);
				    oneline_invoke_object_callback(module, "end", "");
				    break;
				}
			    } else if (frameType == WS_TEXT_FRAME) {
				uint8_t *recievedString = NULL;
				recievedString = malloc(dataSize+1);
				assert(recievedString);
				memcpy(recievedString, data, dataSize);
				recievedString[ dataSize ] = 0;
			       
				char* oneline_char_receiver =oneline_invoke_object_callback(module, "receiver", recievedString);
				prepareGBuffer;
				wsMakeFrame(oneline_char_receiver, strlen(oneline_char_receiver), rBuffer, &frameGSize, WS_TEXT_FRAME);
				free(recievedString);
			   
				if (safeSend(clientSocket, rBuffer, frameGSize) == EXIT_FAILURE)
				    break;
				initNewGFrame;
			    }
			 }
		    } // read/write cycle
	   }
	}
    close(clientSocket);
}

int main(int argc, char** argv)
{
    oneline_init_basics();
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == -1) {
        error("create socket failed");
    }
    
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(PORT);
    if (bind(listenSocket, (struct sockaddr *) &local, sizeof(local)) == -1) {
        error("bind failed");
    }
    
    if (listen(listenSocket, 1) == -1) {
        error("listen failed");
    }
    printf("opened %s:%d\n", inet_ntoa(local.sin_addr), ntohs(local.sin_port));
    
    while (TRUE) {
        struct sockaddr_in remote;
        socklen_t sockaddrLen = sizeof(remote);
        int clientSocket = accept(listenSocket, (struct sockaddr*)&remote, &sockaddrLen);
        if (clientSocket == -1) {
            error("accept failed");
        }
        
        printf("connected %s:%d\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));
        clientWorker(clientSocket);
 	 
        printf("disconnected\n");
    }
    
    close(listenSocket);
    return EXIT_SUCCESS;
}

