#include "pch.h"
#include "WorldChat.h"
#include "GameSession.h"

ErrorCode C_Content::WorldChat::Chat(C_Network::SharedSendBuffer sharedSendBuffer)
{
	for (auto& [id, weakGameSessionPtr] : _userIdToSessionMap)
	{
		if (weakGameSessionPtr.expired())
			continue;

		GameSessionPtr gameSessionPtr = weakGameSessionPtr.lock();

		gameSessionPtr->Send(sharedSendBuffer);
	}

	return ErrorCode::NONE;
}

void C_Content::WorldChat::RegisterMember(GameSessionPtr gameSessionPtr)
{
	ULONGLONG userId = gameSessionPtr->GetUserId();

	_userIdToSessionMap.insert(std::make_pair(userId, gameSessionPtr));
}

void C_Content::WorldChat::RemoveMember(ULONGLONG userId)
{
	if(_userIdToSessionMap.contains(userId))
	_userIdToSessionMap.erase(userId);
}
