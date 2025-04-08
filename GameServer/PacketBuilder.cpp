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

C_Network::SharedSendBuffer C_Content::PacketBuilder::BuildAttackedNotifyPacket(ULONGLONG entityId, uint16 currentHp)
{
	C_Network::AttackedNotifyPacket attackedNotifyPacket;
	attackedNotifyPacket.entityId = entityId;
	attackedNotifyPacket.currentHp = currentHp;

	C_Network::SharedSendBuffer sendBuffer = C_Network::BufferMaker::MakeSendBuffer(sizeof(attackedNotifyPacket));

	*sendBuffer << attackedNotifyPacket.size << attackedNotifyPacket.type << attackedNotifyPacket.entityId << attackedNotifyPacket.currentHp;

	return sendBuffer;
}

C_Network::SharedSendBuffer C_Content::PacketBuilder::BuildMakeOtherCharacterPacket(ULONGLONG entityId, const Vector3& position)
{
	C_Network::MakeOtherCharacterPacket makeOtherCharacterPacket;

	makeOtherCharacterPacket.entityId = entityId;
	makeOtherCharacterPacket.pos = position;
	
	C_Network::SharedSendBuffer sendBuffer = C_Network::BufferMaker::MakeSendBuffer(sizeof(makeOtherCharacterPacket));
	
	*sendBuffer << makeOtherCharacterPacket.size << makeOtherCharacterPacket.type << makeOtherCharacterPacket.entityId << makeOtherCharacterPacket.pos;

	return sendBuffer;
}

C_Network::SharedSendBuffer C_Content::PacketBuilder::BuildGameInitDonePacket()
{
	C_Network::GameInitDonePacket donePacket;
	C_Network::SharedSendBuffer sendBuffer = C_Network::BufferMaker::MakeSendBuffer(sizeof(donePacket));

	*sendBuffer << donePacket.size << donePacket.type;
	return sendBuffer;
}

C_Network::SharedSendBuffer C_Content::PacketBuilder::BuildDeleteOtherCharacterPacket(ULONGLONG entityId)
{
	C_Network::DeleteOtherCharacterPacket deleteOtherCharacterPacket;
	deleteOtherCharacterPacket.entityId = entityId;
	
	C_Network::SharedSendBuffer sendBuffer = C_Network::BufferMaker::MakeSendBuffer(sizeof(deleteOtherCharacterPacket));
	
	*sendBuffer << deleteOtherCharacterPacket.size << deleteOtherCharacterPacket.type << deleteOtherCharacterPacket.entityId;
	return sendBuffer;
}

C_Network::SharedSendBuffer C_Content::PacketBuilder::BuildMoveStartNotifyPacket(ULONGLONG entityId, const Vector3& pos, float rotY)
{
	C_Network::MoveStartNotifyPacket moveStartNotifyPacket;
	moveStartNotifyPacket.entityId = entityId;
	moveStartNotifyPacket.pos = pos;
	moveStartNotifyPacket.rotY = rotY;

	C_Network::SharedSendBuffer sendBuffer = C_Network::BufferMaker::MakeSendBuffer(sizeof(moveStartNotifyPacket));

	*sendBuffer << moveStartNotifyPacket.size << moveStartNotifyPacket.type << moveStartNotifyPacket.entityId << moveStartNotifyPacket.pos << moveStartNotifyPacket.rotY;

	return sendBuffer;
}

C_Network::SharedSendBuffer C_Content::PacketBuilder::BuildMoveStopNotifyPacket(ULONGLONG entityId, const Vector3& pos, float rotY)
{
	C_Network::MoveStopNotifyPacket moveStopNotifyPacket;
	moveStopNotifyPacket.entityId = entityId;
	moveStopNotifyPacket.pos = pos;
	moveStopNotifyPacket.rotY = rotY;

	C_Network::SharedSendBuffer sendBuffer = C_Network::BufferMaker::MakeSendBuffer(sizeof(moveStopNotifyPacket));

	*sendBuffer << moveStopNotifyPacket.size << moveStopNotifyPacket.type << moveStopNotifyPacket.entityId 
		<< moveStopNotifyPacket.pos << moveStopNotifyPacket.rotY;

	return sendBuffer;
}

C_Network::SharedSendBuffer C_Content::PacketBuilder::BuildUpdateTransformPacket(ULONGLONG timeStamp, ULONGLONG entityId, const Vector3& pos, const Vector3& rot)
{
	C_Network::UpdateTransformPacket updatePacket;

	updatePacket.timeStamp = timeStamp;
	updatePacket.entityId = entityId;
	updatePacket.pos = pos;
	updatePacket.rot = rot;

	C_Network::SharedSendBuffer sendBuffer = C_Network::BufferMaker::MakeSendBuffer(sizeof(updatePacket));

	*sendBuffer << updatePacket.size << updatePacket.type << updatePacket.timeStamp << updatePacket.entityId << updatePacket.pos
		<< updatePacket.rot;

	return sendBuffer;
}

C_Network::SharedSendBuffer C_Content::PacketBuilder::BuildDieNotifyPacket(ULONGLONG entityId)
{
	C_Network::DieNotifyPacket dieNotifyPacket;
	dieNotifyPacket.entityId = entityId;

	C_Network::SharedSendBuffer sendBuffer = C_Network::BufferMaker::MakeSendBuffer(sizeof(dieNotifyPacket));

	*sendBuffer << dieNotifyPacket.size << dieNotifyPacket.type << dieNotifyPacket.entityId;

	return sendBuffer;
}
