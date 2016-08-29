#include "Worlds.h"
#include "RakNet\BitStream.h"
#include "RakNet\ReplicaManager.h"
#include "Packet.h"
#include "WorldServer.h"
#include "ServerDB.h"
#include "Account.h"
#include "Logger.h"
#include "PlayerObject.h"
#include "InventoryDB.h"
#include "Config.h"

bool Worlds::loadWorld(SystemAddress address, ZoneId zone, COMPONENT1_POSITION pos, unsigned short instance, unsigned long clone){
	std::cout << "Instance is: " << instance << "\n";
	SessionInfo client = SessionsTable::getClientSession(address);
	if (client.instanceid == instance) {
		Logger::log("USER", "WARNING", "Canceling loadWorld, Client already in world: " + zone, LOG_WARNING);
		return false;
	}
	RakNet::BitStream * stream = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::MSG_CLIENT_LOAD_STATIC_ZONE);

	if (zone == ZoneId::NO_ZONE){
		//Logger::log("USER", "WARNING", "No zone selected, default to 1200 NIMBUS_STATION", LOG_DEBUG);
		//zone = ZoneId::NIMBUS_STATION;
		Logger::log("USER", "WARNING", "No zone selected, defaulting to 1000 - Venture Explorer", LOG_WARNING);
		zone = ZoneId::VENTURE_EXPLORER;
	}

	stream->Write((unsigned short)zone);
	// Use to switch between servers?
	stream->Write(instance); //Instance
	stream->Write(clone); //Clone

	std::vector<unsigned char> worldTarget = getWorldTarget(zone); //TODO: Replace this with actual numbers
	if (worldTarget.size() == 0) return false;
	stream->Write(worldTarget[2]); //Checksum
	stream->Write(worldTarget[3]);
	stream->Write(worldTarget[4]);
	stream->Write(worldTarget[5]); //End Checksum

	if (pos.isNull()){
		pos = getZoneSpawnPoint(zone);
	}
	stream->Write(pos.x);
	stream->Write(pos.y);
	stream->Write(pos.z);
	stream->Write((unsigned long)0);

	WorldServer::sendPacket(stream, address);

	if (instance != 0){
		//SessionsTable::setInstanceId(address, instance);
		Worlds::switchInstance(address, instance);
	}
	
	
	return true;
}

bool Worlds::switchInstance(SystemAddress address, unsigned short instance){
	RakNet::BitStream *bitStream = WorldServer::initPacket(RemoteConnection::CLIENT, ClientPacketID::SERVER_REDIRECT);

	std::string world = InstancesTable::getInstanceWorld(instance);
	if (world == "0"){
	    Logger::log("USER", "WARNING", "No such Instance Registered", LOG_WARNING);
		return false;
	}
	std::string IP = Config::getIP("World");
	WriteStringToBitStream(IP.c_str(), sizeof(IP), 33, bitStream);
	unsigned short port = Config::getWorldPort(world);
	bitStream->Write(port);
	bitStream->Write((bool)0);
	WorldServer::sendPacket(bitStream, address);
	return true;
}

// Simply checks if an instance is already registered,
// Actual registration is done in the function "InstancesTable::registerInstance(address)" seen below

int Instances::registerInstance(SystemAddress address, std::string world, unsigned short worldID){
	//TODO: We do this a little differently to not rocket up the instanceids,
	//We should delete and recreate the instance once it has more content then just the address
	int instanceid = InstancesTable::getInstanceId(address);
	if (instanceid > -1){
		//Instance was registered
		//InstancesTable::unregisterInstance(address);
		//Clear sessions
		Logger::log("WRLD", "INSTANCE", std::string(address.ToString()) + " already registered, clearing sessions");
		Session::clearForInstance(instanceid);
		return instanceid;
	}
	else{
		return InstancesTable::registerInstance(address, world, worldID);
	}
}

std::unordered_map<long long, ReplicaObject *> ObjectsManager::objects;

