#pragma once
/////////////////////////////////////////////////////////////////////
// StrongComp.cpp - find strong component for a given graph class  //
// Version 1.0                                                     //
// Author: Hongxin WU                                              //
// Object Oriented Design Spring 2017 Project 2                    //
///////////////////////////////////////////////////////////////////// 


#include <stack>
#include <vector>
#include <string>

#include "../DependencyAnalyzer/Graph.h"
#include "StrongComp.h"


// constructor
StrongComp::StrongComp(GraphLib::Graph<std::string, std::string> gra) {
	g = gra;
	dfn = new int[g.size()];
	low = new int[g.size()];
	marked = new bool[g.size()];
	for (size_t i = 0; i < g.size(); i++) {
		dfn[i] = 0;
		low[i] = 0;
		marked[i] = false;
	}
}


//destructor
StrongComp::~StrongComp() {
	delete[] dfn;
	delete[] low;
	delete[] marked;
}


// return Strong Component as vectors
std::vector<std::vector<std::string>> StrongComp::getStrongComps() {
	doDFS();
	size_t maxSize = 0;
	for (size_t i = 0; i < results.size(); i++) {
		if (results[i].size() > maxSize) {
			maxSize = results[i].size();
		}
	}
	std::vector<std::vector<std::string>> strongComponents;
	for (size_t j = 0; j < results.size(); j++) {
		if (results[j].size() == maxSize) {
			strongComponents.push_back(results[j]);
		}
	}
	return strongComponents;
}


// helper function for getStrongComp()
void StrongComp::doDFS() {
	for (auto vertice = g.begin(); vertice != g.end(); vertice++) {
		size_t ci = g.findVertexIndexById(vertice->id());
		if (dfn[ci] == 0) {
			t = 0;
			tarjan(*vertice);
		}
	}
}


// implement Tarjan algorithm
void StrongComp::tarjan(GraphLib::Vertex<std::string, std::string> vertice) {
	size_t currentIndex = g.findVertexIndexById(vertice.id());
	vStack.push(vertice);
	t++;
	dfn[currentIndex] = t;
	low[currentIndex] = t;
	for (auto it = vertice.begin(); it != vertice.end(); it++) {
		size_t st = (*it).first;
		if (marked[st]) {
			continue;
		}
		if (dfn[(*it).first] == 0) {
			tarjan(g[st]);
			if (low[st] < low[currentIndex]) {
				low[currentIndex] = low[st];
			}
		}
		else {
			if (dfn[st] < low[currentIndex]) {
				low[currentIndex] = dfn[st];
			}
		}
	}
	if (dfn[currentIndex] == low[currentIndex]) {
		std::vector<std::string> aresu;
		while (vStack.top().id() != vertice.id()) {
			size_t popIndex = g.findVertexIndexById(vStack.top().id());
			marked[popIndex] = true;
			aresu.push_back(vStack.top().value());
			vStack.pop();
		}
		aresu.push_back(vStack.top().value());
		vStack.pop();
		results.push_back(aresu);
	}
}




#ifdef TEST_STRONG_COMP

using graph = GraphLib::Graph<std::string, std::string>;
using vertex = GraphLib::Vertex<std::string, std::string>;


int main() {
	graph g;
	vertex v1("v1");
	vertex v2("v2");
	vertex v3("v3");
	vertex v4("v4");
	vertex v5("v5");
	g.addVertex(v2);
	g.addVertex(v1);
	g.addVertex(v3);
	g.addVertex(v4);
	g.addVertex(v5);
	g.addEdge("e1", v1, v2);
	g.addEdge("e2", v1, v3);
	g.addEdge("e3", v2, v3);
	g.addEdge("e4", v4, v3);
	g.addEdge("e5", v5, v2);
	g.addEdge("e6", v2, v1);
	g.addEdge("e7", v3, v2);
	StrongComp sc(g);
	std::vector<std::vector<std::string>> re = sc.getStrongComps();
	std::cout << "\n There are " << re.size() << " strong components";
	std::cout << "\n The size of each strong component is " << re.size()<<". They are : \n";
	for (auto fs : re) {
		for (auto f : fs) {
			std::cout << " " << f << " ";
		}
		std::cout << "\n";
	}
	
	std::cout << "\n";
}
#endif