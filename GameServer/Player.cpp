#include "pch.h"
#include "Player.h"
#include "PlayerManager.h"
#include "PacketMaker.h"
#include "PlayerStateController.h"
C_Content::Player::Player(EntityType type,float updateInterval) : Entity(type), _lastUpdatePos{}, _posUpdateInterval(updateInterval)
{
	_stateController = std::make_unique<PlayerStateController>(this);

	const Vector3 pos = GetPosConst();
	printf("Player Constructor - [ID : %llu, Position : [%0.3f, %0.3f, %0.3f]\n", GetEntityId(), pos.x, pos.y, pos.z);
}

C_Content::Player::~Player()
{

}

void C_Content::Player::Update(float delta)
{

	_stateController->Update(delta);
}

void C_Content::Player::Move(float delta)
{
	//_posUpdateInterval -= delta;

	//if (_posUpdateInterval <= 0)
	//{
	//	_posUpdateInterval = posUpdateInterval;

		SendPositionUpdate();
	//}

	_transformComponent.Move(delta);

	//printf(" Transform Update -  EntityID : %llu, pos [ %0.3f, %0.3f, %0.3f ]]\n",GetEntityId(), _transformComponent.GetPosConst().x, _transformComponent.GetPosConst().y, _transformComponent.GetPosConst().z);
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
void C_Content::Player::SendPositionUpdate()
{
	Vector3 currentPos = _transformComponent.GetPosConst();

	if (Vector3::Distance(currentPos, _lastUpdatePos) < 0.1f)
		return;

	C_Network::UpdateTransformPacket updatePacket;

	updatePacket.timeStamp = C_Utility::GetTimeStamp();
	updatePacket.entityId = GetEntityId();
	updatePacket.pos = currentPos;
	updatePacket.rot = _transformComponent.GetRotConst();

	SharedSendBuffer buffer = C_Network::PacketMaker::MakeSendBuffer(sizeof(updatePacket));

	*buffer << updatePacket.size << updatePacket.type << updatePacket.timeStamp << updatePacket.entityId << updatePacket.pos
		<< updatePacket.rot;

	C_Content::PlayerManager::GetInstance().SendToAllPlayer(buffer);

	_lastUpdatePos = currentPos;
}