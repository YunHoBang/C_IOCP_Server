#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "Projectile.h"
#include "GameObject.h"
#include "GameSession.h"
#include "Enemy.h"
#include "Protocol.pb.h"
#include "math.h"
#include "Struct.pb.h"
#include "ClientPacketHandler.h"
#include "Managers.h"
#include "NPC.h"
#include "Item.h"
#include "Summons.h"

#include "DBConnectionPool.h"
#include "DBBind.h"
#include "XmlParser.h"
#include "DBSynchronizer.h"
#include "GenProcedures.h"

shared_ptr<Room> GRoom = make_shared<Room>();

void Room::Init()
{
	// id에 맞는 데이터를 세팅한다.
	_map = GMapManager->GetMapData(0);

	// npc 생성
	for (int i = 0; i < 1; ++i)
	{
		// 적 생성
		NPCRef npc = MakeShared<NPC>();
		{
			npc->GetObjectInfo().set_roomid(++idGenerator);
			npc->GetObjectInfo().set_type(Protocol::NPC);

			//	enemy->_objInfo.set_posx(296);
			//	enemy->_objInfo.set_posy(-367);
			//	enemy->_objInfo.set_posz(100);
			Protocol::PBVector* rotator = new Protocol::PBVector();
			Protocol::PBVector* location = new Protocol::PBVector();

			Protocol::PBVector createPos = _map.CreateAbleRandomPos();

			rotator->set_z(-141);

			location->set_x(-2030);
			location->set_y(-3990);
			location->set_z(createPos.z() + 50);

			npc->GetObjectInfo().set_allocated_rotator(rotator);
			npc->GetObjectInfo().set_allocated_location(location);
			//npc->_map = _map;
			npc->SetMap(_map);



			npc->_data = DataManager::NPCMap.find(1)->second;
			Enter(npc);
		}
	}

	// 몬스터 생성
	for (int i = 0; i < 1; ++i)
	{
		// 적 생성
		EnemyRef enemy = MakeShared<Enemy>();
		{
		
			Protocol::PBVector createPos = _map.CreateAbleRandomPos();
			Protocol::PBVector* location = new Protocol::PBVector();
			//location->set_x(-33);
			//location->set_y(-274);
			//location->set_z(createPos.z() + 50);
			//location->set_x(createPos.x());
			//location->set_y(createPos.y());
			//location->set_z(createPos.z() + 50);
			location->set_x(3010);
			location->set_y(800);
			location->set_z(createPos.z() + 50);

			int ran_3 = (rand() % 7 +1);

			enemy->Set(++idGenerator, 1, *location);

			Enter(enemy);
		}
	}

	Update();
}

