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
	int g;									// ������������ ������������ ������ �Ÿ�
	int h;									// ������������ ������������ �Ÿ�
	CreateAbleIdx posIdx;					// �ش��尡 ���� ��ġ�ε���
	shared_ptr<PQNode> parentNode;			// �θ���
	bool isClosed = false;					// �湮�ߴ��� Ȯ���ϴ� ����
};

class GameMap
{
public:
	GameMap();
	GameMap(int id, int x, int y, int z, int interval, int compoCnt, vector<vector<bool>> data);

public:
	Protocol::PBVector CreateAbleRandomPos(); // ���������� ������ġ�� �����ϴ��Լ�
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

	// ���� ��ġ�� �ε����� ���� ������
	vector<CreateAbleIdx> _createAbledata;
};

