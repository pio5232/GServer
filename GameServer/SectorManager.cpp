#include "pch.h"
#include "SectorManager.h"
#include "Entity.h"
#include "BufferMaker.h"
#include "PlayerManager.h"
#include "GamePlayer.h"
#include "PacketBuilder.h"

bool C_Content::SectorManager::AddEntity(int sectorZ, int sectorX, EntityPtr entity)
{
	if (false == IsValidSector(sectorX, sectorZ) || false == _sectorSet[sectorZ][sectorX].insert(entity).second)
	{
		return false;
	}

	if(entity->GetType() == C_Content::Entity::EntityType::GamePlayer)
		_gamePlayerCount[sectorZ][sectorX]++;
	
	return true;
}

bool C_Content::SectorManager::DeleteEntity(int sectorZ, int sectorX, EntityPtr entity)
{
	if (false == IsValidSector(sectorX, sectorZ) || _sectorSet[sectorZ][sectorX].erase(entity) == 0)
	{
		printf("DeleteEntity False\n");
		return false;
	}
	if (entity->GetType() == C_Content::Entity::EntityType::GamePlayer)
		_gamePlayerCount[sectorZ][sectorX]--;

	C_Network::SharedSendBuffer sendBuffer = C_Content::PacketBuilder::BuildDeleteOtherCharacterPacket(entity->GetEntityId());

	SendPacketAroundSector(sectorX, sectorZ, sendBuffer);

	return true;
}

// 분리할 수가 없어.
void C_Content::SectorManager::SendAllEntityInfo()
{
	std::vector<EntityPtr> aroundEntityInfos;

	C_Network::MakeOtherCharacterPacket makeOtherCharacterPacket;

	// 각 섹터마다 정보를 모아서 주변의 섹터에게 뿌림.
	for (int z = startZSectorPos; z < endZSectorPos; z++)
	{
		for (int x = startXSectorPos; x < endXSectorPos; x++)
		{
			if (_sectorSet[z][x].size() == 0)
				continue;

			aroundEntityInfos.clear();

			for (const auto& entity : _sectorSet[z][x])
			{
				aroundEntityInfos.push_back(entity);
			}

			C_Network::SharedSendBuffer sendBuffer = C_Network::BufferMaker::MakeSendBuffer(sizeof(C_Network::MakeOtherCharacterPacket) * aroundEntityInfos.size());

			for (const EntityPtr& entity : aroundEntityInfos)
			{
				*sendBuffer << makeOtherCharacterPacket.size << makeOtherCharacterPacket.type
					<< entity->GetEntityId() << entity->GetPosition();
			}

			SendPacketAroundSector(x, z, sendBuffer);
		}
	}
}

//void C_Content::SectorManager::SendDeletEntityInfo(EntityPtr delEntity)
//{
//	Sector delEntitySector = delEntity->GetCurrentSector();
//
//	// 현재는 Player밖에 존재하지 않음.
//
//	C_Network::SharedSendBuffer sendBuffer = C_Content::PacketBuilder::BuildDeleteOtherCharacterPacket(delEntity->GetEntityId());
//	for (int dz = -1; dz <= 1; dz++)
//	{
//		for (int dx = -1; dx <= 1; dx++)
//		{
//			if (0 == _gamePlayerCount[delEntitySector.z + dz][delEntitySector.x + dx])
//				continue;
//
//			for (const auto& entity : _sectorSet[delEntitySector.z + dz][delEntitySector.x + dx])
//			{
//				if (entity->GetType() == C_Content::Entity::EntityType::GamePlayer)
//				{
//					printf("PlayerPosition : [%0.3f, %0.3f, %0.3f], Sector [%u, %u]\n", entity->GetPosition().x, entity->GetPosition().y, entity->GetPosition().z, entity->GetCurrentSector().z, entity->GetCurrentSector().x);
//					ULONGLONG userId = std::static_pointer_cast<GamePlayer>(entity)->GetUserId();
//					C_Content::PlayerManager::GetInstance().SendToPlayer(sendBuffer, userId);
//				}
//			}
//		}
//	}
//}