void Room::Enter(GameObjectRef go)
{
	Protocol::ObjectInfo* info = &go->GetObjectInfo();

	switch (go->GetObjectInfo().type())
	{
	case Protocol::PLAYER:
	{
		auto c = static_pointer_cast<Player>(go);
		_players[go->GetObjectInfo().roomid()] = static_pointer_cast<Player>(go);

		// 엔터패킷을 만들어 본인에게 보낸다.
		Protocol::S_SPAWN pkt_enter;
		{
			pkt_enter.set_ismy(true);

			Protocol::ObjectInfo* addInfo = pkt_enter.add_objectinfos();
			*addInfo = *info;

			{
				Protocol::PBCharacterData* s = new Protocol::PBCharacterData();
				{
					s->set_id(c->GetObjectInfo().roomid());
					s->set_level(c->GetLevel());
					s->set_hp(c->GetMaxHp());
					s->set_mp(c->GetMaxMp());
					s->set_maxhp(c->GetMaxHp());
					s->set_maxmp(c->GetMaxMp());
					s->set_exp(c->GetCurrentExp());;
					s->set_maxexp(c->GetMaxExp());
					s->set_gold(c->GetGold());
				//	s->set_allocated_charmeta(new Protocol::CharacterMetaData(c->_info->_data));

					//for (auto tmp : c->_info->_skills)
					//{
					//	Protocol::PBSkillData* addSkill = s->add_skills();
					//	{
					//		//addSkill->set_id(tmp->_id);
					//		addSkill->set_upgrade(tmp->_upgrade);
					//		//addSkill->set_allocated_skillmeta(new Protocol::SkillMetaData(tmp->_data));
					//	}
					//}
				}
				pkt_enter.set_allocated_characterinfo(s);

				// 인벤토리 만큼 순회해서 세팅하자
				for (auto i : c->GetItem())
				{
					if (i->GetData())
					{
						Protocol::PBItemData* itemInfo = pkt_enter.add_inven();
						YHUtils::PB_SetItemData(itemInfo, i->GetData(), i->GetIdx());
					}
				}

				// 장비만큼 순회하자
				for (int i = 0; i < 5; ++i)
				{
					auto e = c->GetEquip()[i];
					if (e)
					{
						Protocol::PBItemData* itemInfo = pkt_enter.add_equip();
						YHUtils::PB_SetItemData(itemInfo, e, i);
					}
				}

			}
			auto enterBuf = ClientPacketHandler::MakeSendBuffer(pkt_enter);
			go->GetSession()->Send(enterBuf);
		}


		// 처음 이터레이터를 받는다
		auto i = _objects.begin();
		

		// 끝까지 돈다.
		while (i != _objects.end())
		{
			// 패킷을 선언해준다.
			Protocol::S_SPAWN pkt_spawn;
			{
				int cnt = 0;

				pkt_enter.set_ismy(false);
				while (i != _objects.end() && cnt < 100)
				{	
					Protocol::ObjectInfo* addInfo = pkt_spawn.add_objectinfos();
					*addInfo = i->second->GetObjectInfo();

					++cnt;
					++i;
				}
				cnt = 0;
			}
			auto spawnBuf = ClientPacketHandler::MakeSendBuffer(pkt_spawn);
			go->GetSession()->Send(spawnBuf);
		}

		/*
		Protocol::S_SPAWN pkt_spawn;
		{
			pkt_enter.set_ismy(false);
			for (auto pair : _objects)
			{
				Protocol::ObjectInfo* addInfo = pkt_spawn.add_objectinfos();
				*addInfo = pair.second->_objInfo;
			}
		}

		auto spawnBuf = ClientPacketHandler::MakeSendBuffer(pkt_spawn);
		go->_ownerSession->Send(spawnBuf);
		*/

	}
	break;
	case Protocol::MONSTER:
		_monsters[go->GetObjectInfo().roomid()] = static_pointer_cast<Enemy>(go); break;
	case Protocol::PROJECTILE:
		_projectils[go->GetObjectInfo().roomid()] = static_pointer_cast<Projectile>(go); break;
	case Protocol::ITEM:
		_items[go->GetObjectInfo().roomid()] = static_pointer_cast<Item>(go); break;
	case Protocol::NPC: break;
	case Protocol::SUMMONS:
		_summons[go->GetObjectInfo().roomid()] = static_pointer_cast<Summons>(go); break;
	default:break;
	}
	_objects[go->GetObjectInfo().roomid()] = go;

	// 업데이트 호출
	go->Update();

	// 스폰 패킷 브로드캐스트
	Protocol::S_SPAWN pkt_spawn;
	{
		Protocol::ObjectInfo* pktInfo = pkt_spawn.add_objectinfos();
		*pktInfo = *info;

		auto spawnBuf = ClientPacketHandler::MakeSendBuffer(pkt_spawn);
		GRoom->DoAsync(&Room::Broadcast, spawnBuf);
	}

	return;
}

