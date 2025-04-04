#pragma once

namespace C_Network
{
	class GameSession : public C_Network::Session
	{
	public:
		GameSession();
		~GameSession() { _aliveGameSessionCount.fetch_sub(1); }
		void Init(ULONGLONG userId);
		virtual void OnConnected() override;
		virtual void OnDisconnected() override;
		virtual void OnRecv(C_Utility::CSerializationBuffer& buffer, uint16 type) override;

		void SetPlayer(GamePlayerPtr playerPtr) { _playerPtr = playerPtr; }
		GamePlayerPtr GetPlayer() { return _playerPtr; }
		ULONGLONG GetUserId() const { return _userId; }

		void OnLoadComplete() { _isLoadCompleted = true; }

		static int GetAliveGameSessionCount() { return _aliveGameSessionCount.load(); }

	private:
		static std::atomic<int> _aliveGameSessionCount;
		ULONGLONG _userId;
		GamePlayerPtr _playerPtr;
	
		bool _isLoadCompleted;
	};
}