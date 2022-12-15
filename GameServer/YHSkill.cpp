#include "pch.h"
#include "YHSkill.h"
#include "Session.h"
#include "Projectile.h"
#include "Enemy.h"
#include "Player.h"

#include "Protocol.pb.h"
#include "Enum.pb.h"
#include "Struct.pb.h"

#include "Summons.h"
#include "GameMap.h"
#include "DataManager.h"

YHSkill::YHSkill(GameObjectRef o, int id)
{
	_owner = o;
	_skillId = id;
	_meta = DataManager::SkillMap.find(_skillId)->second;
}

void YHSkill::Execute(Protocol::C_ATTACK pkt)
{
	// ��Ÿ�Ӱ� mp�� üũ�Ѵ�.
	if (!IsCoolTimeCheck() || _owner.lock()->GetMp() < _meta.mpconsumption())
		return;

	// ��Ÿ���� �����Ѵ�.
	StartCoolTime(_meta.cooldown());

	// MP�� �ٿ��ش�.
	_owner.lock()->MpMin(_meta.mpconsumption());

	// ��Ŷ�� �´� ��ų�� �����Ѵ�.
	GameObjectRef o = _owner.lock();
	if (o)
	{
		switch (o->GetObjectInfo().type())
		{
		case Protocol::PLAYER:
		{
			switch (_skillId)
			{
			case 1001: KnightSkill_1(pkt); 	break;
			case 1002: KnightSkill_2(pkt, o);  break;
			case 1003: KnightSkill_3(pkt, o);  break;
			case 1004: KnightSkill_4(pkt, o);  break;
			case 1006: MageSkill_1(pkt);  break;
			case 1007: MageSkill_2(pkt, o);  break;
			case 1008: MageSkill_3(pkt, o);  break;
			case 1009: MageSkill_4(pkt, o);  break;
			case 1011: GunnerSkill_1(pkt); 	break;
			case 1012: GunnerSkill_2(pkt, o);  break;
			case 1013: GunnerSkill_3(pkt); 	break;
			case 1014: GunnerSkill_4(pkt, o);  break;
			default:break;
			}
		}	break;
		case Protocol::SUMMONS:
		{
			switch (_skillId)
			{
			case 1022: SummonsSkill_1(pkt, o);  break;
			case 1023: SummonsSkill_2(pkt, o);  break;
			case 1024: SummonsSkill_3(pkt, o);  break;
			}
		}	break;
		case Protocol::MONSTER:
		{
			switch (_skillId)
			{
			case 1015: EnemySkill_1(pkt, o); break;
			case 1016: EnemySkill_2(pkt, o); break;
			case 1017: EnemySkill_3(pkt, o); break;
			case 1018: EnemySkill_4(pkt, o); break;
			case 1019: EnemySkill_5(pkt, o); break;
			case 1020: EnemySkill_6(pkt, o); break;
			case 1021: EnemySkill_7(pkt, o); break;
			}
		} break;
		default:break;
		}
	}

	// ��ų�� �����ϰ� �Ǹ� ��ų������Ʈ�� Ŭ���̾�Ʈ�� ������.
	Protocol::S_STATE statePkt;
	{
		statePkt.set_allocated_objectinfo(new Protocol::ObjectInfo(o->GetObjectInfo()));
		statePkt.set_state(Protocol::StateType::SKILL);
		statePkt.set_skillid(_skillId);

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(statePkt);
		GRoom->DoAsync(&Room::Broadcast, sendBuffer);
	}

}

void YHSkill::Progress() //
{
	_progressTime = ::GetTickCount64();

	GameObjectRef o = _owner.lock();
	if (o)
	{
		switch (o->GetObjectInfo().type())
		{
		case Protocol::PLAYER:
		{
			switch (_skillId)
			{
			case 1001: // ����Ʈ �⺻ ����
			{
			}	break;
			case 1002: // ����Ʈ �ٰŸ� ���� ��ȯ�� ��ȯ
			{
			}	break;
			case 1003: // ����Ʈ ������ ���� Ŀ���� �˱� ����
			{
			}	break;
			case 1004: // ����Ʈĳ���� �߽����� ���� ��ȯ��
			{
			}	break;
			case 1006: // ������ �⺻ ����
			{
			}	break;
			case 1007: // ������ ���Ÿ� ���� ��ȯ��
			{
			}	break;
			case 1008: // ������ ĳ���� �߽����� ���������� ����
			{
			}	break;
			case 1009: // ������ ������ �Ʒ��� �������� �����
			{
			}	break;
			case 1011: // �ų� �⺻ ����
			{
			}	break;
			case 1012: // �ų� ���������ʰ� ���Ÿ� �����ϴ� ��ž ��ȯ
			{
			}	break;
			case 1013: // �ų� ��ä�÷� ������ ����
			{
			}	break;
			case 1014: // �ų� ����� ����
			{
			}	break;
			default:
				break;
			}

		}	break;
		case Protocol::MONSTER:
			break;
		default:
			break;
		}
	}

}

