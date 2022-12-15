#include "pch.h"
#include "Skill.h"

Skill::Skill(int id) : MetaData(Protocol::DataType::DATA_TYPE_SKILL, DataManager::SkillMap.find(id)->second)
{

}
