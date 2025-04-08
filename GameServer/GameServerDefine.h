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


constexpr float COS_30 = 0.8660254f;

constexpr float mapXMin = 0;
constexpr float mapXMax = 100.0f;
constexpr float mapZMin = 0;
constexpr float mapZMax = 100.0f;

constexpr float centerX = (mapXMax - mapXMin) / 2.0f;
constexpr float centerZ = (mapZMax - mapZMin) / 2.0f;
constexpr int sectorCriteriaSize = 10;

constexpr int sectorMaxX = ((int)mapXMax - (int)mapXMin) / sectorCriteriaSize + 2; // 0 [ 1 2 3 4 5 ] 6
constexpr int sectorMaxZ = ((int)mapZMax - (int)mapZMin) / sectorCriteriaSize + 2;

// for문 사용
// for( i = startX/Z; i<endX/Z; i++)
const int startXSectorPos = 1;
const int startZSectorPos = 1;
constexpr int endXSectorPos = sectorMaxX - 1;
constexpr int endZSectorPos = sectorMaxZ - 1;

const float edgeThreshold = 10.0f;

const float defaultSlowWalkSpeed = 3.0f;
const float defaultWalkSpeed = 6.0f;
const float defaultRunSpeed = 9.0f;

const uint16 defaultMaxHp = 3;
const uint16 defaultAttackDamage= 1;

const float defaultAttackRange = 1.6f;

const float defaultErrorRange = 3.0f; // 오차 범위
const float posUpdateInterval = 0.2f;


const float attackDuration = 0.8f;
const float attackedDuration = 1.0f;
const float DeadDuration = 3.0f;

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


