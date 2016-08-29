#include "Config.h"
#include "../../FileConfiguration.h"

bool Config::getLogFile() {
	FileConfiguration fs = FileConfiguration::loadConfiguration(".\\config.cfg");
	return fs.readBoolean("General.logFile", true);
}

bool Config::getUseEncryption() {
	FileConfiguration fs = FileConfiguration::loadConfiguration(".\\config.cfg");
	return fs.readBoolean("General.useEncryption", false);
}

bool Config::getLoadObjects() {
    FileConfiguration fs = FileConfiguration::loadConfiguration(".\\config.cfg");
    return fs.readBoolean("General.loadObjects", true);    
}

std::string Config::getIP(std::string role) {
	FileConfiguration fs = FileConfiguration::loadConfiguration(".\\config.cfg");
	return fs.readString("Servers." + role + ".address", "127.0.0.1");
}

int Config::getPort(std::string role) {
	FileConfiguration fs = FileConfiguration::loadConfiguration(".\\config.cfg");
	return fs.readInteger("Servers." + role + ".port", 0);
}

// Should return unsigned short instead
int Config::getWorldPort(std::string world) {
	FileConfiguration fs = FileConfiguration::loadConfiguration(".\\config.cfg");
	return fs.readInteger("Servers.World." + world + ".port", 0);
}

// I would really rather not bother using a map, or the like, to match
// world strings to world ids, so I did it here
int Config::getWorldID(std::string world) {
	FileConfiguration fs = FileConfiguration::loadConfiguration(".\\config.cfg");
	return fs.readInteger("Servers.World." + world + ".ID", 0);
}

std::string Config::getFileWorld(std::string file){
	FileConfiguration fs = FileConfiguration::loadConfiguration(".\\config.cfg");
	return fs.readString("Servers.File." + file + ".world", "EMPTY");
}

std::string Config::getMySQLHost() {
	FileConfiguration fs = FileConfiguration::loadConfiguration(".\\config.cfg");
	return fs.readString("MySQL.host", "127.0.0.1");
}

std::string Config::getMySQLDatabase() {
	FileConfiguration fs = FileConfiguration::loadConfiguration(".\\config.cfg");
	return fs.readString("MySQL.database", "luni");
}

std::string Config::getMySQLUsername() {
	FileConfiguration fs = FileConfiguration::loadConfiguration(".\\config.cfg");
	return fs.readString("MySQL.username", "root");
}

std::string Config::getMySQLPassword() {
	FileConfiguration fs = FileConfiguration::loadConfiguration(".\\config.cfg");
	return fs.readString("MySQL.password", "");
}

//void Config::initConnectInfo(CONNECT_INFO& info, std::string setting) {
//	FileConfiguration fs = FileConfiguration::loadConfiguration(".\\config.cfg");
//
//	strcpy(info.redirectIp, fs.readString("redirectIP", "127.0.0.1").c_str());
//
//	info.logFile = fs.readBoolean("logFile", true);
//	info.useEncryption = fs.readBoolean("useEncryption", false);
//
//	info.listenPort = fs.readInteger(setting + ".listenPort", 0);
//	info.redirectPort = fs.readInteger(setting + ".redirectPort", 0);
//}


