#pragma once
#include "GameMap.h"
#include "List"

class MapManager
{
public:
	MapManager()
	{
	}

	~MapManager()
	{
	}
public:
	// �� �ؽ�Ʈ������ �д´�
	void Init();
	GameMap GetMapData(int idx);
private:
	// ���� ������ �������ִ´�.
	vector<GameMap> _maps;
};

