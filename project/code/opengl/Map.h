#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <GL/glut.h>
#include "Landscape.h"
using namespace std;

/* Class Landscape */

class myMap
{
public:
	int height, width;	//地图长宽
	int map[6][12];	//地图性质 0：山地 1：可以行走/部署的位置 2：部署木箱子的位置 3：部署铁箱子的位置 4：出生点 5：防守点

	void initialize();
	double getheight(double x_map,double y_map,Landscape* landscape);
	double getheight_box(double x_map, double y_map, Landscape* landscape);
};