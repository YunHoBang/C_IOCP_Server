#include "pch.h"
#include "Enemy.h"
#include "Player.h"
#include "Summons.h"
#include "Projectile.h"
#include "Vector2.h"
#include "DataManager.h"
#include "GameObject.h"
#include "YHSkill.h"

void Enemy::Set(int roomId, int metaId, const Protocol::PBVector& location)
{
	_objInfo.set_roomid(GRoom->ProvideRoomId());
	_objInfo.set_type(Protocol::MONSTER);
	_objInfo.set_metaid(_metaId = metaId);
	_objInfo.set_allocated_location(new Protocol::PBVector(location));

	_meta = DataManager::EnemyMap.find(_metaId)->second;

	YHSkillRef s = MakeShared<YHSkill>(shared_from_this(), _meta.skillid());
	_skills.insert(pair<int, YHSkillRef>(_meta.skillid(), s));

	GameObject::SetStat(_meta.stat().hp(), _meta.stat().mp(), _meta.stat().exp());

	_map = GRoom->GetMap();
	_patrolPos = _objInfo.location();
}

void Enemy::Update()
{
	// ���� �����ִ°��� ��ġ���� �ʾҴٸ� �������ش�.
	{
		GameObjectRef e = _capturedEnemy.lock();
		if (e)
		{
			if (!GRoom->GetMap().CanGo(GRoom->GetMap().Pos2Idx(e->GetObjectInfo().location())))
			{
				_capturedEnemy.reset();
			}
		}
	}

	GameObject::Update();
	
	GRoom->DoTimer(shared_from_this(), 5, &GameObject::Update);
}

void Enemy::State_Idle()
{
	// 5�ʵ��� ���̾����� Move�� ����
	if (_idleTime >= 2)
	{
		ChangeState(Protocol::MOVING);
		_idleTime = 0.0f;
	}
	else
	{
		_idleTime += _deltaTime;
	}

	// ���� Ž���� �������� �̵��ϰԲ� ������Ʈ�� ����
	{
		if (_capturedEnemy.lock())
			ChangeState(Protocol::StateType::SKILL, false);
		else
		{
			map<s_uint64, PlayerRef> players = GRoom->GetPlayers();
			map<s_uint64, SummonsRef> summons = GRoom->GetSummons();

			if (players.size() > 0)
			{
				for (auto i : players)
				{
					PlayerRef p = i.second;

					if (YHUtils::DistanceCheck(_objInfo.location(), p->GetObjectInfo().location(), _meta.detectrange()))
					{
						ChangeState(Protocol::StateType::MOVING);
						_capturedEnemy = p;
					}
				}
			}
			if (summons.size() > 0)
			{
				for (auto i : summons)
				{
					SummonsRef p = i.second;

					if (YHUtils::DistanceCheck(_objInfo.location(), p->GetObjectInfo().location(), _meta.detectrange()))
					{
						ChangeState(Protocol::StateType::SKILL);
						_capturedEnemy = p;
					}
				}
			}
		}
	}

	if (_state != Protocol::IDLE)
	{
		_idleTime = 0.0f;
		return;
	}
}

void Enemy::State_Moving()
{
	//return;

	GameObjectRef e = nullptr; 

	// ���� ���� Ȯ��
	{
		if (!_capturedEnemy.expired())
		{
			e = _capturedEnemy.lock();
			_isEnemyValid = true;
		}
		else
		{
			_isEnemyValid = false;
		}
	}

	// ��� �ޱ�
	{
		if (e) // ���� ����
		{
			if (!e->GetIsDead() && !_isDone) // ���� ��� �ְ� �̵����� �ƴ϶�� ��θ� �޴´�.
			{
				_routes = _map.AstarEX(_objInfo.location(), e->GetObjectInfo().location());
				_isDone = true;
			}
		}
		else // ���� ����
		{
			if (!_isDone)
			{
				_routes = _map.MoveAbleRandomPos(_patrolPos);
				_isDone = true;
			}
		}
	}

	// ����߿� ��ֹ��� ������ ��ã�� ����ȭ
	{
		if (e && !_map.IsObstacleCheck(_objInfo.location()) && !_isObstacleCheck)
		{
			_isDone = true;
			_isObstacleCheck = true;
			_routes = _map.Astar(_objInfo.location(), e->GetObjectInfo().location());
		}
	}

	// ���� ������ ����
	{
		// ���� �������� �ð��� ���.
		if (!_isEnemyValid)
			_moveTime += _deltaTime;

		// ���� ������ ������ �ð��� �Ѿ�ٸ�? idle�� �������ش�.
		if (_moveTime >= 5)
			ChangeState(Protocol::StateType::IDLE);
	}

	// ���� �Ÿ�üũ
	{
		if (e)
		{
			if (YHUtils::DistanceCheck(_objInfo.location(), e->GetObjectInfo().location(), _meta.attrange())) // �Ÿ��� ���ؼ� ������Ʈ�� �������ְ�����
			{
				ChangeState(Protocol::StateType::SKILL, false);
			}
		}
	}

	// ���� ���� üũ  
	{
		if (e && e->GetIsDead())
		{
			ChangeState(Protocol::StateType::IDLE);
			_capturedEnemy.reset();
		}

		if (_state != Protocol::MOVING)
		{
			_isDone = false;
			_isObstacleCheck = false;
			_routes.clear();
			_moveTime = 0.0f;
			return;
		}
	}
	
	NewMovePos();
}

void Enemy::State_Skill()
{
	//return;

	if (_capturedEnemy.expired() || _capturedEnemy.lock()->GetIsDead())
	{
		ChangeState(Protocol::StateType::IDLE);
		//_state = Protocol::StateType::IDLE;
		_capturedEnemy.reset();
	}

	Protocol::S_STATE sPkt;
	{
		sPkt.set_allocated_objectinfo(new Protocol::ObjectInfo(_objInfo));

		sPkt.set_state(Protocol::IDLE);
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(sPkt);
		GRoom->DoAsync(&Room::Broadcast, sendBuffer);
	}

	GameObjectRef e = _capturedEnemy.lock();
	if (e)
	{
		// ���� �����Ÿ����� �־����ٸ� Move�� ����
		{
			if (!YHUtils::DistanceCheck(_objInfo.location(), e->GetObjectInfo().location(), _meta.attrange()))
			{
				ChangeState(Protocol::StateType::MOVING, false);
				return;
			}
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

			ChangeState(Protocol::StateType::MOVING, false);
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
}

void Enemy::State_Dead()
{
	if (_isDead == true)
		return;
	_isDead = true;

	GRoom->DoAsync(&Room::CreateRewardObject, shared_from_this());

	// ������Ʈ ��Ŷ�� ������ 5���Ŀ� �����ǰԲ�
	Protocol::S_STATE statePkt;
	{
		Protocol::ObjectInfo* objInfo = new Protocol::ObjectInfo(_objInfo);
		statePkt.set_allocated_objectinfo(objInfo);
		statePkt.set_state(Protocol::DEAD);
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(statePkt);
		GRoom->DoAsync(&Room::Broadcast, sendBuffer);
	}
	GRoom->DoTimer(5000, &Room::Leave, shared_from_this());
}

float Enemy::GetMaxHp()
{
	return _meta.stat().hp();
}