int _testint = 1;
void Room::Leave(GameObjectRef go)
{
	switch (go->GetObjectInfo().type())
	{
	case Protocol::PLAYER:_players.erase(go->GetObjectInfo().roomid()); break;
	case Protocol::MONSTER:
	{
		_monsters.erase(go->GetObjectInfo().roomid());
		// 죽은만큼 다시만들어주자
		// 적 생성
		EnemyRef enemy = MakeShared<Enemy>();
		{
			Protocol::PBVector createPos = _map.CreateAbleRandomPos();
			Protocol::PBVector* location = new Protocol::PBVector();
			/*location->set_x(-33);
			location->set_y(-274);
			location->set_z(createPos.z() + 50);*/
			/*location->set_x(createPos.x());
			location->set_y(createPos.y());
			location->set_z(createPos.z() + 50);*/

			location->set_x(3010);
			location->set_y(800);
			location->set_z(createPos.z() + 50);
			enemy->Set(++idGenerator, ++_testint, *location);

			Enter(enemy);
		}
	}
	
	break;
	case Protocol::PROJECTILE:_projectils.erase(go->GetObjectInfo().roomid()); break;
	case Protocol::ITEM:_items.erase(go->GetObjectInfo().roomid()); break;
	case Protocol::SUMMONS: _summons.erase(go->GetObjectInfo().roomid()); break;
	default:break;
	}
	_objects.erase(go->GetObjectInfo().roomid());

	Protocol::S_LEAVE leavePkt;
	{
		Protocol::ObjectInfo* objInfo = new Protocol::ObjectInfo(go->GetObjectInfo());
		leavePkt.set_allocated_objectinfo(objInfo);
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(leavePkt);
		GRoom->DoAsync(&Room::Broadcast, sendBuffer);
	}
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	for (auto& p : _players)
	{
		p.second->GetSession()->Send(sendBuffer);
	}
}

void Room::ObjectSkillInvoke(GameObjectRef go, GameObjectRef target, Protocol::C_ATTACK pkt)
{
	go->Invoke_Skill(pkt, target);
}

void Room::CreateRewardObject(GameObjectRef go)
{

	int ran_1 = 0;
	int ran_2 = 0;
	int ran_3 = (rand() % 4);

	for (int i = 0; i < 10; ++i)
	{
		ItemRef item = MakeShared<Item>();
		{
			// 잡다한거 세팅
			item->GetObjectInfo().set_roomid(++idGenerator);
			item->GetObjectInfo().set_type(Protocol::ITEM);
			//item->_map = _map;
			item->SetMap(_map);


			// 위치 세팅
			Protocol::PBVector* location = new Protocol::PBVector(go->GetObjectInfo().location());
			location->set_z(10);
			item->GetObjectInfo().set_allocated_location(location);

			// 아이템 데이터 세팅
			ran_1 = rand() % 2;
			if (ran_1 == 0)
			{
				ran_2 = (rand() % 10) + 1001;
				shared_ptr<Equip> e = make_shared<Equip>(99); // 임시
			//	e->_data = DataManager::EquipMap.find(ran_2)->second;
				item->SetItemData(e);
			}
			else
			{
				ran_2 = (rand() % 4) + 1001;
				shared_ptr<Consum> e = make_shared<Consum>(99); // 임시
				//e->_data = DataManager::ConsumMap.find(ran_2)->second;
				e->Set((rand() % 4)+1,DataManager::ConsumMap.find(ran_2)->second);
				item->SetItemData(e);
			}

			item->Set();
			Enter(item);
		}
	}
}

void Room::GetItem(PlayerRef player, Protocol::C_GETITEM pkt)
{
	if (_items.find(pkt.id()) != _items.end())
	{
		//cout << "find" << endl;
		auto iter = _items.find(pkt.id());

		ItemRef i = iter->second;

		// 아이템데이터를 인벤토리에 추가
		int idx = player->AddItem2Inventory(i->GetItemData());
		if (idx == -1) { return; }

		// 획득한 아이템의 정보를 클라이언트로 보내준다.
		Protocol::S_GETITEM getPkt;
		{
			Protocol::PBItemData* itemInfo = new Protocol::PBItemData();
			{
				itemInfo->set_type(i->GetItemData()->GetDataType());
				// 타입확인
				switch (i->GetItemData()->GetDataType())
				{
				case Protocol::DataType::DATA_TYPE_CONSUM:
				{
					// 형변환
					shared_ptr<Consum> d = static_pointer_cast<Consum>(i->GetItemData());

					// 필요한데이터를 넣자
					itemInfo->set_count(d->_count);
			//		itemInfo->set_allocated_consumdata(new Protocol::ConsumMetaData(d->_data));
					
				}	break;
				case Protocol::DataType::DATA_TYPE_EQUIP:
				{
					// 형변환
					shared_ptr<Equip> d = static_pointer_cast<Equip>(i->GetItemData());
					// 필요한데이터를 넣자
					//itemInfo->set_count(d->_count);
					itemInfo->set_upgrade(d->_upgrade);
		//			itemInfo->unsafe_arena_set_allocated_equipdata(new Protocol::EquipMetaData(d->_data));

				}	break;

				default: break;
				}
			}

			getPkt.set_allocated_data(itemInfo);
			getPkt.set_idx(idx);

			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(getPkt);
			GRoom->DoAsync(&Room::Broadcast, sendBuffer);
		}

		// 획득한 아이템을 해당맵에서 삭제해준다.
		GRoom->DoAsync(&Room::Leave, static_pointer_cast<GameObject>(i));

	}
	else
	{
		//cout << "Not Find" << endl;
	}
}

