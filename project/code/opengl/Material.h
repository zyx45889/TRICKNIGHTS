/* Class Model */
#ifndef Material_H
#define Material_H

#include <vector>
#include <string>
#include <fstream>
#include <GL/glut.h>

/* Class Materail */

struct material {
	std::string material_name="";
	int texture_id=-1;
	float Ns = 1024;
	//Ns exponent ָ�����ʵķ���ָ���������˷���߹��.exponent�Ƿ���ָ��ֵ����ֵԽ����߹�Խ�ܼ���һ��ȡֵ��Χ��0~1000��
	float d = 1;
	//ȡֵΪ1.0��ʾ��ȫ��͸����ȡֵΪ0.0ʱ��ʾ��ȫ͸��
	float Ni = 0.001;
	//Ni ptical density ָ�����ʱ���Ĺ��ܶȣ�������ֵ������0.001��10֮�����ȡֵ����ȡֵΪ1.0������ͨ�������ʱ�򲻷���������������������Ϊ1.5��
	int illum = 2;
	// Highlight on
	GLfloat Ka[3] = { 1.0, 0.0, 0.0 };
	//��������
	GLfloat Kd[3] = { 1.0, 0.0, 0.0 };
	//������
	GLfloat Ks[3] = { 1.0, 0.0, 0.0 };
	//���淴��
};

class Material
{
public:
	void loadFile(const char* file_name);
	void load_texture(const char* file_name);

	//vectors
	std::vector<material> material_list;
	std::vector<GLuint> texture_list;

};

#endif
