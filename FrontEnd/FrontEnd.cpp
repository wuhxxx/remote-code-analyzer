/////////////////////////////////////////////////////////////////////
// FrontEnd.cpp - GUI for Project 4                                //
// Version 1.0                                                     //
// Author: Hongxin WU                                              //
// Resources from Jim Fawcett, CSE687 Spring 2017                  //
// Object Oriented Design Spring 2017 Project 4                    //
/////////////////////////////////////////////////////////////////////

#include "FrontEnd.h"

using namespace FrontEnd;

appUI::appUI(System::String^ severDir, System::String^ clientDir, int severPort, int clientPort) {

	repoDir = severDir;

	// set up client channel
	ObjectFactory* pobjFact = new ObjectFactory;
	pSendr_ = pobjFact->createSendr();
	pClient = pobjFact->creatClient(toStdString(clientDir), severPort, clientPort, pSendr_);
	pClient->run();
	delete pobjFact;


	// set event handlers
	this->Loaded +=
		gcnew System::Windows::RoutedEventHandler(this, &appUI::OnLoaded);
	this->Closing +=
		gcnew CancelEventHandler(this, &appUI::Unloading);

	hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &appUI::browseForFolder);
	hUploadSelectFileButton->Click += gcnew RoutedEventHandler(this, &appUI::uploadSelectedFile);
	hPublishButton->Click += gcnew RoutedEventHandler(this, &appUI::makePublish);

	hDisplayAllButton->Click += gcnew RoutedEventHandler(this, &appUI::displayAll);
	hDisplayDirButton->Click += gcnew RoutedEventHandler(this, &appUI::displayDir);
	hDisplayFileButton->Click += gcnew RoutedEventHandler(this, &appUI::displayFile);
	hDeleteFileButton->Click += gcnew RoutedEventHandler(this, &appUI::deleteFile);

	hLazyDownloadButton->Click += gcnew RoutedEventHandler(this, &appUI::lazyDownload);
	hLocalOpenButton->Click += gcnew RoutedEventHandler(this, &appUI::localOpen);
	hIISOpenButton->Click += gcnew RoutedEventHandler(this, &appUI::iisOpen);
	hShutDownSeverButton->Click += gcnew RoutedEventHandler(this, &appUI::shutdownSever);


	// set Window properties
	this->Title = "Remote Code Publisher";
	this->Width = 800;
	this->Height = 600;

	// attach dock panel to Window
	this->Content = hDockPanel;
	hDockPanel->Children->Add(hStatusBar);
	hDockPanel->SetDock(hStatusBar, Dock::Bottom);
	hDockPanel->Children->Add(hGrid);

	// setup Window controls and views
	setUpStatusBar();
	setUpTabControl();
	setUpUploadAndPubView();
	setUpRepositoryView();
}


appUI::~appUI() {
	delete pClient;
	delete pSendr_;
}


void appUI::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  GUI -> Browsing for folder";
	h1ListBox->Items->Clear();
	System::Windows::Forms::DialogResult result;
	result = h1FolderBrowserDialog->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = h1FolderBrowserDialog->SelectedPath;
		std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
		array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
		for (int i = 0; i < files->Length; ++i)
			h1ListBox->Items->Add(files[i]);
		array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
		for (int i = 0; i < dirs->Length; ++i)
			h1ListBox->Items->Add(L"<> " + dirs[i]);
	}
}


void appUI::uploadSelectedFile(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  GUI -> Uploading Files";
	int index = 0;
	int count = h1ListBox->SelectedItems->Count;
	//std::cout << "\n ---- count = " << count;
	//hStatus->Text = "Show Selected Items";
	if (count > 0) {
		isDownloaded = false;
		for each (String^ item in h1ListBox->SelectedItems)
		{
			//Console::Write(item);
			String^ ss = "sendFile:" + item;
			pSendr_->postMessage(toStdString(ss));
		}
	}
}


void appUI::makePublish(Object^ sender, RoutedEventArgs^ args) {
	std::cout << "\n  GUI -> Sending publish request";
	String^ ps = "publish:";
	pSendr_->postMessage(toStdString(ps));
}


void appUI::displayAll(Object^ sender, RoutedEventArgs^ args) {
	std::cout << "\n  GUI -> Displaying all things in repository";
	h2ListBox->Items->Clear();
	array<String^>^ files = System::IO::Directory::GetFiles(repoDir, L"*.*");
	for (int i = 0; i < files->Length; ++i)
		h2ListBox->Items->Add(files[i]);
	array<String^>^ dirs = System::IO::Directory::GetDirectories(repoDir);
	for (int i = 0; i < dirs->Length; ++i)
		h2ListBox->Items->Add(L"<> " + dirs[i]);
}


void appUI::displayDir(Object^ sender, RoutedEventArgs^ args) {
	std::cout << "\n  GUI -> Displaying categories in repository";
	h2ListBox->Items->Clear();
	array<String^>^ dirs = System::IO::Directory::GetDirectories(repoDir);
	for (int i = 0; i < dirs->Length; ++i)
		h2ListBox->Items->Add(L"<> " + dirs[i]);
}


