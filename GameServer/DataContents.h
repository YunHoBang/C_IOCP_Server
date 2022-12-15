#pragma once
#include "JSONIncludes.h"
#include "JSONBase.h"
#include "Protocol.pb.h"

template<typename Key, typename Value>
struct ILoader : JSONBase
{
public:
	list<Value> _datas;
public:
	virtual map<Key, Value> MakeMap()
	{
		map<Key, Value> returnMap;
		for (auto temp : _datas)
		{
			returnMap.insert(pair<Key, Value>(temp.id(), temp));
		}
		return returnMap;
	}
};

class CharacterDataLoader : public ILoader<int, Protocol::CharacterMetaData>
{
public:
	virtual bool Deserialize(const rapidjson::Value& obj);
};

class SkillDataLoader : public ILoader<int, Protocol::SkillMetaData>
{
public:
	virtual bool Deserialize(const rapidjson::Value& obj);
};

class ConsumDataLoader : public ILoader<int, Protocol::ConsumMetaData>
{
public:
	virtual bool Deserialize(const rapidjson::Value& obj);
};

class EquipDataLoader : public ILoader<int, Protocol::EquipMetaData>
{
public:
	virtual bool Deserialize(const rapidjson::Value& obj);

};

class NPCDataLoader : public ILoader<int, Protocol::NPCMetaData>
{
public:
	virtual bool Deserialize(const rapidjson::Value& obj);

};

class ProjectileDataLoader : public ILoader<int, Protocol::ProjectileMetaData>
{
public:
	virtual bool Deserialize(const rapidjson::Value& obj);

};

class SummonsDataLoader : public ILoader<int, Protocol::SummonsMetaData>
{
public:
	virtual bool Deserialize(const rapidjson::Value& obj);

};


class EnemyDataLoader : public ILoader<int, Protocol::EnemyMetaData>
{
public:
	virtual bool Deserialize(const rapidjson::Value& obj);

};