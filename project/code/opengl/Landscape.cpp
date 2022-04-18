#include "Landscape.h"
#include <opencv2/opencv.hpp>
#include <fstream>
using namespace cv;
using namespace std;

struct myNode {
	double x;
	double y;
	double z;
};

void Landscape::load_y_map(const char* file_name)
{
	Mat img = imread(file_name);
	resize(img, img, Size(this->width * this->size,this->height*this->size));
	//cout << img.size<<endl;
	int min = 255, max = 0;
	for (int i=0;i< this->height * this->size;i++)
		for (int j = 0;j < this->width * this->size;j++)
		{
			//cout << this->height * this->size - 1 - i <<" "<<j<< endl;
			this->y_map[i][j] = img.at<Vec3b>(this->height * this->size - 1 - i, j)[0];
			if (min > this->y_map[i][j])
			{
				min = this->y_map[i][j];
			}
			if (max < this->y_map[i][j])
			{
				max = this->y_map[i][j];
			}
		}
	for (int i = 0;i < this->height * this->size;i++)
		for (int j = 0;j < this->width * this->size;j++)
		{
			y_map[i][j] = 1-(y_map[i][j] - min) / (max - min);
			y_map[i][j] *= 10;
		}
}

myNode cross(myNode a, myNode b)
{
	//叉乘：((aybz-azby),(azbx-axbz),(axby-aybx))
	myNode ret;
	ret.x = a.y * b.z - a.z * b.y;
	ret.y = a.z * b.x - a.x * b.z;
	ret.z = a.x * b.y - a.y * b.x;
	return ret;
}

myNode sub(myNode a, myNode b)
{
	myNode ret;
	ret.x = a.x - b.x;
	ret.y = a.y - b.y;
	ret.z = a.z - b.z;
	return ret;
}

void Landscape::output_obj(const char* file_name)
{
	ofstream outputfile;
	outputfile.open(file_name);
	vector<myNode> ft;

	//生成所有的顶点坐标和顶点纹理坐标
	for (int i = 0;i < this->height * this->size;i++)
		for (int j = 0;j < this->width * this->size;j++)
		{
			outputfile << "v " << j << " " << this->y_map[i][j] << " " << i<<endl;
		}
	for (int i = 0;i < this->height * this->size;i++)
		for (int j = 0;j < this->width * this->size;j++)
		{
			outputfile << "vt " << (double)i/(double)(this->height * this->size) << " " << (double)j/(double)(this->width * this->size)<<endl;
		}

	//生成ft，每个像素两个三角形
	for (int i = 0;i < this->height * this->size-1;i++)
		for (int j = 0;j < this->width * this->size-1;j++)
		{
			myNode a, b, c, d, t1, t2;
			a.x = j;
			a.y = this->y_map[i][j];
			a.z = i;
			b.x = j + 1;
			b.y = this->y_map[i][j + 1];
			b.z = i;
			c.x = j;
			c.y = this->y_map[i + 1][j];
			c.z = i + 1;
			d.x = j + 1;
			d.y = this->y_map[i + 1][j + 1];
			d.z = i + 1;
			//求三角形法向量的方法：按逆时针取出任意两点进行叉乘
			t1 = cross(sub(b, a), sub(c, b));
			t2 = cross(sub(d, b), sub(c, d));

			ft.push_back(t1);
			ft.push_back(t2);
			//生成顶点法向量的方法：取相邻面的法向量的均值
		}

	// 生成vn
	int h = this->height * this->size;
	int w = this->width * this->size;
	w--;
	for (int i = 0;i < this->height * this->size;i++)
		for (int j = 0;j < this->width * this->size;j++)
		{
			if (i == 0 || j == 0 || i == this->height * this->size - 1 || j == this->width * this->size-1)
			{
				outputfile << "vn 0 1 0" << endl;
			}
			else
			{
				double x, y, z;
				x = ft.at(((i - 1) * w + j - 1) * 2 + 1).x + ft.at(((i - 1) * w + j) * 2).x + ft.at(((i - 1) * w + j) * 2 + 1).x +
					ft.at(((i)*w + j - 1) * 2).x + ft.at(((i)*w + j - 1) * 2).x + ft.at(((i)*w + j) * 2).x;
				y = ft.at(((i - 1) * w + j - 1) * 2 + 1).y + ft.at(((i - 1) * w + j) * 2).y + ft.at(((i - 1) * w + j) * 2 + 1).y +
					ft.at(((i)*w + j - 1) * 2).y + ft.at(((i)*w + j - 1) * 2).y + ft.at(((i)*w + j) * 2).y;
				z = ft.at(((i - 1) * w + j - 1) * 2 + 1).z + ft.at(((i - 1) * w + j) * 2).z + ft.at(((i - 1) * w + j) * 2 + 1).z +
					ft.at(((i)*w + j - 1) * 2).z + ft.at(((i)*w + j - 1) * 2).z + ft.at(((i)*w + j) * 2).z;
				outputfile << "vn " << x / 6 << " " << y / 6 << " " << z / 6 << endl;
			}
		}
	outputfile << "usemtl material" << endl;
	w ++;
	for (int i = 0;i < this->height * this->size-1;i++)
		for (int j = 0;j < this->width * this->size-1;j++)
		{
			int a, b, c, d;
			a = i * w + j+1;
			b = i * w + j + 2;
			c = (i + 1) * w + j+1;
			d = (i + 1) * w + j + 2;
			outputfile << "f " << a << "/" << a << "/" << a << " " << b << "/" << b << "/" << b << " " << c << "/" << c << "/" << c << endl;
			outputfile << "f " << d << "/" << d << "/" << d << " " << b << "/" << b << "/" << b << " " << c << "/" << c << "/" << c << endl;
		}

	outputfile.close();
}