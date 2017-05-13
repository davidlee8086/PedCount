#include "VehicleCouting.h"


namespace FAV1
{
  IplImage* img_input1 = 0;
  IplImage* img_input2 = 0;
  int line_A_x0 = 0;
  int line_A_y0 = 0;
  int line_A_x1 = 0;
  int line_A_y1 = 0;
  int line_B_x0 = 0;
  int line_B_y0 = 0;
  int line_B_x1 = 0;
  int line_B_y1 = 0;
  int line_C_x0 = 0;
  int line_C_y0 = 0;
  int line_C_x1 = 0;
  int line_C_y1 = 0;
  int line_D_x0 = 0;
  int line_D_y0 = 0;
  int line_D_x1 = 0;
  int line_D_y1 = 0;
  int numOfRec = 0;
  int startDraw = 0;
  bool cross_the_line = false;
  bool roi_defined = false;
  bool use_roi = true;
  void VehicleCouting_on_mouse(int evt, int x, int y, int flag, void* param)
  {
    if(!use_roi)
      return;
  
    if(evt == CV_EVENT_LBUTTONDOWN)
    {
      if(!startDraw)
      {
        line_B_x0 = x;
        line_B_y0 = y;
        startDraw = 1;
      }
      else
      {
        line_B_x1 = x;
        line_B_y1 = y;
        startDraw = 0;
        roi_defined = true;
      }
    }

    if(evt == CV_EVENT_MOUSEMOVE && startDraw)
    {
      //redraw ROI selection
      img_input2 = cvCloneImage(img_input1);
      cvLine(img_input2, cvPoint(line_B_x0,line_B_y0), cvPoint(x,y), CV_RGB(255,0,255),2);
      cvShowImage("VehicleCouting", img_input2);
      cvReleaseImage(&img_input2);
    }
  }
}



