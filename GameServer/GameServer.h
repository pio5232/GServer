#pragma once
#include <atomic>
#include "Utils.h"

namespace C_Content
{
	class GameWorld;
	class PlayerManager;
}
namespace C_Network
{
	struct GameServerInfo
	{
		ULONGLONG enterToken;
		uint16 myRoomNumber;
		uint16 realPort;

		std::atomic<int> requiredUserCnt;
		uint16 maxUsers;

	};

	class GameServer : public ServerBase
	{
	public:
		
		GameServer(const NetAddress& netAddr, uint maxSessionCnt, SessionCreator creator);
		~GameServer();

		void Init(uint16 roomNumber, uint16 requiredUsers, uint16 maxUsers);

		virtual bool OnConnectionRequest(const SOCKADDR_IN& clientInfo);
		virtual void OnError(int errCode, WCHAR* cause);

		void LanClientConnect(const NetAddress& netAddr);
		void LanClientDisconnect();

		uint16 GetRoomNumber() const { return _gameInfo.myRoomNumber; }
		ULONGLONG GetToken() const { return _gameInfo.enterToken; }
		uint16 GetRealPort() const { return _gameInfo.realPort; }
		uint16 GetRequiredUsers() const { return _gameInfo.requiredUserCnt; }
		uint16 MaxUsers() const { return _gameInfo.maxUsers; }

		void MakeAndSendPlayers();
		ErrorCode TryRun();

		void CheckLoadingAndStartLogic();

		GamePlayerPtr CreatePlayer(GameSessionPtr gameSessionPtr);
		ErrorCode DeletePlayer(GamePlayerPtr gamePlayerPtr);

		// 무조건 우측값의 형태로 받도록함..
		void EnqueueAction(Action&& action);

	private:
		std::shared_ptr<class LanClient> _lanClient;

		std::unique_ptr<class C_Content::GameWorld> _gameWorld;
		GameServerInfo _gameInfo;

		std::atomic<bool> _isRunning;

		std::atomic<int> _loadCompletedCnt;
	 	};

}