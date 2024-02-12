#include"pch.h"
#include "Player.h"
#include "Board.h"
#include<stack>
void Player::Init(Board* board)
{
	_pos = board->GetEnterPos();
	_board = board;

	
	// ������ ��Ģ (��� ��Ģ) (�̷� Ž�� �������� ��� ���������� Ž��)

	// ������ �����ϱ� ������ ��� ����
	Pos dest = board->GetExitPos();
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
		else if (CanGo(pos+front[_dir]))
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
