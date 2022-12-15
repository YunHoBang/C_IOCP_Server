#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "ClientPacketHandler.h"
#include "GameObject.h"
#include "Player.h"
#include "Room.h"

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));

	cout << "server Connect" << endl;

	Protocol::S_SERVERCONNECT connectPkt;
	connectPkt.set_success(true);
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(connectPkt);
	this->Send(sendBuffer);
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));

	if (_currentPlayer)
	{
		if (auto room = _currentPlayer->GetRoom().lock())
			room->DoAsync(&Room::Leave, static_pointer_cast<GameObject>(_currentPlayer));
	}

	_currentPlayer = nullptr;
}

void GameSession::OnRecvPacket(BYTE* buffer, s_int32 len)
{
	PacketSessionRef session = GetPacketSessionRef();
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId �뿪 üũ
	ClientPacketHandler::HandlePacket(session, buffer, len);
}

void GameSession::OnSend(s_int32 len)
{
}

std::shared_ptr<class Character> GameSession::GetPlayerInfo()
{
	return _currentPlayer->GetInfo();
}
