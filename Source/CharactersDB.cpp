#include "CharactersDB.h"
#include "AccountsDB.h"
#include "Database.h"
#include "Logger.h"
#include "GameMessages.h"
#include "PlayerObject.h"
#include "Worlds.h"

#include <sstream>
#include <iostream>

long long CharactersTable::getObjidFromCharacter(std::string name){
	std::stringstream iq;
	iq << "SELECT `objectID` FROM `characters` WHERE `name` = '" << name << "';";
	std::string qrs = iq.str();
	auto iqr = Database::Query(qrs);
	if (mysql_num_rows(iqr) > 0){
		auto ir = mysql_fetch_row(iqr);
		long long objid = std::stoll(ir[0]);
		return objid;
	}
	else{
		return -1; //-1 as return value
	}
}

unsigned int CharactersTable::getAccountFromObjid(long long objid){
	auto qr = Database::Query("SELECT `accountID` FROM `characters` WHERE `objectID` = '" + std::to_string(objid) + "' LIMIT 1;");
	if (mysql_num_rows(qr) == 0) {
		return 0;
	}
	auto r = mysql_fetch_row(qr);
	return std::stoi(r[0]);
}

std::vector<long long> CharactersTable::getCharacters(unsigned int accountid){
	std::vector<long long> chars;
	auto qr = Database::Query("SELECT `id`, `objectID` FROM `characters` WHERE `accountID` = " + std::to_string(accountid) + " LIMIT 4"); // Load chars from MySQL DB
	MYSQL_ROW row;
	chars.reserve(4);
	if (qr == NULL){
		return chars;
		Logger::logError("CHDB", "MYSQL", "getting characters", mysql_error(Database::getConnection()));
	}
	while (row = mysql_fetch_row(qr)) {
		std::stringstream s(row[1]);
		long long cid;
		s >> cid;
		unsigned short id = std::stoi(row[0]);
		chars.push_back(cid);
	}
	return chars;
}

/*ListCharacterInfo CharactersTable::getCharacterInfo(CharacterOwner owner){
	std::stringstream qrs;
	qrs << "SELECT ";
	qrs << "`objectID`, `name`, `unapprovedName`, `nameRejected`, `freeToPlay`, ";
	qrs << "`shirtColor`, `shirtStyle`, `pantsColor`, `hairStyle`, `hairColor`, `lh`, `rh`, `eyebrows`, `eyes`, `mouth`, ";
	qrs << "`lastZoneId`, `mapInstance`, `mapClone`, `x`, `y`, `z` ";
	qrs << "FROM `characters` WHERE `id` = '" << std::to_string(owner.charIndex) << "' AND `accountID` = '" << std::to_string(owner.accountid) << "';";
	std::string qrss = qrs.str();
	auto qr = Database::Query(qrss);
	return CharactersTable::getCharacterInfo(qr);
}*/

ListCharacterInfo CharactersTable::getCharacterInfo(long long objid){
	std::stringstream qrs;
	qrs << "SELECT ";
	// 0 - 6 ~ Info
	qrs << "`accountID`, `objectID`, `name`, `unapprovedName`, `nameRejected`, `freeToPlay`, `gmlevel`, ";
	// 7 - 16 ~ Style
	qrs << "`shirtColor`, `shirtStyle`, `pantsColor`, `hairStyle`, `hairColor`, `lh`, `rh`, `eyebrows`, `eyes`, `mouth`, ";
	// 17 - 22 ~ Place
	qrs << "`lastZoneId`, `mapInstance`, `mapClone`, `x`, `y`, `z`, ";
	// 23 - 30 ~ Attributes
	qrs << "`level`, `uScore`, `health`, `maxHealth`, `imagination`, `maxImagination`, `armor`, `maxArmor` ";
	qrs << "FROM `characters` WHERE `objectID` = '" << std::to_string(objid) << "';";
	std::string qrss = qrs.str();
	auto qr = Database::Query(qrss);
	if (qr == NULL){
		Logger::logError("CHDB", "MYSQL", "getting charinfo by objid", mysql_error(Database::getConnection()));
		return ListCharacterInfo();
	}
	return CharactersTable::getCharacterInfo(qr);
}