void C_Content::SectorManager::UpdateSector(EntityPtr entity)
{
	// 나는 player, ai가 존재하기 때문에 판별 잘 해야한다.
	AroundSector removeAroundSector, addAroundSector;

	GetUpdatedSectorAround(entity, &removeAroundSector, &addAroundSector);
	
	// Player가 아니어도 작동해야함.
	// 1. 삭제되어야할 섹터에 존재하는 플레이어에게 삭제 패킷을 보낸다. (나를 삭제해라!!)
	C_Network::SharedSendBuffer sendBuffer = C_Content::PacketBuilder::BuildDeleteOtherCharacterPacket(entity->GetEntityId());

	for (int i = 0; i < removeAroundSector.sectorCount; i++)
	{
		//printf("1_UpdateSector [%d] : %llu [Sector [%d, %d]]\n", i, entity->GetEntityId(),removeAroundSector.sectors[i].z, removeAroundSector.sectors[i].x);
		SendPacket_Sector(removeAroundSector.sectors[i], sendBuffer);
	}
	

	
	// Player가 아니더라도 작동해야한다.
	// 3. 추가되어야할 섹터에 나를 생성 패킷을 보낸다. (나를 만들어라!!!)
	C_Network::SharedSendBuffer makeBuffer = C_Content::PacketBuilder::BuildMakeOtherCharacterPacket(entity->GetEntityId(), entity->GetPosition());
	C_Network::SharedSendBuffer moveStartBuffer = C_Content::PacketBuilder::BuildMoveStartNotifyPacket(entity->GetEntityId(), entity->GetPosition(), entity->GetRotation().y);

	for (int i = 0; i < addAroundSector.sectorCount; i++)
	{
		//printf("2_UpdateSector [%d] : %llu [Sector [%d, %d]]\n", i, entity->GetEntityId(), removeAroundSector.sectors[i].z, removeAroundSector.sectors[i].x);

		SendPacket_Sector(addAroundSector.sectors[i], makeBuffer);

		// 만약 내가 이동중이면 이동중인 것까지 보내야한다.
		if (entity->IsMoving())
		{
			//printf("2_UpdateSector Move [%d] : %llu [Sector [%d, %d]]\n", i, entity->GetEntityId(), removeAroundSector.sectors[i].z, removeAroundSector.sectors[i].x);
			SendPacket_Sector(addAroundSector.sectors[i], moveStartBuffer);
		}
	}
	
	if (entity->GetType() != C_Content::Entity::EntityType::GamePlayer)
		return;

	ULONGLONG userId = std::static_pointer_cast<GamePlayer>(entity)->GetUserId();

	// Player일 때만 작동.
	// 2. 내가 바라보는.. (Player) 삭제되어야하는 섹터에 존재하는 entity들을 삭제.
	// entity가 플레이어가 아니라면 보낼 필요 없다. (내가 삭제하겠어!!!)
	for (int i = 0; i < removeAroundSector.sectorCount; i++)
	{
		for (const auto& removeEntity : _sectorSet[removeAroundSector.sectors[i].z][removeAroundSector.sectors[i].x])
		{
			C_Network::SharedSendBuffer buffer = C_Content::PacketBuilder::BuildDeleteOtherCharacterPacket(removeEntity->GetEntityId());

			//printf("Hey Remove!! Update Sector 3 : %llu\n", entity->GetEntityId());

			C_Content::PlayerManager::GetInstance().SendToPlayer(buffer, userId);
		}
	}
	

	// Player일 때만 작동
	// 4. 추가되어야할 섹터의 정보들을 나에게 쏜다. (내가 만들겠다!!!!)
	for (int i = 0; i < addAroundSector.sectorCount; i++)
	{
		for (const auto& addEntity : _sectorSet[addAroundSector.sectors[i].z][addAroundSector.sectors[i].x])
		{
			if (addEntity->IsDead())
				continue;

			C_Network::SharedSendBuffer makeBuffer = C_Content::PacketBuilder::BuildMakeOtherCharacterPacket(addEntity->GetEntityId(), addEntity->GetPosition());

			C_Content::PlayerManager::GetInstance().SendToPlayer(makeBuffer, userId);

			if (addEntity->IsMoving())
			{
				C_Network::SharedSendBuffer moveStartBuffer = C_Content::PacketBuilder::BuildMoveStartNotifyPacket(addEntity->GetEntityId(), addEntity->GetPosition(), addEntity->GetRotation().y);
				C_Content::PlayerManager::GetInstance().SendToPlayer(moveStartBuffer, userId);

			}
		}
	}
	




	//{
	//	// Player가 아니어도 작동해야함.
	//	// 1. 삭제되어야할 섹터에 존재하는 플레이어에게 삭제 패킷을 보낸다. (나를 삭제해라!!)
	//	C_Network::SharedSendBuffer sendBuffer = C_Content::PacketBuilder::BuildDeleteOtherCharacterPacket(entity->GetEntityId());
	//	for (int i = 0; i < removeAroundSector.sectorCount; i++)
	//	{
	//		SendPacket_Sector(removeAroundSector.sectors[i], sendBuffer);
	//	}
	//}

	//{
	//	// Player일 때만 작동.
	//	// 2. 내가 바라보는.. (Player) 삭제되어야하는 섹터에 존재하는 entity들을 삭제.
	//	// entity가 플레이어가 아니라면 보낼 필요 없다. (내가 삭제하겠어!!!)

	//	if (isGamePlayer)
	//	{
	//		for (int i = 0; i < removeAroundSector.sectorCount; i++)
	//		{
	//			for (const auto& removeEntity : _sectorSet[removeAroundSector.sectors[i].z][removeAroundSector.sectors[i].x])
	//			{
	//				C_Network::SharedSendBuffer sendBuffer = C_Content::PacketBuilder::BuildDeleteOtherCharacterPacket(removeEntity->GetEntityId());

	//				C_Content::PlayerManager::GetInstance().SendToPlayer(sendBuffer, userId);
	//			}
	//		}
	//	}
	//}

	//{
	//	// Player가 아니더라도 작동해야한다.
	//	// 3. 추가되어야할 섹터에 나를 생성 패킷을 보낸다. (나를 만들어라!!!)
	//	C_Network::SharedSendBuffer makeBuffer = C_Content::PacketBuilder::BuildMakeOtherCharacterPacket(entity->GetEntityId(), entity->GetPosition());
	//	C_Network::SharedSendBuffer moveStartBuffer = C_Content::PacketBuilder::BuildMoveStartNotifyPacket(entity->GetEntityId(), entity->GetPosition(), entity->GetRotation().y);
	//	
	//	for (int i = 0; i < addAroundSector.sectorCount; i++)
	//	{
	//		SendPacket_Sector(addAroundSector.sectors[i], makeBuffer);

	//		// 만약 내가 이동중이면 이동중인 것까지 보내야한다.
	//		if (entity->IsMoving())
	//		{
	//			SendPacket_Sector(addAroundSector.sectors[i], moveStartBuffer);
	//		}
	//	}
	//}

	//{
	//	// Player일 때만 작동
	//	// 4. 추가되어야할 섹터의 정보들을 나에게 쏜다. (내가 만들겠다!!!!)
	//	for (int i = 0; i < addAroundSector.sectorCount; i++)
	//	{
	//		for (const auto& addEntity : _sectorSet[addAroundSector.sectors[i].z][addAroundSector.sectors[i].x])
	//		{
	//			C_Network::SharedSendBuffer makeBuffer = C_Content::PacketBuilder::BuildMakeOtherCharacterPacket(addEntity->GetEntityId(), addEntity->GetPosition());

	//			C_Content::PlayerManager::GetInstance().SendToPlayer(makeBuffer, userId);
	//			
	//			if (addEntity->IsMoving())
	//			{
	//				C_Network::SharedSendBuffer moveStartBuffer = C_Content::PacketBuilder::BuildMoveStartNotifyPacket(addEntity->GetEntityId(), addEntity->GetPosition(), addEntity->GetRotation().y);
	//				C_Content::PlayerManager::GetInstance().SendToPlayer(moveStartBuffer, userId);

	//			}
	//		}
	//	}
	//}
}

