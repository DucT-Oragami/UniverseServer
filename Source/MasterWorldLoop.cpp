#include "Database.h"
#include "ServerDB.h"
#include "LUZFile.h"
#include "Config.h"
#include "Logger.h"
#include "AccountsDB.h"

#include "RakNet\RakSleep.h"
#include "RakNet\RakNetworkFactory.h"

#include <thread>

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
	Database::Query("TRUNCATE TABLE migrations");
}

// Periodically remove failed migrations
void cleanRoutine(){
    bool running = 1;
	while (running){
		auto qr = Database::Query("SELECT `ipaddress` FROM `migrations`");
		if (qr == NULL){
		    Logger::logError("MWorld", "MYSQL", "Database", mysql_error(Database::getConnection()));
	    }
	    if (mysql_num_rows(qr) != 0){
			int i;
			int count = 0;
			auto row = mysql_fetch_row(qr);
			for (i = 0; i < qr->row_count; i++){
				SystemAddress addr;
				addr.SetBinaryAddress(row[i]);
				if (!SessionsTable::isMigrating(addr)) count++;
			}
			if (count) Logger::log("MWorld", "Cleaner", std::to_string(count) + " failed migration(s) removed");
	    }
	    // To Do: Configure wait time
		RakSleep(10000);

	}
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

	if (rakServer->Startup(20, 30, &socketDescriptor, 1)){
		Logger::log("MWorld", "", "started! Listening on port " + std::to_string(Config::getPort("masterWorld")));
	} else QuitError("[MWorld] server init error!");

	rakServer->SetMaximumIncomingConnections(50);
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

		start("LUNIServer.exe", "--world " + world + " " + luzFiles.at(i) + " " + luzPaths.at(i));
	}

	std::thread cleaningThread(cleanRoutine);

	Packet *packet;

	bool MASTER_WORLD = true;

	while (MASTER_WORLD){
		RakSleep(30);
		packet = rakServer->Receive();

		if (packet == NULL) continue;
		// Are we receiving a packing from a valid instance?
		std::stringstream qrs;
		qrs << "SELECT count(1) FROM `instances` WHERE `server_address` = '" << packet->systemAddress.ToString() << "';";
		if (!Database::Query(qrs.str())) continue;

		RakNet::BitStream *data = new RakNet::BitStream(packet->data, packet->length, false);
		unsigned char packetID;
		data->Read(packetID);

		switch (packetID) {

			case 17:
			    Logger::log("MWORLD", "New Connection" , InstancesTable::getInstanceWorld(packet->systemAddress));
				break;

			case 20:
			    Logger::log("MWORLD", "Lost Connection" , InstancesTable::getInstanceWorld(packet->systemAddress));
			    InstancesTable::unregisterInstance(packet->systemAddress);
				break;

			default:
				Logger::log("MWORLD", "", "received unknown packet: " + RawDataToString(packet->data, packet->length));
		}
		rakServer->DeallocatePacket(packet);
	}

	rakServer->Shutdown(0);
	RakNetworkFactory::DestroyRakPeerInterface(rakServer);
}