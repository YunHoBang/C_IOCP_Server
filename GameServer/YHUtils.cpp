#include "pch.h"
#include "YHUtils.h"
#include "Protocol.pb.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "XmlParser.h"
#include "DBSynchronizer.h"
#include "GenProcedures.h"

#include "Player.h"
#include "Room.h"
#include "Projectile.h"
#include "GameSession.h"
#include "MetaData.h"
#include "Skill.h"
#include "Character.h"
#include "DataManager.h"

#include "Equip.h"
#include "Consum.h"

bool YHUtils::DistanceCheck(const Vector3& s, const Vector3& d, float dist)
{
	return abs(s.x - d.x) < dist && abs(s.y - d.y) < dist;
}

bool YHUtils::DistanceCheck(const Protocol::PBVector& s, const Protocol::PBVector& d, float dist)
{
	// Get Current Location
	Vector3 currentLocation(s);
	// Get Current Location
	Vector3 destLocation(d);

	return YHUtils::DistanceCheck(currentLocation, destLocation, dist);
}

float YHUtils::CalDegree(const Vector3& v1, const Vector3& v2)
{
	// 두점 
	// 1. x = 0 y = 1
	Vector2 vec1 = { 0,1 };
	// 2. 몬스터가 바라보는 방향벡터 // 이동해야하는 위치에서 몬스터의 위치를 뺀 벡터
	Vector2 vec2 = { v1.x - v2.x, v1.y - v2.y };

	vec1.Normalize();
	vec2.Normalize();

	auto r = asin(vec1.Cross(vec2));
	auto radian = acos(vec1.Dot(vec2));

	float degree;
	if (vec1.x < vec2.x)
		degree = -radian * 180.0l / PI + 90;
	else
		degree = radian * 180.0l / PI + 90;

	return degree;
}

Vector3 YHUtils::CalForward(Protocol::PBVector s, Protocol::PBVector d)
{
	// PB벡터를 Vector3로 바꿔준다.
	Vector3 v1(s.x(), s.y(), s.z());
	Vector3 v2(d.x(), d.y(), d.z());

	// 벡터를 빼준다. // 정규화해준다.
	return (v1 - v2).Normalize();
}

Protocol::PBVector* YHUtils::CalForwardAndSet(Protocol::PBVector s, Protocol::PBVector d, Protocol::PBVector& v)
{
	auto r = CalForward(s, d);

	v.set_x(r.x);
	v.set_y(r.y);
	v.set_z(r.z);

	return &v;
}

Protocol::PBVector YHUtils::RotateVectorPB(Protocol::PBVector v, float degree)
{
	Vector3 r = RotateVector(Vector3(v.x(), v.y(), v.z()), degree);

	v.set_x(r.x);
	v.set_y(r.y);
	v.set_z(r.z);
	return v;
}

Vector3 YHUtils::RotateVector(const Vector3& v, float degree)
{
	//(xcosΘ - ysinΘ, xsinΘ + ycosΘ)
	float radians = degree * (3.141592f / 180);
	Vector3 v2 = Vector3
	(
		v.x * ::cos(radians) - v.y * ::sin(radians),
		v.x * ::sin(radians) + v.y * ::cos(radians),
		0
	);

	return v2;
}

Vector3 YHUtils::CalParabola(const Vector3 l, const Vector3 f, float speed, float time, float gravity /*= 9.8f*/)
{
	float x = (f.x * speed * time) + l.x;
	float y = (f.y * speed * time) + l.y;
	float z = -(0.5 * gravity * (time * time)) + (speed * time) + l.z;
	return Vector3(x, y, z);
}

Protocol::PBVector YHUtils::CalParabolaPB(const Vector3 l, const Vector3 f, float speed, float time, float gravity /*= 9.8f*/)
{
	Vector3 v = CalParabola(l, f, speed, time, gravity);
	Protocol::PBVector r;
	r.set_x(v.x);
	r.set_y(v.y);
	r.set_z(v.z);
	return r;
}

Vector3 YHUtils::Radian2Vector3(float radians)
{
	return Vector3(
	(float)::sin(radians),
	-(float)::cos(radians),
	0); // 각도를 벡터로 변환해준다

}

Protocol::PBVector YHUtils::Radian2Vector3PB(float radians)
{
	auto vv = Radian2Vector3(radians);
	Protocol::PBVector v;
	v.set_x(vv.x);
	v.set_x(vv.y);
	v.set_x(vv.z);
	return v;
}

float YHUtils::Radian2Degree(float radian)
{
	return radian * 180 / PI;
}

float YHUtils::Degree2Radian(float degree)
{
	return degree * PI / 180;
}

float YHUtils::Vector2Distance(Vector3 v, Vector3 v2)
{
	float diffY = v.y - v2.y;
	float diffX = v.x - v2.x;
	return sqrt((diffY * diffY) + (diffX * diffX));
}

