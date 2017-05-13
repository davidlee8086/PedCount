#pragma once
//Interface Collection for Motion Detection

//Opencv Library Declarations

#include "opencv2/core/core.hpp"
#include "opencv2/flann/miniflann.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"	
#include "opencv2/ml/ml.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/core/core_c.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/legacy/legacy.hpp"

#include "opencv2/video/background_segm.hpp"
//Basic C++ library Declarations
#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <windows.h>

#include <string>
#include <vector>
#include <numeric>
#include <stdio.h>
#include <process.h>
#include <direct.h> 
#include <stdlib.h>
#include <ctype.h>


#include "../package_tracking/BlobTracking.h"
#include "../package_analysis/VehicleCouting.h"
#include "../package_analysis/Record.h"


using namespace std;


class MovingDetection{


private:

	cv::VideoCapture cap;
	cv::Mat background, result, img_blob;
	
	//find all the object contours in the image
	cv::Rect box;
	vector<cv::Rect> boundRect();
	cv::RotatedRect streetcurb;
	cv::RotatedRect streetcurb2;
	int detected[100][2];

	/* 1. Traffic Light Recognition Module */
	int gap, x1, y1, y2, y3;
	int x_p, y_p;
	cv::Mat temp, frame, green, yellow, red, pedlight, singleLight;
	cv::Rect r, y, g, ped; //ROI of single light
	bool display_Ped_Light;
	cv::Size enlarge_ratio;
	// imported from signal detection
	bool isRunning = true;
	cv::String command = "";
	ofstream record, signalChange, jaywalkList;
	cv::String fileType;
	bool everySecond;
	int fps, counter, secCount, presentSignal, lightDisplayLocationX;
	double prevMeanG, prevMeanY, prevMeanR, sensitivity;
	// Sequence:
	// First[3]: Green Light, Yellow Light, Red Light
	// Second[3]: B, G, R
	double previous[3][3];

public:

	// This bool determines whether output each frame
	bool outputEachFrame;
	cv::Mat output, mask;
	std::string fileName, fileName2, fileName3, fullAddr, fullAddr2, fullAddr3;
	//Constructor
	MovingDetection();
	
	//user interface
	void menu();
	void process(cv::Mat &img_input, cv::Mat &img_mask);
	int whatIsTheFrameNumber();

	// Judge whether excecuting the blobTracking module
	bool isGreenLight;
	void checkTrafficSignal(cv::Mat &frame);

	//read in and play a video file
	int readVideo(string address);
	void playVideo();

	//moving tracking functions
	void getBackground(string fileName, string fullAddr);
	//void filterMovingObjects(string address1, string address2);
	void filterMovingObjects(string address1);
	cv::Mat filterTotalBackground(cv::Mat frame);

	cv::BackgroundSubtractorMOG2 bgSubtractor;

	//save the average static background image
	void saveBackgroundImage(string address);

	//Read User Input Functions
	cv::string readString(string message);
	double readDouble(string message);
	int readInt(string message);
	int readOddInt(string message);
};
