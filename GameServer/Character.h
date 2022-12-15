#pragma once
#include "MetaData.h"
#include "Skill.h"

class Character : public MetaData<Protocol::CharacterMetaData>
{
public:
	Character(int id);

public:
	// 플레이어 이름
	string _name;
	// 레벨,경험치,


	//int _level = -1;
	//vector<shared_ptr<Skill>> _skills;
};

