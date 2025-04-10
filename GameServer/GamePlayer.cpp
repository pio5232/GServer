#include "pch.h"
#include <sstream>
#include <math.h>
#include "GamePlayer.h"
#include "GameSession.h"
#include "Player.h"
#include "PlayerManager.h"
#include "BufferMaker.h"
#include "PlayerStateController.h"
#include "PlayerState.h"
#include "PacketBuilder.h"
#include "GameWorld.h"

std::atomic<int> C_Content::GamePlayer::_aliveGamePlayerCount;

using namespace C_Network;
C_Content::GamePlayer::GamePlayer(GameSessionPtr gameSessionPtr, GameWorld* worldPtr) : Player(worldPtr, EntityType::GamePlayer, posUpdateInterval),_ownerSession(gameSessionPtr), _userId(gameSessionPtr->GetUserId())
{
	_aliveGamePlayerCount.fetch_add(1);
}

void C_Content::GamePlayer::Update(float delta)
{
	if (IsDead())
		return;

	Player::Update(delta);

	//printf("Player Position : [%0.3f, %0.3f, %0.3f]\n", _transformComponent.GetPosConst().x, _transformComponent.GetPosConst().y, _transformComponent.GetPosConst().z);

	//printf("Rotation Y : %0.3f     DirNormalized : [%0.3f, %0.3f, %0.3f]\n", _transformComponent.GetRotConst().y, _transformComponent.GetNormalizedDir().x, _transformComponent.GetNormalizedDir().y, _transformComponent.GetNormalizedDir().z);

}

void C_Content::GamePlayer::ProcessMoveStartPacket(const MoveStartRequestPacket& clientPacket)
{
	//printf("Process Move Start [%0.3f, %0.3f, %0.3f] RotY : %f\n", clientPacket.pos.x, clientPacket.pos.y, clientPacket.pos.z, clientPacket.rotY);
	SyncPos(clientPacket.pos);

	_transformComponent.SetDirection(clientPacket.rotY);

	_stateController->ChangeState(&C_Content::PlayerMoveState::GetInstance());

	BroadcastMoveState();
}

void C_Content::GamePlayer::ProcessMoveStopPacket(const MoveStopRequestPacket& clientPacket)
{
	//printf("Process Move Stop [%0.3f, %0.3f, %0.3f] RotY : %f\n", clientPacket.pos.x, clientPacket.pos.y, clientPacket.pos.z,clientPacket.rotY);
	//printf("Process Move Stop [%0.3f, %0.3f, %0.3f], Rot : %0.3f\n", clientPacket.pos.x, clientPacket.pos.y, clientPacket.pos.z,clientPacket.rotY);

	SyncPos(clientPacket.pos);

	_transformComponent.SetDirection(clientPacket.rotY);

	_stateController->ChangeState(&C_Content::PlayerIdleState::GetInstance());

	BroadcastMoveState();
}


void C_Content::GamePlayer::SetAttackState()
{
	_stateController->ChangeState(&C_Content::PlayerAttackState::GetInstance());
}


void C_Content::GamePlayer::SyncPos(const Vector3& clientPos)
{
	Vector3 serverPos = _transformComponent.GetPosConst();

	//if (abs(serverPos.x - clientPos.x) < defaultErrorRange && abs(serverPos.z - clientPos.z) < defaultErrorRange)
	//if (Vector3::Distance(serverPos, clientPos) < defaultErrorRange)
	if ((serverPos - clientPos).sqrMagnitude() < (defaultErrorRange * defaultErrorRange))
	{
		// 오차 범위가 작으면 start / stop 나올 때 clientPos를 기준으로 설정. 
		// 
		_transformComponent.SetPosition(clientPos);
		return;
	}
	CharacterPositionSyncPacket syncPacket;
	syncPacket.entityId = GetEntityId();
	syncPacket.syncPos = serverPos;
	syncPacket.syncRot = _transformComponent.GetRotConst();
	
	SharedSendBuffer buffer = C_Network::BufferMaker::MakeSendBuffer(sizeof(syncPacket));

	*buffer << syncPacket.size << syncPacket.type << syncPacket.entityId << syncPacket.syncPos << syncPacket.syncRot;

	C_Content::PlayerManager::GetInstance().SendToAllPlayer(buffer);

	printf("Sync!! ID - %llu ,PlayerPos [%0.3f, %0.3f, %0.3f], Position [%0.3f, %0.3f, %0.3f]\n", GetEntityId(),clientPos.x, clientPos.y, clientPos.z, serverPos.x, serverPos.y, serverPos.z);
	//printf("Sync Rot ID - %llu ,Server Rotation [%0.3f, %0.3f, %0.3f]\n\n", GetEntityId(),syncPacket.syncRot.x, syncPacket.syncRot.y, syncPacket.syncRot.z);
}
