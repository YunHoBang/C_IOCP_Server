#pragma once
#include "MetaData.h"

class Equip : public MetaData<Protocol::EquipMetaData>
{
public:
	Equip(int id);
public:
	int _upgrade = -1;

	// 기타 스탯 , 업그레이드를 한후에 공격력, 방어력 등등

};

