#pragma once

namespace C_Content
{
	class PacketBuilder
	{
	public:
		static C_Network::SharedSendBuffer BuildAttackNotifyPacket(ULONGLONG entityId);
		static C_Network::SharedSendBuffer BuildMakeOtherCharacterPacket(ULONGLONG entityId, const Vector3& position);
		static C_Network::SharedSendBuffer BuildGameInitDonePacket();
		static C_Network::SharedSendBuffer BuildDeleteOtherCharacterPacket(ULONGLONG entityId);
		static C_Network::SharedSendBuffer BuildMoveStartNotifyPacket(ULONGLONG entityId, const Vector3& pos, float rotY);
		static C_Network::SharedSendBuffer BuildMoveStopNotifyPacket(ULONGLONG entityId, const Vector3& pos, float rotY);
		static C_Network::SharedSendBuffer BuildUpdateTransformPacket(ULONGLONG timeStamp, ULONGLONG entityId, const Vector3& pos, const Vector3& rot);
	};
}

