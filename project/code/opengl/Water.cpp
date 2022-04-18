#include "Water.h"
#include<iostream>
#include <GL/glut.h>

void Water::cal_vertex(double time)   //水面正弦波数组的计算
{
    double base = this->width / 2;
    for (int i = 0; i < this->height*this->size; i++)
        for(int j=0;j<this->width*this->size;j++)
        {
            this->vertex[i][j][0] = (double)i / this->size - base;
            this->vertex[i][j][1] = 0;  //该部分的值后续是要动态计算的,因而此处赋值多少都可以
            this->vertex[i][j][2] = (double)j / this->size - base;
        }
    for (int i = 0; i < this->height * this->size; i++)
        for (int j = 0;j < this->width * this->size; j++)
        {
			if (i > (this->height / 2 - this->hollow_height / 2) * this->size && i< (this->height / 2 + this->hollow_height / 2) * this->size &&
				j>(this->width / 2 - this->hollow_width / 2) * this->size && j < (this->width / 2 + this->hollow_width / 2) * this->size)
			{
				continue;
			}
            const int WaveNumber = 18;
			double d[WaveNumber];
			for (int x = 0;x < 3;x++)
				for (int y = 0;y < 6;y++)
				{
					d[x*6+y]=sqrt((this->vertex[i][j][0]-((double)x*2-3)) * (this->vertex[i][j][0]-((double)x*2-3)) +
						(this->vertex[i][j][2]-((double)y*2-6)) * (this->vertex[i][j][2]-((double)y*2-6)));
				}
            // 最高高度和landscape统一，为10(然后就写了9)(/size)
            double sum = 0;
            for (int k = 0; k < WaveNumber; k++)
            {
                sum += 0.9/this->size/WaveNumber * sin(d[k] * 180.0 / (double)WaveNumber * 3.14 + time * 0.2 * 3.14);
            }
            vertex[i][j][1] = sum;
        }
}

void Water::draw_water(double time, Model* object, double tx, double ty, double tz, double size)
{
	GLint last_texture_ID;
	this->cal_vertex(time);
	glPushMatrix();
	glTranslatef(tx, ty, tz);
	glScalef(size, size, size);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
	for (int i = 0;i < object->face_list.size();i++)
	{
		vector<int>tri = object->face_list.at(i);
		string material_name = object->face_material.at(i);
		GLfloat blue[] = { 0.082, 0.186, 0.255, 0.5 };
		GLfloat white[] = { 1.0, 1.0, 1.0, 0.5 };
		double maxy = 0.02;
		GLfloat shininess[] = { 10.0 };
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
		glBegin(GL_POLYGON);
		for (int j = 0;j < tri.size();j += 3)
		{
			int vertexid = tri.at(j);
			double nowy = this->vertex[vertexid / (int)(this->width * this->size)][vertexid % (int)(this->width * this->size)][1];
			GLfloat color[] = { nowy / maxy * white[0] + (maxy - nowy) / maxy * blue[0] ,
								nowy / maxy * white[1] + (maxy - nowy) / maxy * blue[1] ,
								nowy / maxy * white[2] + (maxy - nowy) / maxy * blue[2],0.5 };
			glMaterialfv(GL_FRONT, GL_SPECULAR, white);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
			int value = tri.at(j + 2);
			glNormal3f(object->normal_list.at(3 * (value - 1)), object->normal_list.at(3 * (value - 1) + 1), object->normal_list.at(3 * (value - 1) + 2));
			value = tri.at(j);
			glVertex3f(object->vertex_list.at(3 * (value - 1)), object->vertex_list.at(3 * (value - 1) + 1), object->vertex_list.at(3 * (value - 1) + 2));
		}
		glEnd();
	}
	glBindTexture(GL_TEXTURE_2D, last_texture_ID);
	glPopMatrix();
}

struct myNode_water {
    double x;
    double y;
    double z;
};

myNode_water cross(myNode_water a, myNode_water b)
{
    //叉乘：((aybz-azby),(azbx-axbz),(axby-aybx))
    myNode_water ret;
    ret.x = a.y * b.z - a.z * b.y;
    ret.y = a.z * b.x - a.x * b.z;
    ret.z = a.x * b.y - a.y * b.x;
    return ret;
}

myNode_water sub(myNode_water a, myNode_water b)
{
    myNode_water ret;
    ret.x = a.x - b.x;
    ret.y = a.y - b.y;
    ret.z = a.z - b.z;
    return ret;
}

