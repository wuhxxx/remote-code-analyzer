#ifndef CLIENT_H
#define CLIENT_H
/////////////////////////////////////////////////////////////////////
// Client.h - Client for Project 4                                 //
// Version 1.0                                                     //
// Author: Hongxin WU                                              //
// Resources from Jim Fawcett, CSE687 Spring 2017                  //
// Object Oriented Design Spring 2017 Project 4                    //
///////////////////////////////////////////////////////////////////// 


/* ============= Manual Information =================
* This package defines the client part of this project.
* There are two pure virtual interface class ISendr()
* and Iclient(), as well as the corresponding derived
* class Sendr() and client(). These four classes alone
* with a factory class ObjectFactory() combine to build
* as a communication channel between GUI and client.
*
* ISendr() and Sendr() holds a Blocking Queue, the WPF 
* application use this class to send message to native 
* C++ console application.
*/


/* ============== Public Interface ==================
* - ISendr()
* - Iclient()
* - ObjectFactory::createSendr()
* - ObjectFactory::creatClient()
*/


/* =============== Required Files ===================
* - Sever.h
* - HttpMessage.h
* - Sockets.h
*/


/* ============ Maintenance Information =============
* Version 1.0 : Apr.27, 2017
* - first release
*/

#ifdef IN_DLL
#define DLL_DECL __declspec(dllexport)
#else
#define DLL_DECL __declspec(dllimport)
#endif


#include <string>
#include <iostream>

class ISendr {
public:
	virtual void postMessage(const std::string& msg) = 0;
};

class Iclient {
public:
	virtual void stop() = 0;
	virtual void run() = 0;
};


extern "C" {
	struct ObjectFactory
	{
		DLL_DECL ISendr* createSendr();
		DLL_DECL Iclient* creatClient(std::string clientDir, int severPort, int clientPort, ISendr* pISendr);
	};
}

#endif