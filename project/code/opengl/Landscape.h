#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <GL/glut.h>
#define N_height 6
#define N_width 12
#define N_size 20
using namespace std;

/* Class Landscape */

class Landscape
{
public:
	void load_y_map(const char* file_name);
	void output_obj(const char* file_name);

	int height, width;	//地图长宽
	int size;	//每个方格有多少个点
	double y_map[N_height * N_size][N_width * N_size];  //地图的高度

};