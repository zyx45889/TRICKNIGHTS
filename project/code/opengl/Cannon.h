#pragma once
#include <vector>
#include <string>
#include <fstream>
using namespace std;

class Cannon
{
public:
	double x_map, y_map;
	double x_world, y_world, z_world;
	double dir;
	double draw_bullet;   // 是否画子弹
	int aim_enemy;  // 目标敌人的id
	double bullet_x, bullet_y, bullet_z;   //  子弹当前位置
	double origin_x, origin_y, origin_z;   // 子弹初始位置
	double bullet_x_history[20], bullet_y_history[20], bullet_z_history[20];  //子弹历史位置，用来画后面的虚线
	double aim_x,aim_y;   // 子弹的目标位置
	double a_x, a_y, a_z;    //子弹的加速度
	double nowstep;    // 子弹已发射多少时间
	double maxstep;    // 子弹多少时间要打中
	double mapsize;
	int state;  // 0:idle 1:runing 2:dead 3:victory

	void update_bullet();
	void update();
};