void YHSkill::End()
{
	ResetCoolTime();
}

void YHSkill::KnightSkill_1(Protocol::C_ATTACK pkt)
{
	GameObjectRef o = _owner.lock();
	Protocol::ObjectInfo info = pkt.objectinfo();

	Vector3 currentLocation(info.location().x(), info.location().y(), 100);
	Vector3 forwardVec(info.forwarddir().x() * 150, info.forwarddir().y() * 150, 100);
	Vector3 colLocation = currentLocation + forwardVec;
	Protocol::PBVector col;
	{
		col.set_x(colLocation.x);
		col.set_y(colLocation.y);
		col.set_z(colLocation.z);
	}
	info.set_allocated_location(new Protocol::PBVector( col));

	ProjectileRef p = MakeShared<Projectile>();
	p->Set(o, 17,GRoom->ProvideRoomId(), info);// pkt.objectinfo());

	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::KnightSkill_2(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	// ���� �ٰŸ� �����ϴ� ��ȯ��
	for (int i = 0; i < 1; ++i)
	{
		SummonsRef s = MakeShared<Summons>();
		{
			s->Set(owner, 1,GRoom->ProvideRoomId(), (SummonsLocation)(i + 1), pkt.objectinfo());
			PlayerRef p = static_pointer_cast<Player>(owner);
			p->GetSummons().insert(pair<int, SummonsRef>(_skillId, s));

			// Add Room
			GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(s));
		}
	}
}

void YHSkill::KnightSkill_3(Protocol::C_ATTACK pkt,GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();

	ProjectileRef p = MakeShared<Projectile>();
	p->Set(o, 4,GRoom->ProvideRoomId(), pkt.objectinfo());
	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::KnightSkill_4(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();

	float d = 0;
	for (int i = 0; i < 4; ++i)
	{
		ProjectileRef p = MakeShared<Projectile>();
		p->Set(o, 5,GRoom->ProvideRoomId(), pkt.objectinfo());
		p->SetRadians(YHUtils::Degree2Radian(d));
		d += 90;

		GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));
	}

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());

}

void YHSkill::MageSkill_1(Protocol::C_ATTACK pkt)
{
	GameObjectRef o = _owner.lock();

	ProjectileRef p = MakeShared<Projectile>();
	p->Set(o, 1,GRoom->ProvideRoomId(), pkt.objectinfo());

	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::MageSkill_2(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	SummonsRef s = MakeShared<Summons>();
	s->Set(owner, 2,GRoom->ProvideRoomId(), (SummonsLocation)(1), pkt.objectinfo());
	PlayerRef p = static_pointer_cast<Player>(owner);
	p->GetSummons().insert(pair<int, SummonsRef>(_skillId, s));

	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(s));
}

void YHSkill::MageSkill_3(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();

	float d = 0;
	for (int i = 0; i < 4; ++i)
	{
		ProjectileRef p = MakeShared<Projectile>();
		p->Set(o, 2,GRoom->ProvideRoomId(), pkt.objectinfo());
		p->SetRadians(YHUtils::Degree2Radian(d));
		d += 90;

		GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));
	}

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::MageSkill_4(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();
	
	ProjectileRef p = MakeShared<Projectile>();
	p->Set(o, 3,GRoom->ProvideRoomId(), pkt.objectinfo());
	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
	
}

void YHSkill::GunnerSkill_1(Protocol::C_ATTACK pkt)
{
	GameObjectRef o = _owner.lock();
	ProjectileRef p = MakeShared<Projectile>();
	p->Set(o, 6,GRoom->ProvideRoomId(), pkt.objectinfo());

	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());

}

void YHSkill::GunnerSkill_2(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	// �ų� ���Ÿ� �����ϴ� ��ȯ��
	SummonsRef s = MakeShared<Summons>();
	s->Set(owner, 3,GRoom->ProvideRoomId(), (SummonsLocation)(1), pkt.objectinfo());

	// Add Room
	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(s));
}

