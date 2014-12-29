#ifndef FILEMGR_H
#define FILEMGR_H
/////////////////////////////////////////////////////////////////////
//  FileMgr.h - Manages and retrieves files based on the argument  //
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

#include <vector>
#include <string>

class FileMgr
{
public:
  typedef std::vector<std::string> files;
  typedef std::vector<std::string> patterns;
  //typedef std::vector<std::string> fullPath;
  files& search(const std::string& path, patterns patts,bool);
  bool dirPathExists(const std::string& path);
  //fullPath& searchPath(const std::string& path, files fileList);
private:
  std::string path;
  files _files;
  patterns _patterns;
  //fullPath _fullPath;
};
#endif
