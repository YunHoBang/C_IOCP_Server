#pragma once
#include "pch.h"
#include <fstream>
#include <string>
#include "MapManager.h"
#include "GameMap.h"

void MapManager::Init()
{
	// 경로에 있는 텍스트파일을 읽는다.
	wchar_t path[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, path);

	wchar_t str[MAX_PATH] = L"\\..\\..\\YHUnreal\\MapData\\MapData.txt";
	wcscat_s(path, str);

	std::ifstream ifs;
	ifs.open(path);
	if (ifs.fail())
	{
		cout << "Text File Read Failed" << endl;
		return;
	}
	// 라인을 읽고 그값을 저장한다.
	char line[3000];

	// 시작위치
	ifs.getline(line, sizeof(line));
	string strX(line);
	int x = std::stoi(strX);

	ifs.getline(line, sizeof(line));
	string strY(line);
	int y = std::stoi(strY);

	ifs.getline(line, sizeof(line));
	string strZ(line);
	int z = std::stoi(strZ);

	// 간격
	ifs.getline(line, sizeof(line));
	string strInterval(line);
	int interval = std::stoi(strInterval);

	// 정보
	vector<vector<bool>> vecMapData;
	while (ifs.getline(line, sizeof(line)))
	{
		// 벡터를 생성하고
		vector<bool> vec;

		// 그정보를 벡터에 넣는다.
		for (int cnt = 0;; ++cnt)
		{
			// 공백문자를 만나면 그만
			if (line[cnt] == '\0')
			{
				break;
			}
			else if (line[cnt] == '0')
			{
				vec.push_back(false);
			}
			else if (line[cnt] == '1')
			{
				vec.push_back(true);
			}
		}

		// 추가한다.
		vecMapData.push_back(vec);
	}

	// 출력
	/*for (vector<bool> data : vecMapData)
	{
		for (auto b : data)
		{
			cout << b;
		}
		cout << endl;
	}*/

	// 컴포넌트 갯수 저장
	int compoCnt = vecMapData[0].size();
	//cout << compoCnt;

	// Set Game Map
	GameMap mapData(1, x, y, z, interval, compoCnt, vecMapData);

	_maps.push_back(mapData);

	ifs.close();
}

GameMap MapManager::GetMapData(int idx)
{
	return _maps[idx];
}
