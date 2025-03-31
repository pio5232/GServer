#pragma once

#include "Player.h"

namespace C_Content
{
	class AIPlayer : public Player
	{
	public:
		AIPlayer();
		~AIPlayer() { printf("~~~AI Destructor\n"); }

		virtual void Update(float delta);

		bool ConsiderToMove();
		void UpdateAIMovement();
		void SendUpdatePos();
		
	private:
		float _movementUpdateInterval;

		float _posUpdateInterval;
		Vector3 _lastUpdatePos;
	};
}