#pragma once
#include "MetaData.h"
#include "Skill.h"

class Character : public MetaData<Protocol::CharacterMetaData>
{
public:
	Character(int id);

public:
	// �÷��̾� �̸�
	string _name;
	// ����,����ġ,


	//int _level = -1;
	//vector<shared_ptr<Skill>> _skills;
};

