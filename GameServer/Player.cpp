#include "pch.h"
#include "Player.h"
#include "Projectile.h"
#include "YHSkill.h"
#include "GameSession.h"
#include "Equip.h"
#include "Consum.h"
#include "DataManager.h"

#include "DBConnectionPool.h"
#include "DBBind.h"
#include "XmlParser.h"
#include "DBSynchronizer.h"
#include "GenProcedures.h"

void Player::Set()
{
	int size = _skills.size();

	
	// 인벤토리 크기 세팅
	_items.resize(100);
	for (int i = 0;i < 100;++i)
	{
		_items[i] = ItemSlotRef(new ItemSlot());
		_items[i]->SetIdx(i);
	}
}

void Player::Set(GameSessionRef session, RoomRef room, int type, int gold, int exp, int level)
{
	_level = level;
	_gold = gold;
	_exp = exp;
	_hp = GetMaxHp();
	_mp = GetMaxMp();

	GameObject::SetStat(GetMaxHp(), GetMaxMp(), exp);

	_room = room;
	_ownerSession = session;
	_objInfo.set_roomid(room->ProvideRoomId());
	_objInfo.set_type(Protocol::PLAYER);

	Set();
}

void Player::Update()
{
	GameObject::Update();

	GRoom->DoTimer(shared_from_this(), 50, &GameObject::Update);
}

void Player::State_Idle()
{

}

void Player::State_Moving()
{
	WRITE_LOCK;
	Vector2 v(_objInfo.location().x(), _objInfo.location().y());
	if (_compareLocation == v)
	{
		ChangeState(Protocol::IDLE);
	}
	else
	{
		_compareLocation = v;
	}
}

void Player::State_Skill()
{
}

void Player::State_Dead()
{
	WRITE_LOCK;
	if (_isDead == true)
		return;

	_isDead = true;

	// 스테이트 패킷을 보내고 5초후에 삭제되게끔
	ChangeState(Protocol::DEAD);

	Protocol::S_DEAD deadPkt;
	auto sendBuffer1 = ClientPacketHandler::MakeSendBuffer(deadPkt);
	_ownerSession->Send(sendBuffer1);

	GRoom->DoTimer(10, &Room::Leave, shared_from_this());
}

float Player::GetAtt()
{
	WRITE_LOCK;
	float r = 0.0f;
	
	r+= GameObject::GetAtt();

	for (int i = 0;i< 5;++i)
	{
		if (_equip[i])
		{
			shared_ptr<GameData> d = _equip[i];
			if (d)
			{
				if (d->GetDataType() == Protocol::DATA_TYPE_EQUIP)
				{
					auto equip = static_pointer_cast<Equip>(d);

					Protocol::EquipMetaData equipData = DataManager::EquipMap.find(equip->GetMetaData().id())->second;
					r += equipData.attack();	
				}
			}
		}
	}
	return r;
}

float Player::GetDepend()
{
	WRITE_LOCK;
	float r = 0.0f;

	r += GameObject::GetDepend();

	for (int i = 0; i < _items.size(); ++i)
	{
		if (_items[i])
		{
			shared_ptr<GameData> d = _items[i]->GetData();
			if (d)
			{
				if (d->GetDataType() == Protocol::DATA_TYPE_EQUIP)
				{
					auto equip = static_pointer_cast<Equip>(d);

					Protocol::EquipMetaData equipData = DataManager::EquipMap.find(equip->GetMetaData().id())->second;
					r += equipData.defense();
				}
			}
		}
	}
	return r;
}

int Player::AddItem2Inventory(shared_ptr<GameData> itemdata)
{
	WRITE_LOCK;
	// 벡터를 순회하면서 아이템이 있으면 
	for (int i = 0; i < 25; ++i)
	{
		auto d = _items[i];

		// 유효성 검사
		if (d->IsValid())
		{
			continue;
		}
		else
		{
			// 여기에 추가
			{
				DBConnection* dbConn = GDBConnectionPool->Pop();
				DBSynchronizer dbSync(*dbConn);

				SP::InsertInventory insertInven(*dbConn);

				insertInven.In_AccountId(_ownerSession->GetId());
				insertInven.In_Idx(i);
				insertInven.In_Type(itemdata->GetDataType());

				insertInven.Execute();

				GDBConnectionPool->Push(dbConn);
				
			}

			d->SetData(i, itemdata);
			return i;
		}
	}
	return -1;
}

