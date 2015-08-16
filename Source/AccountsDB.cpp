#include "AccountsDB.h"
#include "CharactersDB.h"
#include "Database.h"
#include "Encryption.h"

#include <sstream>
#include <iostream>
#include <algorithm>

unsigned int AccountsTable::getAccountID(std::string username){
	auto qr = Database::Query("SELECT `id` FROM `accounts` WHERE `name` = '" + username + "' LIMIT 1;");
	if (mysql_num_rows(qr) == 0) return 0;
	auto ftc = mysql_fetch_row(qr);
	std::istringstream o(ftc[0]);
	uint r;
	o >> r;
	return r;
}

unsigned long long AccountsTable::addAccount(std::string name, std::string password){
	unsigned int id = AccountsTable::getAccountID(name);
	if (id > 0) return 0; //Account name already exists
	std::string hpw = hashPassword(password); //Hash password
	Database::Query("INSERT INTO `accounts` (`id`, `name`, `password`, `email`, `ip`, `rank`, `numChars`, `frontChar`, `lastLog`, `activeSub`, `subTime`, `legoClub`, `locked`, `banned`, `loginTries`) VALUES (NULL, '" + name + "', '" + hpw + "', '', '127.0.0.1', '0', '0', '0', CURRENT_TIMESTAMP, '0', '', '', '0', '0', '0');");
	unsigned long long accountid = mysql_insert_id(Database::getConnection());
	return accountid;
}

bool AccountsTable::checkPassword(std::string password, unsigned int accountid){
	auto qr = Database::Query("SELECT `password` FROM `accounts` WHERE `id` = '" + std::to_string(accountid) + "' LIMIT 1;");
	if (mysql_num_rows(qr) == 0) return false; //Actually this should NEVER happen
	auto r = mysql_fetch_row(qr);
	std::string pwhash = r[0];
	std::string testhash = hashPassword(password);
	if (pwhash == testhash) return true;
	return false;
}

AccountAccessInfo AccountsTable::getAccessInfo(unsigned int accountid){
	AccountAccessInfo accinf;
	auto qr = Database::Query("SELECT `locked`, `banned`, `loginTries` FROM `accounts` WHERE `id` = '" + std::to_string(accountid) + "' LIMIT 1;");
	if (mysql_num_rows(qr) == 0) { //Actually this should NEVER happen
		accinf.loginTries = -1; //If it does set login tries to an impossible value
		return accinf;
	}
	auto r = mysql_fetch_row(qr);
	if (std::stoi(r[0]) == 1) accinf.locked = true;
	if (std::stoi(r[1]) == 1) accinf.banned = true;
	accinf.loginTries = std::stoi(r[2]);
	return accinf;
}

void AccountsTable::setAccessInfo(unsigned int accountid, AccountAccessInfo info){
	std::stringstream str;
	str << "UPDATE `luni`.`accounts` SET `locked` = '";
	if (info.locked) str << "1"; else str << "0";
	str << "', `banned` = '";
	if (info.banned) str << "1"; else str << "0";
	str << "', `loginTries` = '";
	str << std::to_string(info.loginTries);
	str << "' WHERE `accounts`.`id` = ";
	str << std::to_string(accountid);
	str << ";";
}

CharacterCount AccountsTable::getCharCountInfo(unsigned int accountid){
	CharacterCount c;
	auto qr = Database::Query("SELECT `numChars`, `frontChar` FROM `accounts` WHERE `id` = '" + std::to_string(accountid) + "' LIMIT 1;");
	if (mysql_num_rows(qr) == 0) {
		return c;
	}
	auto r = mysql_fetch_row(qr);
	c.count = std::stoi(r[0]);
	c.front = std::stoi(r[1]);
	if (c.count > 4) c.count = 0;
	if (c.front > 3) c.front = 0;
	return c;
}

int AccountsTable::setFrontChar(long long charid){
	CharacterOwner o = CharactersTable::getAccountFromObjid(charid);
	AccountsTable::setFrontChar(o);
	return o.charIndex;
}

void AccountsTable::setFrontChar(CharacterOwner o){
	Database::Query("UPDATE `accounts` SET `frontChar` = " + std::to_string(o.charIndex) + " WHERE `id` = '" + std::to_string(o.accountid) + "'");
}

void AccountsTable::setNumChars(CharacterOwner o){
	Database::Query("UPDATE `accounts` SET `numChars` = " + std::to_string(o.charIndex) + " WHERE `id` = '" + std::to_string(o.accountid) + "'");
}

