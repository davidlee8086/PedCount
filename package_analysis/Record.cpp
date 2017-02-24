#include "Record.h"

void Record::openRecord(std::string name) {
	record.open("traffic_record_" + name + ".txt");
	record << "\tGreen light\t\t\t" << "\tYellow light\t\t\t" << "\tRed light" << endl;
	record << "Second" << "\tBlue\tGreen\tRed\tMean" << "\tBlue\tGreen\tRed\tMean" << "\tBlue\tGreen\tRed\tMean" << endl;
}

void Record::updateRecord(std::string eachLine) {

}