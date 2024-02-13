#include"pch.h"
#include "Player.h"
#include "Board.h"
#include<stack>
void Player::Init(Board* board)
{
	_pos = board->GetEnterPos();
	_board = board;

	//RightHand();
	Bfs();
	

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
