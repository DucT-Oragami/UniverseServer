#pragma once
#include <my_global.h>
#include <mysql.h>

#include "Database.h"
#include "WorldServer.h"
#include "WorldConnection.h"

#include <string>
#include <vector>
#include <unordered_map>

struct WorldPlace{
	unsigned short zoneID = 0; //default to NO_ZONE 
	unsigned short mapInstance = 0;
	unsigned long mapClone = 0;
	float x = 0.0F;
	float y = 0.0F;
	float z = 0.0F;
};

struct CharacterStyle{
	unsigned long shirtColor = 0;
	unsigned long shirtStyle = 0;
	unsigned long pantsColor = 0;
	unsigned long hairColor = 0;
	unsigned long hairStyle = 0;
	unsigned long lh = 0;
	unsigned long rh = 0;
	unsigned long eyebrows = 0;
	unsigned long eyes = 0;
	unsigned long mouth;
};

struct CharacterInfo{
	unsigned int accountid = 0;
	long long objid = 0;
	unsigned short gmlevel = 0;
	std::string name;
	std::string unapprovedName;
	bool nameRejected = false;
	bool isFreeToPlay = false;
};

struct CharacterAttributes{
	unsigned long level = 1;
	long long uScore = 0;
	unsigned long health = 4;
	float maxHealth = 4.0f;
	unsigned long imagination = 0;
	float maxImagination = 0.0f;
	unsigned long armor = 0;
	float maxArmor = 0.0f;
};

struct ListCharacterInfo{
	WorldPlace lastPlace;
	CharacterStyle style;
	CharacterInfo info;
	CharacterAttributes attribute;
};

class CharactersTable {
private:
	static ListCharacterInfo getCharacterInfo(MYSQL_RES *res);
public:
	static long long getObjidFromCharacter(std::string name);
	static unsigned int getAccountFromObjid(long long objid);
	static std::vector<long long> getCharacters(unsigned int accountid);
	//static ListCharacterInfo getCharacterInfo(CharacterOwner owner);
	static ListCharacterInfo getCharacterInfo(long long objid);
	static ListCharacterInfo getCharacterInfo(std::string name);
	static bool setCharactersPlace(long long objid, WorldPlace place);
	static void deleteCharacter(long long objid);
	static std::vector<unsigned char> getCharacterIndices(unsigned int accountid);
	static long long add(CharacterStyle style, unsigned int accountid, CharacterInfo names);
	static bool unapprovedNameExists(std::string unapprovedname);
	static void setGMlevel(long long objid, unsigned short newLevel);

	static bool isCharacterAlive(long long objid);
	static void killCharacter(long long objid);
	static void resurrectCharacter(long long objid, bool bRezImmediately = false);

	static void setCharacterHealth(long long objid, unsigned long health);
    static unsigned long getCharacterHealth(long long objid);
	static void setCharacterMaxHealth(long long objid, float maxHealth);
	static float getCharacterMaxHealth(long long objid);

	static void setCharacterImagination(long long objid, unsigned long imagination);
	static unsigned long getCharacterImagination(long long objid);
	static void setCharacterMaxImagination(long long objid, float maxImagination);
	static float getCharacterMaxImagination(long long objid);

	static void setCharacterArmor(long long objid, unsigned long armor);
	static unsigned long getCharacterArmor(long long objid);
	static void setCharacterMaxArmor(long long objid, float maxArmor);
	static float getCharacterMaxArmor(long long objid);

	static void levelCharacter(long long objid);
	static unsigned long getCharacterLevel(long long objid);
	static void addCharacterUScore(long long objid, long long score);
	static long long getCharacterUScore(long long objid);

	static void setCharacterMoney(long long objid, long long currency);
	static long long getCharacterCurrency(long long objid);

	static void CharactersTable::setCharacterName(long long objid, std::string newName);
};

class FriendsTable {
public:
	static void requestFriend(long long sender, long long reciever);
	static void requestBestFriend(long long sender, long long reciever);
	static std::string getRequestStatus(long long sender, long long reciever);
	static std::string getFriendsStatus(long long charidx, long long charidy);

	static std::vector<std::pair<long long, std::string>> getFriendResponses(long long charobjid);
	static std::vector<long long> getFriendRequests(long long charobjid);
	static std::vector<long long> getBestFriendRequests(long long charobjid);
	static std::vector<long long> getFriends(long long charobjid);
	static std::vector<long long> getBestFriends(long long charobjid);

	//Only works in that direction
	static void setRequestStatus(long long charida, long long charidb, const std::string& status);

	static void accept(long long requester, long long accepter);
	static void decline(long long requester, long long accepter);
};

struct MISSION_DATA{
	int missionid;
	char missioncount;
	time_t timestamp;
};

class MissionsTable {
public:
	static std::vector<MISSION_DATA> getMissions(long long charid);
	static void addMission(long long charid, unsigned long missionid);
	static void deleteMissions(long long charid);
	static void offerMission(long long charid, unsigned long missionid, unsigned long offererid);
};

struct MailData{
	long long id;
	std::string sender;
	long long recipient;
	std::string subject;
	std::string text;
	long long attachment;
	int attachment_count;
	time_t sent_time;
	bool read;
};

class MailsTable {
public:
	static void setIsRead(long long mailid);
	static void deleteMail(long long mailid);
	static void addMail(MailData data);
	static std::vector<MailData> getMails(long long charid);
};