/////////////////////////////////////////////////////////////////////
// Client.cpp - Client for Project 4                               //
// Version 1.0                                                     //
// Author: Hongxin WU                                              //
// Resources from Jim Fawcett, CSE687 Spring 2017                  //
// Object Oriented Design Spring 2017 Project 4                    //
///////////////////////////////////////////////////////////////////// 

#define IN_DLL

#include "Client.h"
#include "../Sever/Sever.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Utilities/Utilities.h"
#include <windows.h>
#include <ShellAPI.h>
#include <string>
#include <iostream>
#include <thread>

using namespace Utilities;

// class Sender, accepts messages from client for consumption by MockChannel
class Sendr : public ISendr
{
public:
	void postMessage(const std::string& msg);
	Async::BlockingQueue<std::string>& queue();
private:
	Async::BlockingQueue<std::string> sendQ_;
};

void Sendr::postMessage(const std::string& msg)
{
	sendQ_.enQ(msg);
}

Async::BlockingQueue<std::string>& Sendr::queue() { return sendQ_; }




// the client class using Iclient interface
class client : public Iclient {

public:
	client(std::string clientDir, int severPort, int clientPort, ISendr* pISendr);
	virtual void stop() override;
	virtual void run() override;
private:
	size_t myPortNumber;
	size_t desPortNumber;
	std::string myDir;
	ISendr* pISendr_;

	std::thread thread_;
	bool state_ = true;

	HttpMessage makeMessage(mySever::commandType type, const std::string& toAddr);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string& filepath, size_t portNum, Socket& socket);
	void handleFrondEndMessage(HttpMessage::Attribute at, Socket& socket);

	void mai();
	void r2();
	void r3();
	void r4();
	void r5();
	void r6();
	void r7();
	void r8();
	void r9();
	void r10();
	void r11();
};


//constructor for client class
client::client(std::string clientDir, int severPort, int clientPort, ISendr* pISendr) : myDir(clientDir), desPortNumber(severPort), myPortNumber(clientPort), pISendr_(pISendr) {}


// function to stop client running
void client::stop() { state_ = false; }


// function for client running
void client::run() {
	mai();
	thread_ = std::thread(
		[this] {
		::SetConsoleTitle(L"Remote Code Publisher Client");
		Sendr* pSendr = dynamic_cast<Sendr*>(pISendr_);
		if (pSendr == nullptr) {
			std::cout << "\n  failed to start GUI-Client message channel\n\n";
			return;
		}
		Async::BlockingQueue<std::string>& sendQ = pSendr->queue();
		std::cout << "\n  Remote Code Publisher Client started \n---------------------------------------";
		SocketSystem ss;
		SocketListener sl(myPortNumber, Socket::IP6);
		mySever::requestHandler rh(myPortNumber, myDir);/// specify client's dir
		sl.start(rh);

		SocketConnecter sc;
		while (!sc.connect("localhost", desPortNumber)) {
			::Sleep(100);
		}

		while (state_) {
			std::string frontEndMsg = sendQ.deQ();
			HttpMessage::Attribute attr = HttpMessage::parseAttribute(frontEndMsg);
			std::cout << "\n\n-----------------------------------------";
			std::cout << "\n  Client -> message received from GUI: " << HttpMessage::attribString(attr);
			std::cout << "\n-----------------------------------------\n";
			handleFrondEndMessage(attr, sc);
		}
		std::cout << "\n  Client stoped!";
	});
}



