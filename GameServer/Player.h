#pragma once
#include "StatComponent.h"
#include "Entity.h"

namespace C_Content
{
	class GameWorld;
	class PlayerStateController;

	class Player : public Entity
	{
	public:
		Player(class GameWorld* worldPtr, EntityType type, float updateInterval);
		~Player();

		virtual void Update(float delta) = 0;

		void Move(float delta);
		bool IsDead() const { return _statComponent.IsDead(); }
		virtual bool IsMoving() const override;
	protected:
		void BroadcastMoveState();
		void SendPositionUpdate();

	protected :
		void SendPacketAround(C_Network::SharedSendBuffer buffer);
		StatComponent _statComponent;
		std::unique_ptr<PlayerStateController> _stateController;

		float _posUpdateInterval;
	private:
		Vector3 _lastUpdatePos;
		class GameWorld* _worldPtr;

	};
}