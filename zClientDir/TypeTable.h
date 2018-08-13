#pragma once
/////////////////////////////////////////////////////////////////////
// TypeTable.h - Hash Table storing types info from AST            //
// Version 1.0                                                     //
// Author: Hongxin WU                                              //
// Object Oriented Design Spring 2017 Project 2                    //
///////////////////////////////////////////////////////////////////// 


/* ============= Manual Information =================
* This head file defines a typeInfo() struct and a 
* TypeTbale() class to store type information to a
* hash table, as type name is key and typeInfo() struct
* is value.
*/


/* ============== Public Interface ==================
* - typeInfo::show() // print out typeInfo
* - TypeTable::recordToTable() // parse AST tree and record to type table
* - TypeTable::showTable() // print out type table's content
* - TypeTable::contains() // if contains certain name, return its typeInfo, else return new typeInfo
*/



/* =============== Required Files ===================
* - AbstrSynTree.h
* - GrammarHelpers.h
* - SemiExp.h
* - itokcollection.h
*/




/* ============ Maintenance Information =============
* Version 1.0 : Mar.02, 2017
* - first release
*
*/

#include <unordered_map>
#include <string>
#include <vector>
#include <iomanip>

#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../SemiExp/itokcollection.h"
#include "../SemiExp/SemiExp.h"
#include "../GrammarHelpers/GrammarHelpers.h"

using namespace CodeAnalysis;

enum typEnum {GlobalFunction, GlobalData, Namespace, Struct, Class, Enum, Typedef, Using};

struct typeInfo {
	std::string name;
	typEnum type; // can be namespace, global function, global data, struct, class, enum, typedef and using
	std::vector<std::string> children; // used when type == namespace
	std::vector<std::string> files; // if type is global function or namespace, then files may have size greater than 1
	//std::vector<std::string> paths; // if type is global function or namespace, then paths may have size greater than 1


	// print out typeInfo
	void show();
};


class TypeTable {
public:

	bool recordToTable(CodeAnalysis::ASTNode* pn);
	void showTable();
	bool contains(std::string name);
	typeInfo getByName(std::string name);

	int sizeOfTable();
private:

	int addType(std::string name, typeInfo elem);
	typeInfo handleDeclNode(CodeAnalysis::DeclarationNode decl, std::string path);
	typeInfo handleASTNode(CodeAnalysis::ASTNode* astn);

	std::unordered_map<std::string, typeInfo> types;
};