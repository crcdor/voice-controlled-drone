#include <iostream>
#include <string>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/*
 * Find Command such as
 * 
 * Climb 5 meter
 * Drop 5 meter
 * go forward 10 meter
 * go backward 10 meter
 * go left 10 meter
 * go right 10 meter
 * circle 10 meter
 * return to home
 * land here
 * fly now
 * 
 * */
using namespace std;
using namespace cv;

void mainCameraFeed(const std_msgs::String& str);
ros::Publisher pub_incoming_reply;

// ############ Start OpenCV Variable ############ 
char video_data[100000];
vector<uchar> buff;
vector<int> param = vector<int>(2);      
Mat image_capture;  
VideoCapture cap(0);			// Membuka Kamera


// ############ End OpenCV Variable ############ 

int main(int argc, char **argv){
	
	// ############  Start Video Init ############   
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);		// Set Lebar gambar
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 360);		// Set tinggi Gambar
    param[0] = CV_IMWRITE_JPEG_QUALITY;			// set tipe encoding 
	param[1] = 90;								// set kualitas encoding

	if(!cap.isOpened()){
		cout << "Error\n";
		return -1;
	}
	// ############  End Video Init ############  
	
	
	ros::init(argc, argv, "camera_feed");
	ros::NodeHandle n;
	ros::Subscriber vd_sub = n.subscribe("art_vrd/voice_data", 10, mainCameraFeed);
	pub_incoming_reply = n.advertise<std_msgs::String>("art_vrd/incoming_reply", 100);
	ROS_INFO("Starting Camera Feed.");
	ros::spin();
	return 0;
}

void mainCameraFeed(const std_msgs::String& vData){
	std_msgs::String incoming_reply;
	int x;
	
	if ( vData.data[0] == 'v' && vData.data[1] == 'f' ){
	
		cap >> image_capture;									// mengambil gambar dari kamera
		cvtColor(image_capture, image_capture, CV_8U);
					
		imencode(".jpeg",image_capture, buff,param);			// meng-encode gambar
					  
		for (x = 0; x < buff.size(); x++){						// merubah data gambar dari tipe vector ke array
			video_data[x] = buff[x];
		}
		
		std::string video_string(video_data,buff.size());
		incoming_reply.data = video_string;
		pub_incoming_reply.publish(incoming_reply);
		ROS_INFO_STREAM( "It's a vf command") ;
				
	}
	else{
		ROS_INFO_STREAM( "It's not a vf command") ;
	}	
	ROS_INFO_STREAM( "Voice Data : " << vData.data) ;
}
