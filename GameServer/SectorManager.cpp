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

// �и��� ���� ����.
void C_Content::SectorManager::SendAllEntityInfo()
{
	std::vector<EntityPtr> aroundEntityInfos;

	C_Network::MakeOtherCharacterPacket makeOtherCharacterPacket;

	// �� ���͸��� ������ ��Ƽ� �ֺ��� ���Ϳ��� �Ѹ�.
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
//	// ����� Player�ۿ� �������� ����.
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
	// ���� player, ai�� �����ϱ� ������ �Ǻ� �� �ؾ��Ѵ�.
	AroundSector removeAroundSector, addAroundSector;

	GetUpdatedSectorAround(entity, &removeAroundSector, &addAroundSector);
	
	// Player�� �ƴϾ �۵��ؾ���.
	// 1. �����Ǿ���� ���Ϳ� �����ϴ� �÷��̾�� ���� ��Ŷ�� ������. (���� �����ض�!!)
	C_Network::SharedSendBuffer sendBuffer = C_Content::PacketBuilder::BuildDeleteOtherCharacterPacket(entity->GetEntityId());

	for (int i = 0; i < removeAroundSector.sectorCount; i++)
	{
		//printf("1_UpdateSector [%d] : %llu [Sector [%d, %d]]\n", i, entity->GetEntityId(),removeAroundSector.sectors[i].z, removeAroundSector.sectors[i].x);
		SendPacket_Sector(removeAroundSector.sectors[i], sendBuffer);
	}
	

	
	// Player�� �ƴϴ��� �۵��ؾ��Ѵ�.
	// 3. �߰��Ǿ���� ���Ϳ� ���� ���� ��Ŷ�� ������. (���� ������!!!)
	C_Network::SharedSendBuffer makeBuffer = C_Content::PacketBuilder::BuildMakeOtherCharacterPacket(entity->GetEntityId(), entity->GetPosition());
	C_Network::SharedSendBuffer moveStartBuffer = C_Content::PacketBuilder::BuildMoveStartNotifyPacket(entity->GetEntityId(), entity->GetPosition(), entity->GetRotation().y);

	for (int i = 0; i < addAroundSector.sectorCount; i++)
	{
		//printf("2_UpdateSector [%d] : %llu [Sector [%d, %d]]\n", i, entity->GetEntityId(), removeAroundSector.sectors[i].z, removeAroundSector.sectors[i].x);

		SendPacket_Sector(addAroundSector.sectors[i], makeBuffer);

		// ���� ���� �̵����̸� �̵����� �ͱ��� �������Ѵ�.
		if (entity->IsMoving())
		{
			//printf("2_UpdateSector Move [%d] : %llu [Sector [%d, %d]]\n", i, entity->GetEntityId(), removeAroundSector.sectors[i].z, removeAroundSector.sectors[i].x);
			SendPacket_Sector(addAroundSector.sectors[i], moveStartBuffer);
		}
	}
	
	if (entity->GetType() != C_Content::Entity::EntityType::GamePlayer)
		return;

	ULONGLONG userId = std::static_pointer_cast<GamePlayer>(entity)->GetUserId();

	// Player�� ���� �۵�.
	// 2. ���� �ٶ󺸴�.. (Player) �����Ǿ���ϴ� ���Ϳ� �����ϴ� entity���� ����.
	// entity�� �÷��̾ �ƴ϶�� ���� �ʿ� ����. (���� �����ϰھ�!!!)
	for (int i = 0; i < removeAroundSector.sectorCount; i++)
	{
		for (const auto& removeEntity : _sectorSet[removeAroundSector.sectors[i].z][removeAroundSector.sectors[i].x])
		{
			C_Network::SharedSendBuffer buffer = C_Content::PacketBuilder::BuildDeleteOtherCharacterPacket(removeEntity->GetEntityId());

			//printf("Hey Remove!! Update Sector 3 : %llu\n", entity->GetEntityId());

			C_Content::PlayerManager::GetInstance().SendToPlayer(buffer, userId);
		}
	}
	

	// Player�� ���� �۵�
	// 4. �߰��Ǿ���� ������ �������� ������ ���. (���� ����ڴ�!!!!)
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
	//	// Player�� �ƴϾ �۵��ؾ���.
	//	// 1. �����Ǿ���� ���Ϳ� �����ϴ� �÷��̾�� ���� ��Ŷ�� ������. (���� �����ض�!!)
	//	C_Network::SharedSendBuffer sendBuffer = C_Content::PacketBuilder::BuildDeleteOtherCharacterPacket(entity->GetEntityId());
	//	for (int i = 0; i < removeAroundSector.sectorCount; i++)
	//	{
	//		SendPacket_Sector(removeAroundSector.sectors[i], sendBuffer);
	//	}
	//}

	//{
	//	// Player�� ���� �۵�.
	//	// 2. ���� �ٶ󺸴�.. (Player) �����Ǿ���ϴ� ���Ϳ� �����ϴ� entity���� ����.
	//	// entity�� �÷��̾ �ƴ϶�� ���� �ʿ� ����. (���� �����ϰھ�!!!)

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
	//	// Player�� �ƴϴ��� �۵��ؾ��Ѵ�.
	//	// 3. �߰��Ǿ���� ���Ϳ� ���� ���� ��Ŷ�� ������. (���� ������!!!)
	//	C_Network::SharedSendBuffer makeBuffer = C_Content::PacketBuilder::BuildMakeOtherCharacterPacket(entity->GetEntityId(), entity->GetPosition());
	//	C_Network::SharedSendBuffer moveStartBuffer = C_Content::PacketBuilder::BuildMoveStartNotifyPacket(entity->GetEntityId(), entity->GetPosition(), entity->GetRotation().y);
	//	
	//	for (int i = 0; i < addAroundSector.sectorCount; i++)
	//	{
	//		SendPacket_Sector(addAroundSector.sectors[i], makeBuffer);

	//		// ���� ���� �̵����̸� �̵����� �ͱ��� �������Ѵ�.
	//		if (entity->IsMoving())
	//		{
	//			SendPacket_Sector(addAroundSector.sectors[i], moveStartBuffer);
	//		}
	//	}
	//}

	//{
	//	// Player�� ���� �۵�
	//	// 4. �߰��Ǿ���� ������ �������� ������ ���. (���� ����ڴ�!!!!)
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
	printf("�� ������ �÷��̾� ��, ��Ƽ������ ȯ�濡�� ��Ȯ���� �ʰ� ��µ� �� ����.\n");
	
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
	// Range�� ���� �ϳ��� ũ�⺸�� ũ�ٸ�..? ��� ó������
	float minRange = range * range;
	EntityPtr minEntity = nullptr;

	// 1. �� �ֺ� ���� ������.
	Sector sector = targetEntity->GetCurrentSector();
	AroundSector aroundSector;
	GetSectorAround(sector.x,sector.z, &aroundSector);
	
	// ���� ���� ����
	Vector3 from = targetEntity->GetNormalizedForward();
	Vector3 targetPosition = targetEntity->GetPosition();

	// 2. ���� ���͸� ��ȸ�ϸ鼭 ���Ѵ�. 
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
	// ��ǥ ���� 9ĭ 
	// [] [] []
	// [] [] []
	// [] [] []

	memset(pAroundSector, 0, sizeof(AroundSector));
	// �׵θ��� ������ �κп� ���������� Sector �߰�.
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
	// ���� ���� 9����, ���� ���� 9���� get
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

	// ��ġ�� �κ��� ������ ���Ӱ� ��� �κ� / ������ �κп� ���� ������ ����
	
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
		printf("Sector������ �ȵǾ��°ɿ�..? �����ε���..\n");
	}

	if (_sectorSet[curSectorZ][curSectorX].insert(entity).second == false)
	{
		printf("Sector �߰��� �ȵǾ��°ɿ�..? �����ε���?\n");
	}

	if (entity->GetType() == Entity::EntityType::GamePlayer)
	{
		_gamePlayerCount[prevSectorZ][prevSectorX]--;
		_gamePlayerCount[curSectorZ][curSectorX]++;
	}
}
