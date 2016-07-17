#include "MissionDB.h"
#include "AccountsDB.h"
#include "Database.h"
#include "Logger.h"

#include <sstream>
// To Do:
// Add Missions, and initialize them

void MissionsTable::initializeAll(){
}

// Remove all missions on startup so they can be readded,
// Please ignore this, i'm still trying to figure out a good way to handle missions.
void MissionsTable::removeMissions(){
	std::stringstream eqqr;
	eqqr << "DELETE from `world_missions` WHERE `temp`='" << 1 << "';";
	Database::Query(eqqr.str());
}

void MissionsTable::initializeMission(bool temp, unsigned long missionid, long long giverid){
	std::stringstream query2;
	query2 << "INSERT INTO `world_missions` (`temp`, `missionID`, `giverID`) ";
	query2 << "VALUES('" << std::to_string(temp) << "', '" << std::to_string(missionid) << "', '" << std::to_string(giverid) << "'); ";
	auto a = Database::Query(query2.str());
	if (a == NULL){
		Logger::logError("MissionDB", "MYSQL", "init Mission", mysql_error(Database::getConnection()));
	}
}

std::vector<MISSION_DATA> MissionsTable::getMissions(long long charid){
	std::string qr = "SELECT `missionid`, `count`, UNIX_TIMESTAMP(`time`) FROM `missions` WHERE `character` = '" + std::to_string(charid) + "';";
	auto qr2 = Database::Query(qr);
	std::vector<MISSION_DATA> missions;
	if (qr2 == NULL){
		Logger::logError("MissionDB", "MYSQL", "get Missions", mysql_error(Database::getConnection()));
	}
	if (mysql_num_rows(qr2) == 0){
		return missions;
	}
	else{
		unsigned int numrows = (unsigned int)mysql_num_rows(qr2);
		missions.reserve(numrows);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr2)){
			MISSION_DATA m;
			m.missionid = std::stoi(row[0]);
			m.missioncount = std::stoi(row[1]);
			m.timestamp = std::stoi(row[2]);
			missions.push_back(m);
		}
		return missions;
	}
}

void MissionsTable::addMission(long long charid, unsigned long missionid){
	std::stringstream eqqr;
	eqqr << "INSERT INTO `luni`.`missions` (`id`, `character`, `missionid`) VALUES(NULL, '" << charid << "', '" << missionid << "');";
	Database::Query(eqqr.str());
}

// Removes all of a player's missions?
void MissionsTable::deleteMissions(long long charid){
	std::stringstream eqqr;
	eqqr << "DELETE FROM `missions` WHERE `character`='" << charid << "';";
	Database::Query(eqqr.str());
}

void MissionsTable::offerMission(long long charid, unsigned long missionid, unsigned long offererid){
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(charid));
	RakNet::BitStream *bs = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_GAME_MSG);

	bs->Write(s.activeCharId);
	bs->Write((unsigned short)248);
	bs->Write((unsigned long)missionid);
	bs->Write((unsigned long)offererid);

	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++){
		WorldServer::sendPacket(bs, sessionsz.at(k).addr);
	}
}