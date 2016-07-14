#pragma once
#include "AccountsDB.h"

#include <vector>
#include <iomanip>
#include <unordered_map>


//#define noDenyMessage
#ifdef noDenyMessage
#define commandAccessDenied()

#else
#define commandAccessDenied() Chat::sendChatMessage(s->addr, L"You don't have permission to use this command!");

#endif

class ChatCommandHandler{
public:
	virtual void handleCommand(SessionInfo *s, std::vector<std::wstring> * params) = 0;
	virtual std::vector<std::wstring> getCommandNames() = 0;
	virtual std::wstring getDescription() = 0;
	virtual std::wstring getShortDescription() = 0;
	virtual std::wstring getSyntax() = 0;
};

class ChatCommandManager{
	static std::vector<ChatCommandHandler *> ChatCommandHandlers;
	static std::unordered_map<std::wstring, ChatCommandHandler *> ChatCommands;
public:
	static void registerCommands(ChatCommandHandler * CommandHandler);
	static void handleCommand(std::wstring command, SessionInfo *s, std::vector<std::wstring> * params);
};

class FlightCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class TeleportCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class WhisperCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class TestmapCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class SwitchCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class AddItemCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class PositionCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class ClientCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class AttributeCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class PacketCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class AnnouncementCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class AdminCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class SpawnObjectCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class DeleteObjectCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class NearMeCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class DebugCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class LevelUpCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class AddUScoreCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class SetMoneyCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class DanceCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class SetNameCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class PlayAnimationCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class EquipOfCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};

class TestSmashCommandHandler : public ChatCommandHandler{
	void handleCommand(SessionInfo *s, std::vector<std::wstring> * params);
	std::vector<std::wstring> getCommandNames();
	std::wstring getDescription();
	std::wstring getShortDescription();
	std::wstring getSyntax();
};