void appUI::displayFile(Object^ sender, RoutedEventArgs^ args) {
	std::cout << "\n  GUI -> Displaying files in repository";
	h2ListBox->Items->Clear();
	array<String^>^ files = System::IO::Directory::GetFiles(repoDir, L"*.*");
	for (int i = 0; i < files->Length; ++i)
		h2ListBox->Items->Add(files[i]);
}


void appUI::deleteFile(Object^ sender, RoutedEventArgs^ args) {
	std::cout << "\n  GUI -> Deleting selected files";
	int index = 0;
	int count = h2ListBox->SelectedItems->Count;
	//hStatus->Text = "Show Selected Items";
	if (count > 0) {
		for each (String^ item in h2ListBox->SelectedItems)
		{
			//Console::Write(item);
			//"deleteFile:filePath"
			String^ ss = "deleteFile:" + item;
			pSendr_->postMessage(toStdString(ss));
		}
	}
}


void appUI::lazyDownload(Object^ sender, RoutedEventArgs^ args) {
	std::cout << "\n  GUI -> Lazy downloading";
	isDownloaded = true;
	String^ ps = "download:";
	pSendr_->postMessage(toStdString(ps));
}


void appUI::localOpen(Object^ sender, RoutedEventArgs^ args) {
	std::cout << "\n  GUI -> Opening file locally";
	int count = h2ListBox->SelectedItems->Count;
	if (count > 0) {
		if (!isDownloaded) {
			String^ ps = "download:";
			pSendr_->postMessage(toStdString(ps));
		}

		for each (String^ item in h2ListBox->SelectedItems)
		{
			//"open:filepath"
			String^ pss = "open:" + item;
			pSendr_->postMessage(toStdString(pss));
		}

	}
}


void appUI::iisOpen(Object^ sender, RoutedEventArgs^ args) {
	std::cout << "\n  GUI -> Openning file through IIS";
	int count = h2ListBox->SelectedItems->Count;
	if (count > 0) {

		for each (String^ item in h2ListBox->SelectedItems)
		{
			//"IISopen:filepath"
			String^ pss = "IISopen:" + item;
			pSendr_->postMessage(toStdString(pss));
		}

	}
	
}


void appUI::shutdownSever(Object^ sender, RoutedEventArgs^ args) {
	std::cout << "\n  GUI -> Sending shutdown signal to sever";
	//"shutDown:"
	String^ ps = "shutDown:";
	pSendr_->postMessage(toStdString(ps));
}


void appUI::setUpStatusBar()
{
	hStatusBar->Items->Add(hStatusBarItem);
	hStatus->Text = "";
	//status->FontWeight = FontWeights::Bold;
	hStatusBarItem->Content = hStatus;
	hStatusBar->Padding = Thickness(10, 2, 10, 2);
}



void appUI::setUpTabControl()
{
	hGrid->Children->Add(hTabControl);
	hUploadAndPubTab->Header = "Upload file";
	hRepositoryTab->Header = "Repository";
	hTabControl->Items->Add(hUploadAndPubTab);
	hTabControl->Items->Add(hRepositoryTab);
}


void appUI::setUpUploadAndPubView() {
	h1FileListGrid->Margin = Thickness(20);
	hUploadAndPubTab->Content = h1FileListGrid;
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	h1FileListGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	h1ListBox->SelectionMode = SelectionMode::Multiple;
	hBorder1->Child = h1ListBox;
	h1FileListGrid->SetRow(hBorder1, 0);
	h1FileListGrid->Children->Add(hBorder1);
	RowDefinition^ hRow2Def = gcnew RowDefinition();///
	hRow2Def->Height = GridLength(75);
	h1FileListGrid->RowDefinitions->Add(hRow2Def);
	hFolderBrowseButton->Content = "Browse Directory";///
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hFolderBrowseButton;
	hUploadSelectFileButton->Content = "Upload";///
	Border^ hBorder3 = gcnew Border();
	hBorder3->Width = 120;
	hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hUploadSelectFileButton;
	hPublishButton->Content = "Publish";///
	Border^ hBorder4 = gcnew Border();
	hBorder4->Width = 120;
	hBorder4->Height = 30;
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hBorder4->Child = hPublishButton;
	hStackPanel1->Children->Add(hBorder2);///
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 100;
	hStackPanel1->Children->Add(hSpacer);
	hStackPanel1->Children->Add(hBorder3);
	TextBlock^ hSpacer2 = gcnew TextBlock();
	hSpacer2->Width = 100;
	hStackPanel1->Children->Add(hSpacer2);
	hStackPanel1->Children->Add(hBorder4);
	hStackPanel1->Orientation = Orientation::Horizontal;
	hStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	h1FileListGrid->SetRow(hStackPanel1, 1);
	h1FileListGrid->Children->Add(hStackPanel1);
}