ListCharacterInfo CharactersTable::getCharacterInfo(std::string name){
	std::stringstream qrs;
	qrs << "SELECT ";
	// 0 - 6 ~ Info
	qrs << "`accountID`, `objectID`, `name`, `unapprovedName`, `nameRejected`, `freeToPlay`, `gmlevel`, ";
	// 7 - 16 ~ Style
	qrs << "`shirtColor`, `shirtStyle`, `pantsColor`, `hairStyle`, `hairColor`, `lh`, `rh`, `eyebrows`, `eyes`, `mouth`, ";
	// 17 - 22 ~ Place
	qrs << "`lastZoneId`, `mapInstance`, `mapClone`, `x`, `y`, `z`, ";
	// 23 - 30 ~ Attributes
	qrs << "`level`, `uScore`, `health`, `maxHealth`, `imagination`, `maxImagination`, `armor`, `maxArmor` ";
	qrs << "FROM `characters` WHERE `name` = '" << name << "';";
	std::string qrss = qrs.str();
	auto qr = Database::Query(qrss);
	return CharactersTable::getCharacterInfo(qr);
}

ListCharacterInfo CharactersTable::getCharacterInfo(MYSQL_RES *res){
	ListCharacterInfo i;
	if (mysql_num_rows(res) == 0) {
		i.info.objid = 0;
		return i;
	}
	else {
		//Info
		auto r = mysql_fetch_row(res);
		i.info.accountid = std::stoul(r[0]);
		i.info.objid = std::stoll(r[1]);
		i.info.name = r[2];
		i.info.unapprovedName = r[3];
		if (std::stoi(r[4]) == 1) i.info.nameRejected = true;
		if (std::stoi(r[5]) == 1) i.info.isFreeToPlay = true;
		i.info.gmlevel = std::stoi(r[6]);
		//Style
		i.style.shirtColor = std::stoul(r[7]);
		i.style.shirtStyle = std::stoul(r[8]);
		i.style.pantsColor = std::stoul(r[9]);
		i.style.hairStyle = std::stoul(r[10]);
		i.style.hairColor = std::stoul(r[11]);
		i.style.lh = std::stoul(r[12]);
		i.style.rh = std::stoul(r[13]);
		i.style.eyebrows = std::stoul(r[14]);
		i.style.eyes = std::stoul(r[15]);
		i.style.mouth = std::stoul(r[16]);
		//Place
		i.lastPlace.zoneID = std::stoi(r[17]);
		i.lastPlace.mapInstance = std::stoi(r[18]);
		i.lastPlace.mapClone = std::stoul(r[19]);
		i.lastPlace.x = std::stof(r[20]);
		i.lastPlace.y = std::stof(r[21]);
		i.lastPlace.z = std::stof(r[22]);
		//Attributes
		i.attribute.level = std::stoul(r[23]);
		i.attribute.uScore = std::stoll(r[24]);
		i.attribute.health = std::stoul(r[25]);
		i.attribute.maxHealth = std::stof(r[26]);
		i.attribute.imagination = std::stoul(r[27]);
		i.attribute.maxImagination = std::stof(r[28]);
		i.attribute.armor = std::stoul(r[29]);
		i.attribute.maxArmor = std::stof(r[30]);
	}
	return i;
}

bool CharactersTable::setCharactersPlace(long long objid, WorldPlace place){
	std::stringstream qr;
	qr <<  "UPDATE `characters` SET ";
	qr << "`lastZoneId` = '" << std::to_string(place.zoneID);
	qr << "', `mapInstance` = '" << std::to_string(place.mapInstance);
	qr << "', `mapClone` = '" << std::to_string(place.mapClone);
	qr << "', `x` = '" << std::to_string(place.x);
	qr << "', `y` = '" << std::to_string(place.y);
	qr << "', `z` = '" << std::to_string(place.z);
	qr << "' WHERE `objectID` = '" << std::to_string(objid) << "';";
	std::string qrs = qr.str();
	auto a = Database::Query(qrs);
	if (a == NULL){
		return false;
	}
	return true;
}

void CharactersTable::deleteCharacter(long long objid){
	std::stringstream query;
	query << "DELETE FROM `characters` WHERE `objectID` = '" << std::to_string(objid) << "';";
	Database::Query(query.str());
}

