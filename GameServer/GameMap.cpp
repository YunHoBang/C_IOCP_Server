#include "pch.h"
#include "GameMap.h"
#include <cstdlib>
#include <ctime>
#include <queue>
#include <random>

GameMap::GameMap(int id, int x, int y, int z, int interval, int compoCnt, vector<vector<bool>> data)
	: _id(id), _x(x), _y(y), _z(z), _interval(interval), _compoCnt(compoCnt), _data(data)
{

	for (int x = 0; x < _compoCnt; ++x)
	{
		for (int y = 0; y < _compoCnt; ++y)
		{
			if (_data[x][y] == false)
			{
				CreateAbleIdx idx;
				{
					idx.x = x;
					idx.y = y;
				}
				_createAbledata.push_back(idx);
			}
		}
	}
}

GameMap::GameMap()
{

}

Protocol::PBVector GameMap::CreateAbleRandomPos()
{
	// 생성 가능한 위치의 사이즈만큼
	int createAbleSize = _createAbledata.size();

	// 랜덤값을 받는다.
	int randomNum = rand() % createAbleSize;

	CreateAbleIdx idx = _createAbledata[randomNum];

	return Idx2Pos(idx);
}

vector<Vector3> GameMap::MoveAbleRandomPos(Vector3 location)
{
	// 랜덤한 숫자를 받는다. 0~ 7 까지
	int r = intRand();
	int ranCnt = (r % 8);
	
	// 위치 -> 인덱스
	CreateAbleIdx posIdx = Pos2Idx(location.ConvertPBVector());

	// 방향
	CreateAbleIdx dir[] =
	{
		CreateAbleIdx { 3, 0},		// UP
		CreateAbleIdx { 3, 3},		// RIGHT UP
		CreateAbleIdx { -3, -3},	// LEFT DOWN
		CreateAbleIdx { -3,  0},	// DOWN
		CreateAbleIdx { 3, -3},		// LEFT UP 
		CreateAbleIdx { 0,  3},		// RIGHT
		CreateAbleIdx {-3, 3},		// RIGHT DOWN
		CreateAbleIdx { 0, -3},		// LEFT
	};

	// 현재 위치 기준으로 

	CreateAbleIdx checkLocation = posIdx + dir[ranCnt];

	// 갈 수 있는 지역은 맞는지 확인
	if (CanGo(checkLocation) == false)
		return vector<Vector3>();

	//cout << "x : " << checkLocation.x <<"y : "<< checkLocation.y<< endl;

	// 만약 갈수있다면 경로를 받는다.
	auto l = Idx2Pos(checkLocation);
	//cout << "Loc x : " << l.x() << "y : " << l.y() << "z : " << l.z() << endl;

	return Astar(location, l);
}

Protocol::PBVector GameMap::Idx2Pos(CreateAbleIdx idx)
{
	Protocol::PBVector pos;

	int posX = _x + (idx.x * _interval);
	int posY = _y + (idx.y * _interval);

	pos.set_x(posX);
	pos.set_y(posY);

	return pos;
}

CreateAbleIdx GameMap::Pos2Idx(Protocol::PBVector pos)
{
	int idxX = (pos.x() - _x) / _interval;
	int idxY = (pos.y() - _y) / _interval;

	CreateAbleIdx ableIdx;
	{
		ableIdx.x = idxX;
		ableIdx.y = idxY;
	}
	return ableIdx;
}

std::vector<Vector3> GameMap::AstarEX(Vector3 start, Vector3 dest)
{
	if (IsObstacleCheck(start.ConvertPBVector()))
	{
		return AstarStraight(start, dest);
	}
	else
	{
		return Astar(start, dest);
	}
}

