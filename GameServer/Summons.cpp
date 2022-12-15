#include "pch.h"
#include "Summons.h"
#include "Enemy.h"
#include "Player.h"
#include "Projectile.h"
#include "Vector2.h"
#include "GameObject.h"
#include "YHSkill.h"
#include "DataManager.h"

void Summons::Set(GameObjectRef owner, int metaId, int roomid, SummonsLocation l, const Protocol::ObjectInfo& info)
{
	_owner = owner;
	_map = GRoom->GetMap();

	_meta = DataManager::SummonsMap.find(_summonsId)->second;
	GameObject::SetStat(_meta.stat().hp(), _meta.stat().mp(), _meta.stat().exp());
	YHSkillRef s = MakeShared<YHSkill>(shared_from_this(), _meta.skillid());
	_skills.insert(pair<int, YHSkillRef>(s->GetSkillId(), s));

	_objInfo.set_roomid(GRoom->ProvideRoomId());
	_objInfo.set_type(Protocol::SUMMONS);
	_objInfo.set_metaid(_summonsId = metaId);
	_objInfo.set_allocated_location(new Protocol::PBVector(info.location()));

	_sLocation = (SummonsLocation)(l);
}

void Summons::Update()
{
	GameObject::Update();

	// 시간체크 & 삭제
	if (_time >= _meta.duration())
		ChangeState(Protocol::DEAD);

	GRoom->DoTimer(shared_from_this(), 100, &GameObject::Update);
}

void Summons::State_Idle()
{
	// 몬스터 특정 거리 이하로 들어오면 플레이어 방향으로 이동한다.
	map<s_uint64, EnemyRef> enemys = GRoom->GetMonsters();
	if (enemys.size() > 0)
	{
		for (auto i : enemys)
		{
			EnemyRef e = i.second;
			if (e->GetIsDead() == false)
			{
				if (YHUtils::DistanceCheck(_objInfo.location(), e->GetObjectInfo().location(), _meta.detectrange()))
				{
					ChangeState(Protocol::MOVING);
					_capturedEnemy = e;
					return;
				}
			}
		}
	}
	// 오너 오브젝트와 위치를 비교해서 위치가 다르다면 move로 변경
	if (!_owner.expired())
	{
		auto o = _owner.lock();
		bool isCompareLocation = !(Vector3(o->GetObjectInfo().location()) == _compareOwner);
		if (YHUtils::DistanceCheck(_objInfo.location(), _owner.lock()->GetObjectInfo().location(), 400))
		{
			return;
		}
		else if (isCompareLocation)
		{
			ChangeState(Protocol::MOVING);
			_compareOwner = o->GetObjectInfo().location();
			return;
		}
	}
}

void Summons::State_Moving()
{
	GameObjectRef e = _capturedEnemy.lock();

	if (e && YHUtils::DistanceCheck(_objInfo.location(), e->GetObjectInfo().location(), _meta.attrange()))
	{
		ChangeState(Protocol::SKILL,false);
		_routes.clear();
		_isDone = false;
		return;
	}
	// 적이 있고 이동중이 아니면 이동경로를 받는다.
	else if (e && !_isDone)
	{
		_isDone = true;
		_routes = _map.AstarStraight(_objInfo.location(), e->GetObjectInfo().location());
	}
	// 플레이어와의 거리가 그리 멀지 않다면 굳이 플레이어 쪽으로 이동하지 않는다.
	else if (YHUtils::DistanceCheck(_objInfo.location(), _owner.lock()->GetObjectInfo().location(), 300))
	{
		ChangeState(Protocol::StateType::IDLE);
		_compareOwner = _owner.lock()->GetObjectInfo().location();
		_routes.clear();
		_isDone = false;
		return;
	}
	else if (!e && !_isDone) // 적이 없는데 이동중이 아니면 오너 쪽의 이동경로를 받는다.
	{
		_isDone = true;
		GetOneLine(_sLocation);
	}
	else if (_capturedEnemy.expired() || e->GetIsDead())
	{
		if (!_isDone)
		{
			ChangeState(Protocol::IDLE);
			_capturedEnemy.reset();
			_routes.clear();
			_isDone = false;
			return;

		}
	}
	
	if (e && !_map.IsObstacleCheck(_objInfo.location()) && !_isObstacleCheck) //이동한다.
	{
		_isDone = true;
		_isObstacleCheck = true;
		_routes = _map.Astar(_objInfo.location(), e->GetObjectInfo().location());
	}
	
	NewMovePos();
}

