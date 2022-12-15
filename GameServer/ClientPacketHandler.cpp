#include "pch.h"
#include "ClientPacketHandler.h"
#include "Player.h"
#include "Room.h"
#include "Projectile.h"
#include "GameSession.h"
#include "MetaData.h"
#include "Skill.h"
#include "Character.h"
#include "DataManager.h"

#include "DBConnectionPool.h"
#include "DBBind.h"
#include "XmlParser.h"
#include "DBSynchronizer.h"
#include "GenProcedures.h"

#include <codecvt>
#include <string>

PacketHandlerFunc GPacketHandler[UINT16_MAX];

bool Handle_INVALID(PacketSessionRef& session, s_BYTE* buffer, s_int32 len)
{
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	// TODO : Log
	return true;
}

bool Handle_C_SIGNUP(PacketSessionRef& session, Protocol::C_SIGNUP& pkt)
{
	cout << "Handle_C_SIGNUP" << endl;
	cout << "Create Id :" << pkt.id() << " Password :" << pkt.password() << endl;

	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	// 테이블에 똑같은 계정이 있는지 확인하고 있다면 따로 보내지 말자
	YHUtils::DB_InsertUser(pkt.id(),pkt.password());
	
	return true;
}

bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt)
{
	cout << "Handle_C_LOGIN" << endl;
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	
	bool isSuccess = YHUtils::DB_Login(gameSession,pkt.id(),pkt.password());
	gameSession->SetAccountId(pkt.id());

	Protocol::S_LOGIN loginPkt;
	loginPkt.set_success(isSuccess);
	auto loginBuf = ClientPacketHandler::MakeSendBuffer(loginPkt);
	gameSession->Send(loginBuf);

	return true;
}

bool Handle_C_CREATECHARACTER(PacketSessionRef& session, Protocol::C_CREATECHARACTER& pkt)
{
	cout << "Handle_C_CREATECHARACTER" << endl;

	if (pkt.type() == Protocol::PLAYER_TYPE_KNIGHT)
		cout << "Create Player Type :" << " KNIGHT" << endl;
	else if (pkt.type() == Protocol::PLAYER_TYPE_MAGE)
		cout << "Create Player Type :" << " MAGE" << endl;
	else if (pkt.type() == Protocol::PLAYER_TYPE_GUNNER)
		cout << "Create Player Type :" << " GUNNER" << endl;
	


	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	YHUtils::DB_InsertCharacter(gameSession->GetId(), pkt.type());

	Protocol::S_LOGIN loginPkt;
	loginPkt.set_success(true);

	auto loginBuf = ClientPacketHandler::MakeSendBuffer(loginPkt);
	gameSession->Send(loginBuf);

	return true;
}

bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt)
{
	cout << "Handle_C_ENTER_GAME" << endl;

	// Get Session
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	
	// DB 
	{
		// Set Player
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			DBSynchronizer dbSync(*dbConn);

			SP::GetPlayer getPlayer(*dbConn);

			getPlayer.In_AccountId(gameSession->GetId());

			s_int32 id = 0;
			s_int32 type = 0;
			s_int32 level = 0;
			s_int32 gold = 0;
			s_int32 exp = 0;

			getPlayer.Out_AccountId(OUT id);
			getPlayer.Out_PlayerType(OUT type);
			getPlayer.Out_Level(OUT level);
			getPlayer.Out_Gold(OUT gold);
			getPlayer.Out_Exp(OUT exp);

			getPlayer.Execute();

			if (getPlayer.Fetch())
			{
				//GConsoleLogger->WriteStdOut(Color::BLUE, L"accountId[%d] playerType[%d] level[%d] gold[%d] exp[%d]\n", id, type, level, gold, exp);
				cout << "AccountId : " << id << " Level : " << level << " Gold : " << gold << endl;


				// Get Player & Setting
				PlayerRef playerRef = MakeShared<Player>();
				playerRef->Set(gameSession, GRoom, type, gold, exp, level);
				playerRef->GetObjectInfo().set_name(gameSession->GetAccountId());//gameSession->name);

				//gameSession->_currentPlayer = playerRef;
				gameSession->SetPlayer(playerRef);
			}
			GDBConnectionPool->Push(dbConn);
		}

		// Set Inventory
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			DBSynchronizer dbSync(*dbConn);

			SP::GetInventory getInven(*dbConn);

			getInven.In_AccountId(gameSession->GetId());

			s_int32 id = 0;
			s_int32 idx = 0;
			s_int32 type = 0;
			s_int32 metaId = 0;
			s_int32 count = 0;

			getInven.Out_AccountId(OUT id);
			getInven.Out_Idx(OUT idx);
			getInven.Out_Type(OUT type);
			getInven.Out_MetaId(OUT metaId);
			getInven.Out_Count(OUT count);

			getInven.Execute();

			while (getInven.Fetch())
			{
				gameSession->GetPlayer()->AddItem2Inventory(idx, type, metaId, count);
			}
			GDBConnectionPool->Push(dbConn);
		}

		// Set Equipment
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			DBSynchronizer dbSync(*dbConn);

			SP::GetEquipInstall getPlayer(*dbConn);

			getPlayer.In_AccountId(gameSession->GetId());

			s_int32 id = 0;
			s_int32 idx = 0;
			s_int32 equipId = 0;

			getPlayer.Out_AccountId(OUT id);
			getPlayer.Out_Idx(OUT idx);
			getPlayer.Out_EquipId(OUT equipId);

			getPlayer.Execute();

			while (getPlayer.Fetch())
			{
				gameSession->GetPlayer()->EquipItem(idx, equipId);
			}
			GDBConnectionPool->Push(dbConn);
		}
	}

	Protocol::S_ENTER_GAME enterPkt;
	{
		enterPkt.set_mapid(1);
		
		auto loginBuf = ClientPacketHandler::MakeSendBuffer(enterPkt);
		gameSession->Send(loginBuf);

	}

	GRoom->DoTimer(1000, &Room::Enter, static_pointer_cast<GameObject>(gameSession->GetPlayer()));

	return true;
}

bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt)
{
	
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	PlayerRef player = gameSession->GetPlayer();
	player->GetObjectInfo() = pkt.objectinfo();

	// 현재 플레이어가 이동가능한 상황인지 
	if (player->GetState() == Protocol::IDLE || player->GetState() == Protocol::MOVING)
	{
		player->GetState() = Protocol::MOVING;
		// 타인에게 전송
		Protocol::S_MOVE movePkt;
		{
			Protocol::ObjectInfo* objInfo = new Protocol::ObjectInfo(player->GetObjectInfo());
			movePkt.set_allocated_objectinfo(objInfo);
			movePkt.set_state(player->GetState());

			auto sendBuffer = ClientPacketHandler::MakeSendBuffer(movePkt);
			GRoom->DoAsync(&Room::Broadcast, sendBuffer);
		}
	}

	return true;
}

bool Handle_C_ATTACK(PacketSessionRef& session, Protocol::C_ATTACK& pkt)
{
	//std::cout << "Handle_C_ATTACK" << endl;

	// Get Session
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	// Invoke Skill
	GRoom->DoAsync(&Room::ObjectSkillInvoke,
	static_pointer_cast<GameObject>(gameSession->GetPlayer()),
	GameObjectRef(),
	pkt);

	return true;
}

bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt)
{
	std::cout << "Handle_C_CHAT" << endl;
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	{
		Protocol::S_CHAT chatPkt;
		chatPkt.set_msg(pkt.msg());
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);
		gameSession->Send(sendBuffer);
		//GRoom->DoAsync(&Room::Broadcast, sendBuffer);
	}
	{
		Protocol::S_SPAWN chatPkt;
		auto sendBuffer = ClientPacketHandler::MakeSendBuffer(chatPkt);
		//GRoom->DoAsync(&Room::Broadcast, sendBuffer);
		gameSession->Send(sendBuffer);
	}

	return true;
}

bool Handle_C_GETITEM(PacketSessionRef& session, Protocol::C_GETITEM& pkt)
{
	std::cout << "Handle_C_GETITEM" << endl;

	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	// 룸에서 패킷의 아이디를 가진 아이템을 찾은후
	// 플레이어에게 아이템을 추가하고
	// 얻을 아이템은 
	// 얻은 아이템을 삭제해주는 패킷을 날려주는데 이건 룸에 넘겨서 룸에서 해주기로하자

	GRoom->DoAsync(&Room::GetItem, gameSession->GetPlayer(), pkt);

	return true;
}

bool Handle_C_CLICKNPC(PacketSessionRef& session, Protocol::C_CLICKNPC& pkt)
{
	std::cout << "Handle_C_CLICKNPC" << endl;

	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	GRoom->DoAsync(&Room::ClickNPC, gameSession->GetPlayer(), pkt);

	return true;
}

bool Handle_C_PURCHASE(PacketSessionRef& session, Protocol::C_PURCHASE& pkt)
{
	std::cout << "Handle_C_PURCHASE" << endl;
	cout << "Purchase Item Meta Id: " << pkt.id()<< " Count: " << pkt.cnt() << endl;

	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	GRoom->DoAsync(&Room::PurchaseNPC, gameSession->GetPlayer(), pkt);

	return true;
}

bool Handle_C_SALE(PacketSessionRef& session, Protocol::C_SALE& pkt)
{
	std::cout << "Handle_C_SALE" << endl;
	cout << "Sale Item Idx: " << pkt.idx() << " Count: " << pkt.cnt() << endl;
	
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	GRoom->DoAsync(&Room::SaleNPC, gameSession->GetPlayer(), pkt);

	return true;
}

