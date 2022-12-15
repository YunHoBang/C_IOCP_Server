#pragma once
#include "Protocol.pb.h"

using namespace ServerCore;
using PacketHandlerFunc = std::function<bool(PacketSessionRef&, s_BYTE*, s_int32)>;
extern PacketHandlerFunc GPacketHandler[UINT16_MAX];

enum : s_uint16
{
	PKT_S_SERVERCONNECT = 1000,
	PKT_C_SIGNUP = 1001,
	PKT_S_SIGNUP = 1002,
	PKT_C_LOGIN = 1003,
	PKT_S_LOGIN = 1004,
	PKT_C_CREATECHARACTER = 1005,
	PKT_C_ENTER_GAME = 1006,
	PKT_S_ENTER_GAME = 1007,
	PKT_S_SPAWN = 1008,
	PKT_S_LEAVE = 1009,
	PKT_C_MOVE = 1010,
	PKT_S_MOVE = 1011,
	PKT_C_ATTACK = 1012,
	PKT_S_ATTACK = 1013,
	PKT_S_STATE = 1014,
	PKT_C_CHAT = 1015,
	PKT_S_CHAT = 1016,
	PKT_C_GETITEM = 1017,
	PKT_S_GETITEM = 1018,
	PKT_C_CLICKNPC = 1019,
	PKT_S_CLICKNPC = 1020,
	PKT_C_PURCHASE = 1021,
	PKT_S_PURCHASE = 1022,
	PKT_C_SALE = 1023,
	PKT_S_SALE = 1024,
	PKT_C_SWAP_ITEM = 1025,
	PKT_S_SWAP_ITEM = 1026,
	PKT_C_USE_ITEM = 1027,
	PKT_S_USE_ITEM = 1028,
	PKT_S_STAT = 1029,
	PKT_C_STAT = 1030,
	PKT_S_DEAD = 1031,
	PKT_C_RESPAWN = 1032,
	PKT_S_RESPAWN = 1033,
	PKT_C_UNEQUIP = 1034,
	PKT_S_UNEQUIP = 1035,
	PKT_S_EXP = 1036,
};

// Custom Handlers
bool Handle_INVALID(PacketSessionRef& session, s_BYTE* buffer, s_int32 len);
bool Handle_C_SIGNUP(PacketSessionRef& session, Protocol::C_SIGNUP& pkt);
bool Handle_C_LOGIN(PacketSessionRef& session, Protocol::C_LOGIN& pkt);
bool Handle_C_CREATECHARACTER(PacketSessionRef& session, Protocol::C_CREATECHARACTER& pkt);
bool Handle_C_ENTER_GAME(PacketSessionRef& session, Protocol::C_ENTER_GAME& pkt);
bool Handle_C_MOVE(PacketSessionRef& session, Protocol::C_MOVE& pkt);
bool Handle_C_ATTACK(PacketSessionRef& session, Protocol::C_ATTACK& pkt);
bool Handle_C_CHAT(PacketSessionRef& session, Protocol::C_CHAT& pkt);
bool Handle_C_GETITEM(PacketSessionRef& session, Protocol::C_GETITEM& pkt);
bool Handle_C_CLICKNPC(PacketSessionRef& session, Protocol::C_CLICKNPC& pkt);
bool Handle_C_PURCHASE(PacketSessionRef& session, Protocol::C_PURCHASE& pkt);
bool Handle_C_SALE(PacketSessionRef& session, Protocol::C_SALE& pkt);
bool Handle_C_SWAP_ITEM(PacketSessionRef& session, Protocol::C_SWAP_ITEM& pkt);
bool Handle_C_USE_ITEM(PacketSessionRef& session, Protocol::C_USE_ITEM& pkt);
bool Handle_C_STAT(PacketSessionRef& session, Protocol::C_STAT& pkt);
bool Handle_C_RESPAWN(PacketSessionRef& session, Protocol::C_RESPAWN& pkt);
bool Handle_C_UNEQUIP(PacketSessionRef& session, Protocol::C_UNEQUIP& pkt);

