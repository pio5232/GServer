#pragma once

namespace C_Network
{
	class GameSession;
	class LanClientSession;
}
using GameSessionPtr = std::shared_ptr<C_Network::GameSession>;

using LanClientSessionPtr = std::shared_ptr<C_Network::LanClientSession>;

const float sectorMinX = 0;
const float sectorMaxX = 100.0f;
const float sectorMinZ = 0;
const float sectorMaxZ = 100.0f;

const float defaultSlowWalkSpeed = 3.0f;
const float defaultWalkSpeed = 6.0f;
const float defaultRunSpeed = 9.0f;

const uint16 defaultMaxHp = 1;
const uint16 defaultAttackDamage= 1;

const float defaultAttackRange = 0.7f;

const float defaultErrorRange = 5.0f; // 오차 범위

namespace C_Content
{
	class Entity;
	class GamePlayer;
	class AIPlayer;
}


using GamePlayerPtr = std::shared_ptr<class C_Content::GamePlayer>;
using AIPlayerPtr = std::shared_ptr<class C_Content::AIPlayer>;
using EntityPtr = std::shared_ptr<class C_Content::Entity>;

using Action = std::function<void()>;


Vector3 GenerateRandomPos();


