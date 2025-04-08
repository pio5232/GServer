#include "pch.h"
#include "GameServer.h"
#include "PacketHandler.h"
#include "PacketDefine.h"
#include "PlayerManager.h"
#include "BufferMaker.h"
#include "GameSession.h"
#include "LanClientSession.h"
#include "GamePlayer.h"
#include "WorldChat.h"

std::unordered_map<uint16, C_Network::GameClientPacketHandler::PacketFunc> C_Network::GameClientPacketHandler::_packetFuncsDic;
std::unordered_map<uint16, C_Network::LanServerPacketHandler::PacketFunc> C_Network::LanServerPacketHandler::_packetFuncsDic;


// ----------------------------- GameClientPacketHandler ------------------

void C_Network::GameClientPacketHandler::Init()
{
	_packetFuncsDic.clear();

	_packetFuncsDic[ENTER_GAME_REQUEST_PACKET] = ProcessEnterGameRequestPacket;
	_packetFuncsDic[GAME_LOAD_COMPELTE_PACKET] = ProcessLoadCompletedPacket;
	
	_packetFuncsDic[MOVE_START_REQUEST_PACKET] = ProcessMoveStartRequestPacket;
	_packetFuncsDic[MOVE_STOP_REQUEST_PACKET] = ProcessMoveStopRequestPacket;
	_packetFuncsDic[ATTACK_REQUEST_PACKET] = ProcessAttackRequestPacket;

	_packetFuncsDic[CHAT_TO_ROOM_REQUEST_PACKET] = ProcessChatRequestPacket;

}

ErrorCode C_Network::GameClientPacketHandler::ProcessEnterGameRequestPacket(GameSessionPtr& gameSessionPtr, C_Utility::CSerializationBuffer& buffer)
{
	std::shared_ptr<GameServer> server = std::static_pointer_cast<GameServer>(gameSessionPtr->GetServer());
	//C_Network::EnterGameRequestPacket packet;
	ULONGLONG userId;
	ULONGLONG token;
	buffer >> userId >> token;
	printf("Enter Game Packet Recv.. %llu, userId : %llu\n", gameSessionPtr->GetSessionId(),userId);

	ULONGLONG serverToken = server->GetToken();

	//SharedSession session = _sessionMgr->GetSession(sessionId);
	
	if (serverToken != token)
	{
		printf("WRONG TOKEN!!\n");
		C_Network::SharedSendBuffer sharedBuffer = C_Network::BufferMaker::MakeErrorPacket(C_Network::PacketErrorCode::CONNECTED_FAILED_WRONG_TOKEN);

		gameSessionPtr->Send(sharedBuffer);

		gameSessionPtr->Disconnect();
		return ErrorCode::WRONG_TOKEN;
	}
	
	gameSessionPtr->Init(userId);
	// 플레이어 입장.
	GamePlayerPtr myPlayer = server->CreatePlayer(gameSessionPtr);
	gameSessionPtr->SetPlayer(myPlayer);

	C_Network::EnterGameResponsePacket enterGameResponsePacket;

	C_Network::SharedSendBuffer sharedBuffer = C_Network::BufferMaker::MakePacket(enterGameResponsePacket);

	gameSessionPtr->Send(sharedBuffer);

	C_Network::MakeMyCharacterPacket makeMyCharacterPacket;
	makeMyCharacterPacket.entityId = myPlayer->GetEntityId();
	makeMyCharacterPacket.pos = myPlayer->GetPosition();

	C_Network::SharedSendBuffer makeMyCharacterBuffer = C_Network::BufferMaker::MakeSendBuffer(sizeof(makeMyCharacterPacket));

	*makeMyCharacterBuffer << makeMyCharacterPacket.size << makeMyCharacterPacket.type <<
		makeMyCharacterPacket.entityId << makeMyCharacterPacket.pos;
	
	gameSessionPtr->Send(makeMyCharacterBuffer);

	server->TryRun();

	return ErrorCode::NONE;
}

ErrorCode C_Network::GameClientPacketHandler::ProcessLoadCompletedPacket(GameSessionPtr& gameSessionPtr, C_Utility::CSerializationBuffer& buffer)
{
	printf("OnLoadComplet %llu\n", gameSessionPtr->GetUserId());

	std::shared_ptr<C_Network::GameServer> gameServer = std::static_pointer_cast<C_Network::GameServer>(gameSessionPtr->GetServer());

	gameSessionPtr->OnLoadComplete();

	gameServer->CheckLoadingAndStartLogic();

	return ErrorCode();
}

ErrorCode C_Network::GameClientPacketHandler::ProcessMoveStartRequestPacket(GameSessionPtr& gameSessionPtr, C_Utility::CSerializationBuffer& buffer)
{
	C_Network::MoveStartRequestPacket packet;
	buffer >> packet.pos >> packet.rotY;

	std::shared_ptr<C_Network::GameServer> gameServer = std::static_pointer_cast<C_Network::GameServer>(gameSessionPtr->GetServer());

	GameSessionPtr myGSession = gameSessionPtr;
	gameServer->EnqueueAction([packet, myGSession]()
		{
			GamePlayerPtr gamePlayer = myGSession->GetPlayer();

			if (gamePlayer->IsDead())
				return;

			gamePlayer->ProcessMoveStartPacket(packet);
		});
	//if(gamePlayerPtr->_transformComponent._position.)

	return ErrorCode::NONE;
}

