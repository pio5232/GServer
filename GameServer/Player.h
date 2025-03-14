#pragma once
#include "StatComponent.h"
#include "TransformComponent.h"
#include "Entity.h"

namespace C_Content
{
	class Player : public Entity
	{
	public:
		Player(EntityType type);
		~Player();

		virtual void Update(float delta);

		const Vector3 GetPosConst() const { return _transformComponent.GetPosConst(); }
	protected:
		void BroadcastMoveState(bool isAi);

	protected :
		StatComponent _statComponent;
		TransformComponent _transformComponent;

	};
}