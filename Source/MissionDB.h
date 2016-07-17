#pragma once
#include <my_global.h>

#include <vector>
#include <string>

struct MISSION_DATA{
	int missionid;
	char missioncount;
	time_t timestamp;
};

class MissionsTable {
public:
	void initializeAll();
	void removeMissions();
	void initializeMission(bool temp, unsigned long missionid, long long giverid);
	static std::vector<MISSION_DATA> getMissions(long long charid);
	static void addMission(long long charid, unsigned long missionid);
	static void deleteMissions(long long charid);
	static void offerMission(long long charid, unsigned long missionid, unsigned long offererid);
};