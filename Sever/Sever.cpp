/////////////////////////////////////////////////////////////////////
// Sever.cpp - Server for Project 4                                 //
// Version 1.0                                                     //
// Author: Hongxin WU                                              //
// Resources from Jim Fawcett, CSE687 Spring 2017                  //
// Object Oriented Design Spring 2017 Project 4                    //
/////////////////////////////////////////////////////////////////////

#include "Sever.h"
#include "../Sockets/Sockets.h"
//#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../CodePublisher/CodePublisher.h"
#include "../HttpMessage/HttpMessage.h"
#include "../FileSystem/FileSystem.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <thread>
#include <cstdio>

using namespace Utilities;

//HttpMessage msg;
//msg.addAttribute(HttpMessage::attribute("Command", "GetFiles"));
//msg.addAttribute(HttpMessage::attribute("ToAddr", "127.0.0.1:8080"));
//msg.addAttribute(HttpMessage::attribute("FromAddr", "127.0.0.1:8081"));
//msg.addAttribute(HttpMessage::attribute("Mode", "OneWay"));
//msg.addAttribute(HttpMessage::attribute("content_length", "10"));


namespace mySever {
	// receiver functionality is defined by this function
		void requestHandler::operator()(Socket socket) {
		while (true) {
			HttpMessage msg = getMessage(socket);
			if (msg.attributes().size() == 0) {
				//Empty msg
				continue;
			}
			std::cout << "\n\n----------Receive message----------\n";
			std::cout << msg.toString();
			std::cout << "-- Processing......\n";
			int messageType = processMessage(msg, socket);
			std::cout << "-- Done!";
			std::cout << "\n-----------------------------------\n";
			if (messageType == 5)
				break;
		}
		std::cout << "\n  Sever shut down!";
	}


	// read the message from client and parse to http message
	HttpMessage requestHandler::getMessage(Socket& socket) {
		HttpMessage msg;
		while (true) {
			std::string attribString = socket.recvString('\n');

			if (attribString.size() > 1) {
				HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
				msg.addAttribute(attrib);
			}
			else {
				break;
			}
		}
		if (msg.attributes().size() == 0) {
			return msg;
		}
		return msg;
	}


	// call actions corresponding to message
	// this is sever, which accepts client's message and call functions
	/*  types of messages are below:
	* - request get support files, Command 0
	msg.addAttribute(HttpMessage::attribute("Command", "0"));
	msg.addAttribute(HttpMessage::attribute("mode", "oneway"));
	msg.addAttribute(HttpMessage::parseAttribute("ToAddr:" + "localhost:808X"));
	msg.addAttribute(HttpMessage::parseAttribute("FromAddr:" + "localhost:808X"));
	* - request upload files, Command 1
	msg.addAttribute(HttpMessage::attribute("Command", "1"));
	msg.addAttribute(HttpMessage::attribute("mode", "oneway"));
	msg.addAttribute(HttpMessage::parseAttribute("ToAddr:" + "localhost:808X"));
	msg.addAttribute(HttpMessage::parseAttribute("FromAddr:" + "localhost:808X"));
	msg.addAttribute(HttpMessage::attribute("File", filename));
	msg.addAttribute(HttpMessage::attribute("content-length", sizeString));
	* - request publish, Command 2
	msg.addAttribute(HttpMessage::attribute("Command", "2"));
	msg.addAttribute(HttpMessage::attribute("mode", "oneway"));
	msg.addAttribute(HttpMessage::parseAttribute("ToAddr:" + "localhost:808X"));
	msg.addAttribute(HttpMessage::parseAttribute("FromAddr:" + "localhost:808X"));
	* - request download, Command 3
	msg.addAttribute(HttpMessage::attribute("Command", "3"));
	msg.addAttribute(HttpMessage::attribute("mode", "oneway"));
	msg.addAttribute(HttpMessage::parseAttribute("ToAddr:" + "localhost:808X"));
	msg.addAttribute(HttpMessage::parseAttribute("FromAddr:" + "localhost:808X"));
	* - request delete, Command 4
	msg.addAttribute(HttpMessage::attribute("Command", "4"));
	msg.addAttribute(HttpMessage::attribute("mode", "oneway"));
	msg.addAttribute(HttpMessage::parseAttribute("ToAddr:" + "localhost:808X"));
	msg.addAttribute(HttpMessage::parseAttribute("FromAddr:" + "localhost:808X"));
	msg.addAttribute(HttpMessage::attribute("FilePath", filepath));
	* - request shutdown sever, Command 5
	msg.addAttribute(HttpMessage::attribute("Command", "5"));
	msg.addAttribute(HttpMessage::attribute("mode", "oneway"));
	msg.addAttribute(HttpMessage::parseAttribute("ToAddr:" + "localhost:808X"));
	msg.addAttribute(HttpMessage::parseAttribute("FromAddr:" + "localhost:808X"));
	*/
	int requestHandler::processMessage(HttpMessage msg, Socket& socket) {
		int type = 6;
		if (msg.attributes()[0].first == "Command") {
			type = Converter<int>::toValue(msg.findValue("Command"));
		}
		else {
			std::cout << "\n  Unsupported message! (in requestHandler::processMessage)\n";
			return type;
		}

		switch (type) {
		case 0:
			sendSupportFile(getportNumber(msg));
			break;
		case 1:
			readFile(msg, socket);
			break;
		case 2:
			publishCode();
			break;
		case 3:
			sendHtmlFile(getportNumber(msg));
			break;
		case 4:
			deleteFile(msg);
			break;
		case 5:
			break;
		default:
			break;
		}
		return type;

	}


