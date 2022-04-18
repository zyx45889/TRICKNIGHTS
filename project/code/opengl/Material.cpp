#include "Material.h"
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void Material::load_texture(const char* file_name)
{
	Mat img = imread(file_name);
	resize(img, img, Size(1024, 1024));
	GLint height = img.rows, width = img.cols, total_bytes;
	GLubyte* pixels = 0;
	GLint last_texture_ID;
	GLuint texture_ID = 0;
	GLint line_bytes = width * 3;
	total_bytes = line_bytes * height;
	pixels = (GLubyte*)malloc(total_bytes);

	//cout << height << " "<<width<<endl;
	for (int i = 0;i < height;i++)
		for (int j = 0;j < width;j++)
		{
			pixels[(i * width + j) * 3] = img.at<Vec3b>(height-1-i, j)[0];
			pixels[(i * width + j) * 3 + 1] = img.at<Vec3b>(height-1-i, j)[1];
			pixels[(i * width + j) * 3 + 2] = img.at<Vec3b>(height-1-i, j)[2];
		}

	glGenTextures(1, &texture_ID);
	if (texture_ID == 0)
	{
		free(pixels);
		return ;
	}

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
	glBindTexture(GL_TEXTURE_2D, texture_ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, last_texture_ID);

	free(pixels);
	this->texture_list.push_back(texture_ID);
}

void Material::loadFile(const char* file_name)
{

	ifstream fileObject(file_name);
	string fileObjectLine;

	if (fileObject.is_open()) {
		while (!fileObject.eof()) {
			getline(fileObject, fileObjectLine);
			if (fileObjectLine.c_str()[0] == 'n' && fileObjectLine.c_str()[1] == 'e')
			{
				istringstream iss(fileObjectLine);
				string temp, name;
				iss >> temp >> name;
				material now_material;
				now_material.material_name = name;
				while (fileObjectLine.c_str()[0] != '\n' && fileObjectLine.c_str()[0] != '\r')
				{
					getline(fileObject, fileObjectLine);
					if (fileObjectLine.size() < 4)
					{
						break;
					}
					else if (fileObjectLine.c_str()[1] == 'N' && fileObjectLine.c_str()[2] == 's')
					{
						istringstream iss(fileObjectLine);
						string temp;
						float Ns;
						iss >> temp >> Ns;
						now_material.Ns = Ns;
					}
					else if (fileObjectLine.c_str()[1] == 'd' && fileObjectLine.c_str()[2] == ' ')
					{
						istringstream iss(fileObjectLine);
						string temp;
						float d;
						iss >> temp >> d;
						now_material.d = d;
					}
					else if (fileObjectLine.c_str()[1] == 'N' && fileObjectLine.c_str()[2] == 'i')
					{
						istringstream iss(fileObjectLine);
						string temp;
						float Ni;
						iss >> temp >> Ni;
						now_material.Ni = Ni;
					}
					else if (fileObjectLine.c_str()[1] == 'i' && fileObjectLine.c_str()[2] == 'l')
					{
						istringstream iss(fileObjectLine);
						string temp;
						float illum;
						iss >> temp >> illum;
						now_material.illum = illum;
					}
					else if (fileObjectLine.c_str()[1] == 'K' && fileObjectLine.c_str()[2] == 'a')
					{
						istringstream iss(fileObjectLine);
						string temp;
						float a, b, c;
						iss >> temp >> a >> b >> c;
						/*a = a / (a + b + c);
						b = b / (a + b + c);
						c = c / (a + b + c);*/
						now_material.Ka[0] = a;
						now_material.Ka[1] = b;
						now_material.Ka[2] = c;
					}
					else if (fileObjectLine.c_str()[1] == 'K' && fileObjectLine.c_str()[2] == 'd')
					{
						istringstream iss(fileObjectLine);
						string temp;
						float a, b, c;
						iss >> temp >> a >> b >> c;
						/*a = a / (a + b + c);
						b = b / (a + b + c);
						c = c / (a + b + c);*/
						now_material.Kd[0] = a;
						now_material.Kd[1] = b;
						now_material.Kd[2] = c;
					}
					else if (fileObjectLine.c_str()[1] == 'K' && fileObjectLine.c_str()[2] == 's')
					{
						istringstream iss(fileObjectLine);
						string temp;
						float a, b, c;
						iss >> temp >> a >> b >> c;
						/*a = a / (a + b + c);
						b = b / (a + b + c);
						c = c / (a + b + c);*/
						now_material.Ks[0] = a;
						now_material.Ks[1] = b;
						now_material.Ks[2] = c;
					}
					else if (fileObjectLine.c_str()[1] == 'm' && fileObjectLine.c_str()[2] == 'a')
					{
						istringstream iss(fileObjectLine);
						string temp, filename;
						iss >> temp >> filename;
						this->load_texture(filename.c_str());
						now_material.texture_id = this->texture_list.size() - 1;
					}
				}
				this->material_list.push_back(now_material);
			}
		}
	}
}