void appUI::setUpRepositoryView() {
	h2FileListGrid->Margin = Thickness(20);
	hRepositoryTab->Content = h2FileListGrid;
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	h2FileListGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	h2ListBox->SelectionMode = SelectionMode::Multiple;
	hBorder1->Child = h2ListBox;
	h2FileListGrid->SetRow(hBorder1, 0);
	h2FileListGrid->Children->Add(hBorder1);
	RowDefinition^ hRow2Def = gcnew RowDefinition();///
	hRow2Def->Height = GridLength(75);
	h2FileListGrid->RowDefinitions->Add(hRow2Def);

	setUpRepositoryView2();
	setUpRepositoryView3();
}


void appUI::setUpRepositoryView2() {
	hDisplayAllButton->Content = "Display All";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hDisplayAllButton;
	hDisplayDirButton->Content = "Display Directory";
	Border^ hBorder3 = gcnew Border();
	hBorder3->Width = 120;
	hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hDisplayDirButton;
	hDisplayFileButton->Content = "Display File";
	Border^ hBorder4 = gcnew Border();
	hBorder4->Width = 120;
	hBorder4->Height = 30;
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hBorder4->Child = hDisplayFileButton;
	hDeleteFileButton->Content = "Delete Selected";
	Border^ hBorder5 = gcnew Border();
	hBorder5->Width = 120;
	hBorder5->Height = 30;
	hBorder5->BorderThickness = Thickness(1);
	hBorder5->BorderBrush = Brushes::Black;
	hBorder5->Child = hDeleteFileButton;
	hStackPanel2->Children->Add(hBorder2);///
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 60;
	hStackPanel2->Children->Add(hSpacer);
	hStackPanel2->Children->Add(hBorder3);
	TextBlock^ hSpacer2 = gcnew TextBlock();
	hSpacer2->Width = 60;
	hStackPanel2->Children->Add(hSpacer2);
	hStackPanel2->Children->Add(hBorder4);
	TextBlock^ hSpacer3 = gcnew TextBlock();
	hSpacer3->Width = 60;
	hStackPanel2->Children->Add(hSpacer3);
	hStackPanel2->Children->Add(hBorder5);
	hStackPanel2->Orientation = Orientation::Horizontal;
	hStackPanel2->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	h2FileListGrid->SetRow(hStackPanel2, 1);
	h2FileListGrid->Children->Add(hStackPanel2);
}


void appUI::setUpRepositoryView3() {
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(75);
	h2FileListGrid->RowDefinitions->Add(hRow2Def);

	hLazyDownloadButton->Content = "Lazy Download";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hLazyDownloadButton;
	hLocalOpenButton->Content = "Open Locally";
	Border^ hBorder3 = gcnew Border();
	hBorder3->Width = 120;
	hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hLocalOpenButton;
	hIISOpenButton->Content = "Open via IIS";
	Border^ hBorder4 = gcnew Border();
	hBorder4->Width = 120;
	hBorder4->Height = 30;
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hBorder4->Child = hIISOpenButton;
	hShutDownSeverButton->Content = "Shutdown Server";
	Border^ hBorder5 = gcnew Border();
	hBorder5->Width = 120;
	hBorder5->Height = 30;
	hBorder5->BorderThickness = Thickness(1);
	hBorder5->BorderBrush = Brushes::Black;
	hBorder5->Child = hShutDownSeverButton;
	hStackPanel3->Children->Add(hBorder2);///
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 60;
	hStackPanel3->Children->Add(hSpacer);
	hStackPanel3->Children->Add(hBorder3);
	TextBlock^ hSpacer2 = gcnew TextBlock();
	hSpacer2->Width = 60;
	hStackPanel3->Children->Add(hSpacer2);
	hStackPanel3->Children->Add(hBorder4);
	TextBlock^ hSpacer3 = gcnew TextBlock();
	hSpacer3->Width = 60;
	hStackPanel3->Children->Add(hSpacer3);
	hStackPanel3->Children->Add(hBorder5);
	hStackPanel3->Orientation = Orientation::Horizontal;
	hStackPanel3->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	h2FileListGrid->SetRow(hStackPanel3, 2);
	h2FileListGrid->Children->Add(hStackPanel3);
}


std::string appUI::toStdString(String^ pStr)
{
	std::string dst;
	for (int i = 0; i < pStr->Length; ++i)
		dst += (char)pStr[i];
	return dst;
}


String^ appUI::toSystemString(std::string& str)
{
	StringBuilder^ pStr = gcnew StringBuilder();
	for (size_t i = 0; i < str.size(); ++i)
		pStr->Append((Char)str[i]);
	return pStr->ToString();
}


void appUI::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  GUI -> Window loaded");
}
void appUI::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
	Console::Write("\n  GUI -> Window closing");
}


[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{

	// command line argument:
	//String^ severDir, String^ clientDir, int severPort, int clientPort
	//appUI(String^ severDir, String^ clientDir, int severPort, int clientPort)

	int severPort = Convert::ToInt32(args[2]);
	int clientPort = Convert::ToInt32(args[3]);
	Application^ app = gcnew Application();
	app->Run(gcnew appUI(args[0], args[1], severPort, clientPort));
	Console::WriteLine(L"\n\n");

}