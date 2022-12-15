#pragma once
#include "Protocol.pb.h"

struct CreateAbleIdx
{
public:
	bool operator==(const CreateAbleIdx& other) const
	{
		return x == other.x && y == other.y;
	}
	CreateAbleIdx operator+(CreateAbleIdx& other)
	{
		CreateAbleIdx ret;
		ret.y = y + other.y;
		ret.x = x + other.x;
		return ret;
	}
public:
	int x;
	int y;
};

struct PQNode
{
public:
	bool operator<(const shared_ptr<PQNode> other) const { return g + h < other->g + other->h; }
	bool operator>(const shared_ptr<PQNode> other) const { return g + h > other->g + other->h; }

	bool operator<(const PQNode& other) const { return g + h < other.g + other.h; }
	bool operator>(const PQNode& other) const { return g + h > other.g + other.h; }

	// f = g + h
	int g;									// 시작지점에서 현재지점까지 누적된 거리
	int h;									// 시작지점에서 목적지까지의 거리
	CreateAbleIdx posIdx;					// 해당노드가 가진 위치인덱스
	shared_ptr<PQNode> parentNode;			// 부모노드
	bool isClosed = false;					// 방문했는지 확인하는 변수
};

class GameMap
{
public:
	GameMap();
	GameMap(int id, int x, int y, int z, int interval, int compoCnt, vector<vector<bool>> data);

public:
	Protocol::PBVector CreateAbleRandomPos(); // 생성가능한 랜덤위치를 리턴하는함수
	vector<Vector3> MoveAbleRandomPos(Vector3 location);

	Protocol::PBVector Idx2Pos(CreateAbleIdx idx);
	CreateAbleIdx Pos2Idx(Protocol::PBVector pos);
	vector<Vector3> AstarEX(Vector3 start, Vector3 dest);
	vector<Vector3> AstarStraight(Vector3 start, Vector3 dest);

	vector<Vector3> Astar(Vector3 start, Vector3 dest);
	vector<Vector3> Astar(const Protocol::PBVector& start, const Protocol::PBVector& dest);
	
	bool CanGo(CreateAbleIdx idx);
	int GetSize() { return _compoCnt; };

	bool IsObstacleCheck(Protocol::PBVector location);

	int intRand();

private:
	int _id = 0;
	int _x = 0;
	int _y = 0;
	int _z = 0;
	int _interval = 0;
	int _compoCnt = 0;
	vector<vector<bool>> _data;

	// 생성 위치의 인덱스를 가진 데이터
	vector<CreateAbleIdx> _createAbledata;
};

