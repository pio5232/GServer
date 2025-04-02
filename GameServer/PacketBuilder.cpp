#include "pch.h"
#include "PacketBuilder.h"
#include "BufferMaker.h"
C_Network::SharedSendBuffer C_Content::PacketBuilder::BuildAttackNotifyPacket(ULONGLONG entityId)
{
	C_Network::AttackNotifyPacket attackNotifyPacket;

	attackNotifyPacket.entityId = entityId;

	C_Network::SharedSendBuffer sendBuffer = C_Network::BufferMaker::MakeSendBuffer(sizeof(attackNotifyPacket));

	*sendBuffer << attackNotifyPacket.size << attackNotifyPacket.type << attackNotifyPacket.entityId;

	return sendBuffer;
}
