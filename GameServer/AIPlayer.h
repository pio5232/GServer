#pragma once

#include "Player.h"

namespace C_Content
{
	class GameWorld;
	class AIPlayer : public Player
	{
	public:
		AIPlayer(GameWorld* worldPtr);
		~AIPlayer() { printf("~~~AI Destructor\n"); }

		virtual void Update(float delta);

		void UpdateAIMovement();
		
	private:
		float _movementUpdateInterval;
	};
}