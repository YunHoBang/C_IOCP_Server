#pragma once
#include "MetaData.h"

class Skill : public MetaData<Protocol::SkillMetaData>
{
public:
	Skill(int id);
public:
	int _upgrade = -1;
};