std::vector<unsigned char> CharactersTable::getCharacterIndices(unsigned int accountid){
	std::vector<unsigned char> chars;
	auto qr = Database::Query("SELECT `id`, `objectID` FROM `characters` WHERE `accountID` = " + std::to_string(accountid) + " LIMIT 4"); // Load chars from MySQL DB
	MYSQL_ROW row;
	while (row = mysql_fetch_row(qr)) {
		unsigned char id = std::stoi(row[0]);
		chars.push_back(id);
	}
	return chars;
}

long long CharactersTable::add(CharacterStyle style, unsigned int accountid, CharacterInfo names){
	std::stringstream query2;
	query2 << "INSERT INTO `characters` (`accountID`, `objectID`, `name`, `unapprovedName`, `nameRejected`, `freeToPlay`, ";
	query2 << "`shirtColor`, `shirtStyle`, `pantsColor`, `hairStyle`, `hairColor`, `lh`, `rh`, `eyebrows`, `eyes`, `mouth`, `lastZoneId`) ";
	query2 << "VALUES('" << std::to_string(accountid) << "', NULL, '" << names.name << "', '" << names.unapprovedName << "', '0', '0', '";
	query2 << std::to_string(style.shirtColor) << "', '" << std::to_string(style.shirtStyle) << "', '";
	query2 << std::to_string(style.pantsColor) << "', '" << std::to_string(style.hairStyle) << "', '" << std::to_string(style.hairColor) << "', '";
	query2 << std::to_string(style.lh) << "', '" << std::to_string(style.rh) << "', '" << std::to_string(style.eyebrows) << "', '";
	query2 << std::to_string(style.eyes) << "', '" << std::to_string(style.mouth) << "', '1000'); ";
	Database::Query(query2.str());
	return mysql_insert_id(Database::getConnection());
}

bool CharactersTable::unapprovedNameExists(std::string unapprovedname){
	auto qr2 = Database::Query("SELECT `unapprovedName` FROM `characters` WHERE `unapprovedName` = '" + unapprovedname + "';");
	if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == NULL) 
		return false;
	else
		return true;
}

void CharactersTable::setGMlevel(long long objid, unsigned short newLevel){
	std::stringstream str;
	str << "UPDATE `characters` SET `gmlevel` = '" << std::to_string(newLevel) << "' WHERE `objectID` = '" << std::to_string(objid) << "'";
	Database::Query(str.str());
}

bool CharactersTable::isCharacterAlive(long long objid){
	std::stringstream str;
	str << "SELECT `isAlive` FROM `characters` WHERE `objectID` = '" << objid << "';";
	auto qr = Database::Query(str.str());
	if (qr == NULL){
		return true;
	}
	else{
		if (mysql_num_rows(qr) == 1){
			return true;
		}
		else{
			return false;
		}
	}
}

void CharactersTable::killCharacter(long long objid){
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(objid));
	RakNet::BitStream *bs = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_GAME_MSG);

	bs->Write(s.activeCharId);
	bs->Write((unsigned short)37);

	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++){
		WorldServer::sendPacket(bs, sessionsz.at(k).addr);
	}

	std::stringstream eqqr;
	eqqr << "UPDATE `characters` SET `isAlive`='0' WHERE `objectID` = '" << objid << "';";
	Database::Query(eqqr.str());
}

void CharactersTable::resurrectCharacter(long long objid, bool bRezImmediately){
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(objid));
	RakNet::BitStream *bs = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_GAME_MSG);

	bs->Write(s.activeCharId);
	bs->Write((unsigned short)160);
	bs->Write(bRezImmediately);

	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++){
		WorldServer::sendPacket(bs, sessionsz.at(k).addr);
	}

	std::stringstream eqqr;
	eqqr << "UPDATE `characters` SET `isAlive`='1' WHERE `objectID` = '" << objid << "';";
	Database::Query(eqqr.str());
}

// Character Health
void CharactersTable::setCharacterHealth(long long objid, unsigned long health){
    PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(objid);
    DestructibleComponent *c7 = player->getComponent7();
    COMPONENT7_DATA4 d4 = c7->getData4();

	float maxHealth = CharactersTable::getCharacterMaxHealth(objid);
	if (health > maxHealth){
		health = maxHealth;
	}
    
    d4.health = health;
    c7->setData4(d4);
    ObjectsManager::serialize(player);

	std::stringstream eqqr;
	eqqr << "UPDATE `characters` SET `health`='" + std::to_string(health) + "' WHERE `objectID` = '" << objid << "';";
	Database::Query(eqqr.str());
}

