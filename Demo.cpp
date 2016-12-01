#include <iostream>
#include <cv.h>
#include <highgui.h>
#include <windows.h>

#include "package_bgs/MovingDetection.h"
#include "package_bgs/PBAS/PixelBasedAdaptiveSegmenter.h"
#include "package_tracking/BlobTracking.h"
#include "package_analysis/VehicleCouting.h"


using namespace std;

int main(int argc, char **argv)
{
  /*
	// For BGSLibrary use
	cv::VideoCapture capture("D:/test.avi");
	if (!capture.isOpened()){
		cout << "Error: Video cannot be loaded" << endl;
		return -1;
	}


	// Background Subtraction Algorithm  
	IBGS *bgs;
	bgs = new PixelBasedAdaptiveSegmenter;
	
  */

	// Previous work
	MovingDetection video = MovingDetection();


  // Blob Tracking Algorithm
  cv::Mat img_blob;
  BlobTracking* blobTracking;
  blobTracking = new BlobTracking;

  // Vehicle Counting Algorithm
  VehicleCouting* vehicleCouting;
  vehicleCouting = new VehicleCouting;

  cout << "Press 'q' to quit..." <<endl;
  int key = 0;
  
  cv::Mat img_input;
  while(key != 'q')
  {
	 
    cv::Mat img_mask;
    
	// Previous work
	video.process(img_input, img_mask);


	if(!img_mask.empty())
    {
		cv::imshow("img_mask", img_mask);
		// Perform blob tracking
		blobTracking->process(img_input, img_mask, img_blob);

		// Perform vehicle counting
		vehicleCouting->setInput(img_blob);
		vehicleCouting->setTracks(blobTracking->getTracks());
		vehicleCouting->process();
    }

		key = cvWaitKey(1);
	}

	delete vehicleCouting;
	delete blobTracking;
	
	cvDestroyAllWindows();
	
	return 0;
}