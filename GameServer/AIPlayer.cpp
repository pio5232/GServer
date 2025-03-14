#include "pch.h"
#include <random>
#include "AIPlayer.h"

C_Content::AIPlayer::AIPlayer() : Player(EntityType::AIPlayer), _dirChangeCool(0)
{
}

void C_Content::AIPlayer::Update(float delta)
{
	_dirChangeCool -= delta;

	if (_dirChangeCool <= 0)
	{
		_dirChangeCool = GetRandDouble(1.0, 4.0, 3);

		_transformComponent.AutoChangeDir();
	
		BroadcastMoveState(true);
	}


	Player::Update(delta);
}
