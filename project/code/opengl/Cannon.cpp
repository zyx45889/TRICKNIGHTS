#include "Cannon.h"

void Cannon::update_bullet()
{
	if (this->nowstep == 0)
	{
		this->a_y = 2 * (this->bullet_y +7.5 ) / (this->maxstep * this->maxstep);
		this->a_x = 2 * (this->bullet_x - this->aim_x) / (this->maxstep * this->maxstep);
		this->a_z = 2 * (this->bullet_z - this->aim_y) / (this->maxstep * this->maxstep);
		this->origin_x = this->bullet_x;
		this->origin_y = this->bullet_y;
		this->origin_z = this->bullet_z;
	}
	else {
		this->bullet_x = this->origin_x - 0.5 * this->a_x * this->nowstep * this->nowstep;
		this->bullet_y = this->origin_y - 0.5 * this->a_y * this->nowstep * this->nowstep;
		this->bullet_z = this->origin_z - 0.5 * this->a_z * this->nowstep * this->nowstep;
		for (int i = 0;i < 20;i++)
		{
			double nowtime = this->nowstep - 1 + (double)i / 20;
			this->bullet_x_history[i]= this->origin_x - 0.5 * this->a_x * nowtime * nowtime;
			this->bullet_y_history[i] = this->origin_y - 0.5 * this->a_y * nowtime * nowtime;
			this->bullet_z_history[i] = this->origin_z - 0.5 * this->a_z * nowtime * nowtime;
		}
	}
	this->nowstep ++;
	if (this->nowstep > this->maxstep)
	{
		this->nowstep = 0;
		this->draw_bullet = 0;
	}
}

void Cannon::update()
{
	this->x_world = (this->y_map + 0.5 - 6) / 6 * this->mapsize / 2;
	this->z_world = (this->x_map + 0.5 - 3) / 3 * this->mapsize / 4;
	this->y_world = -3.5;
}