#pragma once

namespace C_Content
{
	class PacketBuilder
	{
	public:
		static C_Network::SharedSendBuffer BuildAttackNotifyPacket(ULONGLONG entityId);
	};
}