//std::unordered_map<SystemAddress, SessionInfo, SystemAddressHasher> SessionsTable::sessions = std::unordered_map<SystemAddress, SessionInfo, SystemAddressHasher>();

//Connection
SessionInfo SessionsTable::connect(SystemAddress address){
	//sessions.insert(std::make_pair(address, SessionInfo(address)));
	//TODO: Check for existing session, return that if it exists
	std::stringstream str;
	str << "INSERT INTO `sessions` (`ipaddress`) VALUES ('" << address.ToString() << "')";
	Database::Query(str.str());
	return SessionsTable::getClientSession(address);
}

bool SessionsTable::disconnect(SystemAddress address){
	/*auto fnd = sessions.find(address);
	if (fnd == sessions.end()) return false;
	sessions.erase(address);*/
	//TODO: Some kind of check if this server is responsible for the session
	std::stringstream str;
	str << "DELETE FROM `sessions` WHERE `ipaddress` = '" << address.ToString() << "'";
	Database::Query(str.str());
	return true;
}

SessionInfo SessionsTable::getClientSession(SystemAddress address){
	SessionInfo sinfo;
	std::stringstream str;
	str << "SELECT `ipaddress`, `sessionkey`, `phase`, `accountid`, `charid`, `zoneid` FROM `sessions` WHERE `ipaddress` = '" << address.ToString() << "'";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		std::cout << "[ACDB] [MYSQL] " << mysql_error(Database::getConnection()) << std::endl;
		return sinfo;
	}
	if (mysql_num_rows(qr) == 0 || mysql_num_rows(qr) == NULL)
		return sinfo;
	else{
		MYSQL_ROW row = mysql_fetch_row(qr);
		sinfo.addr.SetBinaryAddress(row[0]);
		sinfo.phase = static_cast<SessionPhase>(std::stoi(row[2]));
		if (row[3] != NULL){
			sinfo.accountid = std::stoi(row[3]);
		}
		if (row[4] != NULL){
			sinfo.activeCharId = std::stoll(row[4]);
		}
		if (row[5] != NULL){
			sinfo.zone = std::stoi(row[5]);
		}
		return sinfo;
	}
	
	//auto fnd = sessions.find(address);
	//if (fnd == sessions.end()) return SessionInfo();
	//return fnd->second;
}

//Authentification
SessionInfo SessionsTable::login(SystemAddress address, unsigned int accountid){
	SessionInfo s = SessionsTable::getClientSession(address);
	if (s.phase != SessionPhase::PHASE_NONE){
		s.phase = SessionPhase::PHASE_AUTHENTIFIED;
		s.accountid = accountid;
		std::stringstream str;
		str << "UPDATE `sessions` SET `phase` = '" << std::to_string((uchar)s.phase) << "', `accountid` = '" << std::to_string(s.accountid) << "' WHERE `ipaddress` = '" << address.ToString() << "'";
		auto qr = Database::Query(str.str());
		if (qr == NULL){
			std::cout << "[ACDB] [MYSQL] " << mysql_error(Database::getConnection()) << std::endl;
		}
	}
	return s;
	
	/*auto session = sessions.find(address);
	if (session == sessions.end()) return SessionInfo();

	SessionInfo info = session->second;
	info.phase = SessionPhase::PHASE_AUTHENTIFIED;
	info.accountid = accountid;

	sessions.at(address) = info;
	return info;*/
}

SessionInfo SessionsTable::logout(unsigned int accountid){
	SystemAddress addr = SessionsTable::findAccount(accountid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		SessionInfo s = SessionsTable::getClientSession(addr);
		if (s.phase > SessionPhase::PHASE_CONNECTED){
			s.phase = SessionPhase::PHASE_CONNECTED;
			s.accountid = 0;
			std::stringstream str;
			str << "UPDATE `sessions` SET `phase` = '" << std::to_string((uchar)s.phase) << "', `accountid` = '" << std::to_string(s.accountid) << "' WHERE `ipaddress` = '" << addr.ToString() << "'";
			auto qr = Database::Query(str.str());
			if (qr == NULL){
				std::cout << "[ACDB] [MYSQL] " << mysql_error(Database::getConnection()) << std::endl;
			}
		}

		/*auto session = sessions.find(addr);
		if (session == sessions.end())return SessionInfo();

		SessionInfo info = session->second;
		info.accountid = 0;
		info.phase = SessionPhase::PHASE_CONNECTED;
		sessions.at(addr) = info;*/
		return s;
	}
	else{
		return SessionInfo();
	}
}

