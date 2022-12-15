#pragma once
#include "GameObject.h"

// 소환수의 플레이어 기준 위치 enum

enum SummonsLocation : int
{
	SUMMONS_LOCATION_NONE = 0,
	SUMMONS_LOCATION_1 = 1,
	SUMMONS_LOCATION_2 = 2,
	SUMMONS_LOCATION_3 = 3,
};
class Summons : public GameObject
{
public:
	void Set(GameObjectRef owner, int metaId, int roomid, SummonsLocation l,const Protocol::ObjectInfo& info);

public:
	virtual void Update() override;

protected:
	virtual void State_Idle() override;
	virtual void State_Moving()override;
	virtual void State_Skill() override;
	virtual void State_Dead() override;

	void GetOneLine(SummonsLocation c);

	virtual float GetAtt() override;
	virtual float GetDepend() override;

	virtual float GetMaxHp() override;

public:
	void SetOwnerGo(GameObjectRef owner) {_owner = owner;}
	weak_ptr<GameObject> GetOwnerGo() {return _owner;}


private:
	weak_ptr<GameObject>	_owner;				// 나를 소환한 오브젝트
	weak_ptr<GameObject>	_capturedEnemy;		// 포착된 적
	int						_summonsId = -1;	// 소환수 id // 나중에 메타데이터로 빼자
	Protocol::SummonsMetaData _meta;
private:
	SummonsLocation			_sLocation = SUMMONS_LOCATION_NONE;
	Vector3					_compareOwner;

private:
	const float				_monitoringRange = 500;
	float					_attRange = 150;

};

