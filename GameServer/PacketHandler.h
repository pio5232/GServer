#pragma once

namespace C_Network
{

	class GameClientPacketHandler
	{
	public:
		using PacketFunc = ErrorCode(*)(GameSessionPtr&, C_Utility::CSerializationBuffer&);

		static void Init();

		static ErrorCode ProcessPacket(GameSessionPtr& gameSessionPtr, uint16 packetType, C_Utility::CSerializationBuffer& buffer)
		{
			if (_packetFuncsDic.find(packetType) == _packetFuncsDic.end())
				return ErrorCode::CANNOT_FIND_PACKET_FUNC;

			return _packetFuncsDic[packetType](gameSessionPtr, buffer);

		}

		static ErrorCode ProcessEnterGameRequestPacket(GameSessionPtr& gameSessionPtr, C_Utility::CSerializationBuffer& buffer);
		static ErrorCode ProcessLoadCompletedPacket(GameSessionPtr& gameSessionPtr, C_Utility::CSerializationBuffer& buffer);

		static ErrorCode ProcessMoveStartRequestPacket(GameSessionPtr& gameSessionPtr, C_Utility::CSerializationBuffer& buffer);
		static ErrorCode ProcessMoveStopRequestPacket(GameSessionPtr& gameSessionPtr, C_Utility::CSerializationBuffer& buffer);
		static ErrorCode ProcessChatRequestPacket(GameSessionPtr& gameSessionPtr, C_Utility::CSerializationBuffer& buffer);
	
		static ErrorCode ProcessAttackRequestPacket(GameSessionPtr& gameSessionPtr, C_Utility::CSerializationBuffer& buffer);
	private:
		static std::unordered_map<uint16, PacketFunc> _packetFuncsDic;

	};


	class LanServerPacketHandler
	{
	public:
		using PacketFunc = ErrorCode(*)(LanClientSessionPtr&, C_Utility::CSerializationBuffer&);

		static void Init();

		static ErrorCode ProcessPacket(LanClientSessionPtr& lanClientSessionPtr, uint16 packetType, C_Utility::CSerializationBuffer& buffer)
		{
			if (_packetFuncsDic.find(packetType) == _packetFuncsDic.end())
				return ErrorCode::CANNOT_FIND_PACKET_FUNC;

			return _packetFuncsDic[packetType](lanClientSessionPtr, buffer);

		}

		static ErrorCode ProcessGameServerSettingResponsePacket(LanClientSessionPtr& lanClientSessionPtr, C_Utility::CSerializationBuffer& buffer);

	private:
		static std::unordered_map<uint16, PacketFunc> _packetFuncsDic;

	};

}