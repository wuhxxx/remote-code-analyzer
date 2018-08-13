/////////////////////////////////////////////////////////////////////
// DependencyAnalyzer.cpp - Analyse file dependency                //
// Version 1.0                                                     //
// Resources from Jim Fawcett, CSE687 Spring 2017                  //
// Author: Hongxin WU                                              //
// Object Oriented Design Spring 2017 Project 2                    //
///////////////////////////////////////////////////////////////////// 

#include <string>

#include "DependencyAnalyzer.h"
#include "../TypeAnalyzer/TypeAnalyzer.h"
#include "../Analyzer/Executive.h"
#include "../Tokenizer/Tokenizer.h"
#include "../TypeTable/TypeTable.h"
#include "../FileSystem/FileSystem.h"
#include "../FileMgr/FileMgr.h"
#include "Graph.h"
#include "XmlWriter.h"
#include "XmlReader.h"
#include "../StrongComp/StrongComp.h"


 // constructor
//DependencyAnalyzer::DependencyAnalyzer() {
//	tb = nullptr;
//}
//
//
//// destructor
//DependencyAnalyzer::~DependencyAnalyzer() {
//	delete tb;
//}

// check if command line input good
bool DependencyAnalyzer::checkCommand(std::string path, std::vector<std::string> pattern) {

	try {
		subdir = FileSystem::Path::getFullFileSpec(path);
		if (!FileSystem::Directory::exists(subdir))
		{
			std::cout << "\n\n  path \"" << subdir << "\" does not exist\n\n";
			return false;
		}
		searchPattern.clear();
		for (auto patt : pattern)
		{
			searchPattern.push_back(patt);
		}
		if (searchPattern.size() == 0)
		{
			std::cout << "\n  Please input file directory and search patterns to proceed!";
			std::cout << "\n  For example: ../TestCode *.h *.cpp";
			return false;
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  Error in DependencyAnalyzer::checkCommand():";
		std::cout << "\n  " << ex.what() << "\n\n";
		return false;
	}
	return true;
}


// get file pathes from command line
bool DependencyAnalyzer::generatefilepath() {
	try {
		//fileNames.clear();
		filePathes.clear();
		fileMap.clear();
		CodeAnalysis::AnalFileMgr fm(subdir, fileMap);
		for (auto patt : searchPattern) {
			fm.addPattern(patt);
		}
		fm.search();
		for (auto item : fileMap)
		{
			for (auto file : item.second) {
				//std::cout << "\n" << file;
				//std::cout << "\n" << FileSystem::Path::getName(file);
				//fileNames.push_back(FileSystem::Path::getName(file));
				filePathes.push_back(file);
			}
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  file path parsing error:";
		std::cout << "\n  " << ex.what() << "\n\n";
		return false;
	}
	return true;
}


// check if edge exists
bool DependencyAnalyzer::checkIfEdgeExist(ver currentVertex, std::string depenFileName, size_t depenFileVertexIndex) {
	edg waitingForAdd;
	waitingForAdd.first = depenFileVertexIndex;
	waitingForAdd.second = depenFileName;
	auto it = currentVertex.begin();
	for (it; it != currentVertex.end(); it++) {
		if (*it == waitingForAdd) {
			return true;
		}
	}
	return false;
}


// helper function for dependency analysis
void DependencyAnalyzer::anaHelper(std::string tok, std::string currentFileName, TypeTable& tb) {
	size_t currentVertexID = vertexMap[currentFileName];
	size_t currentVertexIndex = depGraph.findVertexIndexById(currentVertexID);
	ver currentVertex = depGraph[currentVertexIndex];
	if (tb.contains(tok)) {
		typeInfo tokType = tb.getByName(tok);
		for (auto depenFileName : tokType.files) {
			//std::cout << "\n  Current processing file: " << currentFileName;
			//std::cout << "\n  - processing Dep file: " << depenFileName;
			if (depenFileName != currentFileName) {
				//std::cout << "\n    - File name not equal!";
				//add edge
				//first check if edge exists, if not, add corresponding edge
				size_t depenFileVertexID = vertexMap[depenFileName];
				size_t depenFileVertexIndex = depGraph.findVertexIndexById(depenFileVertexID);
				ver depenFileVertex = depGraph[depenFileVertexIndex];
				if (currentVertex.size() == 0 || !checkIfEdgeExist(currentVertex, depenFileName, depenFileVertexIndex)) {
					// depGraph.addEdge(1, parent, child);
					depGraph.addEdge(depenFileName, currentVertex, depenFileVertex);
					//std::cout << "\n    - Adding edge! Current vertex edge size = " << currentVertex.size();
				}
			}
		}
	}
}



// start dependency analysis and build graph
bool DependencyAnalyzer::anaStart(TypeTable& tb) {
	try {
		generateVetex();
		for (auto file : filePathes) {
			Scanner::Toker toker;
			toker.returnComments(false);
			//std::cout << "\n Processing file: " << file;
			std::string currentFileName = FileSystem::Path::getName(file);
			std::ifstream in(file);
			toker.attach(&in);
			do {
				std::string tok = toker.getTok();
				if (tok != "\n") {
					anaHelper(tok, currentFileName, tb);
				}
			} while (in.good());
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  Dependency analysis error:";
		std::cout << "\n  " << ex.what() << "\n\n";
		return false;
	}

	return true;
}


// find strong components
bool DependencyAnalyzer::findStrongComponent() {
	try {
		StrongComp sc(depGraph);
		strongComponents = sc.getStrongComps();
		if (strongComponents.size() > 0) {
			return true;
		}
		else {
			return false;
		}
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  Search strong component error:";
		std::cout << "\n  " << ex.what() << "\n\n";
		return false;
	}
	
}


// save analysis result as xml file to a specific file
bool DependencyAnalyzer::saveToFile(std::string path) {
	bool status;
	try {
		std::ofstream out(path);
		if (out.is_open()) {
			out << DepResult();
			out.close();
		}
		status = true;
	}
	catch (std::exception& ex) {
		std::cout << "\n\n  " << ex.what();
		status = false;
	}
	return status;
}


// print out dependency graph in xml form
void DependencyAnalyzer::showDepGraph() {
	std::cout << "\n";
	std::cout << GraphLib::GraphToXmlString(depGraph);
	std::cout << "\n";
}


// print out strong components
void DependencyAnalyzer::showSC() {
	std::cout << "\n    There are " << strongComponents.size() << " strong components";
	std::cout << "\n    The size of each strong component is " << strongComponents.size() << ". \n    Strong Components are : \n";
	for (auto fs : strongComponents) {
		for (auto f : fs) {
			std::cout << "  " << f << "  ";
		}
		std::cout << "\n";
	}

	std::cout << "\n";
}


// return the dependency graph
GraphLib::Graph<std::string, std::string> DependencyAnalyzer::getGraph() {
	return depGraph;
}


// return filePathes
std::vector<std::string> DependencyAnalyzer::getFilePathes() {
	return filePathes;
}


// generate vetex
void DependencyAnalyzer::generateVetex() {
	vertexMap.clear();
	size_t id = 0;
	for (auto fname : filePathes) {
		std::string currentFileName = FileSystem::Path::getName(fname);
		depGraph.addVertex(ver(currentFileName, id));
		vertexMap.insert({currentFileName, id});
		id++;
	}
}


// generate dependency analysis result as string
std::string DependencyAnalyzer::DepResult() {
	XmlWriter writer;
	writer.indent();
	writer.start("Dependency Analysis");
	writer.start("Dependencies");
	for (auto vertice = depGraph.begin(); vertice != depGraph.end(); vertice++) {
		std::string filename = (*vertice).value();
		writer.start(filename);
		for (auto edge = (*vertice).begin(); edge != (*vertice).end(); edge++) {
			XmlWriter temp;
			std::string depFile = depGraph[edge->first].value();
			temp.start("Depends_On").addBody(depFile).end();
			writer.addBody(temp.xml());
		}
		writer.end();
	}
	writer.end();
	writer.start("Strong_Components");
	for (auto comp : strongComponents) {
		writer.start("Component");
		for (auto fff : comp) {
			XmlWriter temp2;
			temp2.start("File").addBody(fff).end();
			writer.addBody(temp2.xml());
		}
		writer.end();
	}
	writer.end();
	writer.end();
	return writer.xml();
}





// test sub
#ifdef TEST_DEPANA
int main(int argc, char* argv[]) {
	DependencyAnalyzer tda;
	TypeAnalyzer taz;
	TypeTable ttb;
	if (tda.checkCommand(argc, argv) && tda.generatefilepath()) {
		if (taz.analyze(tda.getFilePathes(), ttb)) {
			tda.anaStart(ttb);
		}
		else {
			return 1;
		}
		
	}
	else {
		return 1;
	}
	tda.findStrongComponent();
	std::string str = tda.DepResult();
	std::cout << str << "\n";
	//tda.saveToFile();

}
#endif