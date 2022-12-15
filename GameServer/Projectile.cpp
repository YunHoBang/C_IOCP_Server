#include "pch.h"
#include "Projectile.h"
#include "Enemy.h"
#include "Summons.h"
#include <math.h>
#include "DataManager.h"

using namespace Protocol;

void Projectile::Set(GameObjectRef owner, int metaId, int roomid , const Protocol::ObjectInfo& info)
{
	_owner = owner;
	_ownerSession = (_owner.lock())->GetSession();
	_objInfo.set_type(Protocol::PROJECTILE);
	_objInfo.set_roomid(roomid);
	
	// 메타데이터 세팅
	_objInfo.set_metaid(_projectileId = metaId);
	_meta = DataManager::ProjectileMap.find(_projectileId)->second;

	_objInfo.set_scale(_meta.scale());

	Protocol::PBVector* location = new Protocol::PBVector();
	{
		location->set_x(info.location().x());
		location->set_y(info.location().y());
		location->set_z(94);
	}
	_objInfo.set_allocated_location(location);

	Protocol::PBVector* forwardDir = new Protocol::PBVector(info.forwarddir());
	_objInfo.set_allocated_forwarddir(forwardDir);

	Protocol::PBVector* addDir = new Protocol::PBVector(info.rotator());
	_objInfo.set_allocated_rotator(addDir);

}

void Projectile::Set(GameObjectRef owner, int metaId, int roomid, const Protocol::PBVector& l, const Protocol::PBVector& r, const Protocol::PBVector& f)
{
	_owner = owner;
	_ownerSession = (_owner.lock())->GetSession();
	_objInfo.set_type(Protocol::PROJECTILE);
	_objInfo.set_roomid(roomid);

	// 메타데이터 세팅
	_projectileId = metaId;
	_meta = DataManager::ProjectileMap.find(_projectileId)->second;

	_objInfo.set_scale(_meta.scale());

	Protocol::PBVector* location = new Protocol::PBVector();
	{
		location->set_x(l.x());
		location->set_y(l.y());
		location->set_z(94);
	}
	_objInfo.set_allocated_location(location);

	Protocol::PBVector* forward = new Protocol::PBVector(f);
	_objInfo.set_allocated_forwarddir(forward);

	Protocol::PBVector* rotator = new Protocol::PBVector(r);
	_objInfo.set_allocated_rotator(rotator);
}

void Projectile::Set(GameObjectRef owner, int metaId, int roomid, const Protocol::ObjectInfo& info, GameObjectRef enemy)
{
	Set(owner,metaId,roomid, info);
	_capturedEnemy = enemy;
}

void Projectile::Update()
{
	if (_isDead == true)
	{
		GRoom->DoAsync(&Room::Leave, shared_from_this());
		return;
	}

	GameObject::Update();
	ProjectileUpdate();

	// 시간체크 & 삭제
	if (_time >= _meta.duration())
	{
		_isDead = true;
		GRoom->DoAsync(&Room::Leave, shared_from_this());
	}

	// 무브 패킷 샌드
	{
		Protocol::S_MOVE movePkt;
		movePkt.set_allocated_objectinfo(new Protocol::ObjectInfo(_objInfo));
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(movePkt);
		GRoom->DoAsync(&Room::Broadcast, sendBuffer);
	}
	
	GRoom->DoTimer(shared_from_this(), 50, &GameObject::Update);
}

void Projectile::ProjectileUpdate()
{
	switch (_projectileId)
	{
	case 1: Mage_1(); break;
	case 2: Mage_2(); break;
	case 3:	Mage_3(); break;
	case 4:	Knight_1(); break;
	case 5:	Knight_2(); break;
	case 6:	Gunner_1(); break;
	case 7:	Gunner_2(); break;
	case 8:	Gunner_3(); break;

	case 9:	Summons_1(); break;
	case 10: Summons_2(); break;

	case 11: Monster_1(); break;
	case 12: Monster_2(); break;
	case 13: Monster_3(); break;
	case 14: Monster_4(); break;
	case 15: Monster_5(); break;
	case 16: Monster_6(); break;

	case 17: Knight_0(); break;
	case 18: Summons_0(); break;
	case 19: Monster_0(); break;

	default:break;
	}

}

