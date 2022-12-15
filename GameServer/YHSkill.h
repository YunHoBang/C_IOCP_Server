#pragma once
#include "GameObject.h"

class YHSkill
{
public:
	YHSkill(GameObjectRef o, int id);

	void Execute(Protocol::C_ATTACK pkt);
	void Progress();
	void End();

	bool IsCoolTimeCheck() { return _progressTime > _endTime; } // true = 쿨타임이 지남 false = 쿨타임이 아직 지나지않음
	void StartCoolTime(float coolTime)
	{
		_progressTime = ::GetTickCount64(); // 시작 시간
		_endTime = ::GetTickCount64() + (coolTime * 1000); // 쿨타임 종료 시간 
	}

	void ResetCoolTime() 
	{ 
		_progressTime = 0.0f;
		_endTime = -1; 
	}
	
public:
	void SetEnemy(GameObjectRef enemy) { _enemy = enemy; }
	int GetSkillId() { return _skillId; }

#pragma region Skill
public:
	void KnightSkill_1(Protocol::C_ATTACK pkt);
	void KnightSkill_2(Protocol::C_ATTACK pkt, GameObjectRef owner);
	void KnightSkill_3(Protocol::C_ATTACK pkt, GameObjectRef owner);
	void KnightSkill_4(Protocol::C_ATTACK pkt, GameObjectRef owner);

	void MageSkill_1(Protocol::C_ATTACK pkt); 
	void MageSkill_2(Protocol::C_ATTACK pkt, GameObjectRef owner);
	void MageSkill_3(Protocol::C_ATTACK pkt, GameObjectRef owner); 
	void MageSkill_4(Protocol::C_ATTACK pkt, GameObjectRef owner); 

	void GunnerSkill_1(Protocol::C_ATTACK pkt);
	void GunnerSkill_2(Protocol::C_ATTACK pkt, GameObjectRef owner);
	void GunnerSkill_3(Protocol::C_ATTACK pkt);
	void GunnerSkill_4(Protocol::C_ATTACK pkt, GameObjectRef owner);

	void SummonsSkill_1(Protocol::C_ATTACK pkt, GameObjectRef owner);
	void SummonsSkill_2(Protocol::C_ATTACK pkt, GameObjectRef owner);
	void SummonsSkill_3(Protocol::C_ATTACK pkt, GameObjectRef owner);

	void EnemySkill_1(Protocol::C_ATTACK pkt, GameObjectRef owner);
	void EnemySkill_2(Protocol::C_ATTACK pkt, GameObjectRef owner);
	void EnemySkill_3(Protocol::C_ATTACK pkt, GameObjectRef owner);
	void EnemySkill_4(Protocol::C_ATTACK pkt, GameObjectRef owner);
	void EnemySkill_5(Protocol::C_ATTACK pkt, GameObjectRef owner);
	void EnemySkill_6(Protocol::C_ATTACK pkt, GameObjectRef owner);
	void EnemySkill_7(Protocol::C_ATTACK pkt, GameObjectRef owner);

#pragma endregion

private:
	int _skillId = -1;
	Protocol::SkillMetaData _meta;

	float _progressTime			= 0.0f;
	float _endTime				= -1;

	weak_ptr<GameObject>		_owner;
	weak_ptr<GameObject>		_enemy;

};
