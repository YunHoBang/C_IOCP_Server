#pragma once
#include "MetaData.h"

class Equip : public MetaData<Protocol::EquipMetaData>
{
public:
	Equip(int id);
public:
	int _upgrade = -1;

	// ��Ÿ ���� , ���׷��̵带 ���Ŀ� ���ݷ�, ���� ���

};

