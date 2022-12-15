#pragma once
#include "GameObject.h"

class Projectile : public GameObject
{
public:
	void Set(GameObjectRef owner, int metaId, int roomid, const Protocol::ObjectInfo& info);
	void Set(GameObjectRef owner, int metaId, int roomid, const Protocol::ObjectInfo& info,GameObjectRef enemy);
	void Set(GameObjectRef owner, int metaId, int roomid, const Protocol::PBVector& l , const Protocol::PBVector& r , const Protocol::PBVector& f);

public:
	virtual void Update() override;
	void ProjectileUpdate(); // 업데이트마다 각각의 프로젝타일의 id에 맞게 동작을 구현한 함수
	const float GetDetectionRange() { return (_meta.size() * _meta.scale()) / 2; }
	virtual GameObjectRef CalColEnemy_Go(float distance, int isColCnt = -1, float damege = 0, float deltaTime = 1) override;
	virtual float GetAtt() override;

public:
	void Mage_1();		//마법사 기본공격
	void Mage_2();		//마법사 캐릭터 중심으로 도는 공격
	void Mage_3();		//마법사 투사체 주변으로 애기투사체가 소환되서 공격하는 스킬
	void Knight_1();	//전사 점점 커지는 공격
	void Knight_2();	//전사 캐릭터 중심으로 도는 소환물
	void Gunner_1();	//거너 기본공격
	void Gunner_2();	//거너 유도
	void Gunner_3();	//거너 부채꼴로 퍼지는 공격

	void Summons_1();	
	void Summons_2();	

	void Monster_1();
	void Monster_2();
	void Monster_3();
	void Monster_4();
	void Monster_5();
	void Monster_6();

	void Knight_0(); // 근거리 공격
	void Summons_0();
	void Monster_0();

public:
	void SetRadians(const float radians) { _radians = radians;}
	float GetRadians(){ return _radians;}
private:
	weak_ptr<GameObject> _owner;
	weak_ptr<GameObject> _capturedEnemy; // 포착된 적
	
	int _projectileId = -1; // 프로젝타일 id
	Protocol::ProjectileMetaData _meta;

	#pragma region Skill
	float _radians = 1; // 프로젝트가 회전해야할때 사용하는 값
	float _compareZ = 0; // 특정높이
	bool _isChildCheck = false; // 애기 투사체가 있을경우 한번만 실행되게끔
	#pragma endregion 

};