GameObjectRef Projectile::CalColEnemy_Go(float distance, int isColCnt /*= -1*/, float damege /*= 0*/, float deltaTime /*= 1*/)
{
	GameObjectRef r = nullptr;
	r = GameObject::CalColEnemy_Go(distance, isColCnt, damege, deltaTime);

	EnemyRef e = static_pointer_cast<Enemy>(r);
	
	// 죽은 몬스터가 있다는것
	if (r && !_owner.expired())
	{
		auto o =_owner.lock();
		if (o->GetObjectInfo().type() == Protocol::PLAYER)
		{
			o->ExpAdd(e->_meta.stat().exp());
		}
		else if (o->GetObjectInfo().type() == Protocol::SUMMONS)
		{
			SummonsRef s = static_pointer_cast<Summons>(o);
			
			if (!s->GetOwnerGo().expired())
			{
				auto so = s->GetOwnerGo().lock();

				so->ExpAdd(e->_meta.stat().exp());
			
			}
		}
	}
	return nullptr;
}

float Projectile::GetAtt()
{
	float r = 0.0f;
	if (!_owner.expired())
	{
		auto o = _owner.lock();
		r = _meta.damage() + o-> GetAtt();
	}

	return r;
}

void Projectile::Mage_1()
{
	// 충돌 체크
	CalColEnemy_Go(GetDetectionRange(), 1, GetAtt());

	// 위치 변경
	SetLocation(_meta.speed(), _deltaTime);
}

void Projectile::Mage_2()
{
	// 충돌 체크
	CalColEnemy_Go(GetDetectionRange(), -1, GetAtt(), _deltaTime);

	// 각도 -> 벡터 변환
	Vector3 v = YHUtils::Radian2Vector3(_radians);

	if (!_owner.expired())
	{
		// 위치를 세팅한다.
		SetLocation(Vector3(
			_owner.lock()->GetObjectInfo().location().x() + (v.x * _meta.limitdistance()),
			_owner.lock()->GetObjectInfo().location().y() + (v.y * _meta.limitdistance()),
			94
		));
	}
	else
	{
		_isDead = true;
	}

	// 각도값을 올린다.
	_radians += 1 * (_deltaTime * _meta.speed());

	// 각도가 360도가 넘어가면 초기화해준다.
	if (_radians >= 3.141592 * 4) { _radians = 0; }

}

void Projectile::Mage_3()
{
	if (_isChildCheck == false)
	{
		_isChildCheck = true;
		GameObjectRef o = _owner.lock();

		float d = 0;
		for (int i = 0; i < 4; ++i)
		{
			ProjectileRef p = MakeShared<Projectile>();
			p->Set(shared_from_this(), 2, GRoom->ProvideRoomId(), _objInfo);
			p->_radians = YHUtils::Degree2Radian(d);
			d += 90;

			GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));
		}
	}

	// 충돌 체크
	CalColEnemy_Go(GetDetectionRange(), -1, GetAtt(), _deltaTime);

	// 위치 변경
	SetLocation(_meta.speed(), _deltaTime);
}

void Projectile::Knight_1()
{
	// 충돌 체크
	CalColEnemy_Go(GetDetectionRange(), -1, GetAtt(), _deltaTime);

	// 위치 변경
	SetLocation(_meta.speed(), _deltaTime);

	_meta.set_scale(_meta.scale() + (2 * _deltaTime));
	_meta.set_size(_meta.size() + (2 * _deltaTime));
	_meta.set_damage(_meta.damage() + (2 * _deltaTime));
	_objInfo.set_scale(_meta.scale());
}

void Projectile::Knight_2()
{
	if (_isChildCheck == false)
	{
		_isChildCheck = true;
		GameObjectRef o = _owner.lock();

		float d = 0;
		for (int i = 0; i < 4; ++i)
		{
			ProjectileRef p = MakeShared<Projectile>();
			p->Set(o, 2, GRoom->ProvideRoomId(), _objInfo);
			p->_radians = YHUtils::Degree2Radian(d);
			d += 90;

			GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));
		}
	}

	// 충돌 체크
	//CalColEnemy(GetDetectionRange(), -1, _meta.damage(), _deltaTime);
	CalColEnemy_Go(GetDetectionRange(), -1, GetAtt(), _deltaTime);

	// 위치를 세팅한다.
	SetLocation(Vector3(_owner.lock()->GetObjectInfo().location().x(), _owner.lock()->GetObjectInfo().location().y(), 94));
}

