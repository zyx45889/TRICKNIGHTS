#include "Metalbox.h"

void Metalbox::cal_world()
{
	this->x_world = (this->y_map +0.5 - 6) / 6 * this->mapsize / 2;
	this->z_world = (this->x_map +0.5 - 3) / 3 * this->mapsize / 4;
	this->y_world = -7;
}