#pragma once
#include "pch.h"
#include <fstream>
#include <string>
#include "MapManager.h"
#include "GameMap.h"

void MapManager::Init()
{
	// ��ο� �ִ� �ؽ�Ʈ������ �д´�.
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
	// ������ �а� �װ��� �����Ѵ�.
	char line[3000];

	// ������ġ
	ifs.getline(line, sizeof(line));
	string strX(line);
	int x = std::stoi(strX);

	ifs.getline(line, sizeof(line));
	string strY(line);
	int y = std::stoi(strY);

	ifs.getline(line, sizeof(line));
	string strZ(line);
	int z = std::stoi(strZ);

	// ����
	ifs.getline(line, sizeof(line));
	string strInterval(line);
	int interval = std::stoi(strInterval);

	// ����
	vector<vector<bool>> vecMapData;
	while (ifs.getline(line, sizeof(line)))
	{
		// ���͸� �����ϰ�
		vector<bool> vec;

		// �������� ���Ϳ� �ִ´�.
		for (int cnt = 0;; ++cnt)
		{
			// ���鹮�ڸ� ������ �׸�
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

		// �߰��Ѵ�.
		vecMapData.push_back(vec);
	}

	// ���
	/*for (vector<bool> data : vecMapData)
	{
		for (auto b : data)
		{
			cout << b;
		}
		cout << endl;
	}*/

	// ������Ʈ ���� ����
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