bool Handle_C_SWAP_ITEM(PacketSessionRef& session, Protocol::C_SWAP_ITEM& pkt)
{
	std::cout << "Handle_C_SWAP_ITEM" << endl;

	cout << "Swap Idx 1 : " << pkt.idx_1() << " Idx 2 :" << pkt.idx_2() << endl;;

	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	GRoom->DoAsync(&Room::SwapItem, gameSession->GetPlayer(), pkt);

	return true;
}

bool Handle_C_USE_ITEM(PacketSessionRef& session, Protocol::C_USE_ITEM& pkt)
{
	std::cout << "Handle_C_USE_ITEM" << endl;
	std::cout << "Use Item Idx : " << pkt.idx() << endl;

	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	GRoom->DoAsync(&Room::UseItem, gameSession->GetPlayer(), pkt);


	return true;
}

bool Handle_C_STAT(PacketSessionRef& session, Protocol::C_STAT& pkt)
{
	std::cout << "Handle_C_STAT" << endl;

	return true;
}

bool Handle_C_RESPAWN(PacketSessionRef& session, Protocol::C_RESPAWN& pkt)
{
	std::cout << "Handle_C_RESPAWN" << endl;

	// Get Session
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);

	// 기존 플레이어 삭제
	GRoom->DoAsync(&Room::Leave, static_pointer_cast<GameObject>(gameSession->GetPlayer()));

	// DB 
	{
		// Set Player
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			DBSynchronizer dbSync(*dbConn);

			SP::GetPlayer getPlayer(*dbConn);

			getPlayer.In_AccountId(gameSession->GetId());

			s_int32 id = 0;
			s_int32 type = 0;
			s_int32 level = 0;
			s_int32 gold = 0;
			s_int32 exp = 0;

			getPlayer.Out_AccountId(OUT id);
			getPlayer.Out_PlayerType(OUT type);
			getPlayer.Out_Level(OUT level);
			getPlayer.Out_Gold(OUT gold);
			getPlayer.Out_Exp(OUT exp);

			getPlayer.Execute();

			if (getPlayer.Fetch())
			{
				//GConsoleLogger->WriteStdOut(Color::BLUE, L"accountId[%d] playerType[%d] level[%d] gold[%d] exp[%d]\n", id, type, level, gold, exp);

				// Get Player & Setting
				PlayerRef playerRef = MakeShared<Player>();
				playerRef->Set(gameSession, GRoom, type, gold, exp, level);

				gameSession->SetPlayer(playerRef);
			}
			GDBConnectionPool->Push(dbConn);
		}

		// Set Inventory
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			DBSynchronizer dbSync(*dbConn);

			SP::GetInventory getInven(*dbConn);

			getInven.In_AccountId(gameSession->GetId());

			s_int32 id = 0;
			s_int32 idx = 0;
			s_int32 type = 0;
			s_int32 metaId = 0;
			s_int32 count = 0;

			getInven.Out_AccountId(OUT id);
			getInven.Out_Idx(OUT idx);
			getInven.Out_Type(OUT type);
			getInven.Out_MetaId(OUT metaId);
			getInven.Out_Count(OUT count);

			getInven.Execute();

			while (getInven.Fetch())
			{
				gameSession->GetPlayer()->AddItem2Inventory(idx, type, metaId, count);
			}
			GDBConnectionPool->Push(dbConn);
		}

		// Set Equipment
		{
			DBConnection* dbConn = GDBConnectionPool->Pop();
			DBSynchronizer dbSync(*dbConn);

			SP::GetEquipInstall getPlayer(*dbConn);

			getPlayer.In_AccountId(gameSession->GetId());

			s_int32 id = 0;
			s_int32 idx = 0;
			s_int32 equipId = 0;

			getPlayer.Out_AccountId(OUT id);
			getPlayer.Out_Idx(OUT idx);
			getPlayer.Out_EquipId(OUT equipId);

			getPlayer.Execute();

			while (getPlayer.Fetch())
			{
				gameSession->GetPlayer()->EquipItem(idx, equipId);
			}
			GDBConnectionPool->Push(dbConn);
		}
	}

	Protocol::S_ENTER_GAME enterPkt;
	{
		enterPkt.set_mapid(1);

		auto loginBuf = ClientPacketHandler::MakeSendBuffer(enterPkt);
		gameSession->Send(loginBuf);

	}

	GRoom->DoTimer(2000, &Room::Enter, static_pointer_cast<GameObject>(gameSession->GetPlayer()));

	return true;
}

bool Handle_C_UNEQUIP(PacketSessionRef& session, Protocol::C_UNEQUIP& pkt)
{
	std::cout << "Handle_C_UNEQUIP" << endl;
	// Get Session
	GameSessionRef gameSession = static_pointer_cast<GameSession>(session);
	GRoom->DoAsync(&Room::UnEquip, gameSession->GetPlayer(), pkt);

	return true;
}
