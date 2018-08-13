#pragma once
/////////////////////////////////////////////////////////////////////
// DependencyAnalyzer.h - Analyse file dependency                  //
// Version 1.0                                                     //
// Author: Hongxin WU                                              //
// Object Oriented Design Spring 2017 Project 2                    //
///////////////////////////////////////////////////////////////////// 


/* ============= Manual Information =================
* This head file defines DependencyAnalyzer() class,
* which does the main thing for this project. It can
* accept command line arguments and get a set of file
* names called fileMap. This fileMap then be used by
* TypeAnalyzer to produce a TypeMap, then this class
* accepts the produced TypeMap and produces a graph 
* as result. After this, StrongComp() class is used
* to find strong component.
*/


/* ============== Public Interface ==================
* - checkCommand() // check if command line input good
* - generatefilepath() // get file pathes from command line
* - anaStart() // start dependency analysis and build graph
* - findStrongComponent() // find strong components
* - DepResult() // generate dependency analysis result as string
* - saveToFile() // save analysis result as xml file to a specific file
* - getGraph() // return the dependency graph
* - getFilePathes() // return filePathes
* - showDepGraph() // print out dependency graph in xml form
* - showSC() // print out strong components
*/


/* =============== Required Files ===================
* - TypeTable.h
* - TypeAnalyzer.h
* - Graph.h
* - Tokenizer.h
* - FileSystem.h
* - FileMgr.h
* - XmlWriter.h
* - StrongComp.h
*/




/* ============ Maintenance Information =============
* Version 1.0 : Mar.04, 2017
* - first release
*
*/

#include <string>
#include <unordered_map>

#include "../TypeTable/TypeTable.h"
#include "../TypeAnalyzer/TypeAnalyzer.h"
#include "../Tokenizer/Tokenizer.h"
#include "../FileSystem/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include "Graph.h"
#include "XmlWriter.h"
#include "XmlReader.h"
#include "../StrongComp/StrongComp.h"

class DependencyAnalyzer {

public:
	// each vertex holds a file name as V(std::string) and id(size_t)
	using ver = GraphLib::Vertex<std::string, std::string>;
	// V = file name; E = (std::string) file name on which V's file depends;
	using gra = GraphLib::Graph<std::string, std::string>;
	using edg = std::pair<int, std::string>;

	bool checkCommand(std::string path, std::vector<std::string> pattern);
	bool generatefilepath();
	bool anaStart(TypeTable& tb);
	bool findStrongComponent();
	std::string DepResult();
	bool saveToFile(std::string path = "../DependencyAnalysisResult.xml");

	void showDepGraph();
	void showSC();

	GraphLib::Graph<std::string, std::string> getGraph();
	std::vector<std::string> getFilePathes();

private:

	std::string subdir;
	std::vector<std::string> searchPattern;
	std::unordered_map<std::string, std::vector<std::string>> fileMap;
	std::vector<std::string> filePathes;
	
	bool checkIfEdgeExist(ver currentVertex, std::string depenFileName, size_t depenFileVertexIndex);
	void anaHelper(std::string tok, std::string currentFileName, TypeTable& tb);

	std::unordered_map<std::string, size_t> vertexMap; 
	gra depGraph;
	void generateVetex();

	std::vector<std::vector<std::string>> strongComponents;
};