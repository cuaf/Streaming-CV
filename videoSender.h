#ifndef VIDEOSENDER
#define VIDEOSENDER
#include <pthread.h>
#include <opencv2/core/core.hpp>
using namespace cv;
pthread_mutex_t amutex = PTHREAD_MUTEX_INITIALIZER;
int             is_data_ready = 1;
int             clientSock;
char*       server_ip;
int         server_port;
Size resizeSize(640,500);

Mat img1, img2;

void videoquit(string msg, int retval);
#endif
