#include "Assist.h"
#include<iostream>
using namespace std;

void Assist::update()
{
	this->x_world = (this->y_map + 0.5 - 6) / 6 * this->mapsize / 2;
	this->z_world = (this->x_map + 0.5 - 3) / 3 * this->mapsize / 4;
	this->nowstep += this->step;
	if (this->nowstep >= this->maxstep)
	{
		this->nowstep = 0;
	}
}

void Assist::check_y(double y)
{
	y *= this->mapsize/2;
	this->y_world = -7.5 + y;
}