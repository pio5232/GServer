#include "pch.h"
#include <Windows.h>
#include "GameWorld.h"
#include "Entity.h"
#include "WorldChat.h"
#include "PlayerManager.h"
#include "AIPlayer.h"
#include "BufferMaker.h"
#include "SectorManager.h"
#include "PacketBuilder.h"
C_Content::GameWorld::GameWorld()
{
	_sectorManager = std::make_unique<C_Content::SectorManager>();

	InitializeSRWLock(&_actionLock);

	srand(GetCurrentThreadId());
	
	_isRunning = false;
	_logicThread = std::thread([this]() { this->Update(); });

	_worldChat = std::make_shared<WorldChat>();
}

C_Content::GameWorld::~GameWorld()
{
	_entityDic.clear();
	_entityArr.clear();
}

void C_Content::GameWorld::Start()
{
	_isRunning.store(true);
}

void C_Content::GameWorld::Update()
{
	printf("Update Start !! \n");

	C_Utility::CTimer timer;

	timer.Start();

	float deltaSum = 0;
	// 조건 바꾸기.
	while (true)
	{
		float deltaTime = timer.Lap<float>();

		if (deltaTime > limitDeltaTime)
			deltaTime = limitDeltaTime;

		ProcessActions();

		if (false == _isRunning.load())
			continue;

		deltaSum += deltaTime;
		// 네트워크
		if (deltaSum >= fixedDeltaTime)
		{
			for (EntityPtr& entity : _entityArr)
			{
				entity->Update(fixedDeltaTime);

				if (entity->IsSectorUpdated())
				{
					_sectorManager->UpdateSector(entity);
				}
			}

			deltaSum -= fixedDeltaTime;
		}
	}


}

void C_Content::GameWorld::TryEnqueueAction(Action&& action, bool mustEnqueue)
{
	// 시작과 관계없이 처리되어야 하는 것도 아니고, 시작하지도 않았으면 무시
	if (false == mustEnqueue && false == _isRunning.load())
	{
		return;
	}
	SRWLockGuard lockGuard(&_actionLock);

	_actionQueue.push(std::move(action));
}

void C_Content::GameWorld::ProcessActions()
{
	std::queue<Action> tempActionQueue;

	{
		SRWLockGuard lockGuard(&_actionLock);

		std::swap(_actionQueue, tempActionQueue);
	}

	while (tempActionQueue.size() > 0)
	{
		Action action = std::move(tempActionQueue.front());

		action();

		tempActionQueue.pop();
	}
}

void C_Content::GameWorld::AddEntity(EntityPtr entityPtr)
{
	_entityDic.insert(std::make_pair(entityPtr->GetEntityId(), entityPtr));
	_entityArr.push_back(entityPtr);
	int index = _entityArr.size() - 1;
	_entityToVectorIdxDic.insert(std::make_pair(entityPtr, index));

	Sector sector = entityPtr->GetCurrentSector();
	_sectorManager->AddEntity(sector.z, sector.x, entityPtr);
}

void C_Content::GameWorld::RemoveEntity(ULONGLONG entityId)
{
	auto entityDiciter = _entityDic.find(entityId);

	if (entityDiciter == _entityDic.end())
	{
		printf("Remove Entity - Invalid EntityID\n");
		return;
	}

	EntityPtr entityPtr = entityDiciter->second;
	// 1. _entityDic.erase
	_entityDic.erase(entityDiciter);

	auto entityToVectorIter = _entityToVectorIdxDic.find(entityPtr);
	
	if (entityToVectorIter == _entityToVectorIdxDic.end())
	{
		// entityDic에는 존재하지만, entityToVectorDic에는 존재하지 않는 상황.. 이상한 상황임
		// => insert delete가 동일하게 진행되는지 체크 필요
		printf("Remove Entity - Fatal Error!!!!! Check Please !!!\n");
		return;
	}

	// 2. _entityToVectorIdx.erase
	int idx = entityToVectorIter->second;
	_entityToVectorIdxDic.erase(entityToVectorIter);

	int lastIdx = _entityArr.size() - 1;

	// _entityToVectorIdx에 저장된 vector index를 이용해 삭제
	// 
	// _entityArr[idx]			-------		_entityArr[lastIdx]
	//							  ↓
	// _entityArr[lastIdx]		-------		_entityArr[idx]
	// 탐색하지 않고 마지막 인덱스 원소와 바꿔서 삭제.
	if (lastIdx != idx)
	{
		std::swap(_entityArr[idx], _entityArr[lastIdx]);

		// _entityToVectorIdxDic[ origin lastIdx entity ] = lastIdx;
		//							↓
		// _entityToVectorIdxDic[ orign lastIdx entity ] = idx
		_entityToVectorIdxDic[_entityArr[idx]] = idx;
	}
	// 3. _entityArr.erase 
	_entityArr.pop_back();

	Sector sector = entityPtr->GetCurrentSector();
	_sectorManager->DeleteEntity(sector.z, sector.x, entityPtr);
	
}

void C_Content::GameWorld::Init(uint16 total, uint16 gamePlayerCount)
{
	uint16 aiCount = total - gamePlayerCount;

	for (int i = 0; i < aiCount; i++)
	{
		AIPlayerPtr aiPlayer = C_Content::PlayerManager::GetInstance().CreateAI(this);

		AddEntity(aiPlayer);
	}

	_sectorManager->SendAllEntityInfo();

	// 초기화 내용 모두 전송했다는 의미
	C_Network::SharedSendBuffer sendBuffer = C_Content::PacketBuilder::BuildGameInitDonePacket();

	C_Content::PlayerManager::GetInstance().SendToAllPlayer(sendBuffer);
}

void C_Content::GameWorld::SetDSCount(uint16 predMaxCnt)
{
	_entityDic.reserve(predMaxCnt);
	_entityArr.reserve(predMaxCnt);
	_entityToVectorIdxDic.reserve(predMaxCnt);
}

void C_Content::GameWorld::SendPacketAroundSector(const Sector& sector, C_Network::SharedSendBuffer sendBuffer)
{
	_sectorManager->SendPacketAroundSector(sector, sendBuffer);
}

void C_Content::GameWorld::SendPacketAroundSector(int sectorX, int sectorZ, C_Network::SharedSendBuffer sendBuffer)
{
	_sectorManager->SendPacketAroundSector(sectorX, sectorZ, sendBuffer);
}