	// return the client's port number by parse the http message
	size_t requestHandler::getportNumber(HttpMessage msg) {
		size_t re = 0;
		std::string src = msg.findValue("FromAddr");
		if (src != "") {
			size_t pos = src.rfind(':');
			std::string portString = src.substr(pos + 1, src.size() - 1);
			re = Converter<size_t>::toValue(portString);
		}
		else {
			std::cout << "\n\n  Error in requestHandler::getportNumber(), msg.findValue(\"FromAddr\") return empty string!\n";
		}
		//std::cout << "\n  --- getportNumber() -> " << re;
		return re;
	}


	// make http message for communication
	HttpMessage requestHandler::makeMessage(commandType type, const std::string& toAddr) {
		HttpMessage msg;
		switch (type) {
		case commandType::getSupportFile:
			msg.addAttribute(HttpMessage::attribute("Command", "0"));
			break;
		case commandType::sendFile:
			msg.addAttribute(HttpMessage::attribute("Command", "1"));
			break;
		case commandType::publish:
			msg.addAttribute(HttpMessage::attribute("Command", "2"));
			break;
		case commandType::download:
			msg.addAttribute(HttpMessage::attribute("Command", "3"));
			break;
		case commandType::deleteFile:
			msg.addAttribute(HttpMessage::attribute("Command", "4"));
			break;
		case commandType::shutDown:
			msg.addAttribute(HttpMessage::attribute("Command", "5"));
			break;
		default:
			msg.clear();
			msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
			break;
		}
		msg.addAttribute(HttpMessage::attribute("mode", "oneway"));
		msg.addAttribute(HttpMessage::parseAttribute("ToAddr:" + toAddr));
		std::string portN = "localhost:" + Converter<size_t>::toString(myPortNumber);
		msg.addAttribute(HttpMessage::parseAttribute("FromAddr:" + portN));
		return msg;
	}


	// send message through socket
	void requestHandler::sendMessage(HttpMessage& msg, Socket& socket) {
		std::string msgString = msg.toString();
		socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
	}


	// send file through socket, before send file bytes, send a message
	bool requestHandler::sendFile(const std::string& filepath, size_t portNum, Socket& socket) {
		FileSystem::FileInfo fi(filepath);
		size_t fileSize = fi.size();
		std::string fsizeString = Converter<size_t>::toString(fileSize);
		std::string des = "localhost:" + Converter<size_t>::toString(portNum);

		FileSystem::File file(filepath);
		file.open(FileSystem::File::in, FileSystem::File::binary);
		if (!file.isGood()) {
			return false;
		}
			

		HttpMessage filemsg = makeMessage(commandType::sendFile, des);
		filemsg.addAttribute(HttpMessage::Attribute("File", FileSystem::Path::getName(filepath)));
		filemsg.addAttribute(HttpMessage::Attribute("content-length", fsizeString));
		sendMessage(filemsg, socket);
		
		const size_t BlockSize = 2048;
		Socket::byte buffer[BlockSize];
		while (true)
		{
			FileSystem::Block blk = file.getBlock(BlockSize);
			if (blk.size() == 0)
				break;
			for (size_t i = 0; i < blk.size(); ++i)
				buffer[i] = blk[i];
			socket.send(blk.size(), buffer);
			if (!file.isGood())
				break;
		}
		file.close();
		return true;
	}




