#pragma once
#include "Session.h"
#include "Character.h"
#include "MetaData.h"

class GameSession : public PacketSession
{
public:
	~GameSession() { cout << "~GameSession" << endl; }

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, s_int32 len) override;
	virtual void OnSend(s_int32 len) override;

public:
	PlayerRef GetPlayer() { return _currentPlayer; }
	void SetPlayer(PlayerRef player) { _currentPlayer = player; }
	shared_ptr<class Character> GetPlayerInfo();

	const int GetId() { return _id; }
	void SetId(int id) { _id = id; }

	void SetAccountId(string accountId) { _accountId = accountId; }
	string GetAccountId() { return _accountId; }
private:
	int _id = -1; // 데이터베이스에 적용된 유저 id
	string _accountId; // 계정 아이디
	PlayerRef _currentPlayer; // 인게임에서 보여지는 플레이어의 정보

};