#pragma once
#include "JobQueue.h"
#include "GameMap.h"

class Room : public JobQueue
{
public:
	// 싱글쓰레드 환경인마냥 코딩 // 잡시스템을 사용하기때문에 가능 
	void Init();
	void Enter(GameObjectRef go);
	void Leave(GameObjectRef go);
	void Broadcast(SendBufferRef sendBuffer);
	void ObjectSkillInvoke(GameObjectRef go, GameObjectRef target, Protocol::C_ATTACK pkt);
	void CreateRewardObject(GameObjectRef go);
	void GetItem(PlayerRef player, Protocol::C_GETITEM pkt);
	void ClickNPC(PlayerRef player, Protocol::C_CLICKNPC pkt);
	void PurchaseNPC(PlayerRef player, Protocol::C_PURCHASE pkt);
	void SaleNPC(PlayerRef player, Protocol::C_SALE pkt);
	void SwapItem(PlayerRef player, Protocol::C_SWAP_ITEM pkt);
	void UseItem(PlayerRef player, Protocol::C_USE_ITEM pkt);
	void UnEquip(PlayerRef player, Protocol::C_UNEQUIP pkt);

	void Update();

	map<s_uint64, EnemyRef> GetMonsters() { return _monsters; }
	map<s_uint64, PlayerRef> GetPlayers() { return _players; }
	map<s_uint64, SummonsRef> GetSummons() { return _summons; }

	const int ProvideRoomId() { return ++idGenerator; }
	GameMap GetMap() { return _map; }

private:
	int _roomId;
	GameMap _map;

	Atomic<s_uint64>					idGenerator = 1;
	map<s_uint64, PlayerRef>			_players;
	map<s_uint64, EnemyRef>				_monsters;
	map<s_uint64, ProjectileRef>		_projectils;
	map<s_uint64, ItemRef>				_items;
	map<s_uint64, SummonsRef>			_summons;
	map<s_uint64, GameObjectRef>		_objects;

};

extern shared_ptr<Room> GRoom;