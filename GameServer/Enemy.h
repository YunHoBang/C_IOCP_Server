#pragma once
#include "GameObject.h"

class Enemy : public GameObject
{
public:
	void Set(int roomId,int metaId, const Protocol::PBVector& location);
	virtual void Update() override;

protected:
	virtual void State_Idle() override;
	virtual void State_Moving() override;
	virtual void State_Skill() override;
	virtual void State_Dead() override;

	virtual float GetMaxHp() override;

public:

	int _metaId = -1;
	Protocol::EnemyMetaData _meta;

private:
	weak_ptr<GameObject>	_capturedEnemy;		// Æ÷ÂøµÈ Àû

	float _idleTime = 0.0f;
	float _moveTime = 0.0f;

	Vector3 _patrolPos;

	bool _isEnemyValid = false;
};

