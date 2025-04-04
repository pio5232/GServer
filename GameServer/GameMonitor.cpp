#include "pch.h"
#include "GameMonitor.h"
#include "SectorManager.h"
#include "GameSession.h"
#include "GamePlayer.h"
#include "PlayerManager.h"
void C_Utility::GameMonitor::MonitoringJob()
{
	printf("+-------------------------------------------------------------------------+\n");
	printf("\t\t [Sector]\n");	

	_sectorManager->PrintSectorInfo();

	printf("\t\t [GameSession]\n");
	printf("[GameSession Count : %d],  [SessionManager's Count : %d]\n", C_Network::GameSession::GetAliveGameSessionCount(), _sessionMgr->GetSessionCnt());
	printf("\n\n");

	printf("\t\t [GamePlayer]\n");
	printf("[GamePlayer Count : %d], [PlayerManager's Count : %d]\n",C_Content::GamePlayer::GetAliveGamePlayerCount(), C_Content::PlayerManager::GetInstance().GetPlayerCount());
	printf("+-------------------------------------------------------------------------+\n");

	printf("\n\n\n");
}
