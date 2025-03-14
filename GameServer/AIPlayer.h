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
	private:
		double _dirChangeCool;
	};
}