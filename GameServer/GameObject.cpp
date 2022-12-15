#include "pch.h"
#include "GameObject.h"
#include "YHSkill.h"
#include "Enemy.h"
#include "Player.h"
#include "Summons.h"

#include "DBConnectionPool.h"
#include "DBBind.h"
#include "XmlParser.h"
#include "DBSynchronizer.h"
#include "GenProcedures.h"

GameObject::~GameObject()
{

}

void GameObject::Update()
{
	TimeUpdate();

	StateUpdate();

	SkillProgress();

}

void GameObject::StateUpdate()
{
	switch (_state)
	{
	case Protocol::IDLE:
		State_Idle();
		break;
	case Protocol::MOVING:
		State_Moving();
		break;
	case Protocol::SKILL:
		State_Skill();
		break;
	case Protocol::DEAD: 
		State_Dead();
		break;
	default:
		break;
	}
}

void GameObject::TimeUpdate()
{
	_deltaTime = (GetTickCount64() - _preTime) / (float)1000;// 현재 시간에서 전시간을 빼주자
	_preTime = GetTickCount64(); // 시간을 갱신해주자
	_time += _deltaTime;
}

void GameObject::Invoke_Skill(Protocol::C_ATTACK pkt, GameObjectRef enemy)
{
	// id로 찾아본다.
	auto e = _skills.find(pkt.attackid());

	if (e != _skills.end())
	{
		YHSkillRef skill = e->second;
		if (enemy)
			skill->SetEnemy(enemy);
			//skill->_enemy = enemy;

		skill->Execute(pkt);
	}
}

void GameObject::SkillProgress()
{
	// 스킬을 돌면서 진행중이라면 스킬의 진행함수를 호출해주는 함수
	for (auto iter = _skills.begin(); iter != _skills.end(); iter++)
	{
		YHSkillRef s = iter->second;
		if (!s->IsCoolTimeCheck()) // 쿨타임 진행 상황을 체크하고 
			s->Progress();
		else // 쿨타임이 다 지났다면 리셋해준다.
			s->End();
	}
}

bool GameObject::MovePos()
{
	if (_routes.empty())
	{
		_isDone = false;
		return true;
	}

	// 경로중 하나를 꺼낸다.
	Vector3 r = _routes.back();
	if (YHUtils::DistanceCheck(_objInfo.location(), r, 50))
	{
		// 경로의 범위안에 들어왔다면 뺴준다.
		_routes.pop_back();
		return false;
	}

	// 로그
	//cout << "Player :" << "x : " << iter.x << " y : " << iter.y << endl;
	//cout << "Monster :" << "x : " << _objInfo.location().x() << " y : " << _objInfo.location().y() << endl;

	float degree;
	degree = YHUtils::CalDegree(r, _objInfo.location());

	Protocol::PBVector* rotator = new Protocol::PBVector();
	{
		rotator->set_x(0);
		rotator->set_y(0);
		rotator->set_z(degree);
	}
	_objInfo.set_allocated_rotator(rotator);

	float x = r.x - _objInfo.location().x();
	float y = r.y - _objInfo.location().y();
	float distance = sqrt(x * x + y * y);

	// 정규화된 방향벡터
	x = x / distance;
	y = y / distance;

	// 이동하기전에 체크 여기서 그 위치로 이동했는지 체크
	{
		Protocol::PBVector* location = new Protocol::PBVector();
		{
			int speed = 500;

			location->set_x(_objInfo.location().x() + (x * speed * _deltaTime));
			location->set_y(_objInfo.location().y() + (y * speed * _deltaTime));
			location->set_z(94);
		}
		_objInfo.set_allocated_location(location);

		Protocol::S_MOVE movePkt;
		{
			movePkt.set_allocated_objectinfo(new Protocol::ObjectInfo(_objInfo));
			movePkt.set_state(Protocol::MOVING);
			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(movePkt);
			GRoom->DoAsync(&Room::Broadcast, sendBuffer);
		}
	}
	return false;
}

