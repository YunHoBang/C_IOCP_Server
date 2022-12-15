#pragma once

#include "GenProcedures.h"
/*
전투관련 유틸함수 모음
*/

class YHUtils
{
public:
	static bool DistanceCheck(const Vector3& s ,const Vector3& d, float dist);
	static bool DistanceCheck(const Protocol::PBVector& s, const Protocol::PBVector& d, float dist);
	static float CalDegree(const Vector3& v1,const Vector3& v2);
	static Vector3 CalForward(Protocol::PBVector s, Protocol::PBVector d);
	static Protocol::PBVector* CalForwardAndSet(Protocol::PBVector s, Protocol::PBVector d,Protocol::PBVector& v);
	static Protocol::PBVector RotateVectorPB(Protocol::PBVector v, float degree);
	static Vector3 RotateVector(const Vector3& v,float degree);
	static Vector3 CalParabola(const Vector3 l, const Vector3 f, float speed,float time, float gravity = 9.8f);
	static Protocol::PBVector CalParabolaPB(const Vector3 l, const Vector3 f, float speed, float time, float gravity = 9.8f);
	static Vector3 Radian2Vector3(float radians);
	static Protocol::PBVector Radian2Vector3PB(float radians);
	static float Radian2Degree(float radian);
	static float Degree2Radian(float degree);
	static float Vector2Distance(Vector3 v, Vector3 v2);

	// DB
	
	static bool DB_Login(GameSessionRef gameSession, const string& inId, const string& inPassword);
	static bool DB_InsertUser(const string& inId, const string& inPassword);
	static bool DB_InsertCharacter(s_int32 id, s_int32 type);

	// Protocol
	static bool PB_SetItemData(Protocol::PBItemData* data, shared_ptr<GameData> dd,int idx);

};