// make http message for communication
HttpMessage client::makeMessage(mySever::commandType type, const std::string& toAddr) {
	HttpMessage msg;
	switch (type) {
	case mySever::commandType::getSupportFile:
		msg.addAttribute(HttpMessage::attribute("Command", "0"));
		break;
	case mySever::commandType::sendFile:
		msg.addAttribute(HttpMessage::attribute("Command", "1"));
		break;
	case mySever::commandType::publish:
		msg.addAttribute(HttpMessage::attribute("Command", "2"));
		break;
	case mySever::commandType::download:
		msg.addAttribute(HttpMessage::attribute("Command", "3"));
		break;
	case mySever::commandType::deleteFile:
		msg.addAttribute(HttpMessage::attribute("Command", "4"));
		break;
	case mySever::commandType::shutDown:
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
void client::sendMessage(HttpMessage& msg, Socket& socket) {
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}


// send file through socket, before send file bytes, send a message
bool client::sendFile(const std::string& filepath, size_t portNum, Socket& socket) {
	FileSystem::FileInfo fi(filepath);
	size_t fileSize = fi.size();
	std::string fsizeString = Converter<size_t>::toString(fileSize);
	std::string des = "localhost:" + Converter<size_t>::toString(portNum);

	FileSystem::File file(filepath);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	HttpMessage filemsg = makeMessage(mySever::commandType::sendFile, des);
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


// handle the http message attribute sent from front end
void client::handleFrondEndMessage(HttpMessage::Attribute at, Socket& socket) {
	if (at.first == "")
		return;
	std::string des = "localhost:" + Converter<size_t>::toString(desPortNumber);
	if (at.first == "sendFile") {
		//upload file
		sendFile(at.second, desPortNumber, socket);
	}
	if (at.first == "publish") {
		// send signal, ask sever use CodePublisher to produce html files
		sendMessage(makeMessage(mySever::commandType::publish, des), socket);
	}
	if (at.first == "download") {
		// send signal, ask sever send all html files to client
		sendMessage(makeMessage(mySever::commandType::download, des), socket);
	}
	if (at.first == "deleteFile") {
		// send signal, ask sever to delete a specific file in repo
		HttpMessage delf = makeMessage(mySever::commandType::deleteFile, des);
		delf.addAttribute(HttpMessage::attribute("FilePath", at.second));
		sendMessage(delf, socket);
	}
	if (at.first == "shutDown") {
		// send signal, ask sever to shutdown process
		sendMessage(makeMessage(mySever::commandType::shutDown, des), socket);
	}
	if (at.first == "open") {
		// open in local
		std::string whole = FileSystem::Path::getFullFileSpec(at.second);
		ShellExecuteA(GetDesktopWindow(), "open", whole.c_str(), NULL, NULL, SW_SHOW);
	}
	if (at.first == "IISopen") {
		// open through IIS
		std::string IISpath = "http://localhost:" + Converter<size_t>::toString(desPortNumber);
		size_t pos = at.second.rfind('/');
		if (pos != std::string::npos) {
			IISpath = IISpath + "/" + at.second.substr(pos + 1, at.second.size() - 1);
			std::cout << "\n -- IISpath : " << IISpath;
			ShellExecuteA(GetDesktopWindow(), "open", IISpath.c_str(), NULL, NULL, SW_SHOW);
		}
	}
	return;
}

// factory function for sender
ISendr* ObjectFactory::createSendr() { return new Sendr; }

// struct which return client() class
Iclient* ObjectFactory::creatClient(std::string clientDir, int severPort, int clientPort, ISendr* pISendr) {
	return new client(clientDir, severPort, clientPort, pISendr);
}



// demonstration
void client::mai() {
	std::cout << "\n\n  Demonstration for project 4";
	std::cout << "\n  -- Author: Hongxin WU - hwu139";
	std::cout << "\n\n      !!! IMPORTANT NOTE: Please read readme.pdf in my project4 folder before grading my project! Appreciate!!!!";
	std::cout << "\n\n" << "= = = = = = = = = = = = = = = =Start Demonstration= = = = = = = = = = = = = = = = = =\n";
	r2();
	std::cout << "\n" << "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n";
	r3();
	std::cout << "\n" << "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n";
	r4();
	std::cout << "\n" << "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n";
	r5();
	std::cout << "\n" << "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n";
	r6();
	std::cout << "\n" << "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n";
	r7();
	std::cout << "\n" << "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n";
	r8();
	std::cout << "\n" << "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n";
	r9();
	std::cout << "\n" << "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n";
	r10();
	std::cout << "\n" << "= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =\n";
	r11();
	std::cout << "\n" << "= = = = = = = = = = = = = = = = =End Demonstration= = = = = = = = = = = = = = = = = =\n";
}


void client::r2() {
	std::cout << "\n  Demonstration of requirements #1 & #2\n";
	std::cout << "\n   - I provide a FrontEnd application as GUI, it is written in CLI and uses WPF.";
	std::cout << "\n   - Please check my code in Visual Studio to verify that it is using C++ standard library's streams";
	std::cout << "\n   - for all I/O and new and delete for all heap-based memory management.\n";
}


void client::r3() {
	std::cout << "\n  Demonstration of requirements #3\n";
	std::cout << "\n   - As requested, I provide a Sever program as the server part of this project.";
	std::cout << "\n   - Check the console titled : Remote Code Publisher Server - Runs Forever";
	std::cout << "\n   - The publish functionality is embbed in thie Sever program and GUI can send request to trigger it";
}


void client::r4() {
	std::cout << "\n  Demonstration of requirements #4\n";
	std::cout << "\n   - Please try to upload some files, publish and open using GUI to verify that I meet this requirement";
}



void client::r5() {
	std::cout << "\n  Demonstration of requirements #5\n";
	std::cout << "\n   - I provide a Client program which supports functionalities described in purpose section";
	std::cout << "\n   - TA can use GUI to examine these functionalities";
}



void client::r6() {
	std::cout << "\n  Demonstration of requirements #6\n";
	std::cout << "\n   - I used professor's Socket package to implement message-passing communication system as request";
	std::cout << "\n   - TA can check my code to confirm this, also check out the console while examine GUI";
}



void client::r7() {
	std::cout << "\n  Demonstration of requirements #7\n";
	std::cout << "\n   - The communication system I implement heavily rely on professor's HttpMessage package";
	std::cout << "\n   - My Sever and Client interact with each other with http message";
	std::cout << "\n   - Check out the consoles.";
	
}


void client::r8() {
	std::cout << "\n  Demonstration of requirements #8\n";
	std::cout << "\n   - TA can use GUI to upload files to repository or download files from repository.";
	std::cout << "\n   - This demonstrates the requirement";
}



void client::r9() {
	std::cout << "\n  Demonstration of requirements #9\n";
	std::cout << "\n   - All my packages have unite test, as main() function in .cpp file.";
}

void client::r10() {
	std::cout << "\n  Demonstration of requirements #10\n";
	std::cout << "\n   - The lazy download is implemented, TA can examine that with GUI.";
}

void client::r11() {
	std::cout << "\n  Demonstration of requirements #11\n";
	std::cout << "\n   - This requirement is implemented, make sure IIS is set up, TA can examine that with GUI.";
}


#ifdef TEST_CLIENT

//----< test stub >----------------------------------------------------------

int main()
{

	ObjectFactory* pobjFact = new ObjectFactory;
	pSendr_ = pobjFact->createSendr();
	pClient = pobjFact->creatClient("../zClientDir/", 8081, 8082, pSendr_);
	pClient->run();
	delete pobjFact;
	
	/* HttpMessage::Attribute type:
	* - "sendFile:filepath"
	* - "publish:"
	* - "download:"
	* - "deleteFile:filePath"
	* - "shutDown:"
	* - "open:filepath"
	* - "IISopen:filepath"
	*/
	std::string m1 = HttpMessage::attribString(HttpMessage::attribute("shutDown",""));
	pSendr_->postMessage(m1);
	while (true) {
		//
	}
	//pClient->stop();
}
#endif