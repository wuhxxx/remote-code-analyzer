#pragma once
/////////////////////////////////////////////////////////////////////
// StrongComp.h - find strong component for a given graph class    //
// Version 1.0                                                     //
// Author: Hongxin WU                                              //
// Object Oriented Design Spring 2017 Project 2                    //
///////////////////////////////////////////////////////////////////// 


/* ============= Manual Information =================
* This head file defines a StrongComp() class which
* accepts a graph and use Tarjan algorithm to find
* strong components and return them as vectors of file
* name.
*/


/* ============== Public Interface ==================
* - StrongComp() // constructor
* - ~StrongComp() // destructor
* - getStrongComp() // return Strong Component as vectors
*/



/* =============== Required Files ===================
* - Graph.h
*/




/* ============ Maintenance Information =============
* Version 1.0 : Mar.06, 2017
* - first release
*
*/

#include <stack>
#include <vector>
#include <string>
#include <cstring>

#include "../DependencyAnalyzer/Graph.h"


class StrongComp {

public:
	StrongComp(GraphLib::Graph<std::string, std::string> gra);
	~StrongComp();

	std::vector<std::vector<std::string>> getStrongComps();
private:
	int t = 0;

	int* dfn;
	int* low;
	bool* marked;

	void doDFS();
	void tarjan(GraphLib::Vertex<std::string, std::string> vertice);

	std::stack<GraphLib::Vertex<std::string, std::string>> vStack;
	std::vector<std::vector<std::string>> results;
	GraphLib::Graph<std::string, std::string> g;
	
};
