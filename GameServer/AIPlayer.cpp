#include "pch.h"
#include <random>
#include "AIPlayer.h"
#include "PlayerManager.h"
#include "PacketMaker.h"
#include "PlayerStateController.h"
#include "PlayerState.h"
C_Content::AIPlayer::AIPlayer() : Player(EntityType::AIPlayer), _movementUpdateInterval(0), _posUpdateInterval(aiPosUpdateInterval), _lastUpdatePos{}
{
}

void C_Content::AIPlayer::Update(float delta)
{
	if (IsDead())
		return;

	_movementUpdateInterval -= delta;
	_posUpdateInterval -= delta;

	if (_movementUpdateInterval <= 0)
	{
		_movementUpdateInterval = static_cast<float>(GetRandDouble(1.0, 6.0, 3));

		UpdateAIMovement();
	}

	if (_posUpdateInterval <= 0)
	{
		_posUpdateInterval = aiPosUpdateInterval;

		SendUpdatePos();
	}
	
	_stateController->Update(delta);
}
bool C_Content::AIPlayer::ConsiderToMove()
{
	// 75% Move, 25% Idle
	return static_cast<float>(GetRandDouble(1.0, 4.0, 2)) < 3.25f;
}

void C_Content::AIPlayer::UpdateAIMovement()
{
	if (ConsiderToMove())
	{
		_transformComponent.SetRandomDirection();
		_stateController->ChangeState(&C_Content::PlayerMoveState::GetInstance());
	}
	else
	{
		_stateController->ChangeState(&C_Content::PlayerIdleState::GetInstance());
	}

	BroadcastMoveState();

}
void C_Content::AIPlayer::SendUpdatePos()
{
	Vector3 currentPos = _transformComponent.GetPosConst();

	if (Vector3::Distance(currentPos, _lastUpdatePos) < 0.1f)
		return;

	C_Network::UpdatePositionPacket updatePacket;

	updatePacket.timeStamp = C_Utility::GetTimeStamp();
	updatePacket.entityId = GetEntityId();
	updatePacket.pos = currentPos;

	SharedSendBuffer buffer = C_Network::PacketMaker::MakeSendBuffer(sizeof(updatePacket));

	*buffer << updatePacket.size << updatePacket.type << updatePacket.timeStamp << updatePacket.entityId << updatePacket.pos;

	C_Content::PlayerManager::GetInstance().SendToAllPlayer(buffer);
	
	_lastUpdatePos = currentPos;
}
