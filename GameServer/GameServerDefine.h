#pragma once

namespace C_Network
{
	class GameSession;
	class LanClientSession;
}
using GameSessionPtr = std::shared_ptr<C_Network::GameSession>;

using LanClientSessionPtr = std::shared_ptr<C_Network::LanClientSession>;

const int fixedFrame = 60;
const float fixedDeltaTime = 1.0f / fixedFrame;
// 한번에 측정가능한 최대 델타 (넘으면 날려버림) => delta가 크게 나왔을때의 처리 프레임 제한
const float limitDeltaTime = 0.2f; 


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

const float defaultErrorRange = 3.0f; // 오차 범위
const float posUpdateInterval = 0.2f;


const float attackDuration = 0.8f;
const float attackedDuration = 1.0f;
namespace C_Content
{
	class Entity;
	class GamePlayer;
	class AIPlayer;
	class WorldChat;
}


using GamePlayerPtr = std::shared_ptr<class C_Content::GamePlayer>;
using AIPlayerPtr = std::shared_ptr<class C_Content::AIPlayer>;
using EntityPtr = std::shared_ptr<class C_Content::Entity>;

using WorldChatPtr = std::shared_ptr<class C_Content::WorldChat>;
using Action = std::function<void()>;


Vector3 GenerateRandomPos();


