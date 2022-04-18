#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <GL/glut.h>
using namespace std;

class SkyDome
{
public:
	int R;
	double height;
	void output_obj(const char* file_name);
};