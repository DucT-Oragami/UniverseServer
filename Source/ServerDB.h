#pragma once
#include "RakNet\RakNetTypes.h"
#include "Database.h"

enum ServerRole : unsigned char{
	ROLE_CONSOLE = 0,
	ROLE_AUTH,
	ROLE_WORLD,
	ROLE_CHAT,
	ROLE_MASTER_WORLD
};

class InstancesTable {
public:
	//Registers a server instance on this ip&port, returns the instanceid
	static int registerInstance(SystemAddress addr, std::string world, unsigned short worldID);
	static int getInstanceId(unsigned short worldID);
	static int getInstanceId(SystemAddress addr);
	static int getInstanceId(std::string world);
	static std::string getInstanceWorld(int instanceID);
	//static SystemAddress getInstanceAddress(int instanceID);
	static void unregisterInstance(SystemAddress addr);
};