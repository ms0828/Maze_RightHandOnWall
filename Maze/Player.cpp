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
	// ������ ��Ģ (��� ��Ģ) (�̷� Ž�� �������� ��� ���������� Ž��)

	// ������ �����ϱ� ������ ��� ����
	Pos dest = _board->GetExitPos();
	Pos pos = _pos;

	// ��� �ʱ�ȭ
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
		// 1. ���� �ٶ󺸴� ������ �������� ���������� �� �� �ִ��� Ȯ��
		int32 newDir = (_dir - 1 + DIR_COUNT) % DIR_COUNT;
		if (CanGo(pos + front[newDir]))
		{
			//������ �������� 90�� ȸ��
			_dir = newDir;
			//������ �� �� ����
			pos += front[_dir];

			//��� ����
			_path.push_back(pos);
		}
		// 2. ���� �ٶ󺸴� ������ �������� ������ �� �ִ��� Ȯ��
		else if (CanGo(pos + front[_dir]))
		{
			//������ �� �� ����
			pos += front[_dir];

			//��� ����
			_path.push_back(pos);
		}
		else
		{
			//���� �������� 90�� ȸ��
			_dir = (_dir + 1) % DIR_COUNT;
			
		}

	}

	// ������ ����ؼ� ��ã�⸦ �����ϴ� ���
	// - ���ÿ� path�� �����ϸ鼭, ���� path�� ���� ������ top�� ������(���� �ǵ��� ������) ������ pop! 
	// => �Դ� ���� �ٽ� �ǵ��ư��� �ʴ´�!
	stack<Pos> s;
	for (int i = 0; i < _path.size() - 1; i++)
	{
		if (s.empty() == false && s.top() == _path[i + 1])
			s.pop();
		else
			s.push(_path[i]);
	}

	//������ ������ ����ó��
	if (_path.empty() == false)
		s.push(_path.back());


	// ���� Ư���� �������� ��θ� �ٽ� �ǵ�����
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

	//BFS �ʱ�ȭ
	const int32 size = _board->GetSize();
	vector<vector<bool>> discovered(size, vector<bool>(size, false));
	map<Pos, Pos> parent;		// => key�� �ش��ϴ� ������ value�� �ش��ϴ� �������κ��� �߰�
	
	//BFS �ʱⰪ ����
	queue<Pos> q;
	q.push(pos);
	discovered[pos.y][pos.x] = true; 
	parent[pos] = pos;

	//BFS ����
	while (q.empty() == false)
	{
		pos = q.front();
		q.pop();

		if (pos == dest)
			break;

		//�� �������� �����¿�� Ž��
		for (int32 dir = 0; dir < 4; dir++)
		{
			// ����1. �� �� �ִ���
			Pos nextPos = pos + front[dir];
			if (CanGo(nextPos) == false)
				continue;

			// ����2. �̹� Ž���� ��������
			if (discovered[nextPos.y][nextPos.x])
				continue;

			q.push(nextPos);
			discovered[nextPos.y][nextPos.x] = true;
			parent[nextPos] = pos;
		}
	}

	// BFS�� ã�ƿ� �������� ����, ��θ� �������Ͽ� �ִܰ�� ã��
	_path.clear();
	pos = dest;

	while (true)
	{
		_path.push_back(pos);

		//�������� ã���� ����
		if (pos == parent[pos])
			break;

		pos = parent[pos];	// ������
	}
	//���� ������
	std::reverse(_path.begin(), _path.end());
}
