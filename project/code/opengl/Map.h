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
	int height, width;	//��ͼ����
	int map[6][12];	//��ͼ���� 0��ɽ�� 1����������/�����λ�� 2������ľ���ӵ�λ�� 3�����������ӵ�λ�� 4�������� 5�����ص�

	void initialize();
	double getheight(double x_map,double y_map,Landscape* landscape);
	double getheight_box(double x_map, double y_map, Landscape* landscape);
};