void Summons::State_Skill()
{
	GameObjectRef e = _capturedEnemy.lock();
	if (!e || e->GetIsDead())
	{
		ChangeState(Protocol::StateType::IDLE);
		_capturedEnemy.reset();
		return;
	}

	Protocol::S_STATE sPkt;
	{
		sPkt.set_allocated_objectinfo(new Protocol::ObjectInfo(_objInfo));
		sPkt.set_state(Protocol::IDLE);
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt);
		GRoom->DoAsync(&Room::Broadcast, sendBuffer);
	}

	// 적이 사정거리보다 멀어진다면 Move로 변경
	if (e && !YHUtils::DistanceCheck(_objInfo.location(), e->GetObjectInfo().location(), _meta.attrange()))
	{
		ChangeState(Protocol::StateType::IDLE);
		return;
	}

	// 캐릭터 회전 변경
	{
		float degree;
		degree = YHUtils::CalDegree(e->GetObjectInfo().location(), _objInfo.location());

		Protocol::PBVector* rotator = new Protocol::PBVector();
		{
			rotator->set_x(0);
			rotator->set_y(0);
			rotator->set_z(degree);
		}
		_objInfo.set_allocated_rotator(rotator);

	}

	// 캐릭터 공격
	{
		Protocol::C_ATTACK pkt;
		{
			// 몬스터 스킬을 세팅한다.
			pkt.set_attackid(_meta.skillid());

			// 방향을 세팅한다.
			Protocol::PBVector* f = new Protocol::PBVector();
			_objInfo.set_allocated_forwarddir(YHUtils::CalForwardAndSet(e->GetObjectInfo().location(), _objInfo.location(), *f));

			pkt.set_allocated_objectinfo(new Protocol::ObjectInfo(_objInfo));
		}
		GRoom->DoAsync(&Room::ObjectSkillInvoke,
			static_pointer_cast<GameObject>(shared_from_this()),
			_capturedEnemy.lock(),
			pkt);
	}
}

void Summons::State_Dead()
{
	if (_isDead == true)
		return;
	_isDead = true;

	// 플레이어쪽에서도 삭제해준다.
	PlayerRef p = static_pointer_cast<Player>(_owner.lock());
	p->GetSummons().erase(_summonsId);

	ChangeState(Protocol::DEAD);

	// 스테이트 패킷을 보내고 5초후에 삭제되게끔
	GRoom->DoTimer(5000, &Room::Leave, shared_from_this());
}

void Summons::GetOneLine(SummonsLocation c)
{
	if (auto o = _owner.lock())
	{
		CreateAbleIdx ownerIdx = _map.Pos2Idx(o->GetObjectInfo().location());
		switch (_sLocation)
		{
		case SUMMONS_LOCATION_1:ownerIdx.x += 3; break;
		case SUMMONS_LOCATION_2:ownerIdx.y += 3; break;
		case SUMMONS_LOCATION_3:ownerIdx.y -= 3; break;
		case SUMMONS_LOCATION_NONE:default:break;
		}
		if (_map.CanGo(ownerIdx)) // 갈수있다면 그쪽으로 이동하는 경로를 받는다.
			_routes = _map.AstarStraight(_objInfo.location(), _map.Idx2Pos(ownerIdx));
	}
}

float Summons::GetAtt()
{
	return 0;
}

float Summons::GetDepend()
{
	return 0;
}

float Summons::GetMaxHp()
{
	return _meta.stat().hp();
}