unsigned long CharactersTable::getCharacterHealth(long long objid){
    std::stringstream qrs;
    qrs << "SELECT `health` FROM `characters` WHERE `objectID` = '" << std::to_string(objid) << "';";
    std::string qrss = qrs.str();
    auto qr = Database::Query(qrss);
    
    if (mysql_num_rows(qr) == 0 ){
        return 0;   
    }
    else{
        auto r = mysql_fetch_row(qr);
        return std::stol(r[0]);        
    }
}

void CharactersTable::setCharacterMaxHealth(long long objid, float maxHealth){
    PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(objid);
    DestructibleComponent *c7 = player->getComponent7();
    COMPONENT7_DATA4 d4 = c7->getData4();
    
    d4.maxHealth = maxHealth;
	d4.maxHealthN = maxHealth;
    c7->setData4(d4);
    ObjectsManager::serialize(player);
    
    std::stringstream eqqr;
    eqqr << "UPDATE `characters` SET `maxHealth`='" + std::to_string(maxHealth) + "' WHERE `objectID` = '" << objid << "';";
	Database::Query(eqqr.str());
}

float CharactersTable::getCharacterMaxHealth(long long objid){
	std::stringstream qrs;
	qrs << "SELECT `maxHealth` FROM `characters` WHERE `objectID` = '" << std::to_string(objid) << "';";
	std::string qrss = qrs.str();
	auto qr = Database::Query(qrss);

	if (mysql_num_rows(qr) == 0 ){
		return 0;
	}
	else{
		auto r = mysql_fetch_row(qr);
		return std::stof(r[0]);
	}
}

// Character Imagination
void CharactersTable::setCharacterImagination(long long objid, unsigned long imagination){
    PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(objid);
    DestructibleComponent *c7 = player->getComponent7();
    COMPONENT7_DATA4 d4 = c7->getData4();
    
	float maxImagination = CharactersTable::getCharacterMaxImagination(objid);
	if (imagination > maxImagination){
		imagination = maxImagination;
	}

    // Updating imagination seems to have the same problem as updating uScore,
	// Look into ObjectsManager::serialize to see what is actually going on
    d4.imagination = imagination;
    c7->setData4(d4);
    ObjectsManager::serialize(player);

	std::stringstream eqqr;
	eqqr << "UPDATE `characters` SET `imagination`='" + std::to_string(imagination) + "' WHERE `objectID` = '" << objid << "';";
	Database::Query(eqqr.str());
}

unsigned long CharactersTable::getCharacterImagination(long long objid){
    std::stringstream qrs;
    qrs << "SELECT `imagination` FROM `characters` WHERE `objectID` = '" << std::to_string(objid) << "';";
    std::string qrss = qrs.str();
    auto qr = Database::Query(qrss);
    
    if (mysql_num_rows(qr) == 0 ){
        return 0;   
    }
    else{
        auto r = mysql_fetch_row(qr);
        return std::stol(r[0]);        
    }
}

void CharactersTable::setCharacterMaxImagination(long long objid, float maxImagination){
    PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(objid);
    DestructibleComponent *c7 = player->getComponent7();
    COMPONENT7_DATA4 d4 = c7->getData4();
    
    d4.maxImagination = maxImagination;
	d4.maxImaginationN = maxImagination;
    c7->setData4(d4);
    ObjectsManager::serialize(player);
    
    std::stringstream eqqr;
    eqqr << "UPDATE `characters` SET `maxImagination`='" + std::to_string(maxImagination) + "' WHERE `objectID` = '" << objid << "';";
	Database::Query(eqqr.str());
}

float CharactersTable::getCharacterMaxImagination(long long objid){
	std::stringstream qrs;
	qrs << "SELECT `maxImagination` FROM `characters` WHERE `objectID` = '" << std::to_string(objid) << "';";
	std::string qrss = qrs.str();
	auto qr = Database::Query(qrss);

	if (mysql_num_rows(qr) == 0 ){
		return 0;
	}
	else{
		auto r = mysql_fetch_row(qr);
		return std::stof(r[0]);
	}
}

