#pragma once
#include <string>
#include <fstream>
using namespace std;

class Metalbox
{
public:
	double x_map, y_map;
	double x_world, y_world, z_world;
	int state;  // 0:idle 1:placed
	double mapsize;

	void cal_world();
};