void ObjectsManager::registerObject(ReplicaObject * object){
	objects.insert(std::make_pair(object->getObjectID(), object));
}

void ObjectsManager::unregisterObject(ReplicaObject * object){
	objects.erase(object->getObjectID());
}

ReplicaObject * ObjectsManager::getObjectByID(long long objid){
	std::unordered_map<long long, ReplicaObject *>::iterator it = objects.find(objid);
	if (it != objects.end()) return it->second;
	else return NULL;
}

//This initiates creation
void ObjectsManager::create(long long objid){
	ReplicaObject * object = ObjectsManager::getObjectByID(objid);
	if (object != NULL){
		ObjectsManager::create(object);
	}
	else{
		Logger::log("REPL", "WORLDS", "Cannot create object " + std::to_string(objid) + ", object not registered", LOG_WARNING);
	}
}

void ObjectsManager::create(ReplicaObject * object){
	//When creating a new object, we need to:
	//send the creation to every connected session in that zone
	
	std::vector<SessionInfo> sess = SessionsTable::getClientsInWorld(object->world.zone);
	for (std::vector<SessionInfo>::iterator it = sess.begin(); it != sess.end(); ++it){
		WorldServer::getRM()->Construct(object, false, it->addr, false);
	}
}

void ObjectsManager::serialize(long long objid){
	ReplicaObject * object = ObjectsManager::getObjectByID(objid);
	if (object != NULL){
		ObjectsManager::serialize(object);
	}else{
		Logger::log("REPL", "WORLDS", "Cannot serialize object " + std::to_string(objid) + ", object not registered", LOG_WARNING);
	}
}

void ObjectsManager::serialize(ReplicaObject *object){
	std::vector<SessionInfo> sess = SessionsTable::getClientsInWorld(object->world.zone);
	for (std::vector<SessionInfo>::iterator it = sess.begin(); it != sess.end(); ++it){
		WorldServer::getRM()->SignalSerializeNeeded(object, it->addr, false);
	}
}

void ObjectsManager::destruct(long long objid){
	ReplicaObject * object = ObjectsManager::getObjectByID(objid);
	if (object != NULL){
		ObjectsManager::destruct(object);
	}else{
		Logger::log("REPL", "WORLDS", "Cannot destruct object " + std::to_string(objid) + ", object not registered", LOG_WARNING);
	}
}

void ObjectsManager::destruct(ReplicaObject * object){
	std::vector<SessionInfo> sess = SessionsTable::getClientsInWorld(object->world.zone);
	for (std::vector<SessionInfo>::iterator it = sess.begin(); it != sess.end(); ++it){
		WorldServer::getRM()->Destruct(object, it->addr, false);
	}
}

//This manages player
void ObjectsManager::clientLeaveWorld(long long objid, SystemAddress addr){
	/*	When a client leaves a world
		1) destruct him for everyone
		2) destruct every object for him
	*/
	ReplicaObject * object = ObjectsManager::getObjectByID(objid);
	if (object != NULL){
		ObjectsManager::destruct(object);
		ObjectsManager::unregisterObject(object);
		for (std::unordered_map<long long, ReplicaObject *>::iterator it = objects.begin(); it != objects.end(); ++it){
			if (it->second->world == object->world){
				WorldServer::getRM()->Destruct(it->second, addr, false);
			}
		}
		delete object;
	}
}

void ObjectsManager::clientJoinWorld(ReplicaObject * player, SystemAddress addr){
	//When a client joins a world:
	/*	1) create all objects for him
		2) create his object for all
	*/
	for (std::unordered_map<long long, ReplicaObject *>::iterator it = objects.begin(); it != objects.end(); ++it){
		if (it->second->world == player->world){
			WorldServer::getRM()->Construct(it->second, false, addr, false);
		}
	}
	ObjectsManager::registerObject(player);
	ObjectsManager::create(player);
}