// Character Armor
void CharactersTable::setCharacterArmor(long long objid, unsigned long armor){
    PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(objid);
    DestructibleComponent *c7 = player->getComponent7();
    COMPONENT7_DATA4 d4 = c7->getData4();
    
	float maxArmor = CharactersTable::getCharacterMaxArmor(objid);
	if (armor > maxArmor){
		armor = maxArmor;
	}

    d4.armor = armor;
    c7->setData4(d4);
    ObjectsManager::serialize(player);

	std::stringstream eqqr;
	eqqr << "UPDATE `characters` SET `armor`='" + std::to_string(armor) + "' WHERE `objectID` = '" << objid << "';";
	Database::Query(eqqr.str());
}

unsigned long CharactersTable::getCharacterArmor(long long objid){
    std::stringstream qrs;
    qrs << "SELECT `armor` FROM `characters` WHERE `objectID` = '" << std::to_string(objid) << "';";
    std::string qrss = qrs.str();
    auto qr = Database::Query(qrss);
    
    if (mysql_num_rows(qr) == 0 ){
        return 0;   
    }
    else{
        auto r = mysql_fetch_row(qr);
        return std::stol(r[0]);        
    }
}

void CharactersTable::setCharacterMaxArmor(long long objid, float maxArmor){
    PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(objid);
    DestructibleComponent *c7 = player->getComponent7();
    COMPONENT7_DATA4 d4 = c7->getData4();
    
    d4.maxArmor = maxArmor;
	d4.maxArmorN = maxArmor;
    c7->setData4(d4);
    ObjectsManager::serialize(player);
    
    std::stringstream eqqr;
    eqqr << "UPDATE `characters` SET `maxArmor`='" + std::to_string(maxArmor) + "' WHERE `objectID` = '" << objid << "';";
	Database::Query(eqqr.str());
}

float CharactersTable::getCharacterMaxArmor(long long objid){
	std::stringstream qrs;
	qrs << "SELECT `maxArmor` FROM `characters` WHERE `objectID` = '" << std::to_string(objid) << "';";
	std::string qrss = qrs.str();
	auto qr = Database::Query(qrss);

	if (mysql_num_rows(qr) == 0 ){
		return 0;
	}
	else{
		auto r = mysql_fetch_row(qr);
		return std::stof(r[0]);
	}
}

// To do:
// Check if player is valid for level up,
// Reset player uScore (Or take the current value minus the required value) upon level up

// Note:
// When a player recieves uScore, and they reach the threshold for the next level,
// the client will send a "levelup request" packet to the server
// (See case 1734 in WorldLoop)
void CharactersTable::levelCharacter(long long objid){
	auto qr1 = Database::Query("SELECT `level` FROM `characters` WHERE `objectID` = '" + std::to_string(objid) + "';");
	if (mysql_num_rows(qr1) > 0){
		// Perform calculations
		auto r = mysql_fetch_row(qr1);
		unsigned long level = std::stol(r[0]) + 1;

		// Construct Packet
		RakNet::BitStream *bs = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_GAME_MSG);
	    bs->Write(objid);
	    bs->Write((unsigned short)1735);
		bs->Write((int)level);
		bs->Write((bool)0);

		// Send Packet
		SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(objid));
		std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	    for (unsigned int k = 0; k < sessionsz.size(); k++){
	    	WorldServer::sendPacket(bs, sessionsz.at(k).addr);
	    }

		// Update database
		std::stringstream eqqr;
		eqqr << "UPDATE `characters` SET `level`='" + std::to_string(level) + "' WHERE `objectID` = '" << objid << "';";
	    Database::Query(eqqr.str());
	}
	else{
		Logger::log("CHDB", "levelCharacter", "mysql_num_rows: " + mysql_num_rows(qr1), LOG_ERROR);
	}
};

unsigned long CharactersTable::getCharacterLevel(long long objid){
	std::stringstream qrs;
	qrs << "SELECT `level` FROM `characters` WHERE `objectID` = '" << std::to_string(objid) << "';";
	std::string qrss = qrs.str();
	auto qr = Database::Query(qrss);

	if (mysql_num_rows(qr) == 0) {
		return 0;
	}
	else {
		auto r = mysql_fetch_row(qr);
		return std::stol(r[0]);
	}

}

