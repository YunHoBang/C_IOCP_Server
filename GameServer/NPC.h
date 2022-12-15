#pragma once
#include "GameObject.h"
#include "Protocol.pb.h"

class NPC : public GameObject
{
public:
	virtual void Update() override;

protected:
	virtual void State_Idle() override;
	virtual void State_Moving()override;
	virtual void State_Skill() override;
	virtual void State_Dead() override;
public:
	Protocol::NPCMetaData _data;

private:
	// 판매할 아이템 
	vector<ItemRef> _items;
};