void YHSkill::GunnerSkill_3(Protocol::C_ATTACK pkt)
{
	GameObjectRef o = _owner.lock();

	float d = -30;
	for (int i = 0 ;i < 6; ++i)
	{
		ProjectileRef p = MakeShared<Projectile>();
		p->Set(o, 7,GRoom->ProvideRoomId(), pkt.objectinfo());
		p->GetObjectInfo().set_allocated_forwarddir(new Protocol::PBVector(YHUtils::RotateVectorPB(pkt.objectinfo().forwarddir(), d)));
		d += 10;

		// add Room
		GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));
	}

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::GunnerSkill_4(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();

	float d = 0;
	for (int i = 0; i < 10; ++i)
	{
		ProjectileRef p = MakeShared<Projectile>();
		p->Set(o, 8,GRoom->ProvideRoomId(), pkt.objectinfo());
		p->GetObjectInfo().set_allocated_forwarddir(new Protocol::PBVector(YHUtils::RotateVectorPB(pkt.objectinfo().forwarddir(), d)));
		d += 36;

		GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));
	}
	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::SummonsSkill_1(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();
	Protocol::ObjectInfo info = pkt.objectinfo();

	Vector3 currentLocation(info.location().x(), info.location().y(), 100);
	Vector3 forwardVec(info.forwarddir().x() * 150, info.forwarddir().y() * 150, 100);
	Vector3 colLocation = currentLocation + forwardVec;
	Protocol::PBVector col;
	{
		col.set_x(colLocation.x);
		col.set_y(colLocation.y);
		col.set_z(colLocation.z);
	}
	info.set_allocated_location(new Protocol::PBVector(col));

	ProjectileRef p = MakeShared<Projectile>();
	p->Set(o, 18,GRoom->ProvideRoomId(), info);// pkt.objectinfo());

	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::SummonsSkill_2(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();

	ProjectileRef p = MakeShared<Projectile>();
	p->Set(o, 9,GRoom->ProvideRoomId(), pkt.objectinfo());

	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::SummonsSkill_3(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();

	ProjectileRef p = MakeShared<Projectile>();
	p->Set(o, 10,GRoom->ProvideRoomId(), pkt.objectinfo());

	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::EnemySkill_1(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();
	Protocol::ObjectInfo info = pkt.objectinfo();

	Vector3 currentLocation(info.location().x(), info.location().y(), 100);
	Vector3 forwardVec(info.forwarddir().x() * 150, info.forwarddir().y() * 150, 100);
	Vector3 colLocation = currentLocation + forwardVec;
	Protocol::PBVector col;
	{
		col.set_x(colLocation.x);
		col.set_y(colLocation.y);
		col.set_z(colLocation.z);
	}
	info.set_allocated_location(new Protocol::PBVector(col));

	ProjectileRef p = MakeShared<Projectile>();
	p->Set(o, 19,GRoom->ProvideRoomId(), info);// pkt.objectinfo());

	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::EnemySkill_2(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();

	ProjectileRef p = MakeShared<Projectile>();
	p->Set(o, 11,GRoom->ProvideRoomId(), pkt.objectinfo());

	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::EnemySkill_3(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();

	ProjectileRef p = MakeShared<Projectile>();
	p->Set(o, 12,GRoom->ProvideRoomId(), pkt.objectinfo(),_enemy.lock());

	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::EnemySkill_4(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();

	ProjectileRef p = MakeShared<Projectile>();	
	p->Set(o, 13, GRoom->ProvideRoomId(), pkt.objectinfo(), _enemy.lock());

	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::EnemySkill_5(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();

	ProjectileRef p = MakeShared<Projectile>();
	p->Set(o, 14, GRoom->ProvideRoomId(), pkt.objectinfo(), _enemy.lock());

	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::EnemySkill_6(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();

	ProjectileRef p = MakeShared<Projectile>();
	p->Set(o, 15, GRoom->ProvideRoomId(), pkt.objectinfo());

	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}

void YHSkill::EnemySkill_7(Protocol::C_ATTACK pkt, GameObjectRef owner)
{
	GameObjectRef o = _owner.lock();

	ProjectileRef p = MakeShared<Projectile>();
	p->Set(o, 16, GRoom->ProvideRoomId(), pkt.objectinfo());

	GRoom->DoAsync(&Room::Enter, static_pointer_cast<GameObject>(p));

	if (_enemy.expired() == false)
		_owner.lock()->RotatorObject(_enemy.lock()->GetObjectInfo().location());
}