SystemAddress SessionsTable::findAccount(unsigned int accountid){
	std::stringstream str;
	str << "SELECT `ipaddress` FROM `sessions` WHERE `accountid` = '" << std::to_string(accountid) << "'";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		std::cout << "[ACDB] [MYSQL] " << mysql_error(Database::getConnection()) << std::endl;
		return UNASSIGNED_SYSTEM_ADDRESS;
	}
	else{
		SystemAddress addr;
		auto row = mysql_fetch_row(qr);
		addr.SetBinaryAddress(row[0]);
		return addr;
	}
	/*std::unordered_map<SystemAddress, SessionInfo, SystemAddressHasher>::iterator it = std::find_if(sessions.begin(), sessions.end(), [&](const std::pair<SystemAddress, SessionInfo> & o){
		return o.second.accountid == accountid;
	});
	if (it == sessions.end()){
		return UNASSIGNED_SYSTEM_ADDRESS;
	}
	else{
		return it->first;
	}*/
}

//Character selection
SessionInfo SessionsTable::play(unsigned int accountid, long long charid){
	SystemAddress addr = SessionsTable::findAccount(accountid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		SessionInfo s = SessionsTable::getClientSession(addr);
		if (s.phase == SessionPhase::PHASE_AUTHENTIFIED){
			s.phase = SessionPhase::PHASE_PLAYING;
			s.activeCharId = charid;
			std::stringstream str;
			str << "UPDATE `sessions` SET `phase` = '" << std::to_string((uchar)s.phase) << "', `charid` = '" << std::to_string(s.activeCharId) << "' WHERE `ipaddress` = '" << addr.ToString() << "'";
			auto qr = Database::Query(str.str());
			if (qr == NULL){
				std::cout << "[ACDB] [MYSQL] " << mysql_error(Database::getConnection()) << std::endl;
			}
		}
		return s;
		
		/*auto session = sessions.find(addr);
		if (session == sessions.end())return SessionInfo();

		SessionInfo info = session->second;
		info.activeCharId = charid;
		info.phase = SessionPhase::PHASE_PLAYING;
		sessions.at(addr) = info;
		return info;*/
	}
	else{
		return SessionInfo();
	}
}

SessionInfo SessionsTable::quit(long long charid){
	SystemAddress addr = SessionsTable::findCharacter(charid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		SessionInfo s = SessionsTable::getClientSession(addr);
		if (s.phase > SessionPhase::PHASE_AUTHENTIFIED){
			s.phase = SessionPhase::PHASE_AUTHENTIFIED;
			s.worldJoin = 0;
			s.activeCharId = -1;
			std::stringstream str;
			str << "UPDATE `sessions` SET `phase` = '" << std::to_string((uchar)s.phase) << "', `charid` = '" << std::to_string(s.activeCharId) << "' WHERE `ipaddress` = '" << addr.ToString() << "'";
			auto qr = Database::Query(str.str());
			if (qr == NULL){
				std::cout << "[ACDB] [MYSQL] " << mysql_error(Database::getConnection()) << std::endl;
			}
		}
		return s;
		/*auto session = sessions.find(addr);
		if (session == sessions.end())return SessionInfo();

		SessionInfo info = session->second;
		info.activeCharId = -1;
		info.worldJoin = 0;
		info.phase = SessionPhase::PHASE_AUTHENTIFIED;
		sessions.at(addr) = info;
		return info;*/
	}
	else{
		return SessionInfo();
	}
}

SystemAddress SessionsTable::findCharacter(long long charid){
	std::stringstream str;
	str << "SELECT `ipaddress` FROM `sessions` WHERE `charid` = '" << std::to_string(charid) << "'";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		std::cout << "[ACDB] [MYSQL] " << mysql_error(Database::getConnection()) << std::endl;
		return UNASSIGNED_SYSTEM_ADDRESS;
	}
	else{
		if (mysql_num_rows(qr) > 0){
			SystemAddress addr;
			auto row = mysql_fetch_row(qr);
			addr.SetBinaryAddress(row[0]);
			return addr;
		}
		else{
			return UNASSIGNED_SYSTEM_ADDRESS;
		}
		
	}
	/*std::unordered_map<SystemAddress, SessionInfo, SystemAddressHasher>::iterator it = std::find_if(sessions.begin(), sessions.end(), [&](const std::pair<SystemAddress, SessionInfo> & o){
		return o.second.activeCharId == charid;
	});
	if (it == sessions.end()){
		return UNASSIGNED_SYSTEM_ADDRESS;
	}
	else{
		return it->first;
	}*/
}