bool GameObject::NewMovePos()
{

	if (_routes.empty())
	{
		_isDone = false;
		_isObstacleCheck = false;
		return true;
	}

	// 경로중 하나를 꺼낸다.
	Vector3 r = _routes.back();
	if (r != preV) // 만약 꺼낸경로가 전업데이트때 꺼낸 경로와 일치한다면 패킷을 보내지말자
	{
		// 경로를 갱신해주고 
		preV = r;
		// 패킷을 보낸다.
		Protocol::S_MOVE movePkt;
		{
			ChangeState(Protocol::MOVING, false);
			movePkt.set_state(_state);
			movePkt.set_allocated_objectinfo(new Protocol::ObjectInfo(_objInfo));
			movePkt.set_allocated_movelocation(new Protocol::PBVector(r.ConvertPBVector())); // 이동할 위치
			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(movePkt);
			GRoom->DoAsync(&Room::Broadcast, sendBuffer);
		}
	}

	if (YHUtils::DistanceCheck(_objInfo.location(), r, 50))
	{
		// 경로의 범위안에 들어왔다면 뺴준다.
		_routes.pop_back();
		return false;
	}

	{
		float degree;
		degree = YHUtils::CalDegree(r, _objInfo.location());
		Protocol::PBVector* rotator = new Protocol::PBVector();
		{
			rotator->set_x(0);
			rotator->set_y(0);
			rotator->set_z(degree);
		}
		_objInfo.set_allocated_rotator(rotator);

		float x = r.x - _objInfo.location().x();
		float y = r.y - _objInfo.location().y();
		float distance = sqrt(x * x + y * y);

		// 정규화된 방향벡터
		x = x / distance;
		y = y / distance;

		int speed = 200;
		Protocol::PBVector* location = new Protocol::PBVector();
		location->set_x(_objInfo.location().x() + (x * speed * _deltaTime));
		location->set_y(_objInfo.location().y() + (y * speed * _deltaTime));
		location->set_z(94);
		_objInfo.set_allocated_location(location);

	}
	return false;
}

void GameObject::RotatorObject(Protocol::PBVector d)
{
	float degree = YHUtils::CalDegree(d,_objInfo.location());

	// 로테이터를 적용해준다.
	Protocol::PBVector* rotator = new Protocol::PBVector();
	{
		rotator->set_x(0);
		rotator->set_y(0);
		rotator->set_z(degree);
	}
	_objInfo.set_allocated_rotator(rotator);

	//_owner.lock()->_objInfo.set_allocated_rotator(rotator);;

	// 스테이트 패킷을 보내준다.
	Protocol::S_STATE sPkt;
	{
		sPkt.set_allocated_objectinfo(new Protocol::ObjectInfo(_objInfo));
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt);
		GRoom->DoAsync(&Room::Broadcast, sendBuffer);
	}
}

void GameObject::SetLocation(Vector3 d)
{
	// 위치 변경
	Protocol::PBVector location = _objInfo.location();
	{
		location.set_x(d.x);
		location.set_y(d.y);
		location.set_z(d.z);
		_objInfo.set_allocated_location(new Protocol::PBVector(location));
	}
}

void GameObject::SetLocation(float speed, float deltaTime)
{
	SetLocation(Vector3(
		_objInfo.location().x() + (_objInfo.forwarddir().x() * speed * _deltaTime),
		_objInfo.location().y() + (_objInfo.forwarddir().y() * speed * _deltaTime),
		_objInfo.location().z()));

}

bool GameObject::CalColEnemy(float distance, int isColCnt /*= -1*/, float damege, float deltaTime)
{
	int cnt = 0;
	map<s_uint64, EnemyRef> enemys = GRoom->GetMonsters();
	for (auto e : enemys)
	{
		if (YHUtils::DistanceCheck(_objInfo.location(), e.second->_objInfo.location(), distance))
		{
			// 몬스터가죽었다면 플레이어에게 경험치를 준다.
			if (e.second->HpMin(damege * deltaTime)) 
			{
				
			}
			
			cout << "현재 적 hp :" << e.second->_hp << endl;

			if (isColCnt == -1)
			{
				return true;
			}
			else
			{
				cnt++;
				if (cnt >= isColCnt)
				{
					_isDead = true;
					//GRoom->DoAsync(&Room::Leave, shared_from_this());
					return false; 
				}
			}
		}
	}
}
bool GameObject::CalColPlayer(float distance, int isColCnt /*= -1*/, float damege, float deltaTime)
{
	int cnt = 0;
	map<s_uint64, PlayerRef> players = GRoom->GetPlayers();
	for (auto p : players)
	{
		if (!p.second->_isDead && YHUtils::DistanceCheck(_objInfo.location(), p.second->_objInfo.location(), distance))// distance))
		{
			//cout << "Damage :" << distance << endl;

			p.second->HpMin(damege * deltaTime);
			cout << "현재 적 hp :" << p.second->_hp << endl;


			if (isColCnt == -1) // 계속 지속되게끔
			{
				return true; // 
			}
			else
			{
				cnt++;
				if (cnt >= isColCnt)
				{
					_isDead = true;
					return false; // 
				}
			}
		}
	}
}
bool GameObject::CalColSummons(float distance, int isColCnt /*= -1*/, float damege, float deltaTime)
{
	int cnt = 0;
	map<s_uint64, SummonsRef> summons = GRoom->GetSummons();
	for (auto s : summons)
	{
		if (!s.second->_isDead && YHUtils::DistanceCheck(_objInfo.location(), s.second->_objInfo.location(), distance))
		{
			//cout << "Damage :" << distance << endl;

			s.second->HpMin(damege * deltaTime);

			if (isColCnt == -1)
			{
				return true;
			}
			else
			{
				cnt++;
				if (cnt >= isColCnt)
				{
					_isDead = true;
					//GRoom->DoAsync(&Room::Leave, shared_from_this());
					return false; // 
				}
			}
		}
	}
}