VehicleCouting::VehicleCouting() : firstTime(true), showOutput(true), key(0), countAB(0), countBA(0), countCD(0), countDC(0), showAB(0)
{
  std::cout << "VehicleCouting()" << std::endl;
}
//--------------------------------------------------------------------------------
VehicleCouting::~VehicleCouting()
{
  std::cout << "~VehicleCouting()" << std::endl;
}
//--------------------------------------------------------------------------------
void VehicleCouting::setInput(const cv::Mat &img_blob) {
	//img_blob.copyTo(img_input);
	img_input = img_blob;
}
//--------------------------------------------------------------------------------
void VehicleCouting::setTracks(const cvb::CvTracks &t)
{
  tracks = t;
}
//--------------------------------------------------------------------------------
VehiclePosition VehicleCouting::getVehiclePosition(const CvPoint2D64f centroid, cvb::CvID temp){
	VehiclePosition vehiclePosition = VP_NONE;
	cvb::CvID trackID = temp;

	
	slope_A = 1.0 * (FAV1::line_A_y1 - FAV1::line_A_y0) / (FAV1::line_A_x1 - FAV1::line_A_x0);
	intercept_A = FAV1::line_A_y0 - slope_A * FAV1::line_A_x0;// y = kx + b -> b = y - kx
	slope_B = 1.0 * (FAV1::line_B_y1 - FAV1::line_B_y0) / (FAV1::line_B_x1 - FAV1::line_B_x0);
	intercept_B = FAV1::line_B_y0 - slope_B * FAV1::line_B_x0;
	slope_C = 1.0 * (FAV1::line_C_y1 - FAV1::line_C_y0) / (FAV1::line_C_x1 - FAV1::line_C_x0);
	intercept_C = FAV1::line_C_y0 - slope_C * FAV1::line_C_x0;
	slope_D = 1.0 * (FAV1::line_D_y1 - FAV1::line_D_y0) / (FAV1::line_D_x1 - FAV1::line_D_x0);
	intercept_D = FAV1::line_D_y0 - slope_D * FAV1::line_D_x0;

	// distance_to_line_B = distance between line and points
	distance_to_line_A = (slope_A * centroid.x - centroid.y + intercept_A) / sqrt(slope_A * slope_A + 1);
	distance_to_line_B = (slope_B * centroid.x - centroid.y + intercept_B) / sqrt(slope_B * slope_B + 1);
	distance_to_line_C = (slope_C * centroid.x - centroid.y + intercept_C) / sqrt(slope_C * slope_C + 1);
	distance_to_line_D = (slope_D * centroid.x - centroid.y + intercept_D) / sqrt(slope_D * slope_D + 1);
	
	/*
	bool close_to_curb = (distance_to_line_B <5) && (distance_to_line_B >-5) && (centroid.x > (FAV1::line_B_x0 - 2)) && (centroid.x < (FAV1::line_B_x1 + 2));
	if (close_to_curb){
	*/	
	//std::cout << "\n(" << int(centroid.x) << "," << int(centroid.y) << ")" << std::endl;
	

	if (distance_to_line_A < 0){
		vehiclePosition = A_Left;
	}
	// Between A, B
	if ((distance_to_line_B <= 0) && (distance_to_line_A >= 0)){
		vehiclePosition = AB;
	}
	// Between B, C
	else if ((distance_to_line_B > 0) && (distance_to_line_C < 0)){
		vehiclePosition = BC;
	}
	else if ((distance_to_line_C >= 0) && (distance_to_line_D <= 0)){
		vehiclePosition = CD;
	}
	else if (distance_to_line_D > 0){
		vehiclePosition = D_Right;
	}

	/*
	// Code below is technically the same functionality as the code above. Distance detection output added.
	if (distance_to_line_A < 0){
		vehiclePosition = A_Left;
		if (distance_to_line_A > -2) {
			std::cout << "\n(" << int(centroid.x) << "," << int(centroid.y) << ")\tDistance to A is " << distance_to_line_A << std::endl;
		}
	}
	// Between A, B
	if ((distance_to_line_B <= 0) && (distance_to_line_A >= 0)){
		vehiclePosition = AB;
		if (distance_to_line_B > -2) {
			std::cout << "\n(" << int(centroid.x) << "," << int(centroid.y) << ")\tDistance to B is " << distance_to_line_B << std::endl;
		} 
		if (distance_to_line_A < 2) {
			std::cout << "\n(" << int(centroid.x) << "," << int(centroid.y) << ")\tDistance to A is " << distance_to_line_A << std::endl;
		}
	}
	// Between B, C
	else if ((distance_to_line_B > 0) && (distance_to_line_C < 0)){
		vehiclePosition = BC;
		if (distance_to_line_C > -2) {
			std::cout << "\n(" << int(centroid.x) << "," << int(centroid.y) << ")\tDistance to C is " << distance_to_line_B << std::endl;
		}
		if (distance_to_line_B < 2) {
			std::cout << "\n(" << int(centroid.x) << "," << int(centroid.y) << ")\tDistance to B is " << distance_to_line_A << std::endl;
		}
	}
	else if ((distance_to_line_C >= 0) && (distance_to_line_D <= 0)){
		vehiclePosition = CD;
		if (distance_to_line_D > -2) {
			std::cout << "\n(" << int(centroid.x) << "," << int(centroid.y) << ")\tDistance to D is " << distance_to_line_D << std::endl;
		}
		if (distance_to_line_C < 2) {
			std::cout << "\n(" << int(centroid.x) << "," << int(centroid.y) << ")\tDistance to C is " << distance_to_line_C << std::endl;
		}
	}
	else if (distance_to_line_D > 0){
		vehiclePosition = D_Right;
		if (distance_to_line_D < 2) {
			std::cout << "\n(" << int(centroid.x) << "," << int(centroid.y) << ")\tDistance to D is " << distance_to_line_D << std::endl;
		}
	}
	*/
	/*
	}
	*/
	return vehiclePosition;
}
//--------------------------------------------------------------------------------
void VehicleCouting::process(int frameNumber, int presentSignal, bool outputEachFrame) {
	
	
	// Setting for the output image quality
	std::vector<int> quality;
	quality.push_back(CV_IMWRITE_JPEG_QUALITY);
	quality.push_back(50);

	if (presentSignal == 1) {
		color = "Green";
	} else if (presentSignal == 2) {
		color = "Yellow";
	} else if (presentSignal == 3) {
		color = "Red";
	}
	if (img_input.empty()){
		return;
	}
    
	img_w = img_input.size().width;
	img_h = img_input.size().height;

	loadConfig();

	//--------------------------------------------------------------------------

	if(FAV1::use_roi == true && FAV1::roi_defined == false && firstTime == true) {
		do {
			cv::putText(img_input, "Please, set the counting line", cv::Point(1100,600), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0,0,255), 2);
			cv::imshow("PedestrianCount", img_input);
			FAV1::img_input1 = new IplImage(img_input);
			cvSetMouseCallback("VehicleCouting", FAV1::VehicleCouting_on_mouse, NULL);
			key = cvWaitKey(0);
			delete FAV1::img_input1;

			if(FAV1::roi_defined) {
			std::cout << "Counting line defined (" << FAV1::line_B_x0 << "," << FAV1::line_B_y0 << "," << FAV1::line_B_x1 << "," << FAV1::line_B_y1 << ")" << std::endl;
			break;
		}
		else
			std::cout << "Counting line undefined!" << std::endl;
		}
		while(1);
	}
	// Default, draw the line from XML
	if (FAV1::use_roi == true && FAV1::roi_defined == true){
		cv::line(img_input, cv::Point(FAV1::line_A_x0, FAV1::line_A_y0), cv::Point(FAV1::line_A_x1, FAV1::line_A_y1), cv::Scalar(0, 0, 255), 1);
		cv::line(img_input, cv::Point(FAV1::line_B_x0, FAV1::line_B_y0), cv::Point(FAV1::line_B_x1, FAV1::line_B_y1), cv::Scalar(0, 0, 255), 1);
		cv::line(img_input, cv::Point(FAV1::line_C_x0, FAV1::line_C_y0), cv::Point(FAV1::line_C_x1, FAV1::line_C_y1), cv::Scalar(0, 0, 255), 1);
		cv::line(img_input, cv::Point(FAV1::line_D_x0, FAV1::line_D_y0), cv::Point(FAV1::line_D_x1, FAV1::line_D_y1), cv::Scalar(0, 0, 255), 1);
	}
    
	bool ROI_OK = false;

	if (FAV1::use_roi == true && FAV1::roi_defined == true)
		ROI_OK = true;

  if (ROI_OK)
  {
	  laneOrientation = LO_NONE;

	  if (abs(FAV1::line_B_x0 - FAV1::line_B_x1) < abs(FAV1::line_B_y0 - FAV1::line_B_y1))
	  {
		  if (!firstTime)
			  //cv::putText(img_input, "Vertical", cv::Point(1100, img_h - 140), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
			  laneOrientation = LO_HORIZONTAL;
	  }

	  if (abs(FAV1::line_B_x0 - FAV1::line_B_x1) > abs(FAV1::line_B_y0 - FAV1::line_B_y1))
	  {
		  if (!firstTime)
			  //cv::putText(img_input, "Horizontal", cv::Point(1100, img_h - 140), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
			  laneOrientation = LO_VERTICAL;
		  cv::putText(img_input, "A", cv::Point(FAV1::line_A_x1 + 3, FAV1::line_A_y1 + 25), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
		  cv::putText(img_input, "B", cv::Point(FAV1::line_B_x1 + 3, FAV1::line_B_y1 + 25), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
		  cv::putText(img_input, "C", cv::Point(FAV1::line_C_x1 + 3, FAV1::line_C_y1 + 25), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
		  cv::putText(img_input, "D", cv::Point(FAV1::line_D_x1 + 3, FAV1::line_D_y1 + 25), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
	  }
	 }

	  //--------------------------------------------------------------------------
	  std::string frameN = "Frame " + std::to_string(frameNumber);
	  cv::putText(img_input, frameN, cv::Point(96, 42), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255), 2);
	  

		// frame capture：This part is for transforming video with tracking into pictures
		// 5-digit output only supports video less than 100,000 frames (1000s=16min, 10000s=2.8hour)
	  if (outputEachFrame) {
		  std::string addr = "";
		  if (frameNumber < 10) {
			  addr = "0000" + std::to_string(frameNumber) + ".jpg";
		  }
		  else if (frameNumber < 100) {
			  addr = "000" + std::to_string(frameNumber) + ".jpg";
		  }
		  else if (frameNumber < 1000) {
			  addr = "00" + std::to_string(frameNumber) + ".jpg";
		  }
		  else if (frameNumber < 10000) {
			  addr = "0" + std::to_string(frameNumber) + ".jpg";
		  }
		  else if (frameNumber < 100000) {
			  addr = std::to_string(frameNumber) + ".jpg";
		  }
		  bool bSuccess = cv::imwrite(addr, img_input, quality);
		  if (!bSuccess){
			  std::cout << "Error: Failed to save the frame" << std::endl;
		  }
	  }

	  int it_indicator = 0;
		// iterator: std::pair;
		// iterator.first:	CvID;
		// iterator.second:	CvTrack;
		// std::map<CvID, CvTrack *> CvTracks --- brief List of tracks.(begin, end)
	  for(std::map<cvb::CvID,cvb::CvTrack*>::iterator it = tracks.begin() ; it != tracks.end(); it++) {
		//std::cout << (*it).first << " => " << (*it).second << std::endl;
		cvb::CvID id = (*it).first; 
		// struct CvTrack: id
		cvb::CvTrack* track = (*it).second;

		CvPoint2D64f centroid = track->centroid;
    
		/*it_indicator++;
			std::cout << "---------------------------------------------------------------" << std::endl;
			std::cout << it_indicator << ")id:" << id << " (" << centroid.x << "," << centroid.y << ")" << std::endl;
			std::cout << "track->active:" << track->active << std::endl;
			std::cout << "track->inactive:" << track->inactive << std::endl;
			std::cout << "track->lifetime:" << track->lifetime << std::endl;
		*/
		//----------------------------------------------------------------------------

	if (points.count(id) > 0)
	{
		// Main List of id-location map
		std::map<cvb::CvID, std::vector<CvPoint2D64f>>::iterator it2 = points.find(id);
		std::vector<CvPoint2D64f> centroids = it2->second;

		std::vector<CvPoint2D64f> centroids2;
		// During the blue box period
		if (track->inactive == 0 && centroids.size() < 30)
		{
			// std::vector::push_back
			// Appends the given element value to the end of the container.
			centroids2.push_back(centroid);

			// iterate through those blob locations
			for (std::vector<CvPoint2D64f>::iterator it3 = centroids.begin(); it3 != centroids.end(); it3++)
			{
				
				cvb::CvID id2 = (*it2).first;
				// Push points back in list "centroids2"
				centroids2.push_back(*it3);
				//Real-time printing the id, location)
				std::string label = " (" + std::to_string(int(centroid.x)) + "," + std::to_string(int(centroid.y)) + ")";
				//std::string label = " (" + std::to_string(int(centroid.x)) + "," + std::to_string(int(centroid.y)) + ")";
				// args: Input, Label, Print_Location, Font, Size, Color, Thinkness
				cv::putText(img_input, label, cv::Point(int(centroid.x) + 20, int(centroid.y) - 20), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(0, 0, 255), 2);
				
				//cv::circle(img_input, cv::Point((*it3).x, (*it3).y), 3, cv::Scalar(255, 0, 0), 2);
			}
			
			/* std::map<cvb::CvID, std::vector<CvPoint2D64f> > points */
			points.erase(it2);
			points.insert(std::pair<cvb::CvID, std::vector<CvPoint2D64f>>(id, centroids2));
		}
		else
		{
			points.erase(it2);
		}
	}
	else
	{
		if (track->inactive == 0)
		{
			std::vector<CvPoint2D64f> centroids;
			centroids.push_back(centroid);
			points.insert(std::pair<cvb::CvID, std::vector<CvPoint2D64f>>(id, centroids));
		}
	}

    //----------------------------------------------------------------------------

    if(track->inactive == 0) {
      if(positions.count(id) > 0) {

        std::map<cvb::CvID, VehiclePosition>::iterator it2 = positions.find(id);
        VehiclePosition old_position = it2->second;
		VehiclePosition current_position = getVehiclePosition(centroid, id);

		std::string xing = "(" + std::to_string(int(centroid.x)) + "," + std::to_string(int(centroid.y)) + ")";

		// When the position changed,
		// If crossing line the distance to line should change slightly.
		if (current_position != old_position){
			// 1. Crossing from A_Left to AB
			if ((!idExisted(int(id), 1)) && abs(distance_to_line_A) < 5 && old_position == A_Left && current_position == AB && (centroid.x >= FAV1::line_A_x0) && (centroid.x <= FAV1::line_A_x1)) {
				
				countAB++;
				id_List.push_back(int(id));// Record the pedestrian id
				
				cv::circle(img_input, cv::Point(centroid.x, centroid.y), 3, cv::Scalar(255, 255, 255), 2);
				cv::putText(img_input, xing, cv::Point(1050, centroid.y), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 255, 255), 2);
				//std::cout << "\nFrame " << frameNumber << "\t\tid=" << id << " crossing from A to B (" << int(centroid.x) << ", " << int(centroid.y) << ")\n" << std::endl;
				std::cout << "\nFrame " << frameNumber << ": one crossing from A to B (" << int(centroid.x) << ", " << int(centroid.y) << ")\n" << std::endl;
				// Save the crossing snapshots
				std::string address = "Frame-" + std::to_string(frameNumber) + "-AB-" + xing  + ".jpg";
				jaywalkList.open("jaywalk.txt", std::ios::app);
				jaywalkList << frameNumber << "\t" << "AB\t" << color << "\t" << int(centroid.x) << "\t" << int(centroid.y) << std::endl;
				jaywalkList.close();
				bool bSuccess = cv::imwrite(address, img_input, quality);
				if (!bSuccess){
					std::cout << "Error: Failed to save the image" << std::endl;
				}
			}
			// 2. Crossing from BC to AB
			if ((!idExisted(int(id), 2)) && (abs(distance_to_line_B) < 5) && old_position == BC && current_position == AB && (centroid.x >= FAV1::line_B_x0) && (centroid.x <= FAV1::line_B_x1)) {
				
				countBA++;
				id_List.push_back(int(id));// Record the pedestrian id
				
				cv::circle(img_input, cv::Point(centroid.x, centroid.y), 3, cv::Scalar(255, 255, 255), 2);
				cv::putText(img_input, xing, cv::Point(1050, centroid.y), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 255, 255), 2);
				//std::cout << "\nFrame " << frameNumber << "\t\tid=" << id << " crossing from B to A (" << int(centroid.x) << ", " << int(centroid.y) << ")\n" << std::endl;
				std::cout << "\nFrame " << frameNumber << ": one crossing from B to A (" << int(centroid.x) << ", " << int(centroid.y) << ")\n" << std::endl;
				// Save the crossing snapshots
				std::string address = "Frame-" + std::to_string(frameNumber) + "-BA-" + xing + ".jpg";
				jaywalkList.open("jaywalk.txt", std::ios::app);
				jaywalkList << frameNumber << "\t" << "BA\t" << color << "\t" << int(centroid.x) << "\t" << int(centroid.y) << std::endl;
				jaywalkList.close();
				bool bSuccess = cv::imwrite(address, img_input, quality);
				if (!bSuccess){
					std::cout << "Error: Failed to save the image" << std::endl;
				}
			}
			// 3. Crossing from BC to CD
			if ((!idExisted(int(id), 3)) && abs(distance_to_line_C) < 5 && old_position == BC && current_position == CD && (centroid.x >= FAV1::line_C_x0) && (centroid.x <= FAV1::line_C_x1)) {
				countCD++;
				// Record the pedestrian id
				id_List.push_back(int(id));
				cv::circle(img_input, cv::Point(centroid.x, centroid.y), 3, cv::Scalar(255, 255, 255), 2);
				cv::putText(img_input, xing, cv::Point(1050, centroid.y), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 255, 255), 2);
				//std::cout << "\nFrame " << frameNumber << "\t" << color << "\tid=" << id << " crossing from C to D (" << int(centroid.x) << ", " << int(centroid.y) << ")\n" << std::endl;
				std::cout << "\nFrame " << frameNumber << ": one crossing from C to D (" << int(centroid.x) << ", " << int(centroid.y) << ")\n" << std::endl;
				// Save the crossing snapshots
				std::string address = "Frame-" + std::to_string(frameNumber) + "-CD-" + xing + ".jpg";
				jaywalkList.open("jaywalk.txt", std::ios::app);
				jaywalkList << frameNumber << "\t" << "CD\t" << color << "\t" << int(centroid.x) << "\t" << int(centroid.y) << std::endl;
				jaywalkList.close();
				bool bSuccess = cv::imwrite(address, img_input, quality);
				if (!bSuccess){
					std::cout << "Error: Failed to save the image" << std::endl;
				}
			}
			// 4. Crossing from D_Right to CD
			if ((!idExisted(int(id), 4)) && abs(distance_to_line_D) < 5 && old_position == D_Right && current_position == CD && (centroid.x >= FAV1::line_D_x0) && (centroid.x <= FAV1::line_D_x1)) {
				countDC++;
				id_List.push_back(int(id));// Record the pedestrian id
				
				cv::circle(img_input, cv::Point(centroid.x, centroid.y), 3, cv::Scalar(255, 255, 255), 2);
				cv::putText(img_input, xing, cv::Point(1050, centroid.y), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 255, 255), 2);
				//std::cout << "\nFrame " << frameNumber << "\t\tid=" << id << ": one crossing from D to C (" << int(centroid.x) << ", " << int(centroid.y) << ")\n" << std::endl;
				std::cout << "\nFrame " << frameNumber << ": one crossing from D to C (" << int(centroid.x) << ", " << int(centroid.y) << ")\n" << std::endl;
				// Save the crossing snapshots
				std::string address = "Frame-" + std::to_string(frameNumber) + "-DC-" + xing + ".jpg";
				jaywalkList.open("jaywalk.txt", std::ios::app);
				jaywalkList << frameNumber << "\t" << "DC\t" << color << "\t" << int(centroid.x) << "\t" << int(centroid.y) << std::endl;
				jaywalkList.close();
				bool bSuccess = cv::imwrite(address, img_input, quality);
				if (!bSuccess){
					std::cout << "Error: Failed to save the image" << std::endl;
				}
			}
			positions.erase(positions.find(id));
		}
	  }
	  else if (positions.count(id) <= 0)
      {
		 
        VehiclePosition vehiclePosition = getVehiclePosition(centroid, id);

        if(vehiclePosition != VP_NONE)
          positions.insert(std::pair<cvb::CvID, VehiclePosition>(id,vehiclePosition));
      }
    }
	// else (track->inactive != 0)
    /*
	else
    {
      if(positions.count(id) > 0)
        positions.erase(positions.find(id));
    }
	*/
   
    //cv::waitKey(0);
  }
  
  //--------------------------------------------------------------------------
  
  std::string countABstr = "A>>B: " + std::to_string(countAB);
  std::string countBAstr = "B>>A: " + std::to_string(countBA);
  std::string countCDstr = "C>>D: " + std::to_string(countCD);
  std::string countDCstr = "D>>C: " + std::to_string(countDC);
  std::string total = "Total: " + std::to_string(countAB + countBA + countCD + countDC);


  if(showAB == 0) {
	//A->B and B->A are both displayed.
	cv::putText(img_input, countABstr, cv::Point(5, 75), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
	cv::putText(img_input, countBAstr, cv::Point(5, 100), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
	cv::putText(img_input, countCDstr, cv::Point(5, 125), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
	cv::putText(img_input, countDCstr, cv::Point(5, 150), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
	cv::putText(img_input, total, cv::Point(5, 175), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
  }
  // Only Display A->B
  if (showAB == 1) {
	cv::putText(img_input, countABstr, cv::Point(1100, img_h - 120), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
  }
  // Only Display B->A
  if (showAB == 2) {
	cv::putText(img_input, countBAstr, cv::Point(1100, img_h - 80), cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(0, 0, 255), 2);
  }
  
if (showOutput) {
	cv::imshow("PedCount", img_input);
	cv::moveWindow("PedCount", 0, 0);

}
if (firstTime){
	saveConfig();
}
	firstTime = false;
}
//--------------------------------------------------------------------------------
// Get rid of pedestrian who has been counted in the recent crossing
bool VehicleCouting::idExisted(int id, int direction) {
	for (int i = 0; i < id_List.size(); i++) {
		//if (id_List.at(i) == id && direction_List.at(i) == direction) {
		// Only search existed id, not considering direction.
		// However, this func should only be used during green light period
		if (id_List.at(i) == id) {
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------------------
void VehicleCouting::saveConfig() {
	CvFileStorage* fs = cvOpenFileStorage("config/PedCount.xml", 0, CV_STORAGE_WRITE);
  
	cvWriteInt(fs, "showOutput", showOutput);
	cvWriteInt(fs, "showAB", showAB);

	cvWriteInt(fs, "fav1_use_roi", FAV1::use_roi);
	cvWriteInt(fs, "fav1_roi_defined", FAV1::roi_defined);
	cvWriteInt(fs, "line_A_x0", FAV1::line_A_x0);
	cvWriteInt(fs, "line_A_y0", FAV1::line_A_y0);
	cvWriteInt(fs, "line_A_x1", FAV1::line_A_x1);
	cvWriteInt(fs, "line_A_y1", FAV1::line_A_y1);
	cvWriteInt(fs, "line_B_x0", FAV1::line_B_x0);
	cvWriteInt(fs, "line_B_y0", FAV1::line_B_y0);
	cvWriteInt(fs, "line_B_x1", FAV1::line_B_x1);
	cvWriteInt(fs, "line_B_y1", FAV1::line_B_y1);
	cvWriteInt(fs, "line_C_x0", FAV1::line_C_x0);
	cvWriteInt(fs, "line_C_y0", FAV1::line_C_y0);
	cvWriteInt(fs, "line_C_x1", FAV1::line_C_x1);
	cvWriteInt(fs, "line_C_y1", FAV1::line_C_y1);
	cvWriteInt(fs, "line_D_x0", FAV1::line_D_x0);
	cvWriteInt(fs, "line_D_y0", FAV1::line_D_y0);
	cvWriteInt(fs, "line_D_x1", FAV1::line_D_x1);
	cvWriteInt(fs, "line_D_y1", FAV1::line_D_y1);

	cvReleaseFileStorage(&fs);
}
//--------------------------------------------------------------------------------
void VehicleCouting::loadConfig() {
	CvFileStorage* fs = cvOpenFileStorage("config/PedCount.xml", 0, CV_STORAGE_READ);
	/* int cvReadIntByName(const CvFileStorage* fs, const CvFileNode* map, const char* name, int default_value=0 ) */
	showOutput = cvReadIntByName(fs, 0, "showOutput", true);
	showAB = cvReadIntByName(fs, 0, "showAB", 1);
  
	FAV1::use_roi = cvReadIntByName(fs, 0, "fav1_use_roi", true);
	FAV1::roi_defined = cvReadIntByName(fs, 0, "fav1_use_roi", false);
	FAV1::line_A_x0 = cvReadIntByName(fs, 0, "line_A_x0", 0);
	FAV1::line_A_y0 = cvReadIntByName(fs, 0, "line_A_y0", 0);
	FAV1::line_A_x1 = cvReadIntByName(fs, 0, "line_A_x1", 0);
	FAV1::line_A_y1 = cvReadIntByName(fs, 0, "line_A_y1", 0);
	FAV1::line_B_x0 = cvReadIntByName(fs, 0, "line_B_x0", 0);
	FAV1::line_B_y0 = cvReadIntByName(fs, 0, "line_B_y0", 0);
	FAV1::line_B_x1 = cvReadIntByName(fs, 0, "line_B_x1", 0);
	FAV1::line_B_y1 = cvReadIntByName(fs, 0, "line_B_y1", 0);
	FAV1::line_C_x0 = cvReadIntByName(fs, 0, "line_C_x0", 0);
	FAV1::line_C_y0 = cvReadIntByName(fs, 0, "line_C_y0", 0);
	FAV1::line_C_x1 = cvReadIntByName(fs, 0, "line_C_x1", 0);
	FAV1::line_C_y1 = cvReadIntByName(fs, 0, "line_C_y1", 0);
	FAV1::line_D_x0 = cvReadIntByName(fs, 0, "line_D_x0", 0);
	FAV1::line_D_y0 = cvReadIntByName(fs, 0, "line_D_y0", 0);
	FAV1::line_D_x1 = cvReadIntByName(fs, 0, "line_D_x1", 0);
	FAV1::line_D_y1 = cvReadIntByName(fs, 0, "line_D_y1", 0);
	cvReleaseFileStorage(&fs);
}