void Room::ClickNPC(PlayerRef player, Protocol::C_CLICKNPC pkt)
{
	// id에 맞는 npc를 찾은다음
	// 그 npc가 가지고있는 아이템 id들을 보내준다.
	if (_objects.find(pkt.id()) != _objects.end())
	{
		cout << "find" << endl;
		auto iter = _objects.find(pkt.id());

		// NPC를 찾은다음 형변환 해준다.
		NPCRef i = static_pointer_cast<NPC>(iter->second);

		// npc가 가진 메타데이터의 데이터를 보내준다.
		Protocol::S_CLICKNPC pkt;
		{
			for (int j = 0; j < i->_data.ids_size(); j++) 
			{
				pkt.add_ids(i->_data.ids(j));
			}

			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(pkt);
			player->GetSession()->Send(sendBuffer);
		}
	}
	else
	{
		cout << "Not Find" << endl;
	}
}

void Room::PurchaseNPC(PlayerRef player, Protocol::C_PURCHASE pkt)
{
	// id에 맞는 아이템을 찾는다.
	auto f = DataManager::ConsumMap.find(pkt.id());

	// 데이터를 세팅한다.
	shared_ptr<Consum> c = MakeShared<Consum>(12); // 임시
	c->Set(pkt.cnt(), f->second);

//	if (player->GetGold() < c->_data.price()) return;

	// 플레이어에게 데이터를 추가한다.
	int idx = player->AddItem2Inventory(static_pointer_cast<GameData>(c));
	if (idx == -1) { return; }

	int p =0; //-(int)(c->_data.price() * pkt.cnt());
	// 플레이어에게 돈을 추가해준다.
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();
		DBSynchronizer dbSync(*dbConn);

		SP::UpdatePlayerGold updatePlayerGold(*dbConn);

		updatePlayerGold.In_AccountId(player->GetSession()->GetId());
		updatePlayerGold.In_Gold(p);
		updatePlayerGold.Execute();

		GDBConnectionPool->Push(dbConn);
	}

	//player->MinGold(c->_data.price() * pkt.cnt());


	// 플레이어에게 추가한 데이터를 보내준다.
	Protocol::S_PURCHASE getPkt;
	{
		Protocol::PBItemData* itemInfo = new Protocol::PBItemData();
		{
			itemInfo->set_type(c->GetDataType());
			itemInfo->set_count(c->_count);
			//itemInfo->set_allocated_consumdata(new Protocol::ConsumMetaData(c->_data));
		}
		getPkt.set_allocated_data(itemInfo);
		getPkt.set_idx(idx);
		getPkt.set_gold(player->GetGold());

		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(getPkt);
		player->GetSession()->Send(sendBuffer);
	}
}

