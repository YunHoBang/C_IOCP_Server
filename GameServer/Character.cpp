#include "pch.h"
#include "Character.h"
#include "DataManager.h"

Character::Character(int id) : MetaData(Protocol::DataType::DATA_TYPE_CHARACTER,DataManager::CharacterMap.find(id)->second)
{

}
//
//void Character::Set()
//{
//	for (auto skillID : _data.skills())
//	{
//		Protocol::SkillMetaData skill = DataManager::SkillMap.find(skillID)->second;
//		
//		shared_ptr<Skill> addSkill = MakeShared<Skill>();
//		{
//			addSkill->_id = skill.id();
//			addSkill->_data = skill;
//			addSkill->_upgrade = 1; // default // 나중에 데이터베이스로 수정
//
//		}
//		_skills.push_back(addSkill);
//	}
//}
