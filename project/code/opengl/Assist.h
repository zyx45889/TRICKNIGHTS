#pragma once
#include <vector>
#include <string>
#include <fstream>
using namespace std;

class Assist
{
public:
	double x_map, y_map;
	double x_world, y_world, z_world;
	double step;
	double mapsize;
	int state;  // 0:idle 1:runing 2:dead 3:victory
	double nowstep;
	double maxstep;

	void update();
	void check_y(double y);
};