#pragma once

namespace C_Network
{
	class LanClient : public C_Network::ClientBase
	{
	public:
		LanClient(const NetAddress& targetNetAddr, SessionCreator creator);
		~LanClient();

	};
}