void Projectile::Gunner_1()
{
	// 충돌 체크
	CalColEnemy_Go(GetDetectionRange(), 1, GetAtt());

	// 위치 변경
	SetLocation(_meta.speed(), _deltaTime);
}

void Projectile::Gunner_2()
{
	if (_isChildCheck)
	{
		if (_capturedEnemy.lock())
		{
			if (_capturedEnemy.lock()->GetIsDead())
			{
				_isDead = true;
				return;
			}

			// 충돌 체크
			CalColEnemy_Go(GetDetectionRange(), 1, GetAtt());

			EnemyRef e = static_pointer_cast<Enemy>(_capturedEnemy.lock());

			// 이동로직
			Protocol::PBVector* f = new Protocol::PBVector();
			YHUtils::CalForwardAndSet(e->GetObjectInfo().location(), _objInfo.location(), *f);

			_meta.set_speed(_meta.speed() * 1.5);

			Protocol::PBVector location = _objInfo.location();
			SetLocation(Vector3(
				location.x() + (f->x() * _meta.speed() * _deltaTime),
				location.y() + (f->y() * _meta.speed() * _deltaTime),
				location.z() + (f->z() * _meta.speed() * _deltaTime)
			));
		}
		else // 포착한 적이 없다면 
		{
			map<s_uint64, EnemyRef> monsters = GRoom->GetMonsters();
			for (auto monster : monsters)
			{
				if (YHUtils::DistanceCheck(_objInfo.location(), monster.second->GetObjectInfo().location(), 300))
				{
					_capturedEnemy = monster.second;
					return;
				}
			}
		}
	}
	else
	{
		// 위치를 변경해주어야한다.
		_meta.set_speed(17);

		const float gravity = 9.8f * 2;
		Protocol::PBVector location = _objInfo.location();

		_objInfo.set_allocated_location(new Protocol::PBVector(YHUtils::CalParabolaPB(location, _objInfo.forwarddir(), _meta.speed() , _time, gravity)));

		if (_compareZ > _objInfo.location().z())
		{
			_isChildCheck = true;

			Protocol::PBVector location = _objInfo.location();
			{
				location.set_x(location.x());
				location.set_y(location.y());
				location.set_z(_compareZ);
				_objInfo.set_allocated_location(new Protocol::PBVector(location));
			}
			return;
		}
		_compareZ = _objInfo.location().z();
	}
}

void Projectile::Gunner_3()
{
	if (_isChildCheck)
	{
		// 충돌 체크
		CalColEnemy_Go(GetDetectionRange(), 1, GetAtt());
	}
	else
	{
		Protocol::PBVector location = _objInfo.location();
		_objInfo.set_allocated_location(new Protocol::PBVector(YHUtils::CalParabolaPB(location, _objInfo.forwarddir(),_meta.speed() , _time, GRAVITY * 15)));

		// 투사체가 땅에 닿았는지 체크하는 변수
		if (_objInfo.location().z() <= 0)
		{
			_isChildCheck = true;
			SetLocation(Vector3(location.x(), location.y(), 2));
			return;
		}
	}
}

void Projectile::Summons_1()
{
	// 충돌 체크
	//CalColEnemy(GetDetectionRange(), 1, _meta.damage());
	CalColEnemy_Go(GetDetectionRange(), 1, GetAtt());


	// 위치 변경
	SetLocation(_meta.speed(), _deltaTime);
}

void Projectile::Summons_2()
{
	// 충돌 체크
	CalColEnemy_Go(GetDetectionRange(), 1, GetAtt());

	// 위치 변경
	SetLocation(_meta.speed(), _deltaTime);
}

void Projectile::Monster_1()
{
	// 충돌 체크
	CalColPlayer(GetDetectionRange(), 1, _meta.damage());
	CalColSummons(GetDetectionRange(), 1, _meta.damage());

	// 위치 변경
	SetLocation(_meta.speed(), _deltaTime);
}

