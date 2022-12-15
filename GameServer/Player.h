#pragma once
#include "GameObject.h"

class ItemSlot
{
public:
	bool IsValid() { return _itemdata != nullptr ? true : false; }

	int GetIdx() { return _idx; }
	shared_ptr<GameData> GetData() { return _itemdata; }

	void SetIdx(int idx) { _idx = idx;}
	void SetData(int idx, shared_ptr<GameData> itemdata) {
		_idx = idx;
		_itemdata = itemdata;
	}
	void SetData(shared_ptr<GameData> itemdata) { _itemdata = itemdata; }
private:
	int _idx = -1;
	shared_ptr<GameData> _itemdata = nullptr;
};

class Player : public GameObject
{
public:
	void Set();
	void Set(GameSessionRef session , RoomRef room, int type, int gold, int exp, int level);
public:
	virtual void Update() override;

protected:
	virtual void State_Idle() override;
	virtual void State_Moving()override;
	virtual void State_Skill() override;
	virtual void State_Dead() override;

	virtual float GetAtt() override;
	virtual float GetDepend() override;
public:
	// 인벤토리 관련 함수들
	int AddItem2Inventory(shared_ptr<GameData> itemdata );
	int AddItem2Inventory(int idx,int type,int metaId,int count);

	bool SwapInventoryItem(int idx_1, int idx_2);
	void UseItem(int idx);
	void UnEquip(int idx);
	void EquipItem(int idx, int equipId);

public:
	map<int, SummonsRef> GetSummons() {return _summons;}
	vector<ItemSlotRef> GetItem() {return _items;}
	weak_ptr<class Room> GetRoom() { return _room;}
	shared_ptr<class Character> GetInfo(){return _info;}

	class shared_ptr<GameData>* GetEquip() {return _equip;}
private:
	// 소환수 
	map<int, SummonsRef> _summons;
	// 현재 유저가 접속한 룸
	weak_ptr<class Room> _room;
	// 플레이어 데이터
	shared_ptr<class Character> _info = nullptr; 
	// 인벤토리
	vector<ItemSlotRef> _items;
	// 장비 배열
	class shared_ptr<GameData> _equip[5];

};

