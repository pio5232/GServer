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
	// ���� �ٲٱ�.
	while (true)
	{
		float deltaTime = timer.Lap<float>();

		if (deltaTime > limitDeltaTime)
			deltaTime = limitDeltaTime;

		ProcessActions();

		if (false == _isRunning.load())
			continue;

		deltaSum += deltaTime;
		// ��Ʈ��ũ
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
	// ���۰� ������� ó���Ǿ�� �ϴ� �͵� �ƴϰ�, ���������� �ʾ����� ����
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
		// entityDic���� ����������, entityToVectorDic���� �������� �ʴ� ��Ȳ.. �̻��� ��Ȳ��
		// => insert delete�� �����ϰ� ����Ǵ��� üũ �ʿ�
		printf("Remove Entity - Fatal Error!!!!! Check Please !!!\n");
		return;
	}

	// 2. _entityToVectorIdx.erase
	int idx = entityToVectorIter->second;
	_entityToVectorIdxDic.erase(entityToVectorIter);

	int lastIdx = _entityArr.size() - 1;

	// _entityToVectorIdx�� ����� vector index�� �̿��� ����
	// 
	// _entityArr[idx]			-------		_entityArr[lastIdx]
	//							  ��
	// _entityArr[lastIdx]		-------		_entityArr[idx]
	// Ž������ �ʰ� ������ �ε��� ���ҿ� �ٲ㼭 ����.
	if (lastIdx != idx)
	{
		std::swap(_entityArr[idx], _entityArr[lastIdx]);

		// _entityToVectorIdxDic[ origin lastIdx entity ] = lastIdx;
		//							��
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

	// �ʱ�ȭ ���� ��� �����ߴٴ� �ǹ�
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
