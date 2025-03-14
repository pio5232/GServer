#include "pch.h"
#include "LanClient.h"
#include "GameServer.h"
#include "PacketHandler.h"

C_Network::LanClient::LanClient(const NetAddress& targetNetAddr, C_Network::SessionCreator creator)
	: ClientBase(targetNetAddr, creator)
{
	C_Network::LanServerPacketHandler::Init();
}

C_Network::LanClient::~LanClient()
{
}


