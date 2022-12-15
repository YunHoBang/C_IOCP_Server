#pragma once
#include "Struct.pb.h"
#include "Protocol.pb.h"
#include "Enum.pb.h"
#include "ClientPacketHandler.h"
#include "Room.h"
#include "MapManager.h"
#include "Vector2.h"
#include "GameSession.h"

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject()
	{
		_preTime = GetTickCount64();
		_objInfo.set_allocated_location(new Protocol::PBVector());
		_objInfo.set_allocated_rotator(new Protocol::PBVector());
		_objInfo.set_allocated_forwarddir(new Protocol::PBVector());

	};
	virtual ~GameObject();

public:
	virtual void Update();
	void StateUpdate();
	void TimeUpdate();

#pragma region Skill
public:
	void Invoke_Skill(Protocol::C_ATTACK pkt, GameObjectRef enemy);
	void SkillProgress();
protected:
	// 스킬을 가지고있어야할것같은데 ?
	map<int, YHSkillRef> _skills;

#pragma endregion

#pragma region Astar
public:
	bool MovePos();
	bool NewMovePos();

protected:
	std::vector<Vector3>	_routes;			// 길찾기 루트
	bool					_isDone = false;	// 길찾기가 끝났는지 체크하는 변수
	bool					_isObstacleCheck = false; // 장애물 길찾기 판별변수
#pragma endregion

#pragma region Util
public:
	void RotatorObject(Protocol::PBVector d);

	void SetLocation(Vector3 d);
	void SetLocation(float speed, float deltaTime);

	bool CalColEnemy(float distance, int isColCnt = -1, float damege = 0 , float deltaTime = 1);
	bool CalColPlayer(float distance, int isColCnt = -1, float damege = 0, float deltaTime = 1);
	bool CalColSummons(float distance, int isColCnt = -1, float damege = 0, float deltaTime = 1);

	virtual GameObjectRef CalColEnemy_Go(float distance, int isColCnt = -1, float damege = 0, float deltaTime = 1);

	void ChangeState(Protocol::StateType state, bool isSend = true) 
	{
		if (isSend)
		{
			WRITE_LOCK;
			READ_LOCK;
			if (_state != Protocol::DEAD)
			{
				_state = state;
				Protocol::S_STATE sPkt;
				{
					sPkt.set_allocated_objectinfo(new Protocol::ObjectInfo(_objInfo));

					sPkt.set_state(state);
					auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt);
					GRoom->DoAsync(&Room::Broadcast, sendBuffer);
				}
			}
		}
		else
		{
			_state = state;
		}
	};
#pragma endregion

#pragma region Stat
public:
	virtual void SetStat(int hp, int mp, int exp);

public:
	int GetLevel() { return _level; }

	virtual float GetMaxHp();
	float HpPlus(float hp);
	bool HpMin(float damage)
	{
		_hp -= damage;

		Protocol::S_STAT statPkt;
		statPkt.set_id(_objInfo.roomid());
		statPkt.set_hp(_hp);
		statPkt.set_mp(_mp);
		statPkt.set_maxhp(GetMaxHp());
		statPkt.set_maxmp(GetMaxMp());
		statPkt.set_damage(damage);
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(statPkt);
		GRoom->Broadcast(sendBuffer);

		if (_hp <= 0)
		{
			ChangeState(Protocol::DEAD, false);
			return true;
		}

		return false;
	};

	float GetMaxMp();
	float GetMp() {return _mp;}
	float MpPlus(float mp);
	void MpMin(float minMp)
	{
		_mp -= minMp;
		if (_mp < 0)
			_mp = 0;

		Protocol::S_STAT statPkt;
		statPkt.set_id(_objInfo.roomid());
		statPkt.set_hp(_hp);
		statPkt.set_mp(_mp);
		statPkt.set_maxhp(GetMaxHp());
		statPkt.set_maxmp(GetMaxMp());
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(statPkt);
		GRoom->Broadcast(sendBuffer);
	};

	void ExpAdd(float exp);
	float GetMaxExp();
	float GetCurrentExp() { return _exp; }

	virtual float GetAtt();
	virtual float GetDepend();

	int GetGold() { return _gold; }
	void SetGold(int gold) { _gold = gold;}
	void MinGold(int gold) { _gold -= gold; }
	void PlusGold(int gold) { _gold += gold; }

protected:
	float _hp = 0;
	float _mp = 0;

	float _exp = 0;
	int _level = 1;

	int _gold = 0;

#pragma endregion

#pragma region Time

protected:
	float					_time			= 0.0f; // 오브젝트가 생성하고 꾸준히 더해지는 시간
	float					_preTime		= 0.0f; // 현재 시간
	float					_deltaTime		= 0.0f; // 전프레임과 현재프레임과의 차이시간

#pragma endregion

#pragma region State
public:
	bool GetIsDead() { return _isDead; }
	Protocol::StateType& GetState() { return _state; }
private:
	virtual void State_Idle() {};
	virtual void State_Moving() {};
	virtual void State_Skill() {};
	virtual void State_Dead() {};

protected:
	Protocol::StateType		_state			= Protocol::IDLE;
	bool					_isDead			= false;

#pragma endregion

#pragma region Compare
protected:
	Vector3					preV = Vector3(0, 0, 0);
	Vector2					_compareLocation;

#pragma endregion

public:
	void SetMap(const GameMap& map) { _map = map; }
	Protocol::ObjectInfo& GetObjectInfo(){return _objInfo;}
	GameSessionRef GetSession() { return _ownerSession;}
protected:
	USE_LOCK;
	GameSessionRef			_ownerSession;		// Cycle
	GameMap					_map;
	Protocol::ObjectInfo	_objInfo;			// 인게임에서 사용하는 오브젝트의 정보
};

