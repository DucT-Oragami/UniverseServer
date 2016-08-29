#include "Database.h"
#include "LUZFile.h"
#include "Config.h"
#include "Logger.h"

#include "RakNet\RakSleep.h"
#include "RakNet\RakNetworkFactory.h"

void start(LPCTSTR application, std::string argv){
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_MINIMIZE;
	//si.wShowWindow = SW_HIDE;

	CreateProcess(
		application,
		const_cast<char*>(argv.c_str()),
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi
	);
}

void dropAllInstances(){
	Database::Query("TRUNCATE TABLE instances");
	Database::Query("TRUNCATE TABLE sessions");
}

std::vector<std::string> getFileNames(std::vector<std::string> filePaths){
	std::vector<std::string> fileNames;
	for (int i = 0; i < filePaths.size(); i++){
		std::string::size_type last = filePaths.at(i).find_last_of("/\\");
		std::string fileWithExtension = filePaths.at(i).substr(last+1);

		std::string::size_type pos = fileWithExtension.find('.');
		fileNames.push_back(fileWithExtension.substr(0, pos));
		//fileNames.push_back(filePaths.at(i).substr(last+1));
	}
	return fileNames;
}

void initWorldServers(){

	RakPeerInterface* rakServer = RakNetworkFactory::GetRakPeerInterface();

	InitSecurity(rakServer, Config::getUseEncryption());
	SocketDescriptor socketDescriptor(Config::getPort("masterWorld"), 0);

	if (rakServer->Startup(10, 30, &socketDescriptor, 1)){
		Logger::log("MWorld", "", "started! Listening on port " + std::to_string(Config::getPort("masterWorld")));
	} else QuitError("[MWorld] server init error!");

	rakServer->SetMaximumIncomingConnections(50);

	// Remove all previous world instances
	dropAllInstances();

	// Get all luz files
	std::vector<std::string> luzFiles = LUZFile::findall(".\\Files", 0);
	std::vector<std::string> luzFileNames = getFileNames(luzFiles);
	std::vector<std::string> luzPaths = LUZFile::findall(".\\Files", 1);

	// Start the empty world, used to host players before they actually join the game
    // i.e. still browsing characters
	Logger::log("MWorld", "Start", "EMPTY", LOG_NORMAL);



	start("LUNIServer.exe", "--world EMPTY");
	for (int i = 0; i < luzFileNames.size(); i++){
		Logger::log("MWorld", "Start", luzFileNames.at(i), LOG_NORMAL);
		std::string world = Config::getFileWorld(luzFileNames.at(i));
		if (world == "EMPTY") Logger::log("MWorld", "Start", luzFileNames.at(i) + " returned EMPTY", LOG_WARNING);
		start("LUNIServer.exe", "--world " + Config::getFileWorld(luzFileNames.at(i)) + " " + luzFiles.at(i) + " " + luzPaths.at(i));
	}

	Packet *packet;

	bool MASTER_WORLD = true;

	// Ping all world servers, and disconnect failed ping ones.. or something like that
	while (MASTER_WORLD){
		RakSleep(30);
		packet = rakServer->Receive();

		if (packet == NULL) continue;
		RakNet::BitStream *data = new RakNet::BitStream(packet->data, packet->length, false);
		unsigned char packetID;
		data->Read(packetID);
		switch (packetID) {
		    case 19:
		    {

			}
		}
	}

	rakServer->Shutdown(0);
	RakNetworkFactory::DestroyRakPeerInterface(rakServer);
}