// This creates a very ugly looking config,
// it would be better if we gave each world it's own config file
void Config::generateDefaultConfig() {
	FileConfiguration fs = FileConfiguration::loadConfiguration(".\\config.cfg");

	fs.writeBoolean("General.logFile", true);
	fs.writeBoolean("General.useEncryption", false);
    fs.writeBoolean("General.loadObjects", true);

	fs.writeString("MySQL.host", "127.0.0.1");
	fs.writeString("MySQL.database", "luni");
	fs.writeString("MySQL.username", "root");
	fs.writeString("MySQL.password", "");

	fs.writeString("Servers.Auth.address", "127.0.0.1");
	fs.writeInteger("Servers.Auth.port", 1001);

	fs.writeString("Servers.Chat.address", "127.0.0.1");
	fs.writeInteger("Servers.Chat.port", 2002);

	fs.writeString("Servers.masterWorld.address", "127.0.0.1");
	fs.writeInteger("Servers.masterWorld.port", 2001);

	fs.writeString("Servers.World.address", "127.0.0.1");
	//fs.writeInteger("Servers.World.port", 2003);

	// Each World is run on a seperate server using the same ip, but a different port
	// By default, ports 2003 - 2033 are used

	// I really don't want to bother with using a map to match world strings to world ids, so I did it here

// Empty world used to host players before selecting a character to play
	fs.writeInteger("Servers.World.EMPTY.port", 2003);
	fs.writeInteger("Servers.World.EMPTY.ID", 0);

// Venture Explorer
	fs.writeString("Servers.File.nd_space_ship.world", "VENTURE_EXPLORER");
	fs.writeInteger("Servers.World.VENTURE_EXPLORER.port", 2004);
	fs.writeInteger("Servers.World.VENTURE_EXPLORER.ID", 1000);

	//fs.writeString("Servers.World.VENTURE_EXPLORER_RETURN.luz", "nd_space_ship_battle_instance.luz");
	fs.writeString("Servers.File.nd_space_ship_battle_instance.world", "VENTURE_EXPLORER_RETURN");
	fs.writeInteger("Servers.World.VENTURE_EXPLORER_RETURN.port", 2005);
	fs.writeInteger("Servers.World.VENTURE_EXPLORER_RETURN.ID", 1001);

// Avant Gardens
	fs.writeString("Servers.File.nd_avant_gardens.world", "AVANT_GARDENS");
	fs.writeInteger("Servers.World.AVANT_GARDENS.port", 2006);
	fs.writeInteger("Servers.World.AVANT_GARDENS.ID", 1100);

	fs.writeString("Servers.File.nd_ag_survival_battlefield.world", "AVANT_GARDENS_SURVIVAL");
	fs.writeInteger("Servers.World.AVANT_GARDENS_SURVIVAL.port", 2007);
	fs.writeInteger("Servers.World.AVANT_GARDENS_SURVIVAL.ID", 1101);

	fs.writeInteger("Servers.World.SPIDER_QUEEN_BATTLE.port", 2008);
	fs.writeInteger("Servers.World.SPIDER_QUEEN_BATTLE.ID", 1102);

	fs.writeInteger("Servers.World.BLOCK_YARD.port", 2009);
	fs.writeInteger("Servers.World.BLOCK_YARD.ID", 1150);

	fs.writeInteger("Servers.World.AVANT_GROVE.port", 2010);
	fs.writeInteger("Servers.World.AVANT_GROVE.ID", 1151);

// Nimbus Station
	fs.writeString("Servers.File.nd_nimbus_station.world", "NIMBUS_STATION");
	fs.writeInteger("Servers.World.NIMBUS_STATION.port", 2011);
	fs.writeInteger("Servers.World.NIMBUS_STATION.ID", 1200);

	fs.writeString("Servers.File.nd_ns_pet_ranch.world", "PET_COVE");
	fs.writeInteger("Servers.World.PET_COVE.port", 2012);
	fs.writeInteger("Servers.World.PET_COVE.ID", 1201);

	fs.writeInteger("Servers.World.VERTIGO_LOOP_RACETRACK.port", 2013);
	fs.writeInteger("Servers.World.VERTIGO_LOOP_RACETRACK.ID", 1203);

	fs.writeInteger("Servers.World.BATTLE_OF_NIMBUS_STATION.port", 2014);
	fs.writeInteger("Servers.World.BATTLE_OF_NIMBUS_STATION.ID", 1204);

	fs.writeInteger("Servers.World.NIMBUS_ROCK.port", 2015);
	fs.writeInteger("Servers.World.NIMBUS_ROCK.ID", 1250);

	fs.writeInteger("Servers.World.NIMBUS_ISLE.port", 2016);
	fs.writeInteger("Servers.World.NIMBUS_ISLE.ID", 1251);

// Gnarled Forest
	fs.writeString("Servers.File.nd_gnarled_forest.world", "GNARLED_FOREST");
	fs.writeInteger("Servers.World.GNARLED_FOREST.port", 2017);
	fs.writeInteger("Servers.World.GNARLED_FOREST.ID", 1300);

	fs.writeInteger("Servers.World.CANYON_COVE.port", 2018);
	fs.writeInteger("Servers.World.CANYON_COVE.ID", 1302);

	fs.writeInteger("Servers.World.KEELHAUL_CANYON.port", 2019);
	fs.writeInteger("Servers.World.KEELHAUL_CANYON.ID", 1303);

	fs.writeInteger("Servers.World.CHANTEY_SHANTEY.port", 2020);
	fs.writeInteger("Servers.World.CHANTEY_SHANTEY.ID", 1350);

// Forbidden Valley
	fs.writeString("Servers.File.nd_forbidden_valley.world", "FORBIDDEN_VALLEY");
	fs.writeInteger("Servers.World.FORBIDDEN_VALLEY.port", 2021);
	fs.writeInteger("Servers.World.FORBIDDEN_VALLEY.ID", 1400);

	fs.writeInteger("Servers.World.FORBIDDEN_VALLEY_DRAGON.port", 2022);
	fs.writeInteger("Servers.World.FORBIDDEN_VALLEY_DRAGON.ID", 1402);

	fs.writeInteger("Servers.World.DRAGONMAW_CHASM.port", 2023);
	fs.writeInteger("Servers.World.DRAGONMAW_CHASM.ID", 1403);

	fs.writeInteger("Servers.World.RAVEN_BLUFF.port", 2024);
	fs.writeInteger("Servers.World.RAVEN_BLUFF.ID", 1450);

// The "creative" worlds? Not sure how to describe these
	fs.writeInteger("Servers.World.STAR_BASE_3001.port", 2025);
	fs.writeInteger("Servers.World.STAR_BASE_3001.ID", 1600);

	fs.writeInteger("Servers.World.DEEP_FREEZE.port", 2026);
	fs.writeInteger("Servers.World.DEEP_FREEZE.ID", 1601);

	fs.writeInteger("Servers.World.ROBOT_CITY.port", 2027);
	fs.writeInteger("Servers.World.ROBOT_CITY.ID", 1602);

	fs.writeInteger("Servers.World.MOON_BASE.port", 2028);
	fs.writeInteger("Servers.World.MOON_BASE.ID", 1603);

	fs.writeInteger("Servers.World.PORTABELLO.port", 2029);
	fs.writeInteger("Servers.World.PORTABELLO.ID", 1604);

// Crux Prime|Lego Club|Nexus Tower
	fs.writeString("Servers.File.nd_lego_club.world", "LEGO_CLUB");
	fs.writeInteger("Servers.World.LEGO_CLUB.port", 2030);
	fs.writeInteger("Servers.World.LEGO_CLUB.ID", 1700);

	fs.writeString("Servers.File.nd_aura_mar.world", "CRUX_PRIME");
	fs.writeInteger("Servers.World.CRUX_PRIME.port", 2031);
	fs.writeInteger("Servers.World.CRUX_PRIME.ID", 1800);

	fs.writeString("Servers.File.nd_nexus_tower.world", "NEXUS_TOWER");
	fs.writeInteger("Servers.World.NEXUS_TOWER.port", 2032);
	fs.writeInteger("Servers.World.NEXUS_TOWER.ID", 1900);

// Ninjago
	fs.writeInteger("Servers.World.NINJAGO_MONASTERY.port", 2033);
	fs.writeInteger("Servers.World.NINJAGO_MONASTERY.ID", 2000);

	fs.writeInteger("Servers.World.FRANKJAW_BATTLE.port", 2034);
	fs.writeInteger("Servers.World.FRANKJAW_BATTLE.ID", 2001);

	fs.save();
}