#pragma once
/////////////////////////////////////////////////////////////////////
// TypeAnalyzer.h - Analyse types and write type table             //
// Version 1.0                                                     //
// Resources from Jim Fawcett, CSE687 Spring 2017                  //
// Author: Hongxin WU                                              //
// Object Oriented Design Spring 2017 Project 2                    //
///////////////////////////////////////////////////////////////////// 


/* ============= Manual Information =================
* This head file defines TypeAnalyzer() class, which
* accepts a set of file pathes and a reference of 
* empty TypeTable() and modifies it to preduce a
* useful TypeTable().
*/


/* ============== Public Interface ==================
* - TypeAnalyzer() // constructor
* - analyze() // do type analyze work
*/



/* =============== Required Files ===================
* - TypeTable.h
* - Parser.h
* - ConfigureParser.h
* - ActionsAndRules.h
*/




/* ============ Maintenance Information =============
* Version 1.0 : Mar.04, 2017
* - first release
*
*/

#include <unordered_map>
#include <vector>
#include <string>

#include "../TypeTable/TypeTable.h"
#include "../Parser/Parser.h"
#include "../Parser/ConfigureParser.h"
#include "../Parser/ActionsAndRules.h"


class TypeAnalyzer {
public:
	//using container = std::unordered_map<std::string, std::vector<std::pair<size_t, size_t>>>;

	TypeAnalyzer();
	bool analyze(std::vector<std::string>& fileMap, TypeTable& tab);
	//container getPubInfo();
private:
	CodeAnalysis::Parser* pParser_;
	CodeAnalysis::ConfigParseForCodeAnal configure_;
	CodeAnalysis::Repository* pRepo_;
	//container pubInfo;

	//void parsePubInfo(CodeAnalysis::ASTNode* pgs);
	//void insertInfo(std::string filename, std::pair<size_t, size_t> cp);
};