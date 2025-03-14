#pragma once
namespace C_Network
{
	class LanClientSession : public C_Network::Session
	{
	public:
		LanClientSession(std::weak_ptr<ServerBase> gameServer);

		virtual void OnConnected() override;
		virtual void OnDisconnected() override;
		virtual void OnRecv(C_Utility::CSerializationBuffer& buffer, uint16 type) override;

		std::shared_ptr<class GameServer> GetGameServer();
	private:
		std::weak_ptr<class GameServer> _gameServer;
	};
}