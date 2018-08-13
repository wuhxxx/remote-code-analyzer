
/////////////////////////////////////////////////////////////////////
// TypeTable.cpp - Hash Table storing types info from AST          //
// Version 1.0                                                     //
// Resources from Jim Fawcett, CSE687 Spring 2017                  //
// Author: Hongxin WU                                              //
// Object Oriented Design Spring 2017 Project 2                    //
///////////////////////////////////////////////////////////////////// 

#include <string>
#include <vector>
#include <unordered_map>
#include <exception>
#include <iomanip>

#include "../AbstractSyntaxTree/AbstrSynTree.h"
#include "../Parser/Parser.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/itokcollection.h"
#include "../GrammarHelpers/GrammarHelpers.h"
#include "../FileSystem/FileSystem.h"
#include "TypeTable.h"

using namespace CodeAnalysis;

// print out typeInfo
void typeInfo::show() {
	std::string enums[] = {"GlobalFunction", "GlobalData", "Namespace", "Struct", "Class", "Enum", "Typedef", "Using" };
	std::ostringstream out;
	out.setf(std::ios::adjustfield, std::ios::left);
	out << "\n    " << std::setw(8) << "name" << " : " << name;
	out << "\n    " << std::setw(8) << "type" << " : " << enums[type];
	out << "\n    " << std::setw(8) << "children" << " : { ";
	if (!children.empty()) {
		std::vector<std::string>::iterator it;
		for (it = children.begin(); it != children.end(); it++) {
			out << *it;
			if ((it + 1) != children.end()) {
				out << ", ";
			}
		}
	}
	out << " }";
	out << "\n    " << std::setw(8) << "files" << " : { ";
	if (!files.empty()) {
		std::vector<std::string>::iterator it;
		for (it = files.begin(); it != files.end(); it++) {
			out << *it;
			if ((it + 1) != files.end()) {
				out << ", ";
			}
		}
	}
	out << " }" << "\n";
	std::cout << out.str();
}


// handle insertion for type table
int TypeTable::addType(std::string name, typeInfo elem) {
	try {
		auto fre = types.find(name);
		if (fre == types.end()) {
			types.insert({ name, elem });
			return 0;
		}
		if (fre != types.end() && fre->second.type == elem.type) {
			fre->second.files.push_back(elem.files[0]);
			return 1;
		}
		return 2;
	}
	catch (std::exception& ex) {
		std::cout << "\n\n  caught exception in TypeTable::addType(): " + std::string(ex.what()) + "\n\n";
		return 3;
	}
}


// translate declaration node to typeInfo
typeInfo TypeTable::handleDeclNode(DeclarationNode decl, std::string path) {
	typeInfo re;
	Scanner::SemiExp se;
	se.clone(*(decl.pTc));
	GrammarHelper::removeComments(se);
	if (decl.declType_ == DeclType::usingDecl) {
		re.type = typEnum::Using;
		re.name = se[decl.pTc->find("using") + 1];
	}
	if (decl.declType_ == DeclType::typedefDecl) {
		re.type = typEnum::Typedef;
		re.name = se[se.length() - 2];
	}
	if (decl.declType_ == DeclType::dataDecl) {
		re.type = typEnum::GlobalData;
		size_t i = se.find("=");
		if (i == se.length()) {
			// this is just a global data declaration, no assignment
			re.name = se[se.length() - 2];
		}
		else {
			re.name = se[i - 1];
		}
	}
	re.files.push_back(decl.package_);
	return re;
}


// translate AST node to typeInfo
typeInfo TypeTable::handleASTNode(CodeAnalysis::ASTNode* astn) {
	typeInfo re;
	if (astn->type_ == "namespace") {
		re.type = typEnum::Namespace;
	}
	if (astn->type_ == "class") {
		re.type = typEnum::Class;
	}
	if (astn->type_ == "struct") {
		re.type = typEnum::Struct;
	}
	if (astn->type_ == "enum") {
		re.type = typEnum::Enum;
	}
	if (astn->type_ == "function") {
		re.type = typEnum::GlobalFunction;
	}
	re.name = astn->name_;
	re.files.push_back(astn->package_);
	return re;
}


// parse AST tree and record to type table
bool TypeTable::recordToTable(ASTNode* pn) {
	try {
		for (DeclarationNode dn : pn->decl_) {
			typeInfo dt = handleDeclNode(dn, pn->path_);
			int succ = addType(dt.name, dt);
			if (succ == 0 && pn->name_ != "Global Namespace") {
				types.at(pn->name_).children.push_back(dt.name);
			}
		}
		for (ASTNode* an : pn->children_) {
			if (an->type_ != "anonymous") {
				typeInfo at = handleASTNode(an);
				int suc = addType(at.name, at);
				if (at.type == typEnum::Namespace || at.type == typEnum::Class) {
					recordToTable(an);
				}
				if (suc == 0 && pn->name_ != "Global Namespace") {
					types.at(pn->name_).children.push_back(at.name);
				}
			}
		}
	}
	catch (std::exception& ex) {
		std::cout << "\n\n  caught exception in TypeTable::handleAST: " + std::string(ex.what()) + "\n\n";
		return false;
	}
	return true;
}


// print out type table's content
void TypeTable::showTable() {
	for (auto pair : types) {
		pair.second.show();
	}
}


// if contains certain name, return its typeInfo, else return new typeInfo
bool TypeTable::contains(std::string name) {
	auto fre = types.find(name);
	if (fre != types.end()) {
		return true;
	}
	else {
		return false;
	}
}


// return specific typeInfo corresponding to key
typeInfo TypeTable::getByName(std::string name) {
	return types[name];
}


// return the size of table
int TypeTable::sizeOfTable() {
	return types.size();
}


// test sub
#ifdef TEST_TYPETABLE
int main(int argc, char* argv[]) {

	try {
		std::vector<std::string> fileMap;
		std::string p1 = "C:/Users/Huuha/Desktop/CIS 687/Proj2/Project2/TestCode/typeTest1.h";
		std::string p2 = "C:/Users/Huuha/Desktop/CIS 687/Proj2/Project2/TestCode/typeTest2.h";
		fileMap.push_back(p1);
		fileMap.push_back(p2);

		ConfigParseForCodeAnal configure;
		Parser* pParser = configure.Build();
		Repository* pRepo = Repository::getInstance();

		for (auto file : fileMap) {
			pRepo->package() = FileSystem::Path::getName(file);
			if (pParser) {
				if (!configure.Attach(file)) {
					std::cout << "\n  could not open file " << file << std::endl;
					continue;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
				return 1;
			}
			pRepo->language() = Language::Cpp;
			pRepo->currentPath() = file;
			while (pParser->next()) {
				pParser->parse();
			}
		}
		ASTNode* pGlobalScope = pRepo->getGlobalScope();
		TypeTable rough;
		rough.recordToTable(pGlobalScope);
		rough.showTable();
	}
	catch (std::exception& except)
	{
		std::cout << "\n\n  caught exception in TypeTable::main: " + std::string(except.what()) + "\n\n";
		return 1;
	}
	return 0;
}

#endif