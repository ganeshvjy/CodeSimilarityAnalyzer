
#include "AppStruct.h"

std::string element::show()
{
	std::ostringstream temp;
	temp << "(";
	temp << type;
	temp << ", ";
	temp << name;
	temp << ", ";
	temp << lineCount;
	temp << ", ";
	temp << lineBegin;
	temp << ", ";
	temp << lineEnd;
	temp << ", ";
	temp << complexity;
	temp << ")";
	return temp.str();
}