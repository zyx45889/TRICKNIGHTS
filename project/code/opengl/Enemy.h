#pragma once
#include <vector>
#include <string>
#include <fstream>
#include "Map.h"
using namespace std;

/* Class Landscape */

class Enemy
{
public:
	double x_map, y_map;
	double x_world, y_world, z_world;
	double y_base;
	double rotate;
	double step;
	double mapsize;
	int nowdir;
	double blood;
	int state;  // 0:idle 1:runing 2:ÕıÔÚËÀÍö 3:victory 4:dead
	double nowstep;
	int deadtime = 0;
	
	int update(myMap* map);
	void check_y(double y);
	void dead(int maxdeadtime);
};