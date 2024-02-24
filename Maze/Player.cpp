#include"pch.h"
#include "Player.h"
#include "Board.h"
#include<stack>
void Player::Init(Board* board)
{
	_pos = board->GetEnterPos();
	_board = board;

	//RightHand();
	//Bfs();
	AStar();
}

void Player::Update(uint64 deltaTick)
{
	if (_pathIndex >= _path.size())
		return;
	
	_sumTick += deltaTick;
	if (_sumTick >= MOVE_TICK)
	{
		_sumTick = 0;

		_pos = _path[_pathIndex];
		_pathIndex++;
	}
}

bool Player::CanGo(Pos pos)
{
	TileType tileType = _board->GetTileType(pos);
	return tileType == TileType::EMPTY;
}

void Player::RightHand()
{
	// 오른손 법칙 (우수 법칙) (미로 탐색 과정에서 계속 오른쪽으로 탐색)

	// 목적지 도착하기 전에는 계속 실행
	Pos dest = _board->GetExitPos();
	Pos pos = _pos;

	// 경로 초기화
	_path.clear();
	_path.push_back(pos);

	Pos front[4] =
	{
		Pos{-1,0},	//up
		Pos{0,-1},	//left
		Pos{1,0},	//down
		Pos{0,1},	//right
	};


	while (pos != dest)
	{
		// 1. 현재 바라보는 방향을 기준으로 오른쪽으로 갈 수 있는지 확인
		int32 newDir = (_dir - 1 + DIR_COUNT) % DIR_COUNT;
		if (CanGo(pos + front[newDir]))
		{
			//오른쪽 방향으로 90도 회전
			_dir = newDir;
			//앞으로 한 보 전진
			pos += front[_dir];

			//경로 저장
			_path.push_back(pos);
		}
		// 2. 현재 바라보는 방향을 기준으로 전진할 수 있는지 확인
		else if (CanGo(pos + front[_dir]))
		{
			//앞으로 한 보 전진
			pos += front[_dir];

			//경로 저장
			_path.push_back(pos);
		}
		else
		{
			//왼쪽 방향으로 90도 회전
			_dir = (_dir + 1) % DIR_COUNT;
			
		}

	}

	// 스택을 사용해서 길찾기를 개선하는 방법
	// - 스택에 path를 저장하면서, 다음 path가 현재 스택의 top과 같으면(길을 되돌아 왔으면) 스택을 pop! 
	// => 왔던 길을 다시 되돌아가지 않는다!
	stack<Pos> s;
	for (int i = 0; i < _path.size() - 1; i++)
	{
		if (s.empty() == false && s.top() == _path[i + 1])
			s.pop();
		else
			s.push(_path[i]);
	}

	//마지막 도착점 예외처리
	if (_path.empty() == false)
		s.push(_path.back());


	// 스택 특성상 뒤집어진 경로를 다시 되돌리기
	vector<Pos> path;
	while (s.empty() == false)
	{
		path.push_back(s.top());
		s.pop();
	}

	std::reverse(path.begin(), path.end());
	_path = path;
}

void Player::Bfs()
{
	Pos dest = _board->GetExitPos();
	Pos pos = _pos;

	Pos front[4] =
	{
		Pos{-1,0},	//up
		Pos{0,-1},	//left
		Pos{1,0},	//down
		Pos{0,1},	//right
	};

	//BFS 초기화
	const int32 size = _board->GetSize();
	vector<vector<bool>> discovered(size, vector<bool>(size, false));
	map<Pos, Pos> parent;		// => key에 해당하는 정점이 value에 해당하는 정점으로부터 발견
	
	//BFS 초기값 설정
	queue<Pos> q;
	q.push(pos);
	discovered[pos.y][pos.x] = true; 
	parent[pos] = pos;

	//BFS 시작
	while (q.empty() == false)
	{
		pos = q.front();
		q.pop();

		if (pos == dest)
			break;

		//현 지점에서 상하좌우로 탐색
		for (int32 dir = 0; dir < 4; dir++)
		{
			// 예외1. 갈 수 있는지
			Pos nextPos = pos + front[dir];
			if (CanGo(nextPos) == false)
				continue;

			// 예외2. 이미 탐색한 지역인지
			if (discovered[nextPos.y][nextPos.x])
				continue;

			q.push(nextPos);
			discovered[nextPos.y][nextPos.x] = true;
			parent[nextPos] = pos;
		}
	}

	// BFS로 찾아온 목적지로 부터, 경로를 역추적하여 최단경로 찾기
	_path.clear();
	pos = dest;

	while (true)
	{
		_path.push_back(pos);

		//시작점을 찾으면 종료
		if (pos == parent[pos])
			break;

		pos = parent[pos];	// 역추적
	}
	//순서 뒤집기
	std::reverse(_path.begin(), _path.end());
}


// Astar
// - 이전 우수법과 BFS 길찾기와 다르게, 목적지 개념이 존재!
// - 다익스트라 알고리즘(간선의 가중치를 보며 최적의 노드로 접근)에서 더 나아가서,
//		목적지 부터의 간선 가중치도 고려!  (목적지 개념이 도입되었기 때문에, 각 노드간 점수(최적의 가중치)를 매기는 방법이 달라짐)

