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
	double draw_bullet;   // �Ƿ��ӵ�
	int aim_enemy;  // Ŀ����˵�id
	double bullet_x, bullet_y, bullet_z;   //  �ӵ���ǰλ��
	double origin_x, origin_y, origin_z;   // �ӵ���ʼλ��
	double bullet_x_history[20], bullet_y_history[20], bullet_z_history[20];  //�ӵ���ʷλ�ã����������������
	double aim_x,aim_y;   // �ӵ���Ŀ��λ��
	double a_x, a_y, a_z;    //�ӵ��ļ��ٶ�
	double nowstep;    // �ӵ��ѷ������ʱ��
	double maxstep;    // �ӵ�����ʱ��Ҫ����
	double mapsize;
	int state;  // 0:idle 1:runing 2:dead 3:victory

	void update_bullet();
	void update();
};