
#include "pch.h"
#include <iostream>
#include <thread>
#include <conio.h>
#include "NetworkBase.h"
#include "PacketHandler.h"
#include <crtdbg.h>
#include "GameServer.h"
#include "GameSession.h"
#define SERVERPORT 8768

C_Utility::CCrashDump dump;
C_Network::NetAddress lanServerAddr(std::wstring(L"127.0.0.1"), SERVERPORT);
//std::unique_ptr<C_Network::GameServer> gameServer = nullptr;
// 
//
//int main(int argc, char *argv[])
//{
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//
//    std::thread t1 = std::thread([]() 
//    {
//			C_Network::LanServer lanServer(addr, 1000);
//
//			printf("MyPort : %d\n", lanServer.GetNetAddr().GetPort());
//		if (lanServer.Begin() != C_Network::NetworkErrorCode::NONE)
//		{
//			printf("Begin : 에러 발생\n");
//			return 0;
//		}
//
//		while (1)
//		{
//			if (_kbhit())
//			{
//				char c = _getch();
//				if (c == 'q' || c == 'Q')
//					break;
//				else if (c == 'r' || c == 'R')
//				{
//					C_Network::LogInRequestPacket packet;
//					packet.logInId = 1234;
//					packet.logInPw = 3456;
//
//					C_Network::SharedSendBuffer buffer = C_Network::ChattingClientPacketHandler::MakePacket(packet);
//
//					lanServer.Send(2, buffer);
//				}
//			}
//
//		}
//
//		if (lanServer.End() != C_Network::NetworkErrorCode::NONE)
//		{
//			printf("End : 에러 발생\n");
//			return 0;
//		}
//
//    });
//
//
//	std::thread t2 = std::thread([]()
//	{
//		C_Network::LanClient lanClient(addr);
//
//		lanClient.Connect();
//
//		while (1)
//		{
//			if (_kbhit())
//			{
//				char c = _getch();
//				if (c == 'w' || c == 'W')
//					break;
//
//				else if (c == 'e' || c == 'E')
//				{
//					C_Network::LogInRequestPacket packet;
//					packet.logInId = 1234;
//					packet.logInPw = 3456;
//
//					C_Network::SharedSendBuffer buffer = C_Network::ChattingClientPacketHandler::MakePacket(packet);
//
//					lanClient.Send(buffer);
//				}
//			}
//		}
//
//		lanClient.Disconnect();
//	});
//
//	t1.join();
//	t2.join();
//    return 0;
//}

int main()
{
	//gameServer = std::make_unique<C_Network::GameServer>(C_Network::NetAddress(std::wstring(L"127.0.0.1"), 0), 500,roomNumber,requiredUsers,maxUsers);

	std::shared_ptr<C_Network::GameServer> gameServer = std::make_shared<C_Network::GameServer>(C_Network::NetAddress(std::wstring(L"127.0.0.1"), 0), 0,
		[]() { return std::static_pointer_cast<C_Network::Session>(std::make_shared<C_Network::GameSession>()); });

	//printf("[ GameServer Room Num : %d]", roomNumber);

	gameServer->Begin();

	gameServer->LanClientConnect(lanServerAddr);

	while (1)
	{

	}

	gameServer->LanClientDisconnect();
	gameServer->End();


	return 0;
}