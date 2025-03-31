#include "pch.h"
#include "Player.h"
#include "PlayerManager.h"
#include "PacketMaker.h"
#include "PlayerStateController.h"
C_Content::Player::Player(EntityType type) : Entity(type)
{
	_stateController = std::make_unique<PlayerStateController>(this);

	const Vector3 pos = GetPosConst();
	printf("Player Constructor - [ID : %llu, Position : [%0.3f, %0.3f, %0.3f]\n", GetEntityId(), pos.x, pos.y, pos.z);
}

C_Content::Player::~Player()
{

}

//void C_Content::Player::Update(float delta)
//{
//	if (CanMove() && !IsDead())
//	{
//		_transformComponent.Move(delta);
//	}
//	_statComponent.Update(delta);
//}

void C_Content::Player::Move(float delta)
{
	_transformComponent.Move(delta);
}


void C_Content::Player::BroadcastMoveState()
{
	if (_stateController->GetMoveType() == C_Content::PlayerMoveStateBase::MoveState::Move)
	{
		MoveStartNotifyPacket moveNotifyPacket;
		moveNotifyPacket.entityId = GetEntityId();
		moveNotifyPacket.pos = _transformComponent.GetPosConst();
		moveNotifyPacket.rotY = _transformComponent.GetRotConst().y;

		C_Network::SharedSendBuffer buffer = C_Network::PacketMaker::MakeSendBuffer(sizeof(moveNotifyPacket));

		*buffer << moveNotifyPacket.size << moveNotifyPacket.type << moveNotifyPacket.entityId << moveNotifyPacket.pos << moveNotifyPacket.rotY;

		C_Content::PlayerManager::GetInstance().SendToAllPlayer(buffer);

	}
	else if(_stateController->GetMoveType() == C_Content::PlayerMoveStateBase::MoveState::Idle)
	{
		MoveStopNotifyPacket stopNotifyPacket;
		stopNotifyPacket.entityId = GetEntityId();
		stopNotifyPacket.pos = _transformComponent.GetPosConst();
		stopNotifyPacket.rotY = _transformComponent.GetRotConst().y;

		C_Network::SharedSendBuffer buffer = C_Network::PacketMaker::MakeSendBuffer(sizeof(stopNotifyPacket));

		*buffer << stopNotifyPacket.size << stopNotifyPacket.type << stopNotifyPacket.entityId << stopNotifyPacket.pos << stopNotifyPacket.rotY;

		C_Content::PlayerManager::GetInstance().SendToAllPlayer(buffer);
	}
}