std::vector<SessionInfo> SessionsTable::getClientsInWorld(unsigned short zoneid){
	std::vector<SessionInfo> query;
	std::stringstream str;
	str << "SELECT `ipaddress`, `sessionkey`, `phase`, `accountid`, `charid`, `zoneid` FROM `sessions` WHERE `zoneid` = '" << std::to_string(zoneid) << "'";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		std::cout << "[ACDB] [MYSQL] " << mysql_error(Database::getConnection()) << std::endl;
		return query;
	}
	auto num = mysql_num_rows(qr);
	if (num == NULL || num == 0)
		return query;
	else{
		query.reserve((unsigned int) num);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr)){
			SessionInfo sinfo;
			sinfo.addr.SetBinaryAddress(row[0]);
			sinfo.phase = static_cast<SessionPhase>(std::stoi(row[2]));
			if (row[3] != NULL){
				sinfo.accountid = std::stoi(row[3]);
			}
			if (row[4] != NULL){
				sinfo.activeCharId = std::stoll(row[4]);
			}
			if (row[5] != NULL){
				sinfo.zone = std::stoi(row[5]);
			}
			query.push_back(sinfo);
		}
		return query;
	}
	
	/*std::unordered_map<SystemAddress, SessionInfo, SystemAddressHasher>::iterator it = std::find_if(sessions.begin(), sessions.end(), [&](const std::pair<SystemAddress, SessionInfo> & o){
		return o.second.zone == zoneid;
	});
	
	while (it != sessions.end()){
		query.push_back(it->second);
		++it;
		it = std::find_if(it, sessions.end(), [&](const std::pair<SystemAddress, SessionInfo> & o){
			return o.second.zone == zoneid;
		});
	}
	return query;*/
}

//Worlds
SessionInfo SessionsTable::enter(long long charid, unsigned short zoneId){
	SystemAddress addr = SessionsTable::findCharacter(charid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		SessionInfo s = SessionsTable::getClientSession(addr);
		if (s.phase == SessionPhase::PHASE_PLAYING){
			s.phase = SessionPhase::PHASE_INWORLD;
			s.zone = zoneId;
			std::stringstream str;
			str << "UPDATE `sessions` SET `phase` = '" << std::to_string((uchar)s.phase) << "', `zoneid` = '" << std::to_string(s.zone) << "' WHERE `ipaddress` = '" << addr.ToString() << "'";
			auto qr = Database::Query(str.str());
			if (qr == NULL){
				std::cout << "[ACDB] [MYSQL] " << mysql_error(Database::getConnection()) << std::endl;
			}
		}
		return s;

		/*auto session = sessions.find(addr);
		if (session == sessions.end())return SessionInfo();

		SessionInfo info = session->second;
		info.zone = zoneId;
		info.worldJoin++;
		info.phase = SessionPhase::PHASE_INWORLD;
		sessions.at(addr) = info;
		return info;*/
	}
	else{
		return SessionInfo();
	}
}

SessionInfo SessionsTable::leave(long long charid){
	SystemAddress addr = SessionsTable::findCharacter(charid);
	if (addr != UNASSIGNED_SYSTEM_ADDRESS){
		SessionInfo s = SessionsTable::getClientSession(addr);
		if (s.phase > SessionPhase::PHASE_PLAYING){
			s.phase = SessionPhase::PHASE_PLAYING;
			s.zone = 0;
			std::stringstream str;
			str << "UPDATE `sessions` SET `phase` = '" << std::to_string((uchar)s.phase) << "', `zoneid` = '" << std::to_string(s.zone) << "' WHERE `ipaddress` = '" << addr.ToString() << "'";
			auto qr = Database::Query(str.str());
			if (qr == NULL){
				std::cout << "[ACDB] [MYSQL] " << mysql_error(Database::getConnection()) << std::endl;
			}
		}
		return s;

		/*auto session = sessions.find(addr);
		if (session == sessions.end())return SessionInfo();

		SessionInfo info = session->second;
		info.zone = 0;
		info.phase = SessionPhase::PHASE_PLAYING;
		sessions.at(addr) = info;
		return info;*/
	}
	else{
		return SessionInfo();
	}
}

unsigned int SessionsTable::count(){
	std::stringstream str;
	str << "SELECT `ipaddress` FROM `sessions`";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		std::cout << "[ACDB] [MYSQL] " << mysql_error(Database::getConnection()) << std::endl;
		return 0;
	}
	auto num = mysql_num_rows(qr);
	if (num == NULL)
		return 0;
	else{
		return (unsigned int) num;
	}
	//return sessions.size();
}