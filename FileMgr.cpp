/////////////////////////////////////////////////////////////////////
//  FileMgr.cpp - Manages and retrieves files based on the argument//
//  ver 2.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2013                                 //
//  Platform:      Toshiba Satellite, Windows 8.1                  //
//  Application:   Prototype for Code Similarity Analyzer          //
//  Author:        Ganesh Thiagarajan Syracuse University          //
//                 gthiagar@syr.edu                                //
/////////////////////////////////////////////////////////////////////
// Module operations:                                              //
// ==================                                              //
//           - To search the directory for the list of files based //
//             on User input.                                      //
//           - Take a path, pattern and key to search sub directry //
//             as inputs and returns the file list                 //
/////////////////////////////////////////////////////////////////////

#include "FileMgr.h"
#include <iostream>
#include "FileSystem.h"

FileMgr::files& FileMgr::search(const std::string& path, FileMgr::patterns patts,bool subDir)
{
  for (auto patt : patts){
    files temp = FileSystem::Directory::getFiles(path, patt);
	for (auto file : temp){
		std::string fullPath = path +"\\"+ file;
		_files.push_back(fullPath);
	}
  }
  if (subDir){
	  files dirs = FileSystem::Directory::getDirectories(path);
	  for (size_t i = 0; i < dirs.size(); i++){
		  if (dirs[i] == "." || dirs[i] == ".."){
			  continue;
		  }
		  else{
			  std::string newpath = path + "\\" + dirs[i];
			  search(newpath, patts,subDir);
		  }
	  }
  }
  
  return _files;
}

bool FileMgr::dirPathExists(const std::string& path)
{
	if (FileSystem::Directory::exists(path) == 0){
		return true;
	}
	else{
		return false;
	}
}

#ifdef TEST_FILEMGR

int main(int argc, char* argv[])
{
  for (int i = 0; i < argc; ++i)
    std::cout << "\n  " << argv[i];
  FileMgr fm;
  FileMgr::patterns patts{ "*.h", "*.cpp" };
  FileMgr::files foundFiles = fm.search(".", patts);
  for (auto file: foundFiles)
    std::cout << "\n  " << file;
  std::cout << "\n\n";
}
#endif
