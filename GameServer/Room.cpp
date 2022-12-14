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
	// id俊 嘎绰 单捞磐甫 技泼茄促.
	_map = GMapManager->GetMapData(0);

	// npc 积己
	for (int i = 0; i < 1; ++i)
	{
		// 利 积己
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

	// 阁胶磐 积己
	for (int i = 0; i < 1; ++i)
	{
		// 利 积己
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

		// 浚磐菩哦阑 父甸绢 夯牢俊霸 焊辰促.
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

				// 牢亥配府 父怒 鉴雀秦辑 技泼窍磊
				for (auto i : c->GetItem())
				{
					if (i->GetData())
					{
						Protocol::PBItemData* itemInfo = pkt_enter.add_inven();
						YHUtils::PB_SetItemData(itemInfo, i->GetData(), i->GetIdx());
					}
				}

				// 厘厚父怒 鉴雀窍磊
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


		// 贸澜 捞磐饭捞磐甫 罐绰促
		auto i = _objects.begin();
		

		// 场鳖瘤 捣促.
		while (i != _objects.end())
		{
			// 菩哦阑 急攫秦霖促.
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

	// 诀单捞飘 龋免
	go->Update();

	// 胶迄 菩哦 宏肺靛某胶飘
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
		// 磷篮父怒 促矫父甸绢林磊
		// 利 积己
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
			// 棱促茄芭 技泼
			item->GetObjectInfo().set_roomid(++idGenerator);
			item->GetObjectInfo().set_type(Protocol::ITEM);
			//item->_map = _map;
			item->SetMap(_map);


			// 困摹 技泼
			Protocol::PBVector* location = new Protocol::PBVector(go->GetObjectInfo().location());
			location->set_z(10);
			item->GetObjectInfo().set_allocated_location(location);

			// 酒捞袍 单捞磐 技泼
			ran_1 = rand() % 2;
			if (ran_1 == 0)
			{
				ran_2 = (rand() % 10) + 1001;
				shared_ptr<Equip> e = make_shared<Equip>(99); // 烙矫
			//	e->_data = DataManager::EquipMap.find(ran_2)->second;
				item->SetItemData(e);
			}
			else
			{
				ran_2 = (rand() % 4) + 1001;
				shared_ptr<Consum> e = make_shared<Consum>(99); // 烙矫
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

		// 酒捞袍单捞磐甫 牢亥配府俊 眠啊
		int idx = player->AddItem2Inventory(i->GetItemData());
		if (idx == -1) { return; }

		// 裙垫茄 酒捞袍狼 沥焊甫 努扼捞攫飘肺 焊郴霖促.
		Protocol::S_GETITEM getPkt;
		{
			Protocol::PBItemData* itemInfo = new Protocol::PBItemData();
			{
				itemInfo->set_type(i->GetItemData()->GetDataType());
				// 鸥涝犬牢
				switch (i->GetItemData()->GetDataType())
				{
				case Protocol::DataType::DATA_TYPE_CONSUM:
				{
					// 屈函券
					shared_ptr<Consum> d = static_pointer_cast<Consum>(i->GetItemData());

					// 鞘夸茄单捞磐甫 持磊
					itemInfo->set_count(d->_count);
			//		itemInfo->set_allocated_consumdata(new Protocol::ConsumMetaData(d->_data));
					
				}	break;
				case Protocol::DataType::DATA_TYPE_EQUIP:
				{
					// 屈函券
					shared_ptr<Equip> d = static_pointer_cast<Equip>(i->GetItemData());
					// 鞘夸茄单捞磐甫 持磊
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

		// 裙垫茄 酒捞袍阑 秦寸甘俊辑 昏力秦霖促.
		GRoom->DoAsync(&Room::Leave, static_pointer_cast<GameObject>(i));

	}
	else
	{
		//cout << "Not Find" << endl;
	}
}

void Room::ClickNPC(PlayerRef player, Protocol::C_CLICKNPC pkt)
{
	// id俊 嘎绰 npc甫 茫篮促澜
	// 弊 npc啊 啊瘤绊乐绰 酒捞袍 id甸阑 焊郴霖促.
	if (_objects.find(pkt.id()) != _objects.end())
	{
		cout << "find" << endl;
		auto iter = _objects.find(pkt.id());

		// NPC甫 茫篮促澜 屈函券 秦霖促.
		NPCRef i = static_pointer_cast<NPC>(iter->second);

		// npc啊 啊柳 皋鸥单捞磐狼 单捞磐甫 焊郴霖促.
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
	// id俊 嘎绰 酒捞袍阑 茫绰促.
	auto f = DataManager::ConsumMap.find(pkt.id());

	// 单捞磐甫 技泼茄促.
	shared_ptr<Consum> c = MakeShared<Consum>(12); // 烙矫
	c->Set(pkt.cnt(), f->second);

//	if (player->GetGold() < c->_data.price()) return;

	// 敲饭捞绢俊霸 单捞磐甫 眠啊茄促.
	int idx = player->AddItem2Inventory(static_pointer_cast<GameData>(c));
	if (idx == -1) { return; }

	int p =0; //-(int)(c->_data.price() * pkt.cnt());
	// 敲饭捞绢俊霸 捣阑 眠啊秦霖促.
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


	// 敲饭捞绢俊霸 眠啊茄 单捞磐甫 焊郴霖促.
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
			// 茫篮芭绊
			f = player->GetItem()[i];
			break;
		}
	}

	Protocol::PBItemData* sendData = nullptr;

	auto d = f->GetData();
	if (d->GetDataType() == Protocol::DATA_TYPE_EQUIP)
	{
		auto ee = static_pointer_cast<Equip>(d);

		// 敲饭捞绢俊霸 捣阑 眠啊秦霖促.
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			DBSynchronizer dbSync(*dbConn);

			SP::UpdatePlayerGold updatePlayerGold(*dbConn);

			updatePlayerGold.In_AccountId(player->GetSession()->GetId());
		//	updatePlayerGold.In_Gold(ee->_data.price());
			updatePlayerGold.Execute();

			GDBConnectionPool->Push(dbConn);
		}
		// 酒捞袍阑 力芭秦霖促.
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
			// 敲饭捞绢俊霸 捣阑 眠啊秦霖促.
			{
				DBConnection* dbConn = GDBConnectionPool->Pop();
				DBSynchronizer dbSync(*dbConn);

				SP::UpdatePlayerGold updatePlayerGold(*dbConn);

				updatePlayerGold.In_AccountId(player->GetSession()->GetId());
			//	updatePlayerGold.In_Gold(e->_data.price() * pkt.cnt());
				updatePlayerGold.Execute();

				GDBConnectionPool->Push(dbConn);
			}
			// 牢亥配府 酒捞袍狼 沥焊甫 官层霖促.
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

			// 俺荐甫 临咯林绊 捣阑 棵妨林磊 
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

	// Player俊霸焊辰促.
	auto enterBuf = ClientPacketHandler::MakeSendBuffer(salePkt);
	player->GetSession()->Send(enterBuf);
		
	
}

void Room::SwapItem(PlayerRef player, Protocol::C_SWAP_ITEM pkt)
{
	// 父距 胶恳窍绰单 角菩沁促搁 菩哦阑 焊郴瘤臼绰促.
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