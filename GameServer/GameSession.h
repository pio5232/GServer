#pragma once
namespace C_Network
{
	class GameSession : public C_Network::Session
	{
	public:
		GameSession();

		void Init(ULONGLONG userId);
		virtual void OnConnected() override;
		virtual void OnDisconnected() override;
		virtual void OnRecv(C_Utility::CSerializationBuffer& buffer, uint16 type) override;

		void SetPlayer(GamePlayerPtr playerPtr) { _playerPtr = playerPtr; }
		GamePlayerPtr GetPlayer() { return _playerPtr; }
		ULONGLONG GetUserId() const { return _userId; }

		void OnLoadComplete() { _isLoadCompleted = true; }
	private:
		ULONGLONG _userId;
		GamePlayerPtr _playerPtr;
	
		bool _isLoadCompleted;
	};
}