std::vector<Vector3> GameMap::Astar(Vector3 start, Vector3 dest)
{
	// 방향 
	enum
	{
		DIR_COUNT = 8
	};
	// 이동할 위치정보를
	CreateAbleIdx front[] =
	{
		CreateAbleIdx { 1, 0},		// UP
		CreateAbleIdx { 0, -1},		// LEFT
		CreateAbleIdx { -1,  0},	// DOWN
		CreateAbleIdx { 0,  1},		// RIGHT
		CreateAbleIdx { 1, -1},		// LEFT UP 
		CreateAbleIdx { 1, 1},		// RIGHT UP
		CreateAbleIdx { -1, -1},	// LEFT DOWN
		CreateAbleIdx {-1, 1},		// RIGHT DOWN
	};
	// 이동할방향의 가격
	int cost[] =
	{
		10, // UP
		10, // LEFT
		10, // DOWN
		10, // RIGHT
		14, // LEFT UP
		14, // RIGHT UP
		14, // LEFT DOWN
		14  // RIGHT DOWN
	};

	// 받은 위치에서 가장 가까운 위치를 확인한다.
	Protocol::PBVector startPos;
	{
		startPos.set_x(start.x);
		startPos.set_y(start.y);
		startPos.set_z(start.z);
	}
	Protocol::PBVector destPos;
	{
		destPos.set_x(dest.x);
		destPos.set_y(dest.y);
		destPos.set_z(dest.z);
	}

	CreateAbleIdx startIdx = Pos2Idx(startPos);
	CreateAbleIdx destIdx = Pos2Idx(destPos);
	
	auto comp = [&](shared_ptr<PQNode> a, shared_ptr<PQNode> b)
	{
		return (a->g + a->h) > (b->g + b->h);
	};
	//우선순위 큐를 선언해준다. // OpenList
	priority_queue<shared_ptr<PQNode>, vector<shared_ptr<PQNode>>, decltype(comp)> pq(comp);

	// 초기값 설정
	{
		int g = 0;
		int h = 10 * (abs(destIdx.y - startIdx.y) + abs(destIdx.x - startIdx.x));

		PQNode* node = new PQNode();
		{
			node->g = g;
			node->h = h;
			node->posIdx = startIdx;
		}
		pq.push(shared_ptr<PQNode>(node));
	}

	int size = GetSize();
	vector<vector<bool>> closed(size, vector<bool>(size, false));

	shared_ptr<PQNode> lastNode;
	
	while (pq.empty() == false)
	{
		// 제일 좋은 후보를 찾는다
		shared_ptr<PQNode> node = pq.top();
		pq.pop();

		// 해당 노드가 이미 방문 했었다면
		if (closed[node->posIdx.x][node->posIdx.y])
			continue;
		if (node->isClosed)
			continue;

		// 방문
		node->isClosed = true;
		closed[node->posIdx.x][node->posIdx.y] = true;

		// 목적지에 도착했으면 바로 종료
		if (node->posIdx == destIdx)
		{
			lastNode = node;
			break;
		}

		for (int dir = 0; dir < DIR_COUNT; dir++)
		{
			CreateAbleIdx nextPos = node->posIdx + front[dir];

			// 갈 수 있는 지역은 맞는지 확인
			if (CanGo(nextPos) == false)
			{
				continue;
			}
			// 이미 방문한 곳이면 스킵
			if (closed[nextPos.x][nextPos.y])
				continue;

			// 비용 계산
			int g = node->g + cost[dir];
			int h = 10 * (abs(dest.y - nextPos.y) + abs(dest.x - nextPos.x));

			// 예약 진행
			PQNode* tmpNode = new PQNode();
			{
				tmpNode->g = g;
				tmpNode->h = h;
				tmpNode->posIdx = nextPos;
				tmpNode->parentNode = node;
			}

			pq.push(shared_ptr<PQNode>(tmpNode));
		}
	}

	if (lastNode == nullptr)
	{
		return std::vector<Vector3>();
	}

	// 마지막 노드에서 부모로 올라가면서 위치를 세팅한다.
	std::vector<Vector3> returnVec;
	while (true)
	{
		if (lastNode->parentNode == nullptr)
		{
			Protocol::PBVector dir(Idx2Pos(lastNode->posIdx));
			Vector3 addPos(dir);
			returnVec.push_back(addPos);
			break;
		}
		// 위치를 저장한다.
		Protocol::PBVector dir(Idx2Pos(lastNode->posIdx));
		Vector3 addPos(dir);

		returnVec.push_back(addPos);
		lastNode = lastNode->parentNode;
	}

	return returnVec;
}

std::vector<Vector3> GameMap::Astar(const Protocol::PBVector& start, const Protocol::PBVector& dest)
{
	// 시작위치를 가지고온다.
	Vector3 startPos (start);

	// 목적지 위치를 가지고 온다.
	Vector3 destPos(dest);

	return Astar(startPos,destPos);
}

std::vector<Vector3> GameMap::AstarStraight(Vector3 start, Vector3 dest)
{
	std::vector<Vector3> v;
	v.push_back(dest);
	return v;
}

bool GameMap::CanGo(CreateAbleIdx idx)
{
	// 해당인덱스가 유효한지와 접근 가는한지 == 즉 1값이 아닌지 체크
	if (idx.x > _compoCnt - 1 || idx.x < 0)
	{
		return false;
	}
	else if (idx.y > _compoCnt - 1 || idx.y < 0)
	{
		return false;
	}
	else if (_data[idx.x][idx.y]) // 해당 인덱스의 값이 1값이라면 접근 못한다 
	{
		return false;
	}

	return true;
}

bool GameMap::IsObstacleCheck(Protocol::PBVector location)
{
	// 위치 -> 인덱스
	CreateAbleIdx posIdx = Pos2Idx(location);
	
	// 방향
	CreateAbleIdx dir[] =
	{
		CreateAbleIdx { 1, 0},		// UP
		CreateAbleIdx { 0, -1},		// LEFT
		CreateAbleIdx { -1,  0},	// DOWN
		CreateAbleIdx { 0,  1},		// RIGHT
		CreateAbleIdx { 1, -1},		// LEFT UP 
		CreateAbleIdx { 1, 1},		// RIGHT UP
		CreateAbleIdx { -1, -1},	// LEFT DOWN
		CreateAbleIdx {-1, 1},		// RIGHT DOWN
	};
	for (int i = 0; i < 8; ++i)
	{
		CreateAbleIdx checkLocation = posIdx + dir[i];

		// 갈 수 있는 지역은 맞는지 확인
		if (CanGo(checkLocation) == false)
			return false;
	}
	return true;
}

int GameMap::intRand()
{
	thread_local std::mt19937 generator(std::random_device{}());

	std::uniform_int_distribution<int> distribution(0, 54651324);

	return distribution(generator);

}