void Water::cal_obj(Model* object)
{
	vector<myNode_water> ft;

	//生成所有的顶点坐标和顶点纹理坐标
	for (int i = 0;i < this->height * this->size;i++)
		for (int j = 0;j < this->width * this->size;j++)
		{
			object->vertex_list.push_back(this->vertex[i][j][0]);
			object->vertex_list.push_back(this->vertex[i][j][1]);
			object->vertex_list.push_back(this->vertex[i][j][2]);
		}
	for (int i = 0;i < this->height * this->size;i++)
		for (int j = 0;j < this->width * this->size;j++)
		{
			object->texture_list.push_back((double)i / (double)(this->height * this->size));
			object->texture_list.push_back((double)j / (double)(this->width * this->size));
		}

	//生成ft，每个像素两个三角形
	for (int i = 0;i < this->height * this->size - 1;i++)
		for (int j = 0;j < this->width * this->size - 1;j++)
		{
			myNode_water a, b, c, d, t1, t2;
			a.x = this->vertex[i][j][0];
			a.y = this->vertex[i][j][1];
			a.z = this->vertex[i][j][2];
			b.x = this->vertex[i][j + 1][0];
			b.y = this->vertex[i][j + 1][1];
			b.z = this->vertex[i][j + 1][2];
			c.x = this->vertex[i + 1][j][0];
			c.y = this->vertex[i + 1][j][1];
			c.z = this->vertex[i + 1][j][2];
			d.x = this->vertex[i + 1][j + 1][0];
			d.y = this->vertex[i + 1][j + 1][1];
			d.z = this->vertex[i + 1][j + 1][2];
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
			if (i == 0 || j == 0 || i == this->height * this->size - 1 || j == this->width * this->size - 1)
			{
				object->normal_list.push_back(0);
				object->normal_list.push_back(1);
				object->normal_list.push_back(0);
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
				object->normal_list.push_back(x / 6);
				object->normal_list.push_back(y / 6);
				object->normal_list.push_back(z / 6);
			}
		}
	w++;
	for (int i = 0;i < this->height * this->size - 1;i++)
		for (int j = 0;j < this->width * this->size - 1;j++)
		{
			int a, b, c, d;
			a = i * w + j + 1;
			b = i * w + j + 2;
			c = (i + 1) * w + j + 1;
			d = (i + 1) * w + j + 2;
			std::vector<int> temp1;
			temp1.push_back(a);
			temp1.push_back(a);
			temp1.push_back(a);
			temp1.push_back(b);
			temp1.push_back(b);
			temp1.push_back(b);
			temp1.push_back(c);
			temp1.push_back(c);
			temp1.push_back(c);
			object->face_list.push_back(temp1);
			object->face_material.push_back("material");
			std::vector<int> temp2;
			temp2.push_back(d);
			temp2.push_back(d);
			temp2.push_back(d);
			temp2.push_back(b);
			temp2.push_back(b);
			temp2.push_back(b);
			temp2.push_back(c);
			temp2.push_back(c);
			temp2.push_back(c);
			object->face_list.push_back(temp2);
			object->face_material.push_back("material");
		}
}

void Water::save_obj(const char* file_name)
{
	ofstream outputfile;
	outputfile.open(file_name);
	vector<myNode_water> ft;

	//生成所有的顶点坐标和顶点纹理坐标
	for (int i = 0;i < this->height * this->size;i++)
		for (int j = 0;j < this->width * this->size;j++)
		{
			outputfile << "v " << this->vertex[i][j][0] << " " << this->vertex[i][j][1] << " " << this->vertex[i][j][2] << endl;
		}
	for (int i = 0;i < this->height * this->size;i++)
		for (int j = 0;j < this->width * this->size;j++)
		{
			outputfile << "vt " << (double)i / (double)(this->height * this->size) << " " << (double)j / (double)(this->width * this->size) << endl;
		}

	//生成ft，每个像素两个三角形
	for (int i = 0;i < this->height * this->size - 1;i++)
		for (int j = 0;j < this->width * this->size - 1;j++)
		{
			myNode_water a, b, c, d, t1, t2;
			a.x = this->vertex[i][j][0];
			a.y = this->vertex[i][j][1];
			a.z = this->vertex[i][j][2];
			b.x = this->vertex[i][j + 1][0];
			b.y = this->vertex[i][j + 1][1];
			b.z = this->vertex[i][j + 1][2];
			c.x = this->vertex[i + 1][j][0];
			c.y = this->vertex[i + 1][j][1];
			c.z = this->vertex[i + 1][j][2];
			d.x = this->vertex[i + 1][j + 1][0];
			d.y = this->vertex[i + 1][j + 1][1];
			d.z = this->vertex[i + 1][j + 1][2];
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
			if (i == 0 || j == 0 || i == this->height * this->size - 1 || j == this->width * this->size - 1)
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
	w++;
	for (int i = 0;i < this->height * this->size - 1;i++)
		for (int j = 0;j < this->width * this->size - 1;j++)
		{
			int a, b, c, d;
			a = i * w + j + 1;
			b = i * w + j + 2;
			c = (i + 1) * w + j + 1;
			d = (i + 1) * w + j + 2;
			outputfile << "f " << a << "/" << a << "/" << a << " " << b << "/" << b << "/" << b << " " << c << "/" << c << "/" << c << endl;
			outputfile << "f " << d << "/" << d << "/" << d << " " << b << "/" << b << "/" << b << " " << c << "/" << c << "/" << c << endl;
		}
}