void Projectile::Monster_2()
{
	if (_capturedEnemy.lock())
	{

		EnemyRef e = static_pointer_cast<Enemy>(_capturedEnemy.lock());

		// 이동로직
		Protocol::PBVector* f = new Protocol::PBVector();
		YHUtils::CalForwardAndSet(e->GetObjectInfo().location(), _objInfo.location(), *f);

		_meta.set_speed(_meta.speed() + (10 * _deltaTime));

		Protocol::PBVector location = _objInfo.location();
		SetLocation(Vector3(
			location.x() + (f->x() * _meta.speed() * _deltaTime),
			location.y() + (f->y() * _meta.speed() * _deltaTime),
			location.z() + (f->z() * _meta.speed() * _deltaTime)
		));

	}

	// 충돌 체크
	CalColPlayer(GetDetectionRange(), 1, _meta.damage());
	CalColSummons(GetDetectionRange(), 1, _meta.damage());

}

void Projectile::Monster_3()
{
	Protocol::PBVector location = _objInfo.location();
	_objInfo.set_allocated_location(new Protocol::PBVector(YHUtils::CalParabolaPB(location, _objInfo.forwarddir(), _meta.speed(), _time, GRAVITY * 50)));

	// 투사체가 땅에 닿았는지 체크하는 변수
	if (_objInfo.location().z() <= 0)
	{
		_isDead = true;
		GRoom->DoAsync(&Room::Leave, shared_from_this());

		SetLocation(Vector3(location.x(), location.y(), 2));
		return;
	}

	// 충돌 체크
	CalColPlayer(GetDetectionRange(), 1, _meta.damage());
	CalColSummons(GetDetectionRange(), 1, _meta.damage());

}

void Projectile::Monster_4()
{
	if (_capturedEnemy.lock())
	{
		EnemyRef e = static_pointer_cast<Enemy>(_capturedEnemy.lock());

		// 이동로직
		Protocol::PBVector* f = new Protocol::PBVector();
		YHUtils::CalForwardAndSet(e->GetObjectInfo().location(), _objInfo.location(), *f);

		_meta.set_speed(_meta.speed() + (10 * _deltaTime));

		Protocol::PBVector location = _objInfo.location();
		SetLocation(Vector3(
			location.x() + (f->x() * _meta.speed() * _deltaTime),
			location.y() + (f->y() * _meta.speed() * _deltaTime),
			location.z() + (f->z() * _meta.speed() * _deltaTime)
		));

	}

	// 충돌 체크
	CalColPlayer(GetDetectionRange(), 1, _meta.damage());
	CalColSummons(GetDetectionRange(), 1, _meta.damage());

}

void Projectile::Monster_5()
{
	Protocol::PBVector location = _objInfo.location();
	_objInfo.set_allocated_location(new Protocol::PBVector(YHUtils::CalParabolaPB(location, _objInfo.forwarddir(), _meta.speed(), _time, GRAVITY * 50)));

	// 투사체가 땅에 닿았는지 체크하는 변수
	if (_objInfo.location().z() <= 0)
	{
		_isDead = true;
		GRoom->DoAsync(&Room::Leave, shared_from_this());

		SetLocation(Vector3(location.x(), location.y(), 2));
		return;
	}

	// 충돌 체크
	CalColPlayer(GetDetectionRange(), 1, _meta.damage());
	CalColSummons(GetDetectionRange(), 1, _meta.damage());

}

void Projectile::Monster_6()
{
	// 충돌 체크
	CalColPlayer(GetDetectionRange(), -1, _meta.damage(),_deltaTime);
	CalColSummons(GetDetectionRange(), -1, _meta.damage(),_deltaTime);

	// 위치 변경
	SetLocation(_meta.speed(), _deltaTime);
}

void Projectile::Knight_0()
{
	// 충돌 체크
	CalColEnemy_Go(GetDetectionRange(), -1, _meta.damage());

	_isDead = true;
}

void Projectile::Summons_0()
{
	// 충돌 체크
	CalColEnemy_Go(GetDetectionRange(), -1, _meta.damage());

	_isDead = true;
}

void Projectile::Monster_0()
{
	// 충돌 체크
	CalColPlayer(GetDetectionRange(), -1, _meta.damage());
	CalColSummons(GetDetectionRange(),-1, _meta.damage());

	_isDead = true;
}