void C_Content::SectorManager::SendPacket_Sector(const Sector& sector, C_Network::SharedSendBuffer sendBuffer)
{
	if (false == IsValidSector(sector.x, sector.z) || 0 == _gamePlayerCount[sector.z][sector.x])
	{
		//printf("SendPacket_Sector Return [%d, %d] \n",sector.z,sector.x);
		return;
	}

	for (const auto& entity : _sectorSet[sector.z][sector.x])
	{
		if (entity->GetType() == C_Content::Entity::EntityType::GamePlayer)
		{
			ULONGLONG userId = std::static_pointer_cast<GamePlayer>(entity)->GetUserId();

			//printf("SendPacket Sector1\n");
			C_Content::PlayerManager::GetInstance().SendToPlayer(sendBuffer, userId);
		}
	}
}

void C_Content::SectorManager::SendPacketAroundSector(const Sector& sector, C_Network::SharedSendBuffer sendBuffer)
{
	if (false == IsValidSector(sector.x, sector.z))
		return;

	AroundSector aroundSector;

	GetSectorAround(sector.x, sector.z, &aroundSector);

	//printf("[AroundSector - Sector]GetAroundSectorCount - %d\n", aroundSector.sectorCount);
	//for (int i = 0; i < aroundSector.sectorCount; i++)
	//{
	//	printf("\t\t AroudnSector [%d] = [%d, %d]\n", i, aroundSector.sectors[i].z, aroundSector.sectors[i].x);
	//}

	for (int i = 0; i < aroundSector.sectorCount; i++)
	{
		SendPacket_Sector(aroundSector.sectors[i], sendBuffer);
	}
}