void CharactersTable::addCharacterUScore(long long objid, long long score){
	auto qr1 = Database::Query("SELECT `uScore` from `characters` WHERE `objectID` = '" + std::to_string(objid) + "';");
	if (mysql_num_rows(qr1) > 0){

		// Perform Calculations
		auto r = mysql_fetch_row(qr1);
		long long finalScore = std::stoll(r[0]) + score;

		// Update in real time ~ Does not work
	// Notes:
		// uScore is updated on server join,
		// but not on world switch
		PlayerObject *player = (PlayerObject *)ObjectsManager::getObjectByID(objid);
        CharacterComponent *c4 = player->getComponent4();
        PLAYER_INFO pi;
		pi.legoScore = finalScore;
		c4->setInfo(pi);
		ObjectsManager::serialize(player);

		SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(objid));

		// Construct Packet
	    RakNet::BitStream *bs = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_GAME_MSG);
	    bs->Write(objid);
	    bs->Write((unsigned short)1459);
		bs->Write((long long)finalScore);
		bs->Write((int)0);

		// Send Packet
		std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	    for (unsigned int k = 0; k < sessionsz.size(); k++){
	    	WorldServer::sendPacket(bs, sessionsz.at(k).addr);
	    }

		// Update Database
		std::stringstream eqqr;
	    eqqr << "UPDATE `characters` SET `uScore`='" + std::to_string(finalScore) + "' WHERE `objectID` = '" << objid << "';";
	    Database::Query(eqqr.str());
	}
	else{
	    Logger::log("CHDB", "addCharacterUScore", "mysql_num_rows: " + mysql_num_rows(qr1), LOG_ERROR);
	}
}

long long CharactersTable::getCharacterUScore(long long objid){
	std::stringstream qrs;
	qrs << "SELECT `uScore` FROM `characters` WHERE `objectID` = '" << std::to_string(objid) << "`;";
	std::string qrss = qrs.str();
	auto qr = Database::Query(qrss);
	
	if  (mysql_num_rows(qr) == 0){
		return 0;
	}

	else{
		auto r = mysql_fetch_row(qr);
		return std::stoll(r[0]);
	}
}

void CharactersTable::setCharacterMoney(long long objid, long long currency) {
	SessionInfo s = SessionsTable::getClientSession(SessionsTable::findCharacter(objid));
	float f = 0.0;

	RakNet::BitStream *bs = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_GAME_MSG);
	bs->Write(objid);
	bs->Write((unsigned short)133);
	bs->Write(currency);
	bs->Write(f);
	bs->Write(f);
	bs->Write(f);
	
	std::vector<SessionInfo> sessionsz = SessionsTable::getClientsInWorld(s.zone);
	for (unsigned int k = 0; k < sessionsz.size(); k++){
		WorldServer::sendPacket(bs, sessionsz.at(k).addr);
	}

	std::stringstream eqqr;
	eqqr << "UPDATE `characters` SET `currency`='" + std::to_string(currency) + "' WHERE `objectID` = '" << objid << "';";
	Database::Query(eqqr.str());
}

long long CharactersTable::getCharacterCurrency(long long objid) {
	std::stringstream qrs;
	qrs << "SELECT `currency` FROM `characters` WHERE `objectID` = '" << std::to_string(objid) << "';";
	std::string qrss = qrs.str();
	auto qr = Database::Query(qrss);

	if (mysql_num_rows(qr) == 0) {
		return 0;
	}
	else {
		auto r = mysql_fetch_row(qr);
		return std::stoll(r[0]);
	}
}

void CharactersTable::setCharacterName(long long objid, std::string newName) {
	std::stringstream str;
	str << "UPDATE `characters` SET `name`='" << newName << "' WHERE `objectID` = '" << objid << "';";
	Database::Query(str.str());
}

void FriendsTable::requestFriend(long long sender, long long reciever){
	std::stringstream str;
	str << "INSERT INTO `friends` (`id`, `charida`, `charidb`) VALUES( NULL, '" << sender << "', '" << reciever << "');";
	Database::Query(str.str());
}

void FriendsTable::requestBestFriend(long long sender, long long reciever){
	std::stringstream str;
	str << "UPDATE `friends` SET `status` = 'BEST_FRIEND_REQUEST', ";
	str << "`charida` = '" << std::to_string(sender) << "', ";
	str << "`charidb` = '" << std::to_string(reciever) << "' ";
	str << "WHERE (" << "`charida` = '" << std::to_string(sender) << "' AND `charidb` = '" << std::to_string(reciever) << "') ";
	str << "OR  (" << "`charidb` = '" << std::to_string(sender) << "' AND `charida` = '" << std::to_string(reciever) << "');";
	Database::Query(str.str());
}

