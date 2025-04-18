#include "pch.h"
#include <random>
#include "AIPlayer.h"
#include "PlayerManager.h"
#include "BufferMaker.h"
#include "PlayerStateController.h"
#include "PlayerState.h"
#include "GameWorld.h"
C_Content::AIPlayer::AIPlayer(GameWorld* worldPtr) : Player(worldPtr, EntityType::AIPlayer, posUpdateInterval), _movementUpdateInterval(0)
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

void C_Content::AIPlayer::UpdateAIMovement()
{
	if (CheckChance(70))
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

