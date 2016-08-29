#include "Database.h"
#include "ServerDB.h"

#include "Logger.h"

#include <sstream>
#include <iostream>

// I don't know if it's acceptible (safe) to have the variable names to be the same as in the sql table
int InstancesTable::registerInstance(SystemAddress addr, std::string world, unsigned short worldID){
	std::stringstream str;
	str << "INSERT INTO `instances` (`server_address`, `world`, `worldID`) VALUES ('" << addr.ToString() << "', '" << world << "', '" << std::to_string(worldID) << "');";
	//str << "INSERT INTO `instances` (`server_address`, `world`) VALUES ('" << addr.ToString() << "');";
	Database::Query(str.str());
	return (int)mysql_insert_id(Database::getConnection());
}

// Get instance ID by world id
int InstancesTable::getInstanceId(unsigned short worldID){
	std::stringstream str;
	str << "SELECT `instanceid` FROM `instances` WHERE `worldID` = '" << std::to_string(worldID) << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("Server DB", "MYSQL", "getting instance id", mysql_error(Database::getConnection()));
		return -1;
	}
	else{
		if (mysql_num_rows(qr) == 0){
			return -1;
		}
		else{
			auto row = mysql_fetch_row(qr);
			return std::stoi(row[0]);
		}
	}
}

// Get instance ID by world name
int InstancesTable::getInstanceId(std::string world){
	std::stringstream str;
	str << "SELECT `instanceid` FROM `instances` WHERE `world` =  '" << world.c_str() << "';";
    auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("Server DB", "MYSQL", "getting instance id", mysql_error(Database::getConnection()));
		return -1;
	}
	else{
		if (mysql_num_rows(qr) == 0){
			return -1;
		}
		else{
			auto row = mysql_fetch_row(qr);
			return std::stoi(row[0]);
		}
	}
}

// Get instance ID by address (IP + Port)
int InstancesTable::getInstanceId(SystemAddress addr){
	std::stringstream str;
	str << "SELECT `instanceid` FROM `instances` WHERE `server_address` = '" << addr.ToString() << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("SVDB", "MYSQL", "getting instance id", mysql_error(Database::getConnection()));
		return -1;
	}
	else{
		if (mysql_num_rows(qr) == 0){
			return -1;
		}
		else{
			auto row = mysql_fetch_row(qr);
			return std::stoi(row[0]);
		}
	}
}

std::string InstancesTable::getInstanceWorld(int instanceID){
	std::stringstream str;
	str << "SELECT `world` FROM `instances` WHERE `instanceid` = '" << std::to_string(instanceID) << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("Server DB", "MYSQL", "getting world", mysql_error(Database::getConnection()));
		return 0;
	}
	else{
		if (mysql_num_rows(qr) == 0){
			return 0;
		}
		else{
			auto row = mysql_fetch_row(qr);
			return row[0];
		}
	}
}

void InstancesTable::unregisterInstance(SystemAddress addr){
	std::stringstream str;
	str << "DELETE FROM `instances` WHERE `server_address` = '" << addr.ToString() << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		Logger::logError("SVDB", "MYSQL", "unregistering instance", mysql_error(Database::getConnection()));
	}
}