//This query ONLY regards the status in one direction, meaning it is only useful if the requester is known
std::string FriendsTable::getRequestStatus(long long sender, long long reciever){
	auto qr = Database::Query("SELECT `status` FROM `friends` WHERE `charida` = '" + std::to_string(sender) + "' AND `charidb` = '" + std::to_string(reciever) + "';");
	if (mysql_num_rows(qr) == 0 || mysql_num_rows(qr) == NULL){
		return "INVALID";
	}
	else{
		auto row = mysql_fetch_row(qr);
		return row[0];
	}
}

std::string FriendsTable::getFriendsStatus(long long charidx, long long charidy){
	std::stringstream str;
	str << "SELECT `status` FROM `friends` ";
	str << "WHERE (" << "`charida` = '" << std::to_string(charidx) << "' AND `charidb` = '" << std::to_string(charidy) << "') ";
	str << "OR  (" << "`charidb` = '" << std::to_string(charidx) << "' AND `charida` = '" << std::to_string(charidy) << "');";
	auto qr = Database::Query(str.str());
	if (mysql_num_rows(qr) == 0 || mysql_num_rows(qr) == NULL){
		return "INVALID";
	}
	else{
		auto row = mysql_fetch_row(qr);
		return row[0];
	}
}

std::vector<long long> FriendsTable::getFriends(long long charobjid){
	std::string qr = "SELECT `charida`, `charidb` FROM `friends` WHERE (`charida` = '" + std::to_string(charobjid) + "' OR `charidb` = '" + std::to_string(charobjid) + "') AND `status` IN ('FRIENDS', 'BEST_FRIEND_REQUEST', 'ACCEPTED');";
	auto qr2 = Database::Query(qr);
	std::vector<long long> friends;
	if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == NULL)
		return friends;
	else{
		unsigned int numrows = (unsigned int) mysql_num_rows(qr2);
		friends.reserve(numrows);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr2)) {
			long long chara = std::stoll(row[0]);
			long long charb = std::stoll(row[1]);
			long long fr = 0;
			if (chara == charobjid){
				fr = charb;
			}
			else{
				fr = chara;
			}
			friends.push_back(fr);
		}
		return friends;
	}
}

std::vector<long long> FriendsTable::getBestFriends(long long charobjid){
	std::string qr = "SELECT `charida`, `charidb` FROM `friends` WHERE (`charida` = '" + std::to_string(charobjid) + "' OR `charidb` = '" + std::to_string(charobjid) + "') AND `status` IN ('BEST_FRIENDS', 'BEST_FRIENDS_ACCEPTED');";
	auto qr2 = Database::Query(qr);
	std::vector<long long> friends;
	if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == NULL)
		return friends;
	else{
		unsigned int numrows = (unsigned int)mysql_num_rows(qr2);
		friends.reserve(numrows);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr2)) {
			long long chara = std::stoll(row[0]);
			long long charb = std::stoll(row[1]);
			long long fr = 0;
			if (chara == charobjid){
				fr = charb;
			}
			else{
				fr = chara;
			}
			friends.push_back(fr);
		}
		return friends;
	}
}

std::vector<std::pair<long long, std::string>> FriendsTable::getFriendResponses(long long charobjid){
	std::string qr = "SELECT `charidb`, `status` FROM `friends` WHERE `charida` = '" + std::to_string(charobjid) + "' AND `status` in ('ACCEPTED', 'DECLINED');";
	auto qr2 = Database::Query(qr);
	std::vector<std::pair<long long, std::string>> requests;
	if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == NULL)
		return requests;
	else{
		unsigned int numrows = (unsigned int)mysql_num_rows(qr2);
		requests.reserve(numrows);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr2)) {
			long long charb = std::stoll(row[0]);
			std::string status = row[1];
			requests.push_back(std::make_pair(charb, status));
		}
		return requests;
	}
}

