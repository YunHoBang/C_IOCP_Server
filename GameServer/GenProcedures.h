#pragma once
#include "Types.h"
#include <windows.h>
#include "DBBind.h"

namespace SP
{
	
    class InsertUser : public DBBind<2,0>
    {
    public:
    	InsertUser(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spInsertUser(?,?)}") { }
    	template<ServerCore::s_int32 N> void In_LoginId(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<ServerCore::s_int32 N> void In_LoginId(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void In_LoginId(WCHAR* v, ServerCore::s_int32 count) { BindParam(0, v, count); };
    	void In_LoginId(const WCHAR* v, ServerCore::s_int32 count) { BindParam(0, v, count); };
    	template<ServerCore::s_int32 N> void In_Password(WCHAR(&v)[N]) { BindParam(1, v); };
    	template<ServerCore::s_int32 N> void In_Password(const WCHAR(&v)[N]) { BindParam(1, v); };
    	void In_Password(WCHAR* v, ServerCore::s_int32 count) { BindParam(1, v, count); };
    	void In_Password(const WCHAR* v, ServerCore::s_int32 count) { BindParam(1, v, count); };

    private:
    };

    class GetUser : public DBBind<2,3>
    {
    public:
    	GetUser(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spGetUser(?,?)}") { }
    	template<ServerCore::s_int32 N> void In_LoginId(WCHAR(&v)[N]) { BindParam(0, v); };
    	template<ServerCore::s_int32 N> void In_LoginId(const WCHAR(&v)[N]) { BindParam(0, v); };
    	void In_LoginId(WCHAR* v, ServerCore::s_int32 count) { BindParam(0, v, count); };
    	void In_LoginId(const WCHAR* v, ServerCore::s_int32 count) { BindParam(0, v, count); };
    	template<ServerCore::s_int32 N> void In_Password(WCHAR(&v)[N]) { BindParam(1, v); };
    	template<ServerCore::s_int32 N> void In_Password(const WCHAR(&v)[N]) { BindParam(1, v); };
    	void In_Password(WCHAR* v, ServerCore::s_int32 count) { BindParam(1, v, count); };
    	void In_Password(const WCHAR* v, ServerCore::s_int32 count) { BindParam(1, v, count); };
    	void Out_Id(OUT ServerCore::s_int32& v) { BindCol(0, v); };
    	template<ServerCore::s_int32 N> void Out_LoginId(OUT WCHAR(&v)[N]) { BindCol(1, v); };
    	template<ServerCore::s_int32 N> void Out_Password(OUT WCHAR(&v)[N]) { BindCol(2, v); };

    private:
    };

    class InsertPlayer : public DBBind<5,0>
    {
    public:
    	InsertPlayer(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spInsertPlayer(?,?,?,?,?)}") { }
    	void In_AccountId(ServerCore::s_int32& v) { BindParam(0, v); };
    	void In_AccountId(ServerCore::s_int32&& v) { _accountId = std::move(v); BindParam(0, _accountId); };
    	void In_PlayerType(ServerCore::s_int32& v) { BindParam(1, v); };
    	void In_PlayerType(ServerCore::s_int32&& v) { _playerType = std::move(v); BindParam(1, _playerType); };
    	void In_Level(ServerCore::s_int32& v) { BindParam(2, v); };
    	void In_Level(ServerCore::s_int32&& v) { _level = std::move(v); BindParam(2, _level); };
    	void In_Gold(ServerCore::s_int32& v) { BindParam(3, v); };
    	void In_Gold(ServerCore::s_int32&& v) { _gold = std::move(v); BindParam(3, _gold); };
    	void In_Exp(ServerCore::s_int32& v) { BindParam(4, v); };
    	void In_Exp(ServerCore::s_int32&& v) { _exp = std::move(v); BindParam(4, _exp); };

    private:
    	ServerCore::s_int32 _accountId = {};
    	ServerCore::s_int32 _playerType = {};
    	ServerCore::s_int32 _level = {};
    	ServerCore::s_int32 _gold = {};
    	ServerCore::s_int32 _exp = {};
    };

    class GetPlayer : public DBBind<1,5>
    {
    public:
    	GetPlayer(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spGetPlayer(?)}") { }
    	void In_AccountId(ServerCore::s_int32& v) { BindParam(0, v); };
    	void In_AccountId(ServerCore::s_int32&& v) { _accountId = std::move(v); BindParam(0, _accountId); };
    	void Out_AccountId(OUT ServerCore::s_int32& v) { BindCol(0, v); };
    	void Out_PlayerType(OUT ServerCore::s_int32& v) { BindCol(1, v); };
    	void Out_Level(OUT ServerCore::s_int32& v) { BindCol(2, v); };
    	void Out_Gold(OUT ServerCore::s_int32& v) { BindCol(3, v); };
    	void Out_Exp(OUT ServerCore::s_int32& v) { BindCol(4, v); };

    private:
    	ServerCore::s_int32 _accountId = {};
    };

    class UpdatePlayerGold : public DBBind<2,0>
    {
    public:
    	UpdatePlayerGold(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spUpdatePlayerGold(?,?)}") { }
    	void In_AccountId(ServerCore::s_int32& v) { BindParam(0, v); };
    	void In_AccountId(ServerCore::s_int32&& v) { _accountId = std::move(v); BindParam(0, _accountId); };
    	void In_Gold(ServerCore::s_int32& v) { BindParam(1, v); };
    	void In_Gold(ServerCore::s_int32&& v) { _gold = std::move(v); BindParam(1, _gold); };

    private:
    	ServerCore::s_int32 _accountId = {};
    	ServerCore::s_int32 _gold = {};
    };

    class InsertInventory : public DBBind<5,0>
    {
    public:
    	InsertInventory(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spInsertInventory(?,?,?,?,?)}") { }
    	void In_AccountId(ServerCore::s_int32& v) { BindParam(0, v); };
    	void In_AccountId(ServerCore::s_int32&& v) { _accountId = std::move(v); BindParam(0, _accountId); };
    	void In_Idx(ServerCore::s_int32& v) { BindParam(1, v); };
    	void In_Idx(ServerCore::s_int32&& v) { _idx = std::move(v); BindParam(1, _idx); };
    	void In_Type(ServerCore::s_int32& v) { BindParam(2, v); };
    	void In_Type(ServerCore::s_int32&& v) { _type = std::move(v); BindParam(2, _type); };
    	void In_MetaId(ServerCore::s_int32& v) { BindParam(3, v); };
    	void In_MetaId(ServerCore::s_int32&& v) { _metaId = std::move(v); BindParam(3, _metaId); };
    	void In_Count(ServerCore::s_int32& v) { BindParam(4, v); };
    	void In_Count(ServerCore::s_int32&& v) { _count = std::move(v); BindParam(4, _count); };

    private:
    	ServerCore::s_int32 _accountId = {};
    	ServerCore::s_int32 _idx = {};
    	ServerCore::s_int32 _type = {};
    	ServerCore::s_int32 _metaId = {};
    	ServerCore::s_int32 _count = {};
    };

    class GetInventory : public DBBind<1,5>
    {
    public:
    	GetInventory(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spGetInventory(?)}") { }
    	void In_AccountId(ServerCore::s_int32& v) { BindParam(0, v); };
    	void In_AccountId(ServerCore::s_int32&& v) { _accountId = std::move(v); BindParam(0, _accountId); };
    	void Out_AccountId(OUT ServerCore::s_int32& v) { BindCol(0, v); };
    	void Out_Idx(OUT ServerCore::s_int32& v) { BindCol(1, v); };
    	void Out_Type(OUT ServerCore::s_int32& v) { BindCol(2, v); };
    	void Out_MetaId(OUT ServerCore::s_int32& v) { BindCol(3, v); };
    	void Out_Count(OUT ServerCore::s_int32& v) { BindCol(4, v); };

    private:
    	ServerCore::s_int32 _accountId = {};
    };

    class InsertEquipInstall : public DBBind<3,0>
    {
    public:
    	InsertEquipInstall(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spInsertEquipInstall(?,?,?)}") { }
    	void In_AccountId(ServerCore::s_int32& v) { BindParam(0, v); };
    	void In_AccountId(ServerCore::s_int32&& v) { _accountId = std::move(v); BindParam(0, _accountId); };
    	void In_Idx(ServerCore::s_int32& v) { BindParam(1, v); };
    	void In_Idx(ServerCore::s_int32&& v) { _idx = std::move(v); BindParam(1, _idx); };
    	void In_EquipId(ServerCore::s_int32& v) { BindParam(2, v); };
    	void In_EquipId(ServerCore::s_int32&& v) { _equipId = std::move(v); BindParam(2, _equipId); };

    private:
    	ServerCore::s_int32 _accountId = {};
    	ServerCore::s_int32 _idx = {};
    	ServerCore::s_int32 _equipId = {};
    };

    class GetEquipInstall : public DBBind<1,3>
    {
    public:
    	GetEquipInstall(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spGetEquipInstall(?)}") { }
    	void In_AccountId(ServerCore::s_int32& v) { BindParam(0, v); };
    	void In_AccountId(ServerCore::s_int32&& v) { _accountId = std::move(v); BindParam(0, _accountId); };
    	void Out_AccountId(OUT ServerCore::s_int32& v) { BindCol(0, v); };
    	void Out_Idx(OUT ServerCore::s_int32& v) { BindCol(1, v); };
    	void Out_EquipId(OUT ServerCore::s_int32& v) { BindCol(2, v); };

    private:
    	ServerCore::s_int32 _accountId = {};
    };

    class DeleteInventory : public DBBind<2,0>
    {
    public:
    	DeleteInventory(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spDeleteInventory(?,?)}") { }
    	void In_AccountId(ServerCore::s_int32& v) { BindParam(0, v); };
    	void In_AccountId(ServerCore::s_int32&& v) { _accountId = std::move(v); BindParam(0, _accountId); };
    	void In_Idx(ServerCore::s_int32& v) { BindParam(1, v); };
    	void In_Idx(ServerCore::s_int32&& v) { _idx = std::move(v); BindParam(1, _idx); };

    private:
    	ServerCore::s_int32 _accountId = {};
    	ServerCore::s_int32 _idx = {};
    };

    class UpdateCountInventory : public DBBind<3,0>
    {
    public:
    	UpdateCountInventory(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spUpdateCountInventory(?,?,?)}") { }
    	void In_AccountId(ServerCore::s_int32& v) { BindParam(0, v); };
    	void In_AccountId(ServerCore::s_int32&& v) { _accountId = std::move(v); BindParam(0, _accountId); };
    	void In_Idx(ServerCore::s_int32& v) { BindParam(1, v); };
    	void In_Idx(ServerCore::s_int32&& v) { _idx = std::move(v); BindParam(1, _idx); };
    	void In_Count(ServerCore::s_int32& v) { BindParam(2, v); };
    	void In_Count(ServerCore::s_int32&& v) { _count = std::move(v); BindParam(2, _count); };

    private:
    	ServerCore::s_int32 _accountId = {};
    	ServerCore::s_int32 _idx = {};
    	ServerCore::s_int32 _count = {};
    };

    class UpdateInventory : public DBBind<5,0>
    {
    public:
    	UpdateInventory(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spUpdateInventory(?,?,?,?,?)}") { }
    	void In_AccountId(ServerCore::s_int32& v) { BindParam(0, v); };
    	void In_AccountId(ServerCore::s_int32&& v) { _accountId = std::move(v); BindParam(0, _accountId); };
    	void In_Idx(ServerCore::s_int32& v) { BindParam(1, v); };
    	void In_Idx(ServerCore::s_int32&& v) { _idx = std::move(v); BindParam(1, _idx); };
    	void In_Type(ServerCore::s_int32& v) { BindParam(2, v); };
    	void In_Type(ServerCore::s_int32&& v) { _type = std::move(v); BindParam(2, _type); };
    	void In_MetaId(ServerCore::s_int32& v) { BindParam(3, v); };
    	void In_MetaId(ServerCore::s_int32&& v) { _metaId = std::move(v); BindParam(3, _metaId); };
    	void In_Count(ServerCore::s_int32& v) { BindParam(4, v); };
    	void In_Count(ServerCore::s_int32&& v) { _count = std::move(v); BindParam(4, _count); };

    private:
    	ServerCore::s_int32 _accountId = {};
    	ServerCore::s_int32 _idx = {};
    	ServerCore::s_int32 _type = {};
    	ServerCore::s_int32 _metaId = {};
    	ServerCore::s_int32 _count = {};
    };

    class DeleteEquipInstall : public DBBind<2,0>
    {
    public:
    	DeleteEquipInstall(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spDeleteEquipInstall(?,?)}") { }
    	void In_AccountId(ServerCore::s_int32& v) { BindParam(0, v); };
    	void In_AccountId(ServerCore::s_int32&& v) { _accountId = std::move(v); BindParam(0, _accountId); };
    	void In_Idx(ServerCore::s_int32& v) { BindParam(1, v); };
    	void In_Idx(ServerCore::s_int32&& v) { _idx = std::move(v); BindParam(1, _idx); };

    private:
    	ServerCore::s_int32 _accountId = {};
    	ServerCore::s_int32 _idx = {};
    };

    class UpdateExp : public DBBind<3,0>
    {
    public:
    	UpdateExp(DBConnection& conn) : DBBind(conn, L"{CALL dbo.spUpdateExp(?,?,?)}") { }
    	void In_AccountId(ServerCore::s_int32& v) { BindParam(0, v); };
    	void In_AccountId(ServerCore::s_int32&& v) { _accountId = std::move(v); BindParam(0, _accountId); };
    	void In_Level(ServerCore::s_int32& v) { BindParam(1, v); };
    	void In_Level(ServerCore::s_int32&& v) { _level = std::move(v); BindParam(1, _level); };
    	void In_Exp(ServerCore::s_int32& v) { BindParam(2, v); };
    	void In_Exp(ServerCore::s_int32&& v) { _exp = std::move(v); BindParam(2, _exp); };

    private:
    	ServerCore::s_int32 _accountId = {};
    	ServerCore::s_int32 _level = {};
    	ServerCore::s_int32 _exp = {};
    };


     
};