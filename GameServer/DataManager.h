#pragma once
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdio>
#include "JSONIncludes.h"
#include "JSONBase.h"
#include "Protocol.pb.h"
#include "DataContents.h"
#include <map>

using namespace std;

class DataManager
{
public:
	static map<int, Protocol::CharacterMetaData> CharacterMap;
	static map<int, Protocol::SkillMetaData> SkillMap;
	static map<int, Protocol::ConsumMetaData> ConsumMap;
	static map<int, Protocol::EquipMetaData> EquipMap;
	static map<int, Protocol::NPCMetaData> NPCMap;
	static map<int, Protocol::ProjectileMetaData> ProjectileMap;
	static map<int, Protocol::SummonsMetaData> SummonsMap;
	static map<int, Protocol::EnemyMetaData> EnemyMap;

public:
	static void LoadData()
	{
		// 1.loader를 상속받은 클래스
		// 2.키값 
		// 3.프로토콜버프에 추가한 구조체 
		string pathStr = "..\\..\\YHUnreal\\Content\\Data\\";

		CharacterMap = LoadJson<CharacterDataLoader, int, Protocol::CharacterMetaData>(pathStr + "CharacterData.json").MakeMap();
		SkillMap = LoadJson<SkillDataLoader, int, Protocol::SkillMetaData>(pathStr + "SkillData.json").MakeMap();
		ConsumMap = LoadJson<ConsumDataLoader, int, Protocol::ConsumMetaData>(pathStr + "ConsumData.json").MakeMap();
		EquipMap = LoadJson<EquipDataLoader, int, Protocol::EquipMetaData>(pathStr + "EquipData.json").MakeMap();
		NPCMap = LoadJson<NPCDataLoader, int, Protocol::NPCMetaData>(pathStr + "NPCData.json").MakeMap();
		ProjectileMap = LoadJson<ProjectileDataLoader, int, Protocol::ProjectileMetaData>(pathStr + "ProjectileData.json").MakeMap();
		SummonsMap = LoadJson<SummonsDataLoader, int, Protocol::SummonsMetaData>(pathStr + "Summons.json").MakeMap();
		EnemyMap = LoadJson<EnemyDataLoader, int, Protocol::EnemyMetaData>(pathStr + "EnemyData.json").MakeMap();

	}

	template<typename Loader, typename Key, typename Value>
	static Loader LoadJson(const string& filePath)
	{
		Loader json;
		json.DeserializeFromFile(filePath);
		return json;
	}

};