void C_Content::SectorManager::SendPacketAroundSector(int sectorX, int sectorZ, C_Network::SharedSendBuffer sendBuffer)
{
	if (false == IsValidSector(sectorX, sectorZ))
		return;

	AroundSector aroundSector;

	GetSectorAround(sectorX, sectorZ, &aroundSector);

	//printf("[AroundSector - (X,Z)]GetAroundSectorCount - %d\n", aroundSector.sectorCount);
	//for (int i = 0; i < aroundSector.sectorCount; i++)
	//{
	//	printf("\t\t AroudnSector [%d] = [%d, %d]\n", i, aroundSector.sectors[i].z, aroundSector.sectors[i].x);
	//}

	for (int i = 0; i < aroundSector.sectorCount; i++)
	{
		SendPacket_Sector(aroundSector.sectors[i], sendBuffer);
	}
}

void C_Content::SectorManager::PrintSectorInfo() const
{
	printf("각 섹터의 플레이어 수, 멀티스레드 환경에서 정확하지 않게 출력될 수 있음.\n");
	
	for (int z = startZSectorPos; z < endZSectorPos; z++)
	{
		for (int x = startXSectorPos; x < endXSectorPos; x++)
		{
			printf("[%3hu] ", _gamePlayerCount[z][x]);
		}
		printf("\n");
	}
	printf("\n\n");
}

EntityPtr C_Content::SectorManager::GetMinEntityInRange(EntityPtr targetEntity, float range)
{
	// Range가 섹터 하나의 크기보다 크다면..? 어떻게 처리하지
	float minRange = range * range;
	EntityPtr minEntity = nullptr;

	// 1. 내 주변 섹터 얻어오기.
	Sector sector = targetEntity->GetCurrentSector();
	AroundSector aroundSector;
	GetSectorAround(sector.x,sector.z, &aroundSector);
	
	// 내가 보는 방향
	Vector3 from = targetEntity->GetNormalizedForward();
	Vector3 targetPosition = targetEntity->GetPosition();

	// 2. 얻어온 섹터를 순회하면서 비교한다. 
	for (int i = 0; i < aroundSector.sectorCount; i++)
	{
		for (auto& entity : _sectorSet[aroundSector.sectors[i].z][aroundSector.sectors[i].x])
		{
			if (entity->IsDead() || entity == targetEntity)
				continue;

			Vector3 dist = entity->GetPosition() - targetPosition;
			float sqrDist = dist.sqrMagnitude();
			if (sqrDist < minRange)
			{
				Vector3 to = dist.Normalized();

				if (Vector3::Angle(from, to) < 30.0f)
				{
					minRange = sqrDist;
					minEntity = entity;
				}
			}
		}
	}

	return minEntity;
}

