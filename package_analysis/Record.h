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
#include <windows.h>
#include <fstream>
#include <cstdio>
#include <ctime>

using namespace std;

class Record {
	
private:
	// imported from signal detection
	std::ofstream record;

public:
	void Record::openRecord(std::string name);
	void Record::updateRecord(std::string eachLine);
};