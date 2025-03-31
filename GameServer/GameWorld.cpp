#include "pch.h"
#include <Windows.h>
#include "GameWorld.h"
#include "Entity.h"
#include "WorldChat.h"

C_Content::GameWorld::GameWorld()
{
	InitializeSRWLock(&_actionLock);

	srand(GetCurrentThreadId());
	
	_startEvent = CreateEvent(nullptr, false, false, nullptr);

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
	SetEvent(_startEvent);
}

void C_Content::GameWorld::Update()
{
	DWORD ret = WaitForSingleObject(_startEvent, INFINITE);

	if (ret != WAIT_OBJECT_0)
	{
		printf("Update Wait ret is Not Wait_obj_0... Return...\n");
		return;
	}
	else
	{
		printf("Update Start !! \n");
	}

	C_Utility::CTimer timer;

	timer.Start();

	// 조건 바꾸기.
	while (true)
	{
		float delta = timer.Lap<float>();

		ProcessActions();

		for (EntityPtr& entity : _entityArr)
		{
			entity->Update(delta);
		}
	}


}

void C_Content::GameWorld::EnqueueAction(Action&& action)
{
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
	
}

void C_Content::GameWorld::SetDSCount(uint16 predMaxCnt)
{
	_entityDic.reserve(predMaxCnt);
	_entityArr.reserve(predMaxCnt);
	_entityToVectorIdxDic.reserve(predMaxCnt);
}
