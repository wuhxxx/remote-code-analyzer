#ifndef FRONTEND_H
#define FRONTEND_H
/////////////////////////////////////////////////////////////////////
// FrontEnd.h - GUI for Project 4                                  //
// Version 1.0                                                     //
// Author: Hongxin WU                                              //
// Resources from Jim Fawcett, CSE687 Spring 2017                  //
// Object Oriented Design Spring 2017 Project 4                    //
///////////////////////////////////////////////////////////////////// 


/* ============= Manual Information =================
* This package defines the GUI part of project 4.
* Build from professor's code. Implement all the 
* functionalities as required.
*/


/* ============== Public Interface ==================
* - appUI();
* - ~appUI();
*/


/* =============== Required Files ===================
* - Client.h
*/


/* ============ Maintenance Information =============
* Version 1.0 : Apr.27, 2017
* - first release
*/


using namespace System;
using namespace System::Text;
using namespace System::Windows;
using namespace System::Windows::Input;
using namespace System::Windows::Markup;
using namespace System::Windows::Media;                   // TextBlock formatting
using namespace System::Windows::Controls;                // TabControl
using namespace System::Windows::Controls::Primitives;    // StatusBar
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Windows::Threading;
using namespace System::ComponentModel;


#include "../Client/Client.h"
#include <iostream>

namespace FrontEnd {

	ref class appUI : Window {

		// Client references
		ISendr* pSendr_;
		Iclient* pClient;


		// Controls for Window
		DockPanel^ hDockPanel = gcnew DockPanel();      // support docking statusbar at bottom
		Grid^ hGrid = gcnew Grid();
		TabControl^ hTabControl = gcnew TabControl();
		TabItem^ hUploadAndPubTab = gcnew TabItem();
		TabItem^ hRepositoryTab = gcnew TabItem();
		StatusBar^ hStatusBar = gcnew StatusBar();
		StatusBarItem^ hStatusBarItem = gcnew StatusBarItem();
		TextBlock^ hStatus = gcnew TextBlock();


		// Controls for Upload And publish View
		Grid^ h1FileListGrid = gcnew Grid();
		Forms::FolderBrowserDialog^ h1FolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
		ListBox^ h1ListBox = gcnew ListBox();
		StackPanel^ hStackPanel1 = gcnew StackPanel();
		Button^ hFolderBrowseButton = gcnew Button();
		Button^ hUploadSelectFileButton = gcnew Button();
		Button^ hPublishButton = gcnew Button();


		// Controls for Repository View
		Grid^ h2FileListGrid = gcnew Grid();
		Forms::FolderBrowserDialog^ h2FolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
		ListBox^ h2ListBox = gcnew ListBox();
		StackPanel^ hStackPanel2 = gcnew StackPanel();
		StackPanel^ hStackPanel3 = gcnew StackPanel();
		Button^ hDisplayAllButton = gcnew Button();
		Button^ hDisplayDirButton = gcnew Button();
		Button^ hDisplayFileButton = gcnew Button();
		Button^ hDeleteFileButton = gcnew Button();
		Button^ hLazyDownloadButton = gcnew Button();
		Button^ hLocalOpenButton = gcnew Button();
		Button^ hIISOpenButton = gcnew Button();
		Button^ hShutDownSeverButton = gcnew Button();



	public:
		appUI(String^ severDir, String^ clientDir, int severPort, int clientPort);
		~appUI();

		void setUpStatusBar();
		void setUpTabControl();
		void setUpUploadAndPubView();
		void setUpRepositoryView();

		void OnLoaded(Object^ sender, RoutedEventArgs^ args);
		void Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args);

		void browseForFolder(Object^ sender, RoutedEventArgs^ args);
		void uploadSelectedFile(Object^ sender, RoutedEventArgs^ args);
		void makePublish(Object^ sender, RoutedEventArgs^ args);

		void displayAll(Object^ sender, RoutedEventArgs^ args);
		void displayDir(Object^ sender, RoutedEventArgs^ args);
		void displayFile(Object^ sender, RoutedEventArgs^ args);
		void deleteFile(Object^ sender, RoutedEventArgs^ args);

		void lazyDownload(Object^ sender, RoutedEventArgs^ args);
		void localOpen(Object^ sender, RoutedEventArgs^ args);
		void iisOpen(Object^ sender, RoutedEventArgs^ args);
		void shutdownSever(Object^ sender, RoutedEventArgs^ args);
		

	private:
		String^ repoDir;
		bool isDownloaded = false;
		std::string toStdString(String^ pStr);
		String^ toSystemString(std::string& str);
		void setUpRepositoryView2();
		void setUpRepositoryView3();

	};
}


#endif