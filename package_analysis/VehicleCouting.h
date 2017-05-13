#pragma once

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


#include <fstream>
#include <math.h> 
#include "../package_tracking/cvblob/cvblob.h"

//#include "../package_bgs/MovingDetection.h"

enum LaneOrientation
{
  LO_NONE       = 0,
  LO_HORIZONTAL = 1,
  LO_VERTICAL   = 2
};

enum VehiclePosition
{
	VP_NONE = 0,
	A_Left = 1,
	AB = 2,
	BC = 3,
	CD = 4,
	D_Right = 5,
};

class VehicleCouting
{
private:
  bool firstTime;
  bool showOutput;
  int key;
  cv::Mat img_input;

  //Map of id and struct CvTrack(All info) 
  cvb::CvTracks tracks;

  std::map<cvb::CvID, std::vector<CvPoint2D64f> > points;
  std::map<cvb::CvID, VehiclePosition> positions;

  LaneOrientation laneOrientation;
  long countAB;
  long countBA;
  long countCD;
  long countDC;
  int img_w;
  int img_h;
  int showAB;
  double slope_A;
  double intercept_A;
  double slope_B;
  double intercept_B;
  double slope_C;
  double intercept_C;
  double slope_D;
  double intercept_D;
  double distance_to_line_A;
  double distance_to_line_B;
  double distance_to_line_C;
  double distance_to_line_D;
  
  

public:
  VehicleCouting();
  ~VehicleCouting();

  int it_indicator;
  void setInput(const cv::Mat &i);
  void setTracks(const cvb::CvTracks &t);
  void process(int frameNumber, int presentSignal, bool outputEachFrame);
  std::vector<int> id_List;
  std::vector<int> direction_List;
  bool idExisted(int id,int direction);


// imported from signal detection
  bool isRunning = true;
  std::string command = "";
	std::ofstream jaywalkList;
	std::string color;
	
  std::string fileName, fileType;
  bool everySecond;
  int fps = 0;
  int lightDisplayLocationX;
  int x1, y1;// location of green light

private:
	VehiclePosition getVehiclePosition(const CvPoint2D64f centroid, cvb::CvID id);
	void saveConfig();
	void loadConfig();
};