GameObjectRef GameObject::CalColEnemy_Go(float distance, int isColCnt /*= -1*/, float damege /*= 0*/, float deltaTime /*= 1*/)
{
	GameObjectRef r = nullptr;
	
	int cnt = 0;
	map<s_uint64, EnemyRef> enemys = GRoom->GetMonsters();
	for (auto e : enemys)
	{
		if (!e.second->_isDead && YHUtils::DistanceCheck(_objInfo.location(), e.second->_objInfo.location(), distance))
		{
			// 몬스터가죽었다면 플레이어에게 경험치를 준다.
			if (e.second->HpMin(damege * deltaTime))
			{
				r = e.second;
			}

			cout << "현재 적 hp :" << e.second->_hp << endl;

			if (isColCnt == -1)
			{
				return r;
			}
			else
			{
				cnt++;
				if (cnt >= isColCnt)
				{
					_isDead = true;
					//GRoom->DoAsync(&Room::Leave, shared_from_this());
					return r;
				}
			}
		}
	}
	return r;
}

void GameObject::SetStat(int hp, int mp, int exp)
{
	_hp = hp;
	_mp = mp;
	_exp = exp;
}

float GameObject::HpPlus(float hp)
{
	_hp += hp;
	if (_hp > GetMaxHp())
		_hp = GetMaxMp();
	
	return _hp;
}

float GameObject::MpPlus(float mp)
{
	_mp += mp;
	if (_mp > GetMaxMp())
		_mp = GetMaxMp();
	
	return mp;
}

void GameObject::ExpAdd(float exp)
{
	_exp += exp; 

	while (_exp >= GetMaxExp())
	{
		_exp = _exp - GetMaxExp() ;
	
		_level += 1;
		cout << "AcountId: "<< _ownerSession->GetId() << " Level Up: " <<  _level << endl;
	} 

	// Update Exp
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();
		DBSynchronizer dbSync(*dbConn);

		SP::UpdateExp q(*dbConn);

		q.In_AccountId(_ownerSession->GetId());
		q.In_Exp(_exp);
		q.In_Level(_level);
		q.Execute();

		GDBConnectionPool->Push(dbConn);
	}

	Protocol::S_EXP pkt;
	pkt.set_exp(_exp);
	pkt.set_level(_level);
	pkt.set_maxexp(GetMaxExp());
	auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
	_ownerSession->Send(sendBuffer);

	Protocol::S_STAT statPkt;
	statPkt.set_id(_objInfo.roomid());
	statPkt.set_hp(HpPlus(GetMaxHp()));
	statPkt.set_mp(MpPlus(GetMaxMp()));
	statPkt.set_maxhp(GetMaxHp());
	statPkt.set_maxmp(GetMaxMp());
	auto sendBuffer2 = ClientPacketHandler::MakeSendBuffer(statPkt);
	GRoom->Broadcast(sendBuffer2);

}

float GameObject::GetMaxExp()
{
	float r = 0;
	float standardMaxExp = 1;
	
	for (int i = 1;i <= _level;++i)
	{
		standardMaxExp += (standardMaxExp * 1.2);
		r += i * standardMaxExp;
	}
	
	return r;
}

float GameObject::GetMaxHp()
{
	float r = 0;
	float defaultHp = 1;
	float standardMaxHp = 50;

	for (int i = 1; i <= _level; ++i)
	{
		standardMaxHp += (defaultHp * 1.02);
		r += i * standardMaxHp;
	}

	return r;
}

float GameObject::GetMaxMp()
{
	float r = 0;
	float defaultMp = 1;
	float standardMaxMp = 50;

	for (int i = 1; i <= _level; ++i)
	{
		standardMaxMp += (defaultMp * 1.02);
		r += i * standardMaxMp;
	}

	return r;
}

float GameObject::GetAtt()
{
	float r = 0;
	float defaultAtt = 1;

	for (int i = 1; i <= _level; ++i)
	{
		defaultAtt += (defaultAtt * 1.02);
		r += i * defaultAtt;
	}

	return r;
}

float GameObject::GetDepend()
{
	float r = 0;
	float defaultDef = 1;

	for (int i = 1; i <= _level; ++i)
	{
		defaultDef += (defaultDef * 1.02);
		r += i * defaultDef;
	}

	return r;
}
