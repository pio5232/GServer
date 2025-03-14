#include "pch.h"
#include "Player.h"
#include "PlayerManager.h"
#include "PacketMaker.h"

C_Content::Player::Player(EntityType type) : Entity(type)
{
	const Vector3 pos = GetPosConst();
	printf("Player Constructor - [ID : %llu, Position : [%0.3f, %0.3f, %0.3f]\n", GetEntityId(), pos.x, pos.y, pos.z);
}

C_Content::Player::~Player()
{

}

void C_Content::Player::Update(float delta)
{
	_transformComponent.Update(delta);

	_statComponent.Update(delta);
}

void C_Content::Player::BroadcastMoveState(bool isAi)
{
	// isMoving을 얻어서
	// if(true) => 이동 시작 패킷 전송
	// if(false) => 멈춤 패킷 전송
	if (_transformComponent.IsMoving())
	{
		MoveStartNotifyPacket moveNotifyPacket;
		moveNotifyPacket.entityId = GetEntityId();
		moveNotifyPacket.pos = _transformComponent.GetPosConst();
		moveNotifyPacket.moveDir = _transformComponent.GetMoveDir();

		C_Network::SharedSendBuffer buffer = C_Network::PacketMaker::MakeSendBuffer(sizeof(moveNotifyPacket));

		*buffer << moveNotifyPacket.size << moveNotifyPacket.type << moveNotifyPacket.entityId << moveNotifyPacket.pos << moveNotifyPacket.moveDir;

		//printf("BroadCast Move Start State !!\n");
		C_Content::PlayerManager::GetInstance().SendToAllPlayer(buffer);
	}
	else
	{
		MoveStopNotifyPacket stopNotifyPacket;
		stopNotifyPacket.entityId = GetEntityId();
		stopNotifyPacket.stopPos = _transformComponent.GetPosConst();
		stopNotifyPacket.stopDir = _transformComponent.GetMoveDir();

		C_Network::SharedSendBuffer buffer = C_Network::PacketMaker::MakeSendBuffer(sizeof(stopNotifyPacket));

		*buffer << stopNotifyPacket.size << stopNotifyPacket.type << stopNotifyPacket.entityId << stopNotifyPacket.stopPos << stopNotifyPacket.stopDir;

		//printf("BroadCast Move Stop State !!\n");
		C_Content::PlayerManager::GetInstance().SendToAllPlayer(buffer);

	}
}
