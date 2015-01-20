 #include <ctime>
#include <iostream>
#include <raspicam/raspicam_cv.h>

#include <arpa/inet.h>

#include <iostream>
#include <pthread.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "videoSender.h"
#include "videoSender.cpp"

using namespace std;
using namespace cv;

//rows by cols.
extern Size resizeSize;
extern Mat img1, img2;
extern int             is_data_ready;
extern int             clientSock;
extern char*        server_ip;
extern int          server_port;

extern pthread_mutex_t amutex;

raspicam::RaspiCam_Cv Camera;
cv::Mat image, img0, convertedColour, resizedImage;

void* streamClient(void* arg);
void  quit(string msg, int retval);

int main ( int argc,char **argv ) {

    time_t timer_begin,timer_end;

    pthread_t   thread_c;
    int         key;

        if (argc < 3) {
                quit("Usage: netcv_client <server_ip> <server_port> ", 0);
        }

        server_ip   = argv[1];
        server_port = atoi(argv[2]);

    //set camera params
    Camera.set(CV_CAP_PROP_FORMAT, CV_8UC3 );


    //Open camera
    cout<<"Opening Camera..."<<endl;
    if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}
    cout << "sleeping on startup because camera is slow to respond when first started?. Please wait." << '\n';
    sleep(1);
    //Start capture
        Camera.grab();
        Camera.retrieve ( image);

    cv::resize(image, img0, resizeSize);
    img1 = cv::Mat::zeros(img0.rows, img0.cols ,CV_8UC1);
    cout << "Image has " << img0.cols << " width  " << img0.rows << "height \n";

            // run the streaming client as a separate thread
        if (pthread_create(&thread_c, NULL, streamClient, NULL)) {
                quit("\n--> pthread_create failed.", 1);
        }

        cout << "\n--> Press 'q' to quit. \n\n" << endl;

        /* print the width and height of the frame, check this is set up the same on the receive side */
        cout << "\n--> Transferring  (" << img0.cols << "x" << img0.rows << ")  images to the:  " << server_ip << ":" << server_port << endl;

        /* Didn't compile with the namedwindow options because we don't have GTK.
        cv::namedWindow("stream_client", CV_WINDOW_AUTOSIZE);
                        flip(img0, img0, 1);
                        cvtColor(img0, img1, CV_BGR2GRAY);

        */

        while(key != 'q') {
                /* get a frame from camera */
            Camera.grab();
            Camera.retrieve(image);

                cv::resize(image, img0, resizeSize);
                if (img0.empty()) break;

                pthread_mutex_lock(&amutex);

                //cv::flip(img0, img0, 1);
                cv::cvtColor(img0, img1, CV_BGR2GRAY);
                //Image processing can go here before being sent.
                //cv::GaussianBlur(img1, img1, cv::Size(7,7), 1.5, 1.5);
                //cv::Canny(img1, img1, 0, 30, 3);

                is_data_ready = 1;

                pthread_mutex_unlock(&amutex);
                usleep(1000);   //sleep before sending next frame.

                //imshow("stream_client", img0);
                //key = cv::waitKey(30);
        }

        /* user has pressed 'q', terminate the streaming client */
        if (pthread_cancel(thread_c)) {
                quit("\n--> pthread_cancel failed.", 1);
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * this function provides a way to exit nicely from the system
 */
void quit(string msg, int retval)
{
        if (retval == 0) {
                cout << (msg == "NULL" ? "" : msg) << "\n" << endl;
        } else {
                cerr << (msg == "NULL" ? "" : msg) << "\n" << endl;
        }
        if (Camera.isOpened()){
                Camera.release();
        }
        if (!(img0.empty())){
                (~img0);
        }
        if (!(img1.empty())){
                (~img1);
        }
        if (!(img2.empty())){
                (~img2);
        }
        pthread_mutex_destroy(&amutex);
        exit(retval);
}
