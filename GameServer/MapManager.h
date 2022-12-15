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
	// 맵 텍스트파일을 읽는다
	void Init();
	GameMap GetMapData(int idx);
private:
	// 읽은 정보를 가지고있는다.
	vector<GameMap> _maps;
};