	// send .js file and stylesheet to client
	bool requestHandler::sendSupportFile(size_t portNum) {
		try {
			///SocketSystem ss;
			SocketConnecter sc;
			while (!sc.connect("localhost", portNum)) {
				std::cout << "\n  Sever sending support files to client, can't connect to client.\n";
				::Sleep(100);
			}
			std::vector<std::string> filePathes;
			filePathes.push_back(FileSystem::Path::getFullFileSpec("../zJsCssFiles/handleScope.js"));
			filePathes.push_back(FileSystem::Path::getFullFileSpec("../zJsCssFiles/StyleSheet1.css"));
			for (auto path : filePathes) {
				sendFile(path, portNum, sc);
			}
		}
		catch (std::exception& exc) {
			std::cout << "\n  Error in sendSupportFile(): \n" + std::string(exc.what()) + "\n\n";
			return false;
		}
		return true;
	}


	// read a file from client and save
	bool requestHandler::readFile(HttpMessage msg, Socket& socket) {
		std::string ffilename = msg.findValue("File");
		std::string fsString = msg.findValue("content-length");
		size_t filesize = 0;
		if (ffilename != "" && fsString != "") {
			filesize = Converter<size_t>::toValue(fsString);
		}
		else { return false; }
		std::string fqname = hostDir + ffilename;
		FileSystem::File file(fqname);
		file.open(FileSystem::File::out, FileSystem::File::binary);
		bool fileGood = file.isGood();

		const size_t BlockSize = 2048;
		Socket::byte buffer[BlockSize];
		size_t bytesToRead;
		while (true) {
			if (filesize > BlockSize) { bytesToRead = BlockSize; }
			else { bytesToRead = filesize; }
			socket.recv(bytesToRead, buffer);
			if (fileGood) {
				FileSystem::Block blk;
				for (size_t i = 0; i < bytesToRead; ++i)
					blk.push_back(buffer[i]);

				file.putBlock(blk);
			}
			if (filesize < BlockSize)
				break;
			filesize -= BlockSize;
		}
		if (fileGood) { file.close(); }
		else { std::cout << "\n\n  can't open file " + fqname; }
		return fileGood;
	}


	// use project 3 to publish code file in hostDir
	void requestHandler::publishCode() {
		std::string path = hostDir;
		std::vector<std::string> pattern;
		pattern.push_back("*.cpp");
		pattern.push_back("*.h");
		CodePublisher tcp(path, pattern);
		tcp.go();
	}


	// send back generated html files to client
	bool requestHandler::sendHtmlFile(size_t portNum) {
		try {
			///SocketSystem ss;
			SocketConnecter sc;
			while (!sc.connect("localhost", portNum)) {
				std::cout << "\n  Sever sending html files to client, can't connect to client.\n";
				::Sleep(100);
			}
			std::vector<std::string> fileNames = FileSystem::Directory::getFiles(hostDir, "*.html");
			for (auto name : fileNames) {
				std::string path = hostDir + name;
				sendFile(path, portNum, sc);
			}
			
		}
		catch (std::exception& exc) {
			std::cout << "\n  Error in sendHtmlFile(): \n" + std::string(exc.what()) + "\n\n";
			return false;
		}
		return true;
	}


	// delete a file as request
	bool requestHandler::deleteFile(HttpMessage msg) {
		std::string path = msg.findValue("FilePath");
		if (!FileSystem::File::exists(path)) {
			std::cout << "\n\n  Client request to delete a nonexist file: " << path;
			return false;
		}
		std::remove(path.c_str());
		return true;
	}
};

using namespace mySever;

#ifdef RUN_SEVER
int main(int argc, char* argv[]) {
	::SetConsoleTitle(L"Remote Code Publisher Server - Runs Forever");
	try {
		
		std::cout << "\n  Remote Code Publisher Server started";
		std::cout << "\n---------------------------------------";
		std::string severRepo(argv[1]);
		size_t severPort = Converter<size_t>::toValue(argv[2]);
		SocketSystem ss;
		/// specify port number through command line
		SocketListener sl(severPort, Socket::IP6);
		/// specify sever's dir through command line
		requestHandler rh(severPort, severRepo);
		sl.start(rh);
		while (true) {
			// prevent process shut down
		}
	}
	catch (std::exception& exc) {
		std::cout << "\n  " + std::string(exc.what()) + "\n\n";
	}
}
#endif