bool YHUtils::DB_InsertUser(const string& inId, const string& inPassword)
{
	DBConnection* dbConn = GDBConnectionPool->Pop();
	SP::GetUser getUser(*dbConn);

	wstring wide_string = wstring(inId.begin(), inId.end());
	const wchar_t* result = wide_string.c_str();
	getUser.In_LoginId(result, wide_string.size());

	wstring wide_string2 = wstring(inPassword.begin(), inPassword.end());
	const wchar_t* result2 = wide_string2.c_str();
	getUser.In_Password(result2, wide_string2.size());

	s_int32 id = 0;
	WCHAR loginId[200];
	WCHAR password[200];

	getUser.Out_Id(OUT id);
	getUser.Out_LoginId(OUT loginId);
	getUser.Out_Password(OUT password);

	getUser.Execute();

	if (!getUser.Fetch())
	{
		SP::InsertUser insertUser(*dbConn);

		insertUser.In_LoginId(result, wide_string.size());
		insertUser.In_Password(result2, wide_string2.size());
		insertUser.Execute();
	}
	else
	{
		return false;
	}

	GDBConnectionPool->Push(dbConn);

	return true;
}

bool YHUtils::DB_Login(GameSessionRef gameSession, const string& inId, const string& inPassword)
{
	// return value
	bool loginCheck = false;

	// DB Processure Setting
	DBConnection* dbConn = GDBConnectionPool->Pop();
	SP::GetUser getUser(*dbConn);

	// value setting
	wstring wide_string = wstring(inId.begin(), inId.end());
	const wchar_t* result = wide_string.c_str();
	getUser.In_LoginId(result, wide_string.size());

	wstring wide_string2 = wstring(inPassword.begin(), inPassword.end());
	const wchar_t* result2 = wide_string2.c_str();
	getUser.In_Password(result2, wide_string2.size());

	s_int32 id = 0;
	WCHAR loginId[200];
	WCHAR password[200];

	getUser.Out_Id(OUT id);
	getUser.Out_LoginId(OUT loginId);
	getUser.Out_Password(OUT password);

	getUser.Execute();

	// 받을 데이터가 있다면 여기로 들어온다.
	while (getUser.Fetch())
	{
		//GConsoleLogger->WriteStdOut(Color::BLUE, L"ID[%d] LoginId[%s] Name[%s]\n", id, loginId, password);
		
		// Set Data
		gameSession->SetId(id);

		DBConnection* dbConn2 = GDBConnectionPool->Pop();
		DBSynchronizer dbSync2(*dbConn2);

		// 계정 데이터에도 있는지 확인해야함 // 프로시저 생성해야함
		SP::GetPlayer getPlayer(*dbConn2);

		getPlayer.In_AccountId(id);

		s_int32 accountId = 0;
		s_int32 playerType = 0;
		s_int32 level = 0;
		s_int32 gold = 0;
		s_int32 exp = 0;

		getPlayer.Out_AccountId(OUT accountId);
		getPlayer.Out_PlayerType(OUT playerType);
		getPlayer.Out_Level(OUT level);
		getPlayer.Out_Gold(OUT gold);
		getPlayer.Out_Exp(OUT exp);
		getPlayer.Execute();

		if (getPlayer.Fetch())
		{
			//GConsoleLogger->WriteStdOut(Color::BLUE, L"accountId[%d] playerType[%d] level[%d]\n", accountId, playerType, level);
			loginCheck = true;
		}
		else
		{
			//cout << "Login failed" << endl;
			loginCheck = false;
		}
		GDBConnectionPool->Push(dbConn2);
	}
	GDBConnectionPool->Push(dbConn);

	return loginCheck;
}

bool YHUtils::DB_InsertCharacter(s_int32 id, s_int32 type)
{
	DBConnection* dbConn = GDBConnectionPool->Pop();
	DBSynchronizer dbSync(*dbConn);

	// 계정 데이터에도 있는지 확인해야함 // 프로시저 생성해야함
	SP::InsertPlayer insertPlayer(*dbConn);
	insertPlayer.In_AccountId(id);
	insertPlayer.In_PlayerType(type);
	insertPlayer.In_Level(1);
	insertPlayer.In_Exp(0);
	insertPlayer.In_Gold(0);

	insertPlayer.Execute();

	GDBConnectionPool->Push(dbConn);

	return true;
}

bool YHUtils::PB_SetItemData(Protocol::PBItemData* data,shared_ptr<GameData> dd, int idx)
{
	data->set_type(dd->GetDataType()); // 타입을 세팅해야함
	data->set_idx(idx);
	// 타입확인
	switch (dd->GetDataType())
	{
	case Protocol::DataType::DATA_TYPE_CONSUM:
	{
		// 형변환
		shared_ptr<Consum> d = static_pointer_cast<Consum>(dd);

		// 필요한데이터를 넣자
		data->set_count(d->_count);
	//	data->set_allocated_consumdata(new Protocol::ConsumMetaData(d->_data));

	}	break;
	case Protocol::DataType::DATA_TYPE_EQUIP:
	{
		// 형변환
		shared_ptr<Equip> d = static_pointer_cast<Equip>(dd);
		// 필요한데이터를 넣자
		data->set_upgrade(d->_upgrade);
		//data->unsafe_arena_set_allocated_equipdata(new Protocol::EquipMetaData(d->_data));

	}	break;

	default: break;
	}
	return true;
}

