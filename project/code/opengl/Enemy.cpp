#include "Enemy.h"
#include<iostream>
#define N_height 6
#define N_width 12
int path[N_height*N_width];//记录方向：1 左 2 右 3 上 4 下
int flag[N_height][N_width];
int mincost;
int mindir;
using namespace std;

void DFS(int nowx,int nowy,int cost,myMap* map)
{
	if (flag[nowx][nowy] == 1)
		return;
	flag[nowx][nowy] = 1;
	/*for (int i = 0;i < 6;i++)
	{
		for (int j = 0;j < 12;j++)
		{
			if (i == nowx &&j == nowy)
			{
				cout << "* ";
			}
			else cout << map->map[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;*/
	if (map->map[nowx][nowy] == 5)
	{
		if (cost < mincost)
		{
			mincost = cost;
			mindir = path[0];
			//for (int i = 0;i < cost;i++)
			//{
			//	cout << path[i] << " ";
			//}
			//cout << endl;
		}
	}
	if (nowy != 0 && (map->map[nowx][nowy - 1] == 1 || map->map[nowx][nowy - 1] == 2|| map->map[nowx][nowy - 1] == 5))
	{
		path[cost] = 1;
		DFS(nowx, nowy - 1, cost + 1, map);
	}
	if (nowy != map->height-1 && (map->map[nowx][nowy + 1] == 1 || map->map[nowx][nowy + 1] == 2|| map->map[nowx][nowy + 1] == 5))
	{
		path[cost] = 2;
		DFS(nowx, nowy + 1, cost + 1, map);
	}
	if (nowx != 0 && (map->map[nowx-1][nowy] == 1 || map->map[nowx-1][nowy] == 2|| map->map[nowx - 1][nowy] == 5))
	{
		path[cost] = 3;
		DFS(nowx-1, nowy, cost + 1, map);
	}
	if (nowx != map->width-1 && (map->map[nowx + 1][nowy] == 1 || map->map[nowx + 1][nowy] == 2|| map->map[nowx + 1][nowy] == 5))
	{
		path[cost] = 4;
		DFS(nowx + 1, nowy, cost + 1, map);
	}
	flag[nowx][nowy] = 0;
}

int Enemy::update(myMap* map)
{
	if (this->state == 2)
	{
		this->deadtime--;
		if (this->deadtime == 0)
		{
			this->state = 4;
		}
		return 1;
	}
	mincost = 999999999;
	for (int i = 0;i < N_height;i++)
		for (int j = 0;j < N_width;j++)
			flag[i][j] = 0;
	/*for (int i = 0;i < 6;i++)
	{
		for (int j = 0;j < 12;j++)
		{
			cout << map->map[i][j] << " ";
		}
		cout << endl;
	}*/
	if (map->map[(int)this->x_map][(int)this->y_map] == 5)
	{
		this->state = 3;
		return 0;
	}
	if ((int)(this->x_map * 10) % 10 < 5 || (int)(this->y_map * 10) % 10 < 5)
	{
		mindir = this->nowdir;
		mincost = 0;
	}
	else DFS((int)this->x_map, (int)this->y_map, 0, map);
	if (mincost == 999999999)
		return 0;
	//cout << mindir << endl;
	switch (mindir)
	{
		case 1:
			this->y_map -= this->step;
			break;
		case 2:
			this->y_map += this->step;
			break;
		case 3:
			this->x_map -= this->step;
			break;
		case 4:
			this->x_map += this->step;
			break;

	}
	this->nowdir = mindir;
	this->x_world = (this->y_map - 6) / 6 * this->mapsize/2;
	this->z_world = (this->x_map - 3) / 3 * this->mapsize/4;
	this->y_world = this->y_base+1-4*(this->nowstep-0.5)*(this->nowstep-0.5);
	this->nowstep += this->step;
}

void Enemy::dead(int maxdeadtime)
{
	this->state = 2;
	this->deadtime = maxdeadtime;
}

void Enemy::check_y(double y)
{
	y *= this->mapsize;
	if (this->y_world < -8.1 + y)
	{
		this->y_world = -8.1 + y;
		this->nowstep = 0;
		this->y_base = -8.1 + y;
	}
}