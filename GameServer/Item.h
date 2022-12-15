#pragma once
#include "GameObject.h"
#include "GameData.h"
#include "MetaData.h"
#include "Consum.h"
#include "Equip.h"

class Item : public GameObject
{
public:
	void Set();
	void ItemUpdate();
	virtual void Update() override;
	virtual void State_Dead() override;
	
public:
	shared_ptr<GameData> GetItemData() {return _itemdata;}
	void SetItemData(shared_ptr<GameData> data) { _itemdata = data;}

private:
	// 공통 데이터
	shared_ptr<GameData> _itemdata = nullptr;
	bool _isGround = false;
};