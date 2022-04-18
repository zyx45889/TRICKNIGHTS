#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include "Model.h"
#define N_height 30
#define N_width 30
#define N_size 20
using namespace std;

class Water
{
public:
	double height, width;	//landscape同理 高度宽度和每个格子多少个像素
	double size;
	double hollow_height, hollow_width;
	double vertex[N_height * N_size][N_width * N_size][3]; // 每个点的xyz

	void cal_vertex(double time);
	void cal_obj(Model* object);
	void save_obj(const char* file_name);
	void draw_water(double time,Model* object,double tx,double ty,double tz,double size);
};