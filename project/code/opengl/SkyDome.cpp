#include "SkyDome.h"
#include <math.h>

void SkyDome::output_obj(const char* file_name)
{
	ofstream outputfile;
	outputfile.open(file_name);
	for (int i = 0;i <= this->R;i++)
		for (int j = 0;j <= this->R;j++)
		{
			double r = sqrt((double)(this->R * this->R) - (double)(j * j));
			double x = sin((double)i / (double)this->R * 2 * 3.1415926535), y = cos((double)i / (double)this->R * 2 * 3.1415926535);
			outputfile << "v " << r * x << " "<<j<<" " << r * y << endl;
		}
	for (int i = 0;i <= this->R;i++)
		for (int j = 0;j <= this->R;j++)
		{
			outputfile << "vt " << (double)j / (double)this->R <<" "<< (double)i / (double)this->R << endl;
		}
	for (int i = 0;i <= this->R;i++)
		for (int j = 0;j <= this->R;j++)
		{
			double r = sqrt((double)(this->R * this->R) - (double)(j * j));
			double x = sin((double)i / (double)this->R * 2 * 3.1415926535), y = cos((double)i / (double)this->R * 2 * 3.1415926535);
			outputfile << "vn " << -r * x << " " << -j << " " << -r * y << endl;
		}
	outputfile << "usemtl material" << endl;
	for (int i = 0;i < this->R;i++)
		for (int j = 0;j < this->R;j++)
		{
			int a, b, c, d;
			a = i * (this->R+1) + j + 1;
			b = i * (this->R+1) + j + 2;
			d = (i + 1) * (this->R+1) + j + 1;
			c = (i + 1) * (this->R+1) + j + 2;
			outputfile << "f " << a << "/" << a << "/" << a << " " << b << "/" << b << "/" << b << " " << c << "/" << c << "/" << c <<" "<< d << "/" << d << "/" << d << endl;
		}
}