int Player::AddItem2Inventory(int idx, int type, int metaId, int count)
{
	WRITE_LOCK;
	if (type == Protocol::DATA_TYPE_CONSUM)
	{
		shared_ptr<Consum> d = make_shared<Consum>(metaId);
		_items[idx]->SetData(idx, d);
	}
	else if (type == Protocol::DATA_TYPE_EQUIP)
	{
		shared_ptr<Equip> d = make_shared<Equip>(metaId); 
		_items[idx]->SetData(idx, d);
	}

	return 0;
}

bool Player::SwapInventoryItem(int idx_1, int idx_2)
{
	WRITE_LOCK;
	if (idx_1 == idx_2)
		return false;

	auto s = _items[idx_1];
	auto d = _items[idx_2];

	{
		DBConnection* dbConn = GDBConnectionPool->Pop();
		DBSynchronizer dbSync(*dbConn);

		SP::UpdateInventory insertInven(*dbConn);

		insertInven.In_AccountId(_ownerSession->GetId());
		insertInven.In_Idx(idx_1);
		insertInven.In_Type(d->GetData()->GetDataType());

		insertInven.Execute();

		GDBConnectionPool->Push(dbConn);
	}
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();
		DBSynchronizer dbSync(*dbConn);

		SP::UpdateInventory insertInven(*dbConn);

		insertInven.In_AccountId(_ownerSession->GetId());
		insertInven.In_Idx(idx_2);
		insertInven.In_Type(s->GetData()->GetDataType());

		insertInven.Execute();

		GDBConnectionPool->Push(dbConn);
	}

	return true;
}

void Player::UseItem(int idx)
{
	WRITE_LOCK;
	// 인덱스로 들어온 아이템을 찾은다음에
	ItemSlotRef i =_items[idx];
	if (i->GetData() == nullptr)
		return;

	// 보낼패킷에 필요한 데이터들
	int equipIdx						= -1; // 장비한 배열 인덱스
	Protocol::PBItemData* sendItem		= nullptr;
	Protocol::PBItemData* sendInven		= nullptr;
	Protocol::PBCharacterData* sendChar = nullptr;

	// 아이템의 타입을 확인하자 // 타입에 맞게 다르게한다.
	if (i->GetData()->GetDataType() == Protocol::DATA_TYPE_CONSUM)
	{
		shared_ptr<Consum> c = static_pointer_cast<Consum>(i->GetData());

		HpPlus(c->GetMetaData().hp());
		MpPlus(c->GetMetaData().mp());

		// 카운트를 까준다.
		c->_count -= 1;

		// 인벤토리 아이템의 정보를 바꿔준다.
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			DBSynchronizer dbSync(*dbConn);

			SP::UpdateCountInventory deleteInven(*dbConn);

			deleteInven.In_AccountId(_ownerSession->GetId());
			deleteInven.In_Idx(i->GetIdx());
			deleteInven.In_Count(-1);
			deleteInven.Execute();

			GDBConnectionPool->Push(dbConn);
		}

		// 만약 카운트가 0이하라면 없애준다.
		if (c->_count <= 0)
		{
			{
				DBConnection* dbConn = GDBConnectionPool->Pop();
				DBSynchronizer dbSync(*dbConn);

				SP::DeleteInventory deleteInven(*dbConn);

				deleteInven.In_AccountId(_ownerSession->GetId());
				deleteInven.In_Idx(i->GetIdx());
				deleteInven.Execute();

				GDBConnectionPool->Push(dbConn);
			}
			_items[idx]->SetData(nullptr);
		}

		// Set Char Info
		{
			sendChar = new Protocol::PBCharacterData();
			sendChar->set_id(_objInfo.roomid());	// 현재 룸아이디 

			sendChar->set_hp(_hp);
			sendChar->set_mp(_mp);
			sendChar->set_maxhp(GetMaxHp());
			sendChar->set_maxmp(GetMaxMp());
		}
		// Set Inven
		{
			sendInven = new Protocol::PBItemData();
			sendInven->set_type(Protocol::DATA_TYPE_CONSUM);
			sendInven->set_allocated_consumdata(new Protocol::ConsumMetaData(c->GetMetaData()));
			sendInven->set_count(c->_count);
		}
	}
	else if (i->GetData()->GetDataType() == Protocol::DATA_TYPE_EQUIP)
	{
		auto e = static_pointer_cast<Equip>(i->GetData());

		// 장비 테이블에 추가해준다.
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			DBSynchronizer dbSync(*dbConn);

			SP::InsertEquipInstall deleteInven(*dbConn);

			deleteInven.In_AccountId(_ownerSession->GetId());
			deleteInven.In_Idx(e->GetMetaData().equiptype());
			deleteInven.Execute();

			GDBConnectionPool->Push(dbConn);
		}
		// 아이템을 제거해준다.
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			DBSynchronizer dbSync(*dbConn);

			SP::DeleteInventory deleteInven(*dbConn);

			deleteInven.In_AccountId(_ownerSession->GetId());
			deleteInven.In_Idx(i->GetIdx());
			deleteInven.Execute();

			GDBConnectionPool->Push(dbConn);
		}

		_equip[e->GetMetaData().equiptype()] = e;
		equipIdx = e->GetMetaData().equiptype();

		// 해당 아이템의 값을 빼주자.
		_items[idx]->SetData(nullptr);

		// Set Pkt Info
		{
			sendItem = new Protocol::PBItemData();
			sendItem->set_type(Protocol::DATA_TYPE_EQUIP);
			sendItem->set_allocated_equipdata(new Protocol::EquipMetaData(e->GetMetaData()));
			sendItem->set_upgrade(e->_upgrade);
		}
	}
	else
	{
		cout << "item Type Falild" << endl;
		return;
	}

	// Send Pkt
	{
		Protocol::S_USE_ITEM usePkt;
		{
			usePkt.set_id(_objInfo.roomid());
			usePkt.set_allocated_equipdata(sendItem);
			usePkt.set_invenidx(idx);
			usePkt.set_allocated_invendata(sendInven);
			usePkt.set_allocated_chardata(sendChar);
		}
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(usePkt);
		_room.lock()->Broadcast(sendBuffer);
	}
}

