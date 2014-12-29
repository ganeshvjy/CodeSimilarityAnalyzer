#ifndef APPSTRUCT_H
#define APPSTRUCT_H
/////////////////////////////////////////////////////////////////////
//  AppStruct.h   - creates Structures needed for the application  //
//                  this header file will be added to the place    //
//                  structure declarations are needed              //
//  Language:      Visual C++ 2013                                 //
//  Platform:      Toshiba Satllite, Windows 8.1                   //
//  Application:   Prototype for Code Similarity Analyzer          //
//  Author:        Ganesh Thiagarajan, Syracuse University         //
//                 gthiagar@syr.edu                                //
/////////////////////////////////////////////////////////////////////
// Element tree:                                                   //
//     - Has details about the Scope type, name, line count, begin,//
//       end, and level (complexity).                              //
// Tree Content :                                                  //
//     - Has the details about the Function node, level, scope tree//
//       line count of the particular function.                    //
// File Details :                                                  //
//     - File details contains the details of the File name and    //
//       complete tree structure for the program                   // 


using namespace std;
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

struct element
{
	std::string type;
	std::string name;
	size_t lineCount;
	int lineBegin;
	int lineEnd;
	int complexity;
	std::string show();
};
//////////////////////////////////////////////////////////////////////////
// treeContent structure to hold the contents to be printed on to the tree

struct treeContent{
	std::string nodeFunction;
	std::vector<element> inElements;
	int complexity;
	int lineCount;
};

struct fileDetails{
	std::string Fname;
	std::vector<treeContent> FullTree;
};

struct result{
	std::string file1;
	std::string file2;
	treeContent tC1;
	treeContent tC2;
};
#endif