void Room::SaleNPC(PlayerRef player, Protocol::C_SALE pkt)
{
	ItemSlotRef f = nullptr;

	for (size_t i = 0; i < player->GetItem().size(); i++)
	{
		if (player->GetItem()[i]->GetIdx() == pkt.idx())
		{
			// 찾은거고
			f = player->GetItem()[i];
			break;
		}
	}

	Protocol::PBItemData* sendData = nullptr;

	auto d = f->GetData();
	if (d->GetDataType() == Protocol::DATA_TYPE_EQUIP)
	{
		auto ee = static_pointer_cast<Equip>(d);

		// 플레이어에게 돈을 추가해준다.
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			DBSynchronizer dbSync(*dbConn);

			SP::UpdatePlayerGold updatePlayerGold(*dbConn);

			updatePlayerGold.In_AccountId(player->GetSession()->GetId());
		//	updatePlayerGold.In_Gold(ee->_data.price());
			updatePlayerGold.Execute();

			GDBConnectionPool->Push(dbConn);
		}
		// 아이템을 제거해준다.
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			DBSynchronizer dbSync(*dbConn);

			SP::DeleteInventory deleteInven(*dbConn);

			deleteInven.In_AccountId(player->GetSession()->GetId());
			deleteInven.In_Idx(f->GetIdx());
			deleteInven.Execute();

			GDBConnectionPool->Push(dbConn);
		}

		//player->_gold += ee->_data.price();
	//	player->PlusGold(ee->_data.price());


		f->SetData(nullptr);

		sendData = new Protocol::PBItemData();
		{
			sendData->set_type(Protocol::DATA_TYPE_EQUIP);
		}
	}
	else if (d->GetDataType() == Protocol::DATA_TYPE_CONSUM)
	{
		auto e = static_pointer_cast<Consum>(d);

		if (e->_count >= pkt.cnt())
		{
			// 플레이어에게 돈을 추가해준다.
			{
				DBConnection* dbConn = GDBConnectionPool->Pop();
				DBSynchronizer dbSync(*dbConn);

				SP::UpdatePlayerGold updatePlayerGold(*dbConn);

				updatePlayerGold.In_AccountId(player->GetSession()->GetId());
			//	updatePlayerGold.In_Gold(e->_data.price() * pkt.cnt());
				updatePlayerGold.Execute();

				GDBConnectionPool->Push(dbConn);
			}
			// 인벤토리 아이템의 정보를 바꿔준다.
			{
				DBConnection* dbConn = GDBConnectionPool->Pop();
				DBSynchronizer dbSync(*dbConn);

				SP::UpdateCountInventory deleteInven(*dbConn);

				deleteInven.In_AccountId(player->GetSession()->GetId());
				deleteInven.In_Idx(f->GetIdx());
				deleteInven.In_Count((-(int)pkt.cnt()));
				deleteInven.Execute();

				GDBConnectionPool->Push(dbConn);
			}

			// 개수를 줄여주고 돈을 올려주자 
			e->_count -= pkt.cnt();
		//	player->_gold += e->_data.price() * pkt.cnt();
			//player->PlusGold(e->_data.price() * pkt.cnt());

	}
		else return;

		if (e->_count <= 0)
		{
			{
				DBConnection* dbConn = GDBConnectionPool->Pop();
				DBSynchronizer dbSync(*dbConn);

				SP::DeleteInventory deleteInven(*dbConn);

				deleteInven.In_AccountId(player->GetSession()->GetId());
				deleteInven.In_Idx(f->GetIdx());
				deleteInven.Execute();

				GDBConnectionPool->Push(dbConn);
			}

			f->SetData(nullptr);
		}
		

		
		sendData = new Protocol::PBItemData();
		{
			sendData->set_type( Protocol::DATA_TYPE_CONSUM);
		//	sendData->set_allocated_consumdata(new Protocol::ConsumMetaData(e->_data));
			sendData->set_count(e->_count);
		}
	}

	Protocol::S_SALE salePkt;
	{
		salePkt.set_gold(player->GetGold());
		salePkt.set_idx(pkt.idx());
		salePkt.set_allocated_data(sendData);
	}

	// Player에게보낸다.
	auto enterBuf = ClientPacketHandler::MakeSendBuffer(salePkt);
	player->GetSession()->Send(enterBuf);
		
	
}

void Room::SwapItem(PlayerRef player, Protocol::C_SWAP_ITEM pkt)
{
	// 만약 스왑하는데 실패했다면 패킷을 보내지않는다.
	if (player->SwapInventoryItem(pkt.idx_1(), pkt.idx_2()))
	{
		Protocol::S_SWAP_ITEM swapPkt;
		{
			swapPkt.set_idx_1(pkt.idx_1());
			swapPkt.set_idx_2(pkt.idx_2());
			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(swapPkt);
			player->GetSession()->Send(sendBuffer);
		}
	}
}

void Room::UseItem(PlayerRef player, Protocol::C_USE_ITEM pkt)
{
	player->UseItem(pkt.idx());
}

void Room::UnEquip(PlayerRef player, Protocol::C_UNEQUIP pkt)
{

	player->UnEquip(pkt.idx());
}

void Room::Update()
{
	DoTimer(200, &Room::Update);
}