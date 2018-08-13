#ifndef Sever_H
#define Sever_H
/////////////////////////////////////////////////////////////////////
// Sever.h - Server for Project 4                                   //
// Version 1.0                                                     //
// Author: Hongxin WU                                              //
// Resources from Jim Fawcett, CSE687 Spring 2017                  //
// Object Oriented Design Spring 2017 Project 4                    //
///////////////////////////////////////////////////////////////////// 


/* ============= Manual Information =================
* This package defines the server part of this project.
* A enum of message types sent between sever and client
* is defined. Also a request handler class, which receives
* message from client and respond correspondingly.
*/


/* ============== Public Interface ==================
* - requestHandler::requestHandler()
* - requestHandler::operator()()
*/


/* =============== Required Files ===================
* - Sockets.h
* - CodePublisher.h
* - HttpMessage.h
*/


/* ============ Maintenance Information =============
* Version 1.0 : Apr.27, 2017
* - first release
*/

#include "../Sockets/Sockets.h"
//#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../CodePublisher/CodePublisher.h"
#include "../HttpMessage/HttpMessage.h"
#include "../FileSystem/FileSystem.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <thread>

namespace mySever {
	enum commandType {
		getSupportFile = 0,
		sendFile = 1,
		publish = 2,
		download = 3,
		deleteFile = 4,
		shutDown = 5
	};

	class requestHandler {

	public:
		requestHandler(size_t portNumber, std::string Dir) :myPortNumber(portNumber), hostDir(Dir) {};
		void operator()(Socket socket);
	private:
		size_t myPortNumber;
		std::string hostDir;

		HttpMessage getMessage(Socket& socket);
		int processMessage(HttpMessage msg, Socket& socket);

		size_t getportNumber(HttpMessage msg);
		HttpMessage makeMessage(commandType type, const std::string& toAddr);
		void sendMessage(HttpMessage& msg, Socket& socket);
		bool sendFile(const std::string& filepath, size_t portNum, Socket& socket);

		bool sendSupportFile(size_t portNum);
		bool readFile(HttpMessage msg, Socket& socket);
		void publishCode();
		bool sendHtmlFile(size_t portNum);
		bool deleteFile(HttpMessage msg);
	};
};



#endif