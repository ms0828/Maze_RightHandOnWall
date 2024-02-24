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


// Astar
// - ���� ������� BFS ��ã��� �ٸ���, ������ ������ ����!
// - ���ͽ�Ʈ�� �˰���(������ ����ġ�� ���� ������ ���� ����)���� �� ���ư���,
//		������ ������ ���� ����ġ�� ���!  (������ ������ ���ԵǾ��� ������, �� ��尣 ����(������ ����ġ)�� �ű�� ����� �޶���)

// ���� ����
// 1. ���� �߰߿� ���� ���� (������ ������ �߰�) -> ���� (���� ��θ� ���� ��������)
// 2. �̹� �� ���� ��θ� ã�� ���¸� ��ŵ!
// 3. �ڴʰ� �� ���� ���(���)�� �߰ߵ� �� ���� (����ó�� �ʼ�!)


//�� ��ġ�� PQNode�� ���� (�� ��ġ�� ���� ������ ����) 
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
	// ���� �ű��
	// F = G + H ������ ���
	// F : ���� ���� (���� ���� ����, ��ο� ���� �޶���)
	// G : ���������� ���� ��ǥ���� �̵��ϴµ� ��� ���
	// H : ���������� �󸶳� �������	(���� ��)

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

	// �̵� ���
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


	// �湮 ��ġ�� ���	(ClosedList)
	// closed[y][x] -> (y,x)�� �湮 �ߴ����� ���� ����
	vector<vector<bool>> closed(size, vector<bool>(size, false));	

	//�� ��ġ�� ���� ������ 2���� vector�� ����
	//- best[y][x] -> ���ݱ��� (y,x) ��ġ�� ���� ���� ���� ��� (���� ���� ����)
	vector<vector<int32>> best(size, vector<int32>(size, INT32_MAX));

	// �θ� ���� �뵵
	map<Pos, Pos> parent;	

	// ���� ���� �߰� ��� ���� (OpenList)
	// BFS�� �ٸ���, �̹����� discoverd�� ����Ʈ�� �ƴ�, �켱����ť�� �����Ѵ�!!
	priority_queue<PQNode, vector<PQNode>, greater<PQNode>> pq;		// �ֻ��� ��带 ���� ���� ������ ����
	

	//������ �ʱⰪ ����
	{
		int32 g = 0;	//���������� ���� ��ġ �Ÿ�
		int32 h = 10 * (abs(dest.y - start.y) + abs(dest.x - start.x));	//������ �����̴�. ��Ÿ��� ����� ����ص� ��! (�� ����� ������ ����)
		pq.push(PQNode{ g + h, h, start });
		best[start.y][start.x] = g + h;
		parent[start] = start;
	}


	// ���� ���� �ĺ� ã��	(�켱 ���� ť�� ����߱� ������, ���� ������ ��� ��ȸ�� �ʿ䰡 ����)
	while (pq.empty() == false)
	{
		PQNode node = pq.top();
		pq.pop();

		// ������ ��ǥ�� �� ���� ��η� ���� �̹� �湮�� ��� ��ŵ
		// closed�� �Ǻ��ϰų� INT32_MAX�� �ʱⰪ ������ best[y][x]�� �Ǻ��ϰų� ����!
		if (closed[node.pos.y][node.pos.x])
			continue;
		if (best[node.pos.y][node.pos.x] < node.f)
			continue;

		closed[node.pos.y][node.pos.x] = true;

		if (node.pos == dest)
			break;

		// �� ��ġ���� �����¿�� �� �� �ִ� ���� Ž�� -> �߰�
		for (int32 dir = 0; dir < DIR_COUNT; dir++)
		{
			Pos nextPos = node.pos + front[dir];
			if (CanGo(nextPos) == false)
				continue;
			
			// ��� ���
			int32 g = node.g + cost[dir];		//�� �湮 ���� + �����¿�� �Ѿ �� ��� ���
			int32 h = 10 * (abs(dest.y - nextPos.y) + abs(dest.x - nextPos.x));
			

			// �̹� �ٸ� ��ο��� �� ���� ���� ã������ ��ŵ
			if (best[nextPos.y][nextPos.x] <= g + h)
				continue;


			// ���� (�߰� ��� �߰�)
			best[nextPos.y][nextPos.x] = g + h;
			pq.push(PQNode{ g + h, g , nextPos });
			parent[nextPos] = node.pos;
		}
	}


	// ã�ƿ� �������� ����, ��θ� �������Ͽ� �ִܰ�� ã��
	Pos pos = dest;

	_path.clear();
	_pathIndex = 0;
	
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