/* Class Model */
 #ifndef Model_H
#define Model_H

#include <vector>
#include <string>
#include <fstream>
#include <GL/glut.h>
#include <string>
using namespace std;

/* Class Model */
 
class Model
{
public: 		
   void loadFile(const char* file_name);
	void calculateVertex();
	void getMaxMins();
	void applyTransfToMatrix();
	
	//vectors
	std::vector<float> vertex_list;
	std::vector<float> normal_list;
	std::vector<float> texture_list;
	std::vector<std::string> s_list;
	std::vector<std::vector<int>> face_list;
	std::vector<std::string> face_material;
	float mean_x;
	float mean_y;
	float mean_z;

	float max_x;
	float min_x;
	float max_y;
	float min_y;
	float max_z;
	float min_z;

	float max_scale;

	float model_x;
	float model_y;
	float model_z;

	float model_rotx;
	float model_roty;
	float model_rotz;

};

#endif
