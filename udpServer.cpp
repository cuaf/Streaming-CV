//This plays the data back from the network.
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//for mac.
#include <unistd.h>

#include <iostream>
#include <string>
#include <pthread.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

Mat     img;
int     is_data_ready = 0;
int     listenSock, connectSock;
int 	listenPort;

pthread_mutex_t amutex = PTHREAD_MUTEX_INITIALIZER;

void* streamServer(void* arg);
void  quit(string msg, int retval);

int main(int argc, char** argv)
{
        pthread_t thread_s;
        int width, height, key;
	width = 480;
	height = 320;

	if (argc != 2) {
                quit("Usage: netcv_server <listen_port> ", 0);
        }

	listenPort = atoi(argv[1]);

        img = Mat::zeros( height,width, CV_8UC1);

        /* run the streaming server as a separate thread */
        if (pthread_create(&thread_s, NULL, streamServer, NULL)) {
                quit("pthread_create failed.", 1);
        }

        cout << "\n-->Press 'q' to quit." << endl;
        namedWindow("stream_server", CV_WINDOW_AUTOSIZE);

        while(key != 'q') {

                pthread_mutex_lock(&amutex);
                        if (is_data_ready) {
                                imshow("stream_server", img);
                                is_data_ready = 0;
                        }
                pthread_mutex_unlock(&amutex);
                key = waitKey(10);
        }

        if (pthread_cancel(thread_s)) {
                quit("pthread_cancel failed.", 1);
        }

        destroyWindow("stream_server");
        quit("NULL", 0);
//return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * This is the streaming server, run as separate thread
 */
void* streamServer(void* arg)
{
        struct  sockaddr_in   serverAddr,  clientAddr;
        socklen_t             clientAddrLen = sizeof(clientAddr);

        /* make this thread cancellable using pthread_cancel() */
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

        //if ((listenSock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        if ((listenSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            quit("socket() failed.", 1);
        }

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(listenPort);

        if (bind(listenSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
                quit("bind() failed", 1);
        }
/*
        if (listen(listenSock, 5) == -1) {
                quit("listen() failed.", 1);
        }
*/
        int  imgSize = img.total()*img.elemSize();
        char sockData[imgSize];
        int  bytes=0;

        /* start receiving images */
        while(1)
        {
	        cout << "-->Waiting for UDP packet on port " << listenPort << " ...\n\n";

		/* accept a request from a client */
            /*
	        if ((connectSock = accept(listenSock, (sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
	                quit("accept() failed", 1);
	        }else{
		    	cout << "-->Receiving image from " << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port) << "..." << endl;
		}
            */

/*
              len = sizeof(cliaddr);
      n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
      sendto(sockfd,mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));
      printf("-------------------------------------------------------\n");
      mesg[n] = 0;
      printf("Received the following:\n");
      printf("%s",mesg);
      printf("-------------------------------------------------------\n");
*/
		while(1){
                        memset(sockData, 0x0, sizeof(sockData));
                        bytes = recvfrom(listenSock, sockData, imgSize , 0,(struct sockaddr *)&clientAddr,&clientAddrLen);
                        cout << "Received a packet" << '\n';

                        /*
                	for (int i = 0; i < imgSize; i += bytes) {

                        	if ((bytes = recvfrom(listenSock, sockData +i, imgSize  - i, 0,(struct sockaddr *)&clientAddr,&clientAddrLen)) == -1) {
 	                              	quit("recv failed", 1);
				}
                	}
                    */
                	/* convert the received data to OpenCV's Mat format, thread safe */
                	pthread_mutex_lock(&amutex);
                        	for (int i = 0;  i < img.rows; i++) {
                        	        for (int j = 0; j < img.cols; j++) {
                        	                (img.row(i)).col(j) = (uchar)sockData[((img.cols)*i)+j];
                        	        }
                        	}
                        	is_data_ready = 1;
				memset(sockData, 0x0, sizeof(sockData));
                	pthread_mutex_unlock(&amutex);
		}
	}

        /* have we terminated yet? */
        pthread_testcancel();

  	/* no, take a rest for a while */
        usleep(1000);

}
/////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * This function provides a way to exit nicely from the system
 */
void quit(string msg, int retval)
{
        if (retval == 0) {
                cout << (msg == "NULL" ? "" : msg) << "\n" <<endl;
        } else {
                cerr << (msg == "NULL" ? "" : msg) << "\n" <<endl;
        }

        if (listenSock){
                close(listenSock);
        }

        if (connectSock){
                close(connectSock);
        }

        if (!img.empty()){
                (img.release());
        }

        pthread_mutex_destroy(&amutex);
        exit(retval);
}