ErrorCode C_Network::GameClientPacketHandler::ProcessMoveStopRequestPacket(GameSessionPtr& gameSessionPtr, C_Utility::CSerializationBuffer& buffer)
{
	C_Network::MoveStopRequestPacket packet;
	buffer >> packet.pos >> packet.rotY;

	std::shared_ptr<C_Network::GameServer> gameServer = std::static_pointer_cast<C_Network::GameServer>(gameSessionPtr->GetServer());

	GameSessionPtr myGSession = gameSessionPtr;
	
	gameServer->EnqueueAction([packet, myGSession]()
		{
			GamePlayerPtr gamePlayer = myGSession->GetPlayer();
			
			if (gamePlayer->IsDead())
				return;

			gamePlayer->ProcessMoveStopPacket(packet);
		});

	return ErrorCode::NONE;
}

ErrorCode C_Network::GameClientPacketHandler::ProcessChatRequestPacket(GameSessionPtr& gameSessionPtr, C_Utility::CSerializationBuffer& buffer)
{
	// GameWorld Chat
	// roomNum은 무시하도록 한다.
	uint16 roomNum;
	uint16 messageLen;

	buffer >> roomNum >> messageLen;

	char* payLoad = static_cast<char*>(malloc(messageLen));

	buffer.GetData(payLoad, messageLen);

	ULONGLONG userId = gameSessionPtr->GetUserId();

	PacketHeader packetHeader;

	// --- NotifyPacket
	packetHeader.size = sizeof(userId) + sizeof(messageLen) + messageLen;
	packetHeader.type = CHAT_NOTIFY_PACKET;

	C_Network::SharedSendBuffer notifyBuffer = C_Network::BufferMaker::MakeSendBuffer(sizeof(packetHeader) + packetHeader.size);

	*notifyBuffer << packetHeader << userId << messageLen;
	notifyBuffer->PutData(reinterpret_cast<const char*>(payLoad), messageLen);

	free(payLoad);

	WorldChatPtr worldChatPtr = std::static_pointer_cast<C_Network::GameServer>(gameSessionPtr->GetServer())->GetWorldChatPtr();

	worldChatPtr->DoAsync(&C_Content::WorldChat::Chat, notifyBuffer);

	return ErrorCode::NONE;
}

ErrorCode C_Network::GameClientPacketHandler::ProcessAttackRequestPacket(GameSessionPtr& gameSessionPtr, C_Utility::CSerializationBuffer& buffer)
{
	C_Network::AttackRequestPacket packet;

	std::shared_ptr<C_Network::GameServer> gameServer = std::static_pointer_cast<C_Network::GameServer>(gameSessionPtr->GetServer());

	gameServer->ProxyAttackPacket(gameSessionPtr, packet);

	return ErrorCode::NONE;
}

// --------------------------- LanServerPacketHandler ---------------------
void C_Network::LanServerPacketHandler::Init()
{
	_packetFuncsDic.clear();

	_packetFuncsDic[GAME_SERVER_SETTING_RESPONSE_PACKET] = ProcessGameServerSettingResponsePacket;
}

ErrorCode C_Network::LanServerPacketHandler::ProcessGameServerSettingResponsePacket(LanClientSessionPtr& lanClientSessionPtr, C_Utility::CSerializationBuffer& buffer)
{
	GameServerSettingResponsePacket responsePacket;
	
	uint16 roomNum;
	uint16 requiredUsers;
	uint16 maxUsers;

	buffer >> roomNum >> requiredUsers >> maxUsers;

	printf("방 번호 [%d]\n", roomNum);
	printf("인원	[%d]\n", requiredUsers);
	printf("제한 인원  [%d]\n", maxUsers);

	std::shared_ptr<GameServer> gameServer = lanClientSessionPtr->GetGameServer();//lanClientSessionPtr->GetGameServer();
	
	if (gameServer == nullptr)
	{
		// LOG
		printf("GameServer is Not Exist\n");
		return ErrorCode::SERVER_IS_NOT_EXIST;
	}

	// G
	gameServer->Init(roomNum, requiredUsers, maxUsers);
	
	ULONGLONG xorAfterValue = gameServer->GetToken() ^ xorTokenKey;

	C_Network::GameServerLanInfoPacket packet;

	C_Network::SharedSendBuffer sendBuffer = C_Network::BufferMaker::MakeSendBuffer((sizeof(packet)));

	const std::wstring ip = gameServer->GetNetAddr().GetIpAddress();

	uint16 port = gameServer->GetNetAddr().GetPort();
	if (0 == port)
		port = gameServer->GetRealPort();

	wcscpy_s(packet.ipStr, IP_STRING_LEN, ip.c_str());

	*sendBuffer << packet.size << packet.type;

	sendBuffer->PutData(reinterpret_cast<const char*>(packet.ipStr), IP_STRING_LEN * sizeof(WCHAR));

	*sendBuffer << port << gameServer->GetRoomNumber() << xorAfterValue;

	printf("ClientJoined Success\n");

	wprintf(L"size : [ %d ]\n", packet.size);
	wprintf(L"type : [ %d ]\n", packet.type);
	wprintf(L"ip: [ %s ]\n", packet.ipStr);
	wprintf(L"port : [ %d ]\n", port);
	wprintf(L"Room : [ %d ]\n", gameServer->GetRoomNumber());
	wprintf(L"xorToken : [ %llu ], After : [%llu]\n", gameServer->GetToken(), xorAfterValue);

	lanClientSessionPtr->Send(sendBuffer);
	// 
	// 
	//gameServer->Begin();
	
	return ErrorCode::NONE;
}

