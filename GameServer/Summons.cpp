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

	// �ð�üũ & ����
	if (_time >= _meta.duration())
		ChangeState(Protocol::DEAD);

	GRoom->DoTimer(shared_from_this(), 100, &GameObject::Update);
}

void Summons::State_Idle()
{
	// ���� Ư�� �Ÿ� ���Ϸ� ������ �÷��̾� �������� �̵��Ѵ�.
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
	// ���� ������Ʈ�� ��ġ�� ���ؼ� ��ġ�� �ٸ��ٸ� move�� ����
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
	// ���� �ְ� �̵����� �ƴϸ� �̵���θ� �޴´�.
	else if (e && !_isDone)
	{
		_isDone = true;
		_routes = _map.AstarStraight(_objInfo.location(), e->GetObjectInfo().location());
	}
	// �÷��̾���� �Ÿ��� �׸� ���� �ʴٸ� ���� �÷��̾� ������ �̵����� �ʴ´�.
	else if (YHUtils::DistanceCheck(_objInfo.location(), _owner.lock()->GetObjectInfo().location(), 300))
	{
		ChangeState(Protocol::StateType::IDLE);
		_compareOwner = _owner.lock()->GetObjectInfo().location();
		_routes.clear();
		_isDone = false;
		return;
	}
	else if (!e && !_isDone) // ���� ���µ� �̵����� �ƴϸ� ���� ���� �̵���θ� �޴´�.
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
	
	if (e && !_map.IsObstacleCheck(_objInfo.location()) && !_isObstacleCheck) //�̵��Ѵ�.
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

	// ���� �����Ÿ����� �־����ٸ� Move�� ����
	if (e && !YHUtils::DistanceCheck(_objInfo.location(), e->GetObjectInfo().location(), _meta.attrange()))
	{
		ChangeState(Protocol::StateType::IDLE);
		return;
	}

	// ĳ���� ȸ�� ����
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

	// ĳ���� ����
	{
		Protocol::C_ATTACK pkt;
		{
			// ���� ��ų�� �����Ѵ�.
			pkt.set_attackid(_meta.skillid());

			// ������ �����Ѵ�.
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

	// �÷��̾��ʿ����� �������ش�.
	PlayerRef p = static_pointer_cast<Player>(_owner.lock());
	p->GetSummons().erase(_summonsId);

	ChangeState(Protocol::DEAD);

	// ������Ʈ ��Ŷ�� ������ 5���Ŀ� �����ǰԲ�
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
		if (_map.CanGo(ownerIdx)) // �����ִٸ� �������� �̵��ϴ� ��θ� �޴´�.
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
