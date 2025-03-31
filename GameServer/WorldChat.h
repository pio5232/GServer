#pragma once

namespace C_Content
{
	class WorldChat : public C_Utility::JobQueue
	{
	public:
		ErrorCode Chat(C_Network::SharedSendBuffer sharedSendBuffer);

		void RegisterMember(GameSessionPtr gameSessionPtr);
		void RemoveMember(ULONGLONG userId);

	private:
		
		// userId, session
		std::unordered_map<ULONGLONG, std::weak_ptr<class C_Network::GameSession>> _userIdToSessionMap; 

	};
}

