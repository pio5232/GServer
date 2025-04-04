#pragma once
#include "CMonitor.h"

namespace C_Content
{
	class SectorManager;
}
namespace C_Utility
{
	class GameMonitor : public C_Utility::CMonitor
	{
	public:
		GameMonitor(class C_Network::SessionManager* sessionMgr, const class C_Content::SectorManager* const sectorManager) : _sessionMgr(sessionMgr), _sectorManager(sectorManager) {}
		virtual void MonitoringJob() override;

	private:
		class C_Network::SessionManager* _sessionMgr;
		const class C_Content::SectorManager* const _sectorManager;
	};
}
