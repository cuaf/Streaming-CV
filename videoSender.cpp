#include <sys/socket.h>
#include "videoSender.h"
extern pthread_mutex_t amutex;

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * This is the streaming client, run as separate thread
 */
void* streamClient(void* arg)
{
        struct  sockaddr_in serverAddr;
    socklen_t           serverAddrLen = sizeof(serverAddr);

        /* make this thread cancellable using pthread_cancel() */
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

        if ((clientSock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
            videoquit("\n--> socket() failed.", 1);
        }

        serverAddr.sin_family = PF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(server_ip);
        serverAddr.sin_port = htons(server_port);

        if (connect(clientSock, (sockaddr*)&serverAddr, serverAddrLen) < 0) {
                videoquit("\n--> connect() failed.", 1);
        }

        int  imgSize = img1.total()*img1.elemSize();
        int  bytes=0;
        img2 = (img1.reshape(0,1)); // to make it continuous

        /* start sending images */
        while(1)
        {
                /* send the grayscaled frame, thread safe */
                if (is_data_ready) {
                        pthread_mutex_lock(&amutex);
                                if ((bytes = send(clientSock, img2.data, imgSize, 0)) < 0){
                    cerr << "\n--> bytes = " << bytes << endl;
                                    videoquit("\n--> send() failed", 1);
                }
                is_data_ready = 0;
                        pthread_mutex_unlock(&amutex);
            memset(&serverAddr, 0x0, serverAddrLen);
                }
                /* if something went wrong, restart the connection */
                if (bytes != imgSize) {
                        cerr << "\n-->  Connection closed (bytes != imgSize)" << endl;
                        close(clientSock);

                        if (connect(clientSock, (sockaddr*) &serverAddr, serverAddrLen) == -1) {
                                videoquit("\n--> connect() failed", 1);
                        }
                }

                /* have we terminated yet? */
                pthread_testcancel();

                /* no, take a rest for a while */
                usleep(1000);   //1000 Micro Sec
        }
}

void videoquit(string msg, int retval)
{
        if (retval == 0) {
                cout << (msg == "NULL" ? "" : msg) << "\n" << endl;
        } else {
                cerr << (msg == "NULL" ? "" : msg) << "\n" << endl;
        }
        if (clientSock){
                close(clientSock);
        }
        pthread_mutex_destroy(&amutex);
        exit(retval);
}

