#pragma once
/////////////////////////////////////////////////////////////////////
// CodePublisher.h - Publish code files                            //
// Version 1.0                                                     //
// Author: Hongxin WU                                              //
// Resources from Jim Fawcett, CSE687 Spring 2017                  //
// Object Oriented Design Spring 2017 Project 3                    //
///////////////////////////////////////////////////////////////////// 


/* ============= Manual Information =================
* This head file defines CodePublisher() class.
* A codePublisher class can  accept command line arguments 
* and get a set of cpp file, analyze these files' dependency
* and publish these file as .html files
*/


/* ============== Public Interface ==================
* - CodePublisher(int argc, char* argv[]) //constructor
* - bool go() //trigger the code publisher to generate html files
*/


/* =============== Required Files ===================
* - TypeTable.h
* - TypeAnalyzer.h
* - DependencyAnalyzer.h
* - Graph.h
* - FileSystem.h
* - FileMgr.h
* - XmlWriter.h
*/




/* ============ Maintenance Information =============
* Version 1.0 : Mar.31, 2017
* - first release
*
*/

#include <string>
#include <fstream>
#include <algorithm>

#include "../TypeTable/TypeTable.h"
#include "../TypeAnalyzer/TypeAnalyzer.h"
#include "../DependencyAnalyzer/DependencyAnalyzer.h"
#include "../DependencyAnalyzer/Graph.h"
#include "../FileSystem/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include "../DependencyAnalyzer/XmlWriter.h"

class CodePublisher {
public:
	using ver = GraphLib::Vertex<std::string, std::string>;
	//using container = std::unordered_map<std::string, std::vector<std::pair<size_t, size_t>>>;///

	CodePublisher(std::string path, std::vector<std::string> pattern);
	bool go();
private:
	std::vector<std::string> filePathes;
	GraphLib::Graph<std::string, std::string> depGraph;
	//container pubInfo;///

	std::string headElement;
	std::string targetPath;
	
	bool preProcess(std::string filepath, std::string filename);
	std::string generatePrologue(std::string filename);
	std::string generateDependencies(ver vertex);
	std::string getPath(std::string file);
	std::string generatePreContent(std::string filename);
	//std::pair<std::vector<size_t>, std::vector<size_t>> sortLines(std::string filename);///
};