// 구현 순서
// 1. 정점 발견에 대한 설계 (인접한 정점을 발견) -> 예약 (좋은 경로를 가진 정점으로)
// 2. 이미 더 좋은 경로를 찾은 상태면 스킵!
// 3. 뒤늦게 더 좋은 경로(노드)가 발견될 수 있음 (예외처리 필수!)


//각 위치를 PQNode로 관리 (각 위치에 대한 점수를 포함) 
struct PQNode		//priorty queue Node
{
	bool operator<(const PQNode& other) const { return f < other.f; }
	bool operator>(const PQNode& other) const { return f > other.f; }

	int32 f;	// f = g + h
	int32 g;
	Pos pos;
};

void Player::AStar()
{
	// 점수 매기기
	// F = G + H 공식을 사용
	// F : 최종 점수 (작을 수록 좋음, 경로에 따라 달라짐)
	// G : 시작점에서 현재 좌표까지 이동하는데 드는 비용
	// H : 목적지에서 얼마나 가까운지	(고정 값)

	Pos start = _pos;
	Pos dest = _board->GetExitPos();
	
	enum
	{
		DIR_COUNT = 4
	};

	Pos front[] =
	{
		Pos{-1,0},	//UP
		Pos{0,-1},	//LEFT
		Pos{1,0},	//DOWN
		Pos{0,1},	//RIGHT
		Pos{-1, -1},// UP_LEFT
		Pos{1, -1}, // DOWN_LEFT
		Pos{1, 1},	// DOWN_RIGHT
		Pos{-1, 1}, // UP_RIGHT
	};

	// 이동 비용
	int32 cost[] =
	{
		10, // UP
		10, // LEFT
		10, // DOWN
		10, // RIGHT
		14, // UP_LEFT 
		14,	// DOWN_LEFT
		14,	// DOWN_RIGHT
		14,	// UP_RIGHT
	};

	const int32 size = _board->GetSize();


	// 방문 위치를 기록	(ClosedList)
	// closed[y][x] -> (y,x)에 방문 했는지에 대한 여부
	vector<vector<bool>> closed(size, vector<bool>(size, false));	

	//각 위치에 대한 점수를 2차원 vector로 유지
	//- best[y][x] -> 지금까지 (y,x) 위치에 대한 가장 좋은 비용 (작을 수록 좋다)
	vector<vector<int32>> best(size, vector<int32>(size, INT32_MAX));

	// 부모 추적 용도
	map<Pos, Pos> parent;	

	// 인접 정점 발견 목록 유지 (OpenList)
	// BFS와 다르게, 이번에는 discoverd를 리스트가 아닌, 우선순위큐로 구현한다!!
	priority_queue<PQNode, vector<PQNode>, greater<PQNode>> pq;		// 최상위 노드를 가장 작은 값으로 유지
	

	//시작점 초기값 설정
	{
		int32 g = 0;	//시작점에서 현재 위치 거리
		int32 h = 10 * (abs(dest.y - start.y) + abs(dest.x - start.x));	//임의의 공식이다. 피타고라스 방식을 사용해도 됨! (이 방법이 연산이 적음)
		pq.push(PQNode{ g + h, h, start });
		best[start.y][start.x] = g + h;
		parent[start] = start;
	}


	// 가장 좋은 후보 찾기	(우선 순위 큐를 사용했기 때문에, 인접 정점을 모두 순회할 필요가 없다)
	while (pq.empty() == false)
	{
		PQNode node = pq.top();
		pq.pop();

		// 동일한 좌표를 더 빠른 경로로 인해 이미 방문된 경우 스킵
		// closed로 판별하거나 INT32_MAX로 초기값 설정한 best[y][x]로 판별하거나 선택!
		if (closed[node.pos.y][node.pos.x])
			continue;
		if (best[node.pos.y][node.pos.x] < node.f)
			continue;

		closed[node.pos.y][node.pos.x] = true;

		if (node.pos == dest)
			break;

		// 현 위치에서 상하좌우로 갈 수 있는 지역 탐색 -> 발견
		for (int32 dir = 0; dir < DIR_COUNT; dir++)
		{
			Pos nextPos = node.pos + front[dir];
			if (CanGo(nextPos) == false)
				continue;
			
			// 비용 계산
			int32 g = node.g + cost[dir];		//현 방문 정점 + 상하좌우로 넘어가 때 드는 비용
			int32 h = 10 * (abs(dest.y - nextPos.y) + abs(dest.x - nextPos.x));
			

			// 이미 다른 경로에서 더 빠른 길을 찾았으면 스킵
			if (best[nextPos.y][nextPos.x] <= g + h)
				continue;


			// 예약 (발견 목록 추가)
			best[nextPos.y][nextPos.x] = g + h;
			pq.push(PQNode{ g + h, g , nextPos });
			parent[nextPos] = node.pos;
		}
	}


	// 찾아온 목적지로 부터, 경로를 역추적하여 최단경로 찾기
	Pos pos = dest;

	_path.clear();
	_pathIndex = 0;
	
	while (true)
	{
		_path.push_back(pos);

		//시작점을 찾으면 종료
		if (pos == parent[pos])
			break;

		pos = parent[pos];	// 역추적
	}
	//순서 뒤집기
	std::reverse(_path.begin(), _path.end());
}