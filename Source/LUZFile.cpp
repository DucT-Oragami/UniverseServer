#include "LUZFile.h"
#include "Logger.h"
#include "dirent.h"

LUZFile::LUZFile() {};

std::vector<std::string> LUZFile::findall(char* path, bool pathOnly){
	std::vector<std::string> Files;
	DIR *dir;
	struct dirent *ent;
	dir = opendir(path);
	if (dir != NULL){
		while ((ent = readdir (dir)) != NULL){
			// Check to see if we found another directory
			std::string tmpString = ent->d_name;
			if (tmpString.find(".") == std::string::npos){
				std::string tmp = "\\";
				tmp = path + tmp + ent->d_name;
				char * newPath = new char[tmp.size() + 1];
				std::copy(tmp.begin(), tmp.end(), newPath);
				newPath[tmp.size()] = '\0';
				std::vector<std::string> other = LUZFile::findall(newPath, pathOnly);
				Files.insert(Files.end(), other.begin(), other.end());
				delete[] newPath;
			}
			else{
				if (strstr (ent->d_name, ".luz" )){
					if (!pathOnly){
						std::string tmp = "\\";
						tmp = path + tmp + ent->d_name;
						char * file = new char[tmp.size() + 1];
						std::copy(tmp.begin(), tmp.end(), file);
						file[tmp.size()] = '\0';

						Files.push_back(file);
					}
					else{
						Files.push_back(path);
					}
				}
			}
		}
		closedir (dir);
		return Files;
	}
	else{
		perror ("");
	}
}

// This is an unused function that does not work, just use the one right under it
LUZFile::LUZFile(std::string filename) {
	std::vector<unsigned char> content = OpenPacket(filename);

	RakNet::BitStream file;
	for (int i = 0; i < content.size(); i++) {
		file.Write(content.at(i));
	}

	unsigned long u32;
	unsigned char u8;

	file.Read(version);
	Logger::log("DEBUG", "LUZFile", "Version: " + std::to_string(version));

	if (version >= 0x24)
		file.Read(u32);

	file.Read(worldID);
	Logger::log("DEBUG", "LUZFile", "WorldID: " + std::to_string(worldID));

	if (version >= 0x26) {
		file.Read(spawnpoint.posX);
		file.Read(spawnpoint.posY);
		file.Read(spawnpoint.posZ);

		file.Read(spawnpoint.rotX);
		file.Read(spawnpoint.rotY);
		file.Read(spawnpoint.rotZ);
		file.Read(spawnpoint.rotW);
	}

	unsigned long scene_amount;

	if (version < 0x25) {
		unsigned char buf;
		file.Read(buf);
		scene_amount = buf;
	}
	else {
		file.Read(scene_amount);
	}

	for (int i = 0; i < scene_amount; i++) {
		unsigned char len;

		file.Read(len);

		std::string file_name = "";
		for (int k = 0; k < len; k++) {
			char buf;
			file.Read(buf);
			file_name += buf;
		}

		file.Read(u32);
		file.Read(u32);

		file.Read(len);

		std::string scene_name = "";
		for (int k = 0; k < len; k++) {
			char buf;
			file.Read(buf);
			scene_name += buf;
		}

		file.Read(u8);
		file.Read(u8);
		file.Read(u8);

		LVLFile child = LVLFile(".\\Files\\" + file_name, scene_name, version, worldID, "Blank");
		children.push_back(child);
	}
}

LUZFile::LUZFile(std::vector<unsigned char> data, std::string path) {
	RakNet::BitStream file;
	for (int i = 0; i < data.size(); i++) {
		file.Write(data.at(i));
	}

	unsigned long u32;
	unsigned char u8;

	file.Read(version);
	Logger::log("DEBUG", "LUZFile", "Version: " + std::to_string(version));

	if (version >= 0x24)
		file.Read(u32);

	file.Read(worldID);
	Logger::log("DEBUG", "LUZFile", "WorldID: " + std::to_string(worldID));

	if (version >= 0x26) {
		file.Read(spawnpoint.posX);
		file.Read(spawnpoint.posY);
		file.Read(spawnpoint.posZ);

		file.Read(spawnpoint.rotX);
		file.Read(spawnpoint.rotY);
		file.Read(spawnpoint.rotZ);
		file.Read(spawnpoint.rotW);
	}

	unsigned long scene_amount;

	if (version < 0x25) {
		unsigned char buf;
		file.Read(buf);
		scene_amount = buf;
	}
	else {
		file.Read(scene_amount);
	}

	for (int i = 0; i < scene_amount; i++) {
		unsigned char len;

		file.Read(len);

		std::string file_name = "";
		for (int k = 0; k < len; k++) {
			char buf;
			file.Read(buf);
			file_name += buf;
		}

		file.Read(u32);
		file.Read(u32);

		file.Read(len);

		std::string scene_name = "";
		for (int k = 0; k < len; k++) {
			char buf;
			file.Read(buf);
			scene_name += buf;
		}

		file.Read(u8);
		file.Read(u8);
		file.Read(u8);

		LVLFile child = LVLFile(file_name, scene_name, version, worldID, path);
		children.push_back(child);
	}
}

LUZFile::~LUZFile() {}

unsigned long LUZFile::getVersion() { return version; }
unsigned long LUZFile::getWorldID() { return worldID; }

Spawnpoint LUZFile::getSpawnpoint() { return spawnpoint; }

std::vector<LVLFile> LUZFile::getChildren() { return children; }