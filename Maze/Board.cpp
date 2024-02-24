#include"pch.h"
#include "Board.h"
#include"ConsoleHelper.h"
#include"Player.h"
#include"DisjointSet.h"

const char* TILE = "■";

Board::Board()
{
}

Board::~Board()
{
}

void Board::Init(int32 size, Player* player)
{
    _size = size;
    _player = player;
    GenerateMap();
}

void Board::Render()
{
    ConsoleHelper::SetCursorPosition(0, 0);
    ConsoleHelper::ShowConsoleCursor(false);


    for (int32 y = 0; y < 25; y++)
    {
        for (int32 x = 0; x < 25; x++)
        {
            ConsoleColor color = GetTileColor(Pos{ y, x });
            ConsoleHelper::SetCursorColor(color);
            cout << TILE;
        }
        cout << endl;
    }

}

void Board::GenerateMap()
{

// Binary Tree 미로 생성 알고리즘
//#pragma region Mazes For Porgrammer's 라는 해외 책에서 나온 알고리즘
//    for (int32 y = 0; y < _size; y++)
//    {
//        for (int32 x = 0; x < _size; x++)
//        {
//            if (x % 2 ==0 || y % 2 ==0)
//            {
//                _tile[y][x] = TileType::WALL;
//            }
//            else
//            {
//                _tile[y][x] = TileType::EMPTY;
//            }
//        }
//    }
//    
//    // 랜덤으로 우측 혹은 아래로 길을 뚫는 작업
//    for (int32 y = 0; y < _size; y++)
//    {
//        for (int32 x = 0; x < _size; x++)
//        {
//            if (x % 2 == 0 || y % 2 == 0)
//                continue;
//
//            if (y == _size - 2 && x == _size - 2)
//            {
//                continue;
//            }
//
//            if (y == _size - 2)
//            {
//                _tile[y][x + 1] = TileType::EMPTY;
//                continue;
//            }
//
//            if (x == _size - 2)
//            {
//                _tile[y + 1][x] = TileType::EMPTY;
//                continue;
//            }
//
//            const int32 randValue = ::rand() % 2;
//            if (randValue == 0)
//            {
//                _tile[y][x + 1] = TileType::EMPTY;
//            }
//            else
//            {
//                _tile[y + 1][x] = TileType::EMPTY;
//            }
//        }
//    }
//#pragma endregion

#pragma Kruskal 알고리즘을 이용한 맵 생성

    for (int32 y = 0; y < _size; y++)
    {
        for (int32 x = 0; x < _size; x++)
        {
            if (x % 2 ==0 || y % 2 ==0)
            {
                _tile[y][x] = TileType::WALL;
            }
            else
            {
                _tile[y][x] = TileType::EMPTY;
            }
        }
    }

    vector<CostEdge> edges;
    // edegs 후보를 랜덤 cost로 등록
    for(int32 y = 0; y < _size; y++)
    {
        for (int32 x = 0; x < _size; x++)
        {
            if (x % 2 == 0 || y % 2 == 0)
                continue;

            // 우측 연결 간선 후보
            if (x < _size - 2)
            {
                const int32 randValue = ::rand() % 100;
                edges.push_back(CostEdge{ randValue, Pos{y,x}, Pos{y,x + 2} });
            }

            // 아래 연결 간선 후보
            if (y < _size - 2)
            {
                const int32 randValue = ::rand() % 100;
                edges.push_back(CostEdge{ randValue, Pos{y, x}, Pos{y + 2,x} });
            }
        }
    }
    
    std::sort(edges.begin(), edges.end());
    

    DisjointSet sets(_size * _size);
    for (CostEdge& edge : edges)
    {
        // u와 v가 Pos이므로, 임의 공식을 적용하여 특정 인덱스 추출
        // - 2차원 좌표를 1차원 배열의 인덱스로 변환하는 공식
        int u = edge.u.y * _size + edge.u.x;
        int v = edge.v.y * _size + edge.v.x;

        //같은 그룹이면 스킵
        if (sets.Find(u) == sets.Find(v))
            continue;

        sets.Merge(u, v);


        // 맵 길 뚫기
        int y = (edge.u.y + edge.v.y) / 2;
        int x = (edge.u.x + edge.v.x) / 2;
        _tile[y][x] = TileType::EMPTY;
        
    }


#pragma endregion
}

TileType Board::GetTileType(Pos pos)
{
    if (pos.x < 0 || pos.x >= _size)
        return TileType::NONE;
    if (pos.y < 0 || pos.y >= _size)
        return TileType::NONE;

    return _tile[pos.y][pos.x];
}

ConsoleColor Board::GetTileColor(Pos pos)
{
    if (_player && _player->GetPos() == pos)
        return ConsoleColor::YELLOW;

    if (GetExitPos() == pos)
        return ConsoleColor::BLUE;

    TileType tileType = GetTileType(pos);

    switch (tileType)
    {
        case TileType::EMPTY:
            return ConsoleColor::GREEN;
        case TileType::WALL:
            return ConsoleColor::RED;
    }

    return ConsoleColor::WHITE;
}
