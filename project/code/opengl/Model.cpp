#include "Model.h"
#include <opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;

void Model::calculateVertex(){
	this->mean_x = 0;
	this->mean_y = 0;
	this->mean_z = 0;
	int cnt1=0, cnt2=0, cnt3=0;

	for (int q = 0; q < (this->vertex_list.size() / 3) ; q++){
		//cout << model_object.vertex_list.at(q) << endl;
			this->mean_x += this->vertex_list.at(3 * q), cnt1++;
	}

	for (int q = 0; q < (this->vertex_list.size() / 3) ; q++){
		//cout << model_object.vertex_list.at(q) << endl;
			this->mean_y += this->vertex_list.at(3 * q + 1), cnt2++;
	}

	for (int q = 0; q < (this->vertex_list.size() / 3) ; q++){
		//cout << model_object.vertex_list.at(q) << endl;
			this->mean_z += this->vertex_list.at(3 * q + 2), cnt3++;
	}

	this->mean_x = this->mean_x / cnt1;
	this->mean_y = this->mean_y / cnt2;
	this->mean_z = this->mean_z / cnt3;
}

void Model::getMaxMins(){
	this->max_x = -999999999;
	for (int q = 0; q < ( this->vertex_list.size() / 3 ); q++)
	{
		if (this->vertex_list.at(3*q) > this->max_x){
			this->max_x = this->vertex_list.at(3*q);
		}
	}

	this->min_x = 999999999;
	for (int q = 0; q < ( this->vertex_list.size() / 3 ); q++)
	{
		if (this->vertex_list.at(3*q) < this->min_x){
			this->min_x = this->vertex_list.at(3*q);
		}
	}

	this->max_y = -999999999;
	for (int q = 0; q < ( this->vertex_list.size() / 3 ); q++)
	{
		if (this->vertex_list.at(3*q + 1) > this->max_y){
			this->max_y = this->vertex_list.at(3*q + 1);
		}
	}

	this->min_y = 999999999;
	for (int q = 0; q < ( this->vertex_list.size() / 3 ); q++)
	{
		if (this->vertex_list.at(3*q + 1) < this->min_y){
			this->min_y = this->vertex_list.at(3*q + 1);
		}
	}

	this->max_z = -999999999;
	for (int q = 0; q < ( this->vertex_list.size() / 3 ); q++)
	{
		if (this->vertex_list.at(3*q + 2) > this->max_z){
			this->max_z = this->vertex_list.at(3*q + 2);
		}
	}

	this->min_z = 999999999;
	for (int q = 0; q < ( this->vertex_list.size() / 3 ); q++)
	{
		if (this->vertex_list.at(3*q + 2) < this->min_z){
			this->min_z = this->vertex_list.at(3*q + 2);
		}
	}

	float scale_x = this->max_x - this->min_x;
	float scale_y = this->max_y - this->min_y;
	float scale_z = this->max_z - this->min_z;

	this->max_scale = scale_x;
	if (scale_y > this->max_scale)
		this->max_scale = scale_y;

	if (scale_z > this->max_scale)
		this->max_scale = scale_z;
}

void Model::applyTransfToMatrix(){
   this->calculateVertex();
   this->getMaxMins();

   
   for (unsigned int h = 0; h < ( this->vertex_list.size() / 3); h++){
	   this->vertex_list.at(3*h) = this->vertex_list.at(3*h) - this->mean_x;
   }
   for (unsigned int h = 0; h < ( this->vertex_list.size() / 3); h++){
	   this->vertex_list.at(3*h + 1) = this->vertex_list.at(3*h + 1) - this->mean_y;
   }
   
   for (unsigned int h = 0; h < ( this->vertex_list.size() / 3); h++){
	   this->vertex_list.at(3*h + 2) = this->vertex_list.at(3*h + 2) - this->mean_z;
   }

   // 法向量归一化
   for (unsigned int i = 0;i < (this->normal_list.size() / 3);i++)
   {
	   float sum = 0;
	   sum = this->normal_list.at(3 * i) + this->normal_list.at(3 * i + 1) + this->normal_list.at(3 * i + 2);
	   this->normal_list.at(3 * i) /= sum;
	   this->normal_list.at(3 * i+1) /= sum;
	   this->normal_list.at(3 * i+2) /= sum;
   }

   for (unsigned int h = 0; h < this->vertex_list.size() ; h++){
	   this->vertex_list.at(h) = this->vertex_list.at(h) / this->max_scale;
   }   	

}

void Model::loadFile(const char* file_name)
{
	ifstream fileObject(file_name);
	string fileObjectLine;

	string now_material = "";
	if (fileObject.is_open()) {
		while (!fileObject.eof()) {
			getline(fileObject, fileObjectLine);
			if (fileObjectLine.c_str()[0] == 'u')
			{
				istringstream iss(fileObjectLine);
				string temp;
				iss >> temp >> now_material;
			}
			if (fileObjectLine.c_str()[0] == 'f') {

				s_list.push_back(fileObjectLine);
				fileObjectLine[0] = ' ';

				istringstream iss(fileObjectLine);

				vector<int> tri;
				//cout << now_material << endl;
				while (iss) {
					int value;
					char x;
					iss >> value;
					if (iss.fail()) {/* cout << "qi" << endl;*/break; }
					tri.push_back(value);
					iss >> x >> value;
					tri.push_back(value);
					iss >> x >> value;
					tri.push_back(value);
				}
				this->face_list.push_back(tri);
				this->face_material.push_back(now_material);
			}
			if (fileObjectLine.c_str()[0] == 'v' && fileObjectLine.c_str()[1] == ' ')
			{
				float x, y, z;
				fileObjectLine[0] = ' ';
				sscanf_s(fileObjectLine.c_str(), "%f %f %f ", &x, &y, &z);
				this->vertex_list.push_back(x);
				this->vertex_list.push_back(y);
				this->vertex_list.push_back(z);
				continue;
			}
			else if (fileObjectLine.c_str()[0] == 'v' && fileObjectLine.c_str()[1] == 'n')
			{
				float x, y, z;
				fileObjectLine[0] = ' ';
				fileObjectLine[1] = ' ';
				sscanf_s(fileObjectLine.c_str(), "%f %f %f ", &x, &y, &z);
				this->normal_list.push_back(x);
				this->normal_list.push_back(y);
				this->normal_list.push_back(z);
				continue;
			}
			else if (fileObjectLine.c_str()[0] == 'v' && fileObjectLine.c_str()[1] == 't')
			{
				float x, y, z;
				fileObjectLine[0] = ' ';
				fileObjectLine[1] = ' ';
				sscanf_s(fileObjectLine.c_str(), "%f %f", &x, &y);
				this->texture_list.push_back(x);
				this->texture_list.push_back(y);
				continue;
			}
		}
	}
	this->applyTransfToMatrix();
}