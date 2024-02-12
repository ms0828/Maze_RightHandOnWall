#include"pch.h"
#include "Player.h"
#include "Board.h"
#include<stack>
void Player::Init(Board* board)
{
	_pos = board->GetEnterPos();
	_board = board;

	
	// 오른손 법칙 (우수 법칙) (미로 탐색 과정에서 계속 오른쪽으로 탐색)

	// 목적지 도착하기 전에는 계속 실행
	Pos dest = board->GetExitPos();
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
		else if (CanGo(pos+front[_dir]))
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
			/*switch (_dir)
			{
			case DIR_UP:
				break;
			case DIR_LEFT:
				break;
			case DIR_DOWN:
				break;
			case DIR_RIGHT:
				break;
			}*/
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
