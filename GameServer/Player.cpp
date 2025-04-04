#include "pch.h"
#include "Player.h"
#include "PlayerManager.h"
#include "BufferMaker.h"
#include "PlayerStateController.h"
#include "PacketBuilder.h"
#include "GameWorld.h"

using namespace C_Network;
C_Content::Player::Player(GameWorld* worldPtr, EntityType type,float updateInterval) : Entity(type),_worldPtr(worldPtr), _lastUpdatePos{}, _posUpdateInterval(updateInterval), _statComponent()
{
	_stateController = std::make_unique<PlayerStateController>(this);

	const Vector3 pos = GetPosition();
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

bool C_Content::Player::IsMoving() const
{
	return _stateController->GetMoveType() == C_Content::PlayerMoveStateBase::MoveState::Move;
}


void C_Content::Player::BroadcastMoveState()
{
	if (_stateController->GetMoveType() == C_Content::PlayerMoveStateBase::MoveState::Move)
	{
		C_Network::SharedSendBuffer buffer = C_Content::PacketBuilder::BuildMoveStartNotifyPacket(GetEntityId(), _transformComponent.GetPosConst(), _transformComponent.GetRotConst().y);	

		_worldPtr->SendPacketAroundSector(GetCurrentSector(), buffer);

		//C_Content::PlayerManager::GetInstance().SendToAllPlayer(buffer);
	}
	else if(_stateController->GetMoveType() == C_Content::PlayerMoveStateBase::MoveState::Idle)
	{
		C_Network::SharedSendBuffer buffer = C_Content::PacketBuilder::BuildMoveStopNotifyPacket(GetEntityId(), _transformComponent.GetPosConst(), _transformComponent.GetRotConst().y);
		
		_worldPtr->SendPacketAroundSector(GetCurrentSector(), buffer);

		//C_Content::PlayerManager::GetInstance().SendToAllPlayer(buffer);
	}
}
void C_Content::Player::SendPositionUpdate()
{
	Vector3 currentPos = _transformComponent.GetPosConst();

	if (Vector3::Distance(currentPos, _lastUpdatePos) < 0.1f)
		return;

	SharedSendBuffer buffer = C_Content::PacketBuilder::BuildUpdateTransformPacket(C_Utility::GetTimeStamp(), GetEntityId(), currentPos, _transformComponent.GetRotConst());

	//C_Content::PlayerManager::GetInstance().SendToAllPlayer(buffer);
	_worldPtr->SendPacketAroundSector(GetCurrentSector(), buffer);

	_lastUpdatePos = currentPos;
}

void C_Content::Player::SendPacketAround(C_Network::SharedSendBuffer buffer)
{
	_worldPtr->SendPacketAroundSector(GetCurrentSector(), buffer);
}