std::vector<long long> FriendsTable::getFriendRequests(long long charobjid){
	std::string qr = "SELECT `charida` FROM `friends` WHERE `charidb` = '" + std::to_string(charobjid) + "' AND `status` = 'REQUEST';";
	auto qr2 = Database::Query(qr);
	std::vector<long long> requests;
	if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == NULL)
		return requests;
	else{
		unsigned int numrows = (unsigned int)mysql_num_rows(qr2);
		requests.reserve(numrows);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr2)) {
			long long chara = std::stoll(row[0]);
			requests.push_back(chara);
		}
		return requests;
	}
}

std::vector<long long> FriendsTable::getBestFriendRequests(long long charobjid){
	std::string qr = "SELECT `charida` FROM `friends` WHERE `charidb` = '" + std::to_string(charobjid) + "' AND `status` = 'BEST_FRIEND_REQUEST';";
	auto qr2 = Database::Query(qr);
	std::vector<long long> requests;
	if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == NULL)
		return requests;
	else{
		unsigned int numrows = (unsigned int)mysql_num_rows(qr2);
		requests.reserve(numrows);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr2)) {
			long long chara = std::stoll(row[0]);
			requests.push_back(chara);
		}
		return requests;
	}
}

void FriendsTable::setRequestStatus(long long charida, long long charidb, const std::string& status){
	std::stringstream str;
	str << "UPDATE `friends` SET `status` = '" << status << "' ";
	str << "WHERE " << "`charida` = '" << std::to_string(charida) << "' AND `charidb` = '" << std::to_string(charidb) << "';";
	Database::Query(str.str());
}

void FriendsTable::accept(long long requester, long long accepter){
	FriendsTable::setRequestStatus(requester, accepter, "ACCEPTED");

}

void FriendsTable::decline(long long requester, long long accepter){
	FriendsTable::setRequestStatus(requester, accepter, "DECLINED");
}

void MailsTable::addMail(MailData data){
	std::stringstream query2;
	query2 << "INSERT INTO `mails` (`sender`, `recipient_id`, `subject`, `text`, `attachment`, `attachment_count`) ";
	query2 << "VALUES('" << data.sender << "', '" << std::to_string(data.recipient) << "', '";
	query2 << data.subject << "', '" << data.text << "', '" << std::to_string(data.attachment) << "', '" << std::to_string(data.attachment_count) << "'); ";
	auto a = Database::Query(query2.str());
	if (a == NULL){
		Logger::logError("CHDB", "MYSQL", "adding mail", mysql_error(Database::getConnection()));
	}
}

void MailsTable::setIsRead(long long mailid){
	std::stringstream str;
	str << "UPDATE `mails` SET `is_read` = '1' ";
	str << "WHERE " << "`id` = '" << std::to_string(mailid) << "';";
	Database::Query(str.str());
}

std::vector<MailData> MailsTable::getMails(long long charid){
	std::string qr = "SELECT `id`, `subject`, `text`, `sender`, `attachment`, `attachment_count`, UNIX_TIMESTAMP(`sent_time`), `is_read` FROM `mails` WHERE `recipient_id` = '" + std::to_string(charid) + "';";
	auto qr2 = Database::Query(qr);
	std::vector<MailData> mails;
	if (qr2 == NULL){
		Logger::logError("CHDB", "MYSQL", "getting mails", mysql_error(Database::getConnection()));
		return mails;
	}
	if (mysql_num_rows(qr2) == 0 || mysql_num_rows(qr2) == NULL)
		return mails;
	else{
		unsigned int numrows = (unsigned int)mysql_num_rows(qr2);
		mails.reserve(numrows);
		MYSQL_ROW row;
		while (row = mysql_fetch_row(qr2)) {
			MailData mail;
			mail.id = std::stoll(row[0]);
			mail.subject = row[1];
			mail.text = row[2];
			mail.sender = row[3];
			mail.attachment = std::stoll(row[4]);
			mail.attachment_count = std::stoi(row[5]);
			mail.sent_time = std::stoi(row[6]);
			unsigned char readN = std::stoi(row[7]);
			if (readN == 1) mail.read = true; else mail.read = false;
			mails.push_back(mail);
		}
		return mails;
	}
}

void MailsTable::deleteMail(long long mailid){
	std::stringstream str;
	str << "DELETE FROM `mails` WHERE `id` = '" << std::to_string(mailid) << "';";
	Database::Query(str.str());
}