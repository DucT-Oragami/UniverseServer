#include "ReplicaObject.h"
#include <algorithm>
#include "Logger.h"
#include "UtfConverter.h"
#include "Replica.h"
#include "RakNet\ReplicaManager.h"
#include "Packet.h"
#include "Worlds.h"
#include "WorldServer.h"
#include "serverLoop.h"

//extern ReplicaManager replicaManager;

int i = 0;

ReplicaObject::~ReplicaObject(){
	this->deleteComponents();
	ObjectsManager::unregisterObject(this);
	WorldServer::getRM()->DereferencePointer(this);
}

std::wstring ReplicaObject::getName(){
	return this->name;
}

long long ReplicaObject::getObjectID(){
	return this->objid;
}

ReplicaComponent *ReplicaObject::getComponent(unsigned int componentid){
	std::vector<ReplicaComponent *>::iterator it = std::find_if(components.begin(), components.end(), [&](ReplicaComponent * o){
		return o->getComponentID() == componentid;
	});
	if (it == components.end()){
		return NULL;
	}
	else{
		return *it;
	}
}

void ReplicaObject::addComponent(ReplicaComponent * component){
	this->components.push_back(component);
}

void ReplicaObject::writeToPacket(RakNet::BitStream * packet, REPLICA_PACKET_TYPE packetType){
	if (packetType == REPLICA_PACKET_TYPE::REPLICA_CONSTRUCTION_PACKET){
		packet->Write(this->objid);
		packet->Write(this->LOT);

		packet->Write((unsigned char)this->name.size());
		for (unsigned int k = 0; k < this->name.size(); k++){
			packet->Write(this->name.at(k));
		}

		packet->Write(3025001UL);
		//packet->Write(0UL); // time_since_created_on_server (possibly)
		packet->Write(false); // Compressed info... to be added later
		packet->Write(trigger_id); // trigger_id
		packet->Write((bool)(spawnerObjID > 0));
		if (spawnerObjID > 0) { packet->Write(spawnerObjID); } // spawnerObjID
		packet->Write((bool)(spawner_node_id >= 0));
		if (spawner_node_id >= 0) { packet->Write(spawner_node_id); } // spawnerNodeID
		packet->Write((bool)(floatData >= 0.0));
		if (floatData >= 0.0) { packet->Write(floatData); } // Unknown floatData
		packet->Write((bool)(objectWorldState >= 0));
		if (objectWorldState >= 0) { packet->Write(objectWorldState); } // objectWorldState
		packet->Write(this->gmlevel > 0);
		if (this->gmlevel > 0){ packet->Write(this->gmlevel); } // gmlevel
	}
	packet->Write(true);
	packet->Write(false);
	packet->Write(false);

	for (std::vector<ReplicaComponent *>::iterator it = components.begin(); it != components.end(); ++it){
		(*it)->writeToPacket(packet, packetType);
	}

	if (packetType == REPLICA_PACKET_TYPE::REPLICA_CONSTRUCTION_PACKET) {
		/*RakNet::BitStream bs;
		bs.Write((char)0x24);
		bs.Write((bool)true);
		bs.Write((short)280);
		bs.Write(packet);

		std::stringstream ss;
		ss << "ReplicaPacket_" << i << ".bin";
		i++;

		SavePacket(ss.str(), (char*)bs.GetData(), bs.GetNumberOfBytesUsed());*/

		RakNet::BitStream bs;
		bs.Write((char)0x27);
		bs.Write((short)48608);
		bs.Write(false);
		bs.Write(false);
		bs.Write(false);
		bs.Write(false);
		bs.Write(false);
		bs.Write(false);
		bs.Write(false);
		bs.Write(true);
		bs.Write((long)1);
		bs.Write((long long)1152921504606847070);

		/*std::stringstream ss;
		ss << "ReplicaPacket_" << i << ".bin";
		i++;

		SavePacket(ss.str(), (char*)bs.GetData(), bs.GetNumberOfBytesUsed());*/
	}
}

void ReplicaObject::deleteComponents(){
	for (int i = components.size() - 1; i >= 0; i--){
		delete components.at(i);
		components.pop_back();
	}
}

ReplicaReturnResult ReplicaObject::SendConstruction(RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp){
	//This is the construction Packet
	Logger::log("REPL", "OBJECT", "Send construction of '" + UtfConverter::ToUtf8(this->name) + "' to " + std::string(systemAddress.ToString()), LOG_DEBUG);
	this->writeToPacket(outBitStream, REPLICA_CONSTRUCTION_PACKET);
	WorldServer::getRM()->SetScope(this, true, systemAddress, false);

	return REPLICA_PROCESSING_DONE;
}

ReplicaReturnResult ReplicaObject::SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp){
	//This is the destruction packet
	Logger::log("REPL", "OBJECT", "Send destruction of '" + UtfConverter::ToUtf8(this->name) + "' to " + std::string(systemAddress.ToString()), LOG_DEBUG);
	return REPLICA_PROCESSING_DONE;
}

ReplicaReturnResult ReplicaObject::ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp){
	//This happens only on the client side
	Logger::log("REPL", "OBJECT", "Recieve destruction of '" + UtfConverter::ToUtf8(this->name) + "' from " + std::string(systemAddress.ToString()), LOG_DEBUG);
	return REPLICA_PROCESSING_DONE;
}

ReplicaReturnResult ReplicaObject::SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp){
	//We have to do this, so the scope change isn't cancelled
	outBitStream->Write(inScope);
	return REPLICA_PROCESSING_DONE;
}

ReplicaReturnResult ReplicaObject::ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp){
	//This happens only on the client side
	return REPLICA_PROCESSING_DONE;
}

ReplicaReturnResult ReplicaObject::Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime,
	PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags){
	//This is the Serialization packet
	//TODO: move this function into here somwhere, and make the LOT dynamic (see construction)
	this->writeToPacket(outBitStream, REPLICA_SERIALIZATION_PACKET);
	return REPLICA_PROCESSING_DONE;
}

ReplicaReturnResult ReplicaObject::Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress){
	//TODO implement deserialize once the Serialization isn't static any more
	Logger::log("REPL", "OBJECT", "Client sent serialize", LOG_DEBUG);
	return REPLICA_PROCESSING_DONE;
}