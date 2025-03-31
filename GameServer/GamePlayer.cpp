#include "pch.h"
#include <sstream>
#include <math.h>
#include "GamePlayer.h"
#include "GameSession.h"
#include "Player.h"
#include "PlayerManager.h"
#include "PacketMaker.h"
#include "PlayerStateController.h"
#include "PlayerState.h"

//C_Network::GamePlayer::GamePlayer(ULONGLONG userId, SharedSession sharedSession, bool isAi, Vector3 pos) : _userId(userId), _mySession(sharedSession),
//_transformComponent(userId, pos), _statComponent(userId), _isAi(isAi)
//{
//}
//

C_Content::GamePlayer::GamePlayer(GameSessionPtr gameSessionPtr) : Player(EntityType::GamePlayer),_ownerSession(gameSessionPtr), _userId(gameSessionPtr->GetUserId())
{
}

void C_Content::GamePlayer::Update(float delta)
{
	if (IsDead())
		return;

	_stateController->Update(delta);

	//printf("Player Position : [%0.3f, %0.3f, %0.3f]\n", _transformComponent.GetPosConst().x, _transformComponent.GetPosConst().y, _transformComponent.GetPosConst().z);

	//printf("Rotation Y : %0.3f     DirNormalized : [%0.3f, %0.3f, %0.3f]\n", _transformComponent.GetRotConst().y, _transformComponent.GetNormalizedDir().x, _transformComponent.GetNormalizedDir().y, _transformComponent.GetNormalizedDir().z);

}

void C_Content::GamePlayer::ProcessMoveStartPacket(const MoveStartRequestPacket& clientPacket)
{
	//printf("Process Move Start [%0.3f, %0.3f, %0.3f]\n", clientPacket.pos.x, clientPacket.pos.y, clientPacket.pos.z);
	CheckSync(clientPacket.pos);

	_transformComponent.SetDirection(clientPacket.rotY);

	_stateController->ChangeState(&C_Content::PlayerMoveState::GetInstance());

	BroadcastMoveState();
}

void C_Content::GamePlayer::ProcessMoveStopPacket(const MoveStopRequestPacket& clientPacket)
{
	//printf("Process Move Stop [%0.3f, %0.3f, %0.3f]\n", clientPacket.pos.x, clientPacket.pos.y, clientPacket.pos.z);
	//printf("Process Move Stop [%0.3f, %0.3f, %0.3f], Rot : %0.3f\n", clientPacket.pos.x, clientPacket.pos.y, clientPacket.pos.z,clientPacket.rotY);

	CheckSync(clientPacket.pos);

	//_transformComponent.SetDirection(clientPacket.stopDir);

	_transformComponent.SetDirection(clientPacket.rotY);

	_stateController->ChangeState(&C_Content::PlayerIdleState::GetInstance());

	BroadcastMoveState();
}


void C_Content::GamePlayer::CheckSync(const Vector3& clientPos)
{
	Vector3 serverPos = _transformComponent.GetPosConst();

	if (abs(serverPos.x - clientPos.x) < defaultErrorRange && abs(serverPos.z - clientPos.z) < defaultErrorRange)
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
	
	SharedSendBuffer buffer = C_Network::PacketMaker::MakeSendBuffer(sizeof(syncPacket));

	*buffer << syncPacket.size << syncPacket.type << syncPacket.entityId << syncPacket.syncPos << syncPacket.syncRot;

	C_Content::PlayerManager::GetInstance().SendToAllPlayer(buffer);

	printf("Sync!! ID - %llu , Position [%0.3f, %0.3f, %0.3f]\n", GetEntityId(), serverPos.x, serverPos.y, serverPos.z);
}