bool C_Content::SectorManager::IsValidSector(int sectorXPos, int sectorZPos)
{
	return sectorXPos >= startXSectorPos && sectorXPos < endXSectorPos &&
		sectorZPos >= startZSectorPos && sectorZPos < endZSectorPos;
}

void C_Content::SectorManager::GetSectorAround(int sectorXPos, int sectorZPos, AroundSector* pAroundSector)
{
	// 좌표 기준 9칸 
	// [] [] []
	// [] [] []
	// [] [] []

	memset(pAroundSector, 0, sizeof(AroundSector));
	// 테두리를 제외한 부분에 속해있으면 Sector 추가.
	int sectorIndex = 0;
	for (int dz = -1; dz <= 1; dz++)
	{
		for (int dx = -1; dx <= 1; dx++)
		{
			if (IsValidSector(sectorXPos+dx, sectorZPos+dz))
			{
				pAroundSector->sectors[sectorIndex].x = sectorXPos + dx;
				pAroundSector->sectors[sectorIndex].z = sectorZPos + dz;

				sectorIndex++;
			}
		}
	}
	pAroundSector->sectorCount = sectorIndex;

	//printf("GetSectorAround sector count -> %d\n", sectorIndex);
}

void C_Content::SectorManager::GetUpdatedSectorAround(EntityPtr entity, AroundSector* pRemoveAroundSector, AroundSector* pAddAroundSector)
{
	int prevSectorX = entity->GetPrevSector().x;
	int prevSectorZ = entity->GetPrevSector().z;

	int curSectorX = entity->GetCurrentSector().x;
	int curSectorZ = entity->GetCurrentSector().z;

	//printf("GetUpdatedSector [%d, %d] ---> [%d, %d]\n", prevSectorZ, prevSectorX, curSectorZ, curSectorX);
	// 이전 섹터 9방향, 다음 섹터 9방향 get
	GetSectorAround(prevSectorX, prevSectorZ, pRemoveAroundSector);
	GetSectorAround(curSectorX, curSectorZ, pAddAroundSector);

	std::set<Sector> addSet;
	std::set<Sector> removeSet;

	for (int i = 0; i < pRemoveAroundSector->sectorCount; i++)
	{
		removeSet.insert({ pRemoveAroundSector->sectors[i].x, pRemoveAroundSector->sectors[i].z });
	}

	for (int i = 0; i < pAddAroundSector->sectorCount; i++)
	{
		addSet.insert({ pAddAroundSector->sectors[i].x, pAddAroundSector->sectors[i].z });
	}

	std::vector<Sector> pureAddVec;
	std::vector<Sector> pureRemoveVec;

	// 
	for (const auto& sector : addSet)
	{
		if (removeSet.find(sector) == removeSet.end())
			pureAddVec.push_back(sector);
	}

	for (const auto& sector : removeSet)
	{
		if (addSet.find(sector) == addSet.end())
			pureRemoveVec.push_back(sector);
	}

	memset(pRemoveAroundSector, 0, sizeof(AroundSector));
	memset(pAddAroundSector, 0, sizeof(AroundSector));

	// 겹치는 부분을 제외한 새롭게 얻는 부분 / 삭제할 부분에 대한 정보를 얻음
	
	int idx = 0;
	for (const Sector& sector : pureAddVec)
	{
		pAddAroundSector->sectors[idx++] = sector;
	}
	pAddAroundSector->sectorCount = idx;

	idx = 0;
	for (const Sector& sector : pureRemoveVec)
	{
		pRemoveAroundSector->sectors[idx++] = sector;
	}
	pRemoveAroundSector->sectorCount = idx;

	if (_sectorSet[prevSectorZ][prevSectorX].erase(entity) == 0)
	{
		printf("Sector삭제가 안되었는걸요..? 문제인데요..\n");
	}

	if (_sectorSet[curSectorZ][curSectorX].insert(entity).second == false)
	{
		printf("Sector 추가가 안되었는걸요..? 문제인데요?\n");
	}

	if (entity->GetType() == Entity::EntityType::GamePlayer)
	{
		_gamePlayerCount[prevSectorZ][prevSectorX]--;
		_gamePlayerCount[curSectorZ][curSectorX]++;
	}
}
