#include "pch.h"
#include "DataContents.h"

bool CharacterDataLoader::Deserialize(const rapidjson::Value& obj)
{
	Protocol::CharacterMetaData d;

	d.set_id(obj["id"].GetInt());
	d.set_type((Protocol::PlayerType)obj["type"].GetInt());

	const rapidjson::Value& data = obj["skills"];
	if (!data.IsArray())
		return false;

	for (rapidjson::Value::ConstValueIterator itr = data.Begin(); itr != data.End(); ++itr)
	{
		d.add_skills(itr->GetInt());
	}

	if (obj.HasMember("stat"))
	{
		Protocol::StatMetaData* s = new Protocol::StatMetaData();
		const rapidjson::Value& data = obj["stat"];
		s->set_hp(data["hp"].GetInt());
		s->set_mp(data["mp"].GetInt());
		s->set_exp(data["exp"].GetInt());
		d.set_allocated_stat(s);
	}
	_datas.push_back(d);
	return true;
}

bool SkillDataLoader::Deserialize(const rapidjson::Value& obj)
{
	Protocol::SkillMetaData d;

	d.set_id(obj["id"].GetInt());
	//d.set_name(obj["name"].GetString());
	d.set_cooldown(obj["cooldown"].GetFloat());
	d.set_damage(obj["damage"].GetInt());
	d.set_mpconsumption(obj["mpconsumption"].GetInt());

	_datas.push_back(d);
	return true;
}

bool ConsumDataLoader::Deserialize(const rapidjson::Value& obj)
{
	Protocol::ConsumMetaData d;
	{
		d.set_id(obj["id"].GetInt());
		d.set_name(obj["name"].GetString());
		d.set_hp(obj["hp"].GetInt());
		d.set_mp(obj["mp"].GetInt());
		
		d.set_percentage(obj["percentage"].GetInt());
		d.set_price(obj["price"].GetInt());
		d.set_resource(obj["resource"].GetString());
	}
	_datas.push_back(d);
	return true;
}

bool EquipDataLoader::Deserialize(const rapidjson::Value& obj)
{
	Protocol::EquipMetaData d;
	{
		d.set_id(obj["id"].GetInt());
		d.set_name(obj["name"].GetString());
		d.set_attack(obj["attack"].GetInt());
		d.set_defense(obj["defense"].GetInt());
		d.set_equiptype(Protocol::EquipType(obj["equiptype"].GetInt()));

		d.set_percentage(obj["percentage"].GetInt());
		d.set_price(obj["price"].GetInt());
		d.set_resource(obj["resource"].GetString());
	}
	_datas.push_back(d);
	return true;
}

bool NPCDataLoader::Deserialize(const rapidjson::Value& obj)
{
	Protocol::NPCMetaData d;
	{
		d.set_id(obj["id"].GetInt());
		d.set_name(obj["name"].GetString());
	}
	const rapidjson::Value& data = obj["ids"];
	if (!data.IsArray())
		return false;

	for (rapidjson::Value::ConstValueIterator itr = data.Begin(); itr != data.End(); ++itr)
	{
		d.add_ids(itr->GetInt());
	}

	_datas.push_back(d);
	return true;
}

bool ProjectileDataLoader::Deserialize(const rapidjson::Value& obj)
{
	Protocol::ProjectileMetaData d;
	{
		d.set_id(obj["id"].GetInt());
		d.set_damage(obj["damage"].GetInt());
		d.set_speed(obj["speed"].GetInt());
		d.set_size(obj["size"].GetInt());
		d.set_duration(obj["duration"].GetInt());
		d.set_limitdistance(obj["limitDistance"].GetInt());
		d.set_childprojectileid(obj["childProjectileId"].GetInt());

		d.set_resource(obj["resource"].GetString());
		if (obj.HasMember("resourceMaterial"))
			d.set_resourcematerial(obj["resourceMaterial"].GetString());
		d.set_scale(obj["scale"].GetFloat());

		if (obj.HasMember("effect"))
			d.set_effect(obj["effect"].GetString());

		if (obj.HasMember("effectLocation"))
		{
			Protocol::PBVector* v = new Protocol::PBVector();
			d.set_allocated_effectlocation(v);

			const rapidjson::Value& data = obj["effectLocation"];
			v->set_x(data["x"].GetFloat());
			v->set_y(data["y"].GetFloat());
			v->set_z(data["z"].GetFloat());
		}
		if (obj.HasMember("effectscale"))
			d.set_effectscale(obj["effectscale"].GetFloat());

		if (obj.HasMember("effectMesh"))
			d.set_effectmesh(obj["effectMesh"].GetString());

		if (obj.HasMember("effectMaterial"))
			d.set_effectmaterial(obj["effectMaterial"].GetString());

		if (obj.HasMember("effectMeshLocation"))
		{
			Protocol::PBVector* v = new Protocol::PBVector();
			d.set_allocated_effectmeshlocation(v);

			const rapidjson::Value& data = obj["effectMeshLocation"];
			v->set_x(data["x"].GetFloat());
			v->set_y(data["y"].GetFloat());
			v->set_z(data["z"].GetFloat());
		}
		if (obj.HasMember("effectMeshscale"))
			d.set_effectmeshscale(obj["effectMeshscale"].GetFloat());
		if (obj.HasMember("explosionEffect"))
			d.set_explosioneffect(obj["explosionEffect"].GetString());
	}
	_datas.push_back(d);
	return true;
}

bool SummonsDataLoader::Deserialize(const rapidjson::Value& obj)
{
	Protocol::SummonsMetaData d;
	{
		d.set_id(obj["id"].GetInt());
		d.set_skillid(obj["skillId"].GetInt());
		d.set_duration(obj["duration"].GetInt());
		d.set_resource(obj["resource"].GetString());
		d.set_anim(obj["anim"].GetString());
		if (obj.HasMember("stat"))
		{
			Protocol::StatMetaData* s = new Protocol::StatMetaData();
			const rapidjson::Value& data = obj["stat"];
			s->set_hp(data["hp"].GetInt());
			s->set_mp(data["mp"].GetInt());
			s->set_exp(data["exp"].GetInt());
			d.set_allocated_stat(s);
		}
		d.set_detectrange(obj["detectrange"].GetInt());
		d.set_attrange(obj["attrange"].GetInt());
	}
	_datas.push_back(d);
	return true;
}

bool EnemyDataLoader::Deserialize(const rapidjson::Value& obj)
{
	Protocol::EnemyMetaData d;
	{
		d.set_id(obj["id"].GetInt());
		d.set_skillid(obj["skillId"].GetInt());
		d.set_anim(obj["anim"].GetString());
		d.set_resource(obj["resource"].GetString());
		if (obj.HasMember("stat"))
		{
			Protocol::StatMetaData* s = new Protocol::StatMetaData();
			const rapidjson::Value& data = obj["stat"];
			s->set_hp(data["hp"].GetInt());
			s->set_mp(data["mp"].GetInt());
			s->set_exp(data["exp"].GetInt());
			d.set_allocated_stat(s);
		}
		d.set_detectrange(obj["detectrange"].GetInt());
		d.set_attrange(obj["attrange"].GetInt());
	}
	_datas.push_back(d);
	return true;
}