void Player::UnEquip(int idx)
{
	WRITE_LOCK;
	// 장비슬롯에서 빼내기전에 인벤토리에 추가해준다.
	shared_ptr<GameData> g = _equip[idx];
	if (!g) return;

	// 기존 장비슬롯에서 내려준다.
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();
		DBSynchronizer dbSync(*dbConn);

		SP::DeleteEquipInstall deleteInven(*dbConn);

		deleteInven.In_AccountId(_ownerSession->GetId());
		deleteInven.In_Idx(idx);
		deleteInven.Execute();

		GDBConnectionPool->Push(dbConn);
	}
	_equip[idx] = nullptr;

	// 아이템데이터를 인벤토리에 추가
	int idx2 = AddItem2Inventory(g);
	if (idx2 == -1) { return; }

	Protocol::S_UNEQUIP pkt;
	{
		pkt.set_id(_objInfo.roomid());
		pkt.set_idx(idx);
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
		_room.lock()->Broadcast(sendBuffer);
	}

	// 획득한 아이템의 정보를 클라이언트로 보내준다.
	Protocol::S_GETITEM getPkt;
	{
		Protocol::PBItemData* itemInfo = new Protocol::PBItemData();
		{
			itemInfo->set_type(g->GetDataType());
			// 타입확인
			switch (g->GetDataType())
			{
			case Protocol::DataType::DATA_TYPE_CONSUM:
			{
				shared_ptr<Consum> d = static_pointer_cast<Consum>(g);
				itemInfo->set_count(d->_count);
				itemInfo->set_allocated_consumdata(new Protocol::ConsumMetaData(d->GetMetaData()));

			}	break;
			case Protocol::DataType::DATA_TYPE_EQUIP:
			{
				shared_ptr<Equip> d = static_pointer_cast<Equip>(g);
				itemInfo->set_upgrade(d->_upgrade);
				itemInfo->unsafe_arena_set_allocated_equipdata(new Protocol::EquipMetaData(d->GetMetaData()));

			}	break;
			default: break;
			}
		}
		getPkt.set_allocated_data(itemInfo);
		getPkt.set_idx(idx2);

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(getPkt);
		GRoom->DoAsync(&Room::Broadcast, sendBuffer);
	}
}

void Player::EquipItem(int idx, int equipId)
{
	WRITE_LOCK;
	shared_ptr<Equip> e = make_shared<Equip>(equipId); 
	_equip[idx] = e;
}
