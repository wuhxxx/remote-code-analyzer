/////////////////////////////////////////////////////////////////////
// TypeAnalyzer.cpp - Analyse types and write type table           //
// Version 1.0                                                     //
// Resources from Jim Fawcett, CSE687 Spring 2017                  //
// Author: Hongxin WU                                              //
// Object Oriented Design Spring 2017 Project 2                    //
///////////////////////////////////////////////////////////////////// 

#include <unordered_map>
#include <vector>
#include <string>

#include "../TypeTable/TypeTable.h"
#include "../Parser/Parser.h"
#include "../Parser/ConfigureParser.h"
#include "../Parser/ActionsAndRules.h"
#include "TypeAnalyzer.h"



// constructor
TypeAnalyzer::TypeAnalyzer() {
	pParser_ = configure_.Build();
	if (pParser_ == nullptr)
	{
		throw std::exception("couldn't create parser");
	}
	pRepo_ = CodeAnalysis::Repository::getInstance();
}


// do type analyze work
bool TypeAnalyzer::analyze(std::vector<std::string>& fileMap, TypeTable& tab) {
	try {
		for (auto file : fileMap) {
			pRepo_->package() = FileSystem::Path::getName(file);

			if (!configure_.Attach(file))
			{
				std::cout << "\n  could not open file " << file << "\n";
				continue;
			}
			// parse file
			pRepo_->language() = CodeAnalysis::Language::Cpp;
			pRepo_->currentPath() = file;
			while (pParser_->next())
			{
				pParser_->parse();
			}
		}
		CodeAnalysis::ASTNode* pGlobalScope = pRepo_->getGlobalScope();
		tab.recordToTable(pGlobalScope);
		//parsePubInfo(pGlobalScope);
	}
	catch (std::exception& except) {
		std::cout << "\n\n  caught exception in TypeAnalyzer::analyze(): " + std::string(except.what()) + "\n\n";
		return false;
	}
	return true;

}


//// return pubInfo for code publish
//typename TypeAnalyzer::container TypeAnalyzer::getPubInfo() {
//	return pubInfo;
//}


//// parse the ast tree and get information for code publish
//void TypeAnalyzer::parsePubInfo(CodeAnalysis::ASTNode* pgs) {
//	try {
//		for (CodeAnalysis::ASTNode* an : pgs->children_) {
//			if (an->type_ == "class" || an->type_ == "struct" || an->type_ == "function") {
//				if (an->startLineCount_ != an->endLineCount_) {
//					std::string filename = an->package_;
//					auto cp = std::make_pair(an->startLineCount_, an->endLineCount_);
//					insertInfo(filename, cp);
//				}
//			}
//			if (an->type_ == "class" || an->type_ == "struct" || an->type_ == "namespace") { 
//				parsePubInfo(an);
//			}
//		}
//	}
//	catch (std::exception& ex) {
//		std::cout << "\n\n  caught exception in TypeAnalyzer::parsePubInfo: " + std::string(ex.what()) + "\n\n";
//		}
//}

//
//// handle insertion of pubInfo
//void TypeAnalyzer::insertInfo(std::string filename, std::pair<size_t, size_t> cp) {
//	auto search = pubInfo.find(filename);
//	if (search != pubInfo.end()) {
//		search->second.push_back(cp);
//	}
//	else {
//		std::vector<std::pair<size_t, size_t>> cps;
//		cps.push_back(cp);
//		pubInfo.insert({ filename, cps });
//	}
//}

// test sub
#ifdef TEST_TYPEANA

int main() {

	std::vector<std::string> fileMap;
	std::string p1 = "../TestCode/typeTest1.h";
	std::string p2 = "../TestCode/typeTest1.cpp";
	fileMap.push_back(p1);
	fileMap.push_back(p2);

	TypeTable tpd;
	TypeAnalyzer taz;
	taz.analyze(fileMap, tpd);
	tpd.showTable();

}


#endif