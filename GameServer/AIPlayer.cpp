#include "pch.h"
#include <random>
#include "AIPlayer.h"
#include "PlayerManager.h"
#include "PacketMaker.h"
#include "PlayerStateController.h"
#include "PlayerState.h"
C_Content::AIPlayer::AIPlayer() : Player(EntityType::AIPlayer, posUpdateInterval), _movementUpdateInterval(0)
{
}

void C_Content::AIPlayer::Update(float delta)
{
	if (IsDead())
		return;

	_movementUpdateInterval -= delta;
	//_posUpdateInterval -= delta;

	if (_movementUpdateInterval <= 0)
	{
		_movementUpdateInterval = static_cast<float>(GetRandDouble(1.0, 6.0, 3));

		UpdateAIMovement();
	}

	//if (_posUpdateInterval <= 0)
	//{
	//	_posUpdateInterval = posUpdateInterval;

	//	SendPositionUpdate();
	//}

	//_stateController->Update(delta);

	Player::Update(delta);
}
bool C_Content::AIPlayer::ConsiderToMove()
{
	// 70% Move, 30% Idle
	return static_cast<float>(GetRandDouble(1.0, 4.0, 2)) < 3.1f;
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