class ClientPacketHandler
{
public:
	static void Init()
	{
		for (s_int32 i = 0; i < UINT16_MAX; i++)
			GPacketHandler[i] = Handle_INVALID;
		GPacketHandler[PKT_C_SIGNUP] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_SIGNUP>(Handle_C_SIGNUP, session, buffer, len); };
		GPacketHandler[PKT_C_LOGIN] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_LOGIN>(Handle_C_LOGIN, session, buffer, len); };
		GPacketHandler[PKT_C_CREATECHARACTER] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_CREATECHARACTER>(Handle_C_CREATECHARACTER, session, buffer, len); };
		GPacketHandler[PKT_C_ENTER_GAME] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_ENTER_GAME>(Handle_C_ENTER_GAME, session, buffer, len); };
		GPacketHandler[PKT_C_MOVE] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_MOVE>(Handle_C_MOVE, session, buffer, len); };
		GPacketHandler[PKT_C_ATTACK] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_ATTACK>(Handle_C_ATTACK, session, buffer, len); };
		GPacketHandler[PKT_C_CHAT] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_CHAT>(Handle_C_CHAT, session, buffer, len); };
		GPacketHandler[PKT_C_GETITEM] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_GETITEM>(Handle_C_GETITEM, session, buffer, len); };
		GPacketHandler[PKT_C_CLICKNPC] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_CLICKNPC>(Handle_C_CLICKNPC, session, buffer, len); };
		GPacketHandler[PKT_C_PURCHASE] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_PURCHASE>(Handle_C_PURCHASE, session, buffer, len); };
		GPacketHandler[PKT_C_SALE] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_SALE>(Handle_C_SALE, session, buffer, len); };
		GPacketHandler[PKT_C_SWAP_ITEM] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_SWAP_ITEM>(Handle_C_SWAP_ITEM, session, buffer, len); };
		GPacketHandler[PKT_C_USE_ITEM] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_USE_ITEM>(Handle_C_USE_ITEM, session, buffer, len); };
		GPacketHandler[PKT_C_STAT] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_STAT>(Handle_C_STAT, session, buffer, len); };
		GPacketHandler[PKT_C_RESPAWN] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_RESPAWN>(Handle_C_RESPAWN, session, buffer, len); };
		GPacketHandler[PKT_C_UNEQUIP] = [](PacketSessionRef& session, s_BYTE* buffer, s_int32 len) { return HandlePacket<Protocol::C_UNEQUIP>(Handle_C_UNEQUIP, session, buffer, len); };
	}

	static bool HandlePacket(PacketSessionRef& session, s_BYTE* buffer, s_int32 len)
	{
		PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
		return GPacketHandler[header->id](session, buffer, len);
	}
	static SendBufferRef MakeSendBuffer(Protocol::S_SERVERCONNECT& pkt) { return MakeSendBuffer(pkt, PKT_S_SERVERCONNECT); }
	static SendBufferRef MakeSendBuffer(Protocol::S_SIGNUP& pkt) { return MakeSendBuffer(pkt, PKT_S_SIGNUP); }
	static SendBufferRef MakeSendBuffer(Protocol::S_LOGIN& pkt) { return MakeSendBuffer(pkt, PKT_S_LOGIN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_ENTER_GAME& pkt) { return MakeSendBuffer(pkt, PKT_S_ENTER_GAME); }
	static SendBufferRef MakeSendBuffer(Protocol::S_SPAWN& pkt) { return MakeSendBuffer(pkt, PKT_S_SPAWN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_LEAVE& pkt) { return MakeSendBuffer(pkt, PKT_S_LEAVE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_MOVE& pkt) { return MakeSendBuffer(pkt, PKT_S_MOVE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_ATTACK& pkt) { return MakeSendBuffer(pkt, PKT_S_ATTACK); }
	static SendBufferRef MakeSendBuffer(Protocol::S_STATE& pkt) { return MakeSendBuffer(pkt, PKT_S_STATE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_CHAT& pkt) { return MakeSendBuffer(pkt, PKT_S_CHAT); }
	static SendBufferRef MakeSendBuffer(Protocol::S_GETITEM& pkt) { return MakeSendBuffer(pkt, PKT_S_GETITEM); }
	static SendBufferRef MakeSendBuffer(Protocol::S_CLICKNPC& pkt) { return MakeSendBuffer(pkt, PKT_S_CLICKNPC); }
	static SendBufferRef MakeSendBuffer(Protocol::S_PURCHASE& pkt) { return MakeSendBuffer(pkt, PKT_S_PURCHASE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_SALE& pkt) { return MakeSendBuffer(pkt, PKT_S_SALE); }
	static SendBufferRef MakeSendBuffer(Protocol::S_SWAP_ITEM& pkt) { return MakeSendBuffer(pkt, PKT_S_SWAP_ITEM); }
	static SendBufferRef MakeSendBuffer(Protocol::S_USE_ITEM& pkt) { return MakeSendBuffer(pkt, PKT_S_USE_ITEM); }
	static SendBufferRef MakeSendBuffer(Protocol::S_STAT& pkt) { return MakeSendBuffer(pkt, PKT_S_STAT); }
	static SendBufferRef MakeSendBuffer(Protocol::S_DEAD& pkt) { return MakeSendBuffer(pkt, PKT_S_DEAD); }
	static SendBufferRef MakeSendBuffer(Protocol::S_RESPAWN& pkt) { return MakeSendBuffer(pkt, PKT_S_RESPAWN); }
	static SendBufferRef MakeSendBuffer(Protocol::S_UNEQUIP& pkt) { return MakeSendBuffer(pkt, PKT_S_UNEQUIP); }
	static SendBufferRef MakeSendBuffer(Protocol::S_EXP& pkt) { return MakeSendBuffer(pkt, PKT_S_EXP); }

private:
	template<typename PacketType, typename ProcessFunc>
	static bool HandlePacket(ProcessFunc func, PacketSessionRef& session, s_BYTE* buffer, s_int32 len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

	template<typename T>
	static SendBufferRef MakeSendBuffer(T& pkt, s_uint16 pktId)
	{
		const s_uint16 dataSize = static_cast<s_uint16>(pkt.ByteSizeLong());
		const s_uint16 packetSize = dataSize + sizeof(PacketHeader);

		SendBufferRef sendBuffer = GSendBufferManager->Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		sendBuffer->Close(packetSize);

		return sendBuffer;
	}
};