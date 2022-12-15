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
	void ProjectileUpdate(); // ������Ʈ���� ������ ������Ÿ���� id�� �°� ������ ������ �Լ�
	const float GetDetectionRange() { return (_meta.size() * _meta.scale()) / 2; }
	virtual GameObjectRef CalColEnemy_Go(float distance, int isColCnt = -1, float damege = 0, float deltaTime = 1) override;
	virtual float GetAtt() override;

public:
	void Mage_1();		//������ �⺻����
	void Mage_2();		//������ ĳ���� �߽����� ���� ����
	void Mage_3();		//������ ����ü �ֺ����� �ֱ�����ü�� ��ȯ�Ǽ� �����ϴ� ��ų
	void Knight_1();	//���� ���� Ŀ���� ����
	void Knight_2();	//���� ĳ���� �߽����� ���� ��ȯ��
	void Gunner_1();	//�ų� �⺻����
	void Gunner_2();	//�ų� ����
	void Gunner_3();	//�ų� ��ä�÷� ������ ����

	void Summons_1();	
	void Summons_2();	

	void Monster_1();
	void Monster_2();
	void Monster_3();
	void Monster_4();
	void Monster_5();
	void Monster_6();

	void Knight_0(); // �ٰŸ� ����
	void Summons_0();
	void Monster_0();

public:
	void SetRadians(const float radians) { _radians = radians;}
	float GetRadians(){ return _radians;}
private:
	weak_ptr<GameObject> _owner;
	weak_ptr<GameObject> _capturedEnemy; // ������ ��
	
	int _projectileId = -1; // ������Ÿ�� id
	Protocol::ProjectileMetaData _meta;

	#pragma region Skill
	float _radians = 1; // ������Ʈ�� ȸ���ؾ��Ҷ� ����ϴ� ��
	float _compareZ = 0; // Ư������
	bool _isChildCheck = false; // �ֱ� ����ü�� ������� �ѹ��� ����ǰԲ�
	#pragma endregion 

};

