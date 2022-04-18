#include "Model.h"
#include "Material.h"
#include "Landscape.h"
#include "Metalbox.h"
#include "Map.h"
#include "Enemy.h"
#include "Assist.h"
#include "Water.h"
#include "SkyDome.h"
#include "Tower.h"
#include "Cannon.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cassert>
#include <GL/glut.h>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <math.h>
#define N 16    //多少个要加载的obj
#define N_monster 5
#define N_box1 5
#define N_assist 5
#define N_cannon 2
#define N_setting 3
#define N_tower 2 
#define PI 3.1415926535898
using namespace std;
using namespace cv;

struct Object {
    string objfile;
    string mtlfile;
    Model objmodel;
    Material mtlmodel;
    double x=0;
    double y=0;
    double z=0;
    double dir=0;    //面朝哪个方向，即绕y轴转动多少
    double size=15;
}object[N];
Object land;
Material number;
myMap mymap;
Enemy enemy[N_monster];
int enemy_time[N_monster] = { 0,20,40,60,80 };
Metalbox box1[N_box1];
Landscape landscape;
Assist atk1[N_assist];
Tower tower[N_tower];
GLdouble perspective = 100.0;
Water water;
SkyDome sky;
Cannon atk2[N_cannon];
double rot_x = 0, rot_y = 0;
double dx = 0.0, dy = 0.0,dz=0.0;
double mouse_x = 0.0, mouse_y = 0.0;
double ddx = 0.0, ddy = 0.0;   // 每帧改变视角的量
int changeframe = 5;    // 改变视角需要多少帧
int changingview = 0;   //是否正在改变视角  0：否 1-changefram：还需改变多少帧
int setting = 0;   // 是否在设置状态 0：不是 1：是，还没点击第一次 2：是，已点击第一次
int controling = 0; // 是否在控制炮台状态 0：不是 1：是，还没控制完 2：是，已控制完
double rotate_center_x= 268, rotate_center_y=301;
double rotate_start_x, rotate_start_y;
double rotate_end_x, rotate_end_y;
double pre_view_x, pre_view_y;
int setid = 0;  //选中的是哪个物品
int add_setting[N_setting][2][2] = { {{141,86},{163,115}},{{198,80},{236,124}},{{263,80},{299,121}} };   //每个物体左上和右下的xy坐标
int moving = 0;
int main_time=0;
int max_deadtime = 5;
double xspeed = 0.08;
double yspeed = 0.06;     // 死亡效果
int cost;
double env_light = 0.8;
double light_dx, light_dy, light_dz;

void set_light()
{
    glTranslatef(0, -7, 0);
    float light[] = { -light_dx,-light_dy,light_dz,1.0 };
    GLfloat sun_light_ambient[] = { env_light, env_light, env_light, 1.0f };
    GLfloat sun_light_diffuse[] = { 5.0f, 5.0f, 5.0f, 1.0f };
    GLfloat sun_light_specular[] = { 5.0f, 5.0f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sun_light_ambient); //GL_AMBIENT表示各种光线照射到该材质上，
                                                              //经过很多次反射后最终遗留在环境中的光线强度（颜色）
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_light_diffuse); //漫反射后
    glLightfv(GL_LIGHT0, GL_SPECULAR, sun_light_specular);//镜面反射后

    glEnable(GL_LIGHT0); //使用第0号光照
    glTranslatef(0, 6, 0);
}

void set_camera()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75, 1, 1, 200);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.1, 0.1, 20, 0, 0, 0, 0, 1, 0);
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(perspective, (GLfloat)w / (GLfloat)h, 10.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -30.0);
}


void initialize()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING); //在后面的渲染中使用光照
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    set_light();
}

void drawobject(Object* object,double deadtime)
{
    GLint last_texture_ID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
    glPushMatrix();
    glTranslatef(object->x, object->y, object->z);
    glScalef(object->size, object->size, object->size);
    glRotatef(object->dir, 0, 1, 0);
    if (deadtime != 0)
    {
        deadtime = max_deadtime - deadtime;
        GLfloat black[] = { 0.0, 0.0, 0.0 };
        glMaterialfv(GL_FRONT, GL_SPECULAR, black);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, black);
        GLfloat shininess[] = { 10.0 };
        glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
        for (int i = 0;i < object->objmodel.face_list.size();i++)
        {
            vector<int>tri = object->objmodel.face_list.at(i);
            int count = 0;
            double xsum = 0, ysum = 0, zsum = 0;
            for (int j = 0;j < tri.size();j += 3)
            {
                int value = tri.at(j);
                count++;
                xsum += object->objmodel.vertex_list.at(3 * (value - 1));
                ysum += object->objmodel.vertex_list.at(3 * (value - 1) + 1);
                zsum += object->objmodel.vertex_list.at(3 * (value - 1) + 2);
            }
            double center[3] = { xsum / count,ysum / count,zsum / count };
            double dy = -0.5 * deadtime * deadtime * yspeed;
            double d = deadtime * xspeed;
            double dx = d / sqrt(center[0] * center[0] + center[2] * center[2]) * center[0];
            double dz = d / sqrt(center[0] * center[0] + center[2] * center[2]) * center[2];
            glBegin(GL_POLYGON);
            for (int j = 0;j < tri.size();j += 3)
            {
                int value = tri.at(j + 2);
                glNormal3f(object->objmodel.normal_list.at(3 * (value - 1)), object->objmodel.normal_list.at(3 * (value - 1) + 1), object->objmodel.normal_list.at(3 * (value - 1) + 2));
                value = tri.at(j);
                double nowx = object->objmodel.vertex_list.at(3 * (value - 1));
                double nowy = object->objmodel.vertex_list.at(3 * (value - 1) + 1);
                double nowz = object->objmodel.vertex_list.at(3 * (value - 1) + 2);
                glVertex3f(nowx - (nowx - center[0]) / max_deadtime * deadtime + dx,
                        nowy - (nowy - center[1]) / max_deadtime * deadtime + dy,
                        nowz - (nowz - center[2]) / max_deadtime * deadtime + dz);
            }
            glEnd();
        }
        glBindTexture(GL_TEXTURE_2D, last_texture_ID);
        glPopMatrix();
        return;
    }
    for (int i = 0;i < object->objmodel.face_list.size();i++)
    {
        vector<int>tri = object->objmodel.face_list.at(i);
        string material_name = object->objmodel.face_material.at(i);
        material now_material;
        for (int k = 0;k < object->mtlmodel.material_list.size();k++)
        {
            now_material = object->mtlmodel.material_list.at(k);
            if (now_material.material_name == material_name)
            {
                break;
            }
        }
        if (now_material.texture_id != -1)
        {
            GLfloat ambient[] = { 0.0f, 0.0f, 0.5f, 1.0f };
            GLfloat diffuse[] = { 0.0f, 0.0f, 0.5f, 1.0f };
            GLfloat specular[] = { 0.0f, 0.0f, 1.0f, 1.0f };
            GLfloat shininess = 30.0f;

            glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
            glMaterialf(GL_FRONT, GL_SHININESS, shininess);
            glBindTexture(GL_TEXTURE_2D, object->mtlmodel.texture_list.at(now_material.texture_id));
            glBegin(GL_POLYGON);
            for (int j = 0;j < tri.size();j += 3)
            {
                //cout << i << " " << j <<":"<< endl;
                int value = tri.at(j + 1);
                //cout << value << endl;
                glTexCoord2f(object->objmodel.texture_list.at(2 * (value - 1)), object->objmodel.texture_list.at(2 * (value - 1) + 1));
                value = tri.at(j + 2);
                //cout << value << endl;
                glNormal3f(object->objmodel.normal_list.at(3 * (value - 1)), object->objmodel.normal_list.at(3 * (value - 1) + 1), object->objmodel.normal_list.at(3 * (value - 1) + 2));
                value = tri.at(j);
                //cout << value << endl;
                glVertex3f(object->objmodel.vertex_list.at(3 * (value - 1)), object->objmodel.vertex_list.at(3 * (value - 1) + 1), object->objmodel.vertex_list.at(3 * (value - 1) + 2));
            }
            glEnd();
        }
        else {
            //GLfloat red[] = { 1.0, 0.0, 0.0, 0.5 };
            glMaterialfv(GL_FRONT, GL_SPECULAR, now_material.Ks);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, now_material.Kd);
            //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, now_material.Ka);
            GLfloat shininess[] = { 10.0 };
            glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
            glBegin(GL_POLYGON);
            for (int j = 0;j < tri.size();j += 3)
            {
                int value = tri.at(j + 2);
                glNormal3f(object->objmodel.normal_list.at(3 * (value - 1)), object->objmodel.normal_list.at(3 * (value - 1) + 1), object->objmodel.normal_list.at(3 * (value - 1) + 2));
                value = tri.at(j);
                glVertex3f(object->objmodel.vertex_list.at(3 * (value - 1)), object->objmodel.vertex_list.at(3 * (value - 1) + 1), object->objmodel.vertex_list.at(3 * (value - 1) + 2));
            }
            glEnd();
        }
    }
    glBindTexture(GL_TEXTURE_2D, last_texture_ID);
    glPopMatrix();
}

double cal_dis(double x1, double y1, double x2, double y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

void update_game()
{
    if (changingview != 0)
    {
        rot_x += ddx;
        rot_y += ddy;
        changingview--;
        return;
    }
    if (setting !=0 || controling !=0)
    {
        return;
    }
    for (int i = 0;i < N_assist;i++)
    {
        if (atk1[i].state == 1)
        {
            atk1[i].update();
        }
    }
    for (int i = 0;i < N_monster;i++)
    {
        if (enemy_time[i] == main_time)
        {
            enemy[i].state = 1;
        }
        if (enemy[i].state == 1)
        {
            enemy[i].update(&mymap);
            enemy[i].check_y(mymap.getheight_box(enemy[i].x_map, enemy[i].y_map, &landscape));
            switch (enemy[i].nowdir) {  //记录方向：1 左 2 右 3 上 4 下
            case 1:
                enemy[i].rotate = -90;
                break;
            case 2:
                enemy[i].rotate = 90;
                break;
            case 3:
                enemy[i].rotate = 180;
                break;
            case 4:
                enemy[i].rotate = 0;
                break;
            }
            for (int j = 0;j < N_assist;j++)
                if (atk1[j].state == 1)
                {
                    double center[2] = { atk1[j].x_world, atk1[j].z_world };
                    double r = atk1[j].nowstep;
                    double size = object[4].size;
                    double edge[2] = { enemy[i].x_world,enemy[i].z_world };
                    if (cal_dis(center[0], center[1], edge[0] + size, edge[1]) < r || cal_dis(center[0], center[1], edge[0] - size, edge[1]) < r ||
                        cal_dis(center[0], center[1], edge[0], edge[1] + size) < r || cal_dis(center[0], center[1], edge[0], edge[1] - size) < r)
                    {
                        enemy[i].blood -= 0.2;
                    }
                    if (enemy[i].blood <= 0)
                    {
                        enemy[i].dead(max_deadtime);
                        cost += 10;
                        break;
                    }
                }
        }
        else if (enemy[i].state == 2)
        {
            enemy[i].update(&mymap);
        }
    }
}

void draw_blood(double width, double percent)
{
    GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat white[] = { 1.0, 1.0,1.0, 1.0 };
    GLfloat shininess[] = { 0.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, red);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    //glLightModelfv(GL_LIGHT_MODEL_AMBIENT, red);    //设置材质
    glTranslatef(-width / 2, 0, 0);
    glBegin(GL_POLYGON);
    glVertex3f(0,0 ,0 );
    glVertex3f(width*percent, 0, 0);
    glVertex3f(width * percent, 0.2, 0);
    glVertex3f(0, 0.2, 0);
    glEnd();
}

void draw_door()
{
    glPushMatrix();
    double x_map, y_map;
    double x_world, y_world, z_world;
    x_map = 1.5;
    y_map = 0;
    x_world = (y_map +0.3 - 6) / 6 * 30 / 2;
    z_world = (x_map-0.05 - 3) / 3 * 30 / 4;
    y_world = -7.3;
    glTranslatef(x_world, y_world, z_world);
    glRotatef(90, 0, 1, 0);
    drawobject(&object[2],0);
    glPopMatrix();
    glPushMatrix();
    x_map = 3.5;
    y_map = 11;
    x_world = (y_map + 0.3 - 6) / 6 * 30 / 2;
    z_world = (x_map - 0.05 - 3) / 3 * 30 / 4;
    y_world = -7.3;
    glTranslatef(x_world, y_world, z_world);
    glRotatef(90, 0, 1, 0);
    drawobject(&object[2],0);
    glPopMatrix();
}

void drawcircle(double r,double width)
{
    double step = 5;
    GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat white[] = { 1.0, 1.0,1.0, 1.0 };
    GLfloat shininess[] = { 0.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, red);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    for (int i = 0;i <=360;i += step)
    {
        glBegin(GL_POLYGON);
        double x, y;
        x = sin((double)i / 360 * 2 * PI);
        y = cos((double)i / 360 * 2 * PI);
        glVertex3f(x*r, 0, y*r);
        glVertex3f(x * (r + width),0, y * (r + width));
        x = sin((double)(i+step) / 360 * 2 * PI);
        y = cos((double)(i+step) / 360 * 2 * PI);
        glVertex3f(x * (r + width), 0, y * (r + width));
        glVertex3f(x * r, 0, y * r);
        glEnd();
    }
}

void draw_number()
{
    glPushMatrix();
    GLint last_texture_ID;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
    glBindTexture(GL_TEXTURE_2D, number.texture_list.at(0));
    int num[10] = { 0 };
    int len = 0;
    int temp = cost;
    while (temp)
    {
        num[len] = temp % 10;
        if (num[len] == 0)
            num[len] = 10;
        temp /= 10;
        len++;
    }
    if (len == 0)
        len = 1, num[0] = 10;
    for (int i = len - 1;i >= 0;i--)
    {
        glBegin(GL_POLYGON);
        glTexCoord2f(1,(double)(num[i] - 1) / 10);
        glNormal3f(0,1,0);
        glVertex3f(-0.5, 0.5, 0);
        glTexCoord2f(1,(double)(num[i]) / 10);
        glNormal3f(0, 1, 0);
        glVertex3f(0.5, 0.5, 0);
        glTexCoord2f(0,(double)(num[i]) / 10);
        glNormal3f(0, 1, 0);
        glVertex3f(0.5, -0.5, 0);
        glTexCoord2f(0,(double)(num[i] - 1) / 10);
        glNormal3f(0, 1, 0);
        glVertex3f(-0.5, -0.5, 0);
        glEnd();
        glTranslatef(1, 0, 0);
    }
    glBindTexture(GL_TEXTURE_2D, last_texture_ID);
    glPopMatrix();
}

void drawwindow()
{
    glPushMatrix();
    glTranslatef(-11, 9, 0);
    glTranslatef(0, 3, 0);
    drawobject(&object[15], 0);
    glTranslatef(4, 0, 0);
    draw_number();
    glTranslatef(-4, 0, 0);
    glTranslatef(0, -3, 0);
    drawobject(&object[10],0);
    glTranslatef(5, 0, 0);
    glScalef(2, 2, 2);
    drawobject(&object[5],0);
    glPushMatrix();
    glTranslatef(2, 0, 0);
    glScalef(0.5, 0.5, 0.5);
    glRotatef(25, 0, 1, 0);
    drawobject(&object[1],0);
    glTranslatef(4.6, 1, 0);
    glScalef(0.8, 0.8, 0.8);
    glRotatef(-35, 0, 0, 1);
    drawobject(&object[13], 0);
    glPopMatrix();
    glTranslatef(7, 0, 0);
    drawobject(&object[14], 0);
    glPopMatrix();
}

void draw_control(double dir)
{
    double angle1 = dir - 60, angle2 = dir + 60;
    double r = 3.5;
    double width = 0.1;
    double step = 5;
    GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat white[] = { 1.0, 1.0,1.0, 1.0 };
    GLfloat shininess[] = { 0.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, red);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    for (int i = angle1;i <= angle2;i += step)
    {
        glBegin(GL_POLYGON);
        double x, y;
        x = sin((double)i / 360 * 2 * PI);
        y = cos((double)i / 360 * 2 * PI);
        glVertex3f(x * r, 0, y * r);
        glVertex3f(x * (r + width), 0, y * (r + width));
        x = sin((double)(i + step) / 360 * 2 * PI);
        y = cos((double)(i + step) / 360 * 2 * PI);
        glVertex3f(x * (r + width), 0, y * (r + width));
        glVertex3f(x * r, 0, y * r);
        glEnd();
    }
    angle1 += 180;
    angle2 += 180;
    r = 1;
    width = 0.3;
    for (int i = angle1;i <= angle2;i += step)
    {
        glBegin(GL_POLYGON);
        double x, y;
        x = sin((double)i / 360 * 2 * PI);
        y = cos((double)i / 360 * 2 * PI);
        glVertex3f(x * r, 0, y * r);
        glVertex3f(x * (r + width), 0, y * (r + width));
        x = sin((double)(i + step) / 360 * 2 * PI);
        y = cos((double)(i + step) / 360 * 2 * PI);
        glVertex3f(x * (r + width), 0, y * (r + width));
        glVertex3f(x * r, 0, y * r);
        glEnd();
    }
}

void draw_bullet(double x, double y, double z)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat white[] = { 1.0, 1.0,1.0, 1.0 };
    GLfloat shininess[] = { 0.0 };
    glMaterialfv(GL_FRONT, GL_SPECULAR, red);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
    glutSolidSphere(0.1, 20, 20);
    glPopMatrix();
    if (atk2[0].nowstep != 1) {
        for (int i = 0;i < 20;i++)
        {
            glPushMatrix();
            glTranslatef(atk2[0].bullet_x_history[i], atk2[0].bullet_y_history[i], atk2[0].bullet_z_history[i]);
            glutSolidSphere(0.1 * (double)i / 20, 10, 10);
            glPopMatrix();
        }
    }
}

void draw_trees()
{
    object[8].x = -12;
    object[8].y = -6.8;
    object[8].z = -6;
    object[8].size = 3;
    drawobject(&object[8],0);
    object[8].x = -10;
    object[8].y = -6;
    object[8].z = -6;
    object[8].size = 5;
    drawobject(&object[8], 0);
    object[8].x = -10;
    object[8].y = -6;
    object[8].z = 7;
    object[8].size = 5;
    drawobject(&object[8], 0);
    object[8].x = 5.5;
    object[8].y = -6.8;
    object[8].z = 7;
    object[8].size = 3;
    drawobject(&object[8], 0);
}

void display(void)
{
    // 清除屏幕
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 设置光和视角
    set_camera();
    drawwindow();  //游戏操作窗口
    glPushMatrix();
    //处理平移
    glTranslatef(-dx, -dy, dz);
    //处理旋转
    glRotatef(rot_x, 1.0f, 0.0f, 0.0f);
    glRotatef(rot_y, 0.0f, 1.0f, 0.0f);
    set_light();

    //画 
    
    update_game();
    //water.draw_water(main_time, &(object[11].objmodel), object[11].x, object[11].y, object[11].z,object[11].size);
    draw_trees();
    drawobject(&object[11],0);
    drawobject(&land,0);
    drawobject(&object[12], 0);
    draw_door();
    for (int i = 0;i < N_tower;i++)
    {
        if (tower[i].state == 1)
        {
            glPushMatrix();
            glTranslatef(tower[i].x_world, tower[i].y_world, tower[i].z_world);
            drawobject(&object[13], 0);
            glPopMatrix();
        }
    }
    for (int i = 0;i < N_assist;i++)
    {
        if (atk1[i].state == 1)
        {
            glPushMatrix();
            glTranslatef(atk1[i].x_world, atk1[i].y_world, atk1[i].z_world);
            drawcircle(atk1[i].nowstep,0.1);
            drawobject(&object[5],0);
            glPopMatrix();
        }
    }
    for (int i = 0;i < N_cannon;i++)
    {
        if (atk2[i].state == 1)
        {
            glPushMatrix();
            glTranslatef(atk2[i].x_world, atk2[i].y_world, atk2[i].z_world);
            if (controling != 0)
            {
                glPushMatrix();
                draw_control(atk2[i].dir);
                glPopMatrix();
            }
            glRotatef(atk2[i].dir, 0, 1, 0);
            drawobject(&object[9], 0);
            glPopMatrix();
            glPushMatrix();
            if (atk2[i].draw_bullet == 1)
            {
                atk2[i].update_bullet();
                draw_bullet(atk2[i].bullet_x,atk2[i].bullet_y,atk2[i].bullet_z);
                if (atk2[i].draw_bullet == 0)
                {
                    enemy[atk2[i].aim_enemy].dead(max_deadtime);
                    cost += 10;
                }
            }
            glPopMatrix();
        }
    }
    for (int i = 0;i < N_monster;i++)
    {
        if (enemy[i].state == 1)
        {
            glPushMatrix();
            glTranslatef(enemy[i].x_world, enemy[i].y_world, enemy[i].z_world);
            glRotatef(enemy[i].rotate, 0, 1, 0);
            glPushMatrix();
            glTranslatef(0, 0.7, 0);
            draw_blood(1.5, enemy[i].blood);
            glPopMatrix();
            drawobject(&object[4],0);
            glPopMatrix();
        }
    }
    for (int i = 0;i < N_box1;i++)
    {
        if (box1[i].state == 1)
        {
            glPushMatrix();
            glTranslatef(box1[i].x_world, box1[i].y_world, box1[i].z_world);
            drawobject(&object[1],0);
            glPopMatrix();
        }
    }
    for (int i = 0;i < N_monster;i++)
    {
        if (enemy[i].state == 2)
        {
            glPushMatrix();
            glTranslatef(enemy[i].x_world, enemy[i].y_world, enemy[i].z_world);
            glRotatef(enemy[i].rotate, 0, 1, 0);
            drawobject(&object[4], enemy[i].deadtime);
            glPopMatrix();
        }
    }
    glPopMatrix();
    glutSwapBuffers();
}

void timeadd()
{
    main_time++;
    if (setting == 0 && controling == 0 && main_time % 2 == 0)
        cost++;
    glutPostRedisplay();
}

void keyboard_recall(unsigned char key, int x, int y)
{
    if (key == 'a')
    {
        dx -= 0.5;
    }
    else if (key == 'd')
    {
        dx += 0.5;
    }
    else if (key == 'w')
    {
        dy += 0.5;
    }
    else if (key == 's')
    {
        dy -= 0.5;
    }
    else if (key == 'z')
    {
        dz -= 0.5;
    }
    else if (key == 'x')
    {
        dz += 0.5;
    }
    else if (key == 'p')
    {
        env_light += 0.1;
    }
    else if (key == 'o')
    {
        env_light -= 0.1;
    }
    else if (key == 'j')
    {
        light_dx += 0.5;
    }
    else if (key == 'l')
    {
        light_dx -= 0.5;
    }
    else if (key == 'i')
    {
        light_dz -= 0.5;
    }
    else if (key == 'k')
    {
        light_dz += 0.5;
    }
    glutPostRedisplay();
}

void add_object(double x, double y)
{
    double map1[2] = { 78,163 };
    double map2[2] = { 423,336 };
    double map_x=(x-map1[0])/(map2[0]-map1[0])*12, map_y=(y-map1[1])/(map2[1]-map1[1])*6;
    switch (setid) {
        case 0:
            for (int i = 0;i < N_assist;i++)
                if (atk1[i].state == 0)
                {
                    atk1[i].state = 1;
                    atk1[i].x_map = (int)map_y;
                    atk1[i].y_map = (int)map_x;
                    atk1[i].update();
                    atk1[i].check_y(mymap.getheight(atk1[i].x_map+0.5, atk1[i].y_map+0.5, &landscape));
                    break;
                }
            break;
        case 1:
            for (int i = 0;i < N_box1;i++)
                if (box1[i].state == 0)
                {
                    box1[i].state = 1;
                    box1[i].x_map = (int)map_y;
                    box1[i].y_map = (int)map_x;
                    box1[i].cal_world();
                    mymap.map[(int)box1[i].x_map][(int)box1[i].y_map] = 3;
                    break;
                }
            break;
        case 2:
            for (int i = 0;i < N_tower;i++)
                if (tower[i].state == 0)
                {
                    tower[i].state = 1;
                    tower[i].x_map = (int)map_y;
                    tower[i].y_map = (int)map_x;
                    tower[i].cal_world();
                    break;
                }
            break;
    }
}

void mouse_recall(int button, int state, int x, int y)
{
    if (changingview != 0)
    {
        return;
    }
    else if (setting == 1)
    {
        if (state == GLUT_DOWN)
        {
            for (int i = 0;i < N_setting;i++)
            {
                if (add_setting[i][0][0] <= x && add_setting[i][1][0] >= x && add_setting[i][0][1] <= y && add_setting[i][1][1] >= y)
                {
                    setid = i;
                    setting = 2;
                }
            }
        }
        return;
    }
    else if (setting == 2)
    {
        if (state == GLUT_DOWN)
        {
            add_object(x,y);
            setting = 0;
        }
        return;
    }
    else if (controling == 1)
    {
        if (state == GLUT_DOWN)
        {
            controling = 2;
            rotate_start_x = x;
            rotate_start_y = y;
        }
    }
    else if (controling == 2)
    {
        if (state == GLUT_UP)
        {
            controling = 0;
            atk2[0].bullet_x = atk2[0].x_world;
            atk2[0].bullet_y = atk2[0].y_world+0.5;
            atk2[0].bullet_z = atk2[0].z_world;
            for (int i = 0;i < N_monster;i++)
                if (enemy[i].state == 1)
                {
                    double x1 = enemy[i].x_world;
                    double y1 = enemy[i].z_world;
                    double x2 = sin(atk2[0].dir / 2 / 3.1415926535), y2 = cos(atk2[0].dir / 2 / 3.1415926535);
                    double a = sqrt((x2 - atk2[0].x_world) * (x2 - atk2[0].x_world) + (y2 - atk2[0].z_world) * (y2 - atk2[0].z_world));
                    double b = sqrt((x1 - atk2[0].x_world) * (x1 - atk2[0].x_world) + (y1 - atk2[0].z_world) * (y1 - atk2[0].z_world));
                    double c = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
                    double dir= acos((a * a + b * b - c * c) / (2 * a * b)) / 2 / 3.1415926535 * 360;
                    if (dir < 60)
                    {
                        atk2[0].aim_x = x1;
                        atk2[0].aim_y = y1;
                        atk2[0].aim_enemy = i;
                        atk2[0].draw_bullet = 1;
                        atk2[0].maxstep = 5;
                        break;
                    }
                }
            changingview = changeframe;
            ddx = (pre_view_x - rot_x) / changeframe;
            ddy = (pre_view_y - rot_y) / changeframe;
            if (ddx == 0 && ddy == 0)
            {
                changingview = 0;
            }
        }
    }
    else if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            if (cost >= 20)
            {
                if (x >= 30 && x <= 110 && y >= 90 && y <= 112)
                {
                    cost -= 20;
                    setting = 1;
                    changingview = changeframe;
                    ddx = (90 - rot_x) / changeframe;
                    ddy = (0 - rot_y) / changeframe;
                    if (ddx == 0 && ddy == 0)
                    {
                        changingview = 0;
                    }
                    return;
                }
                else if (x >= 339 && x <= 421 && y >= 92 && y <= 111)
                {
                    cost -= 20;
                    controling = 1;
                    changingview = changeframe;
                    pre_view_x = rot_x;
                    pre_view_y = rot_y;
                    ddx = (90 - rot_x) / changeframe;
                    ddy = (0 - rot_y) / changeframe;
                    if (ddx == 0 && ddy == 0)
                    {
                        changingview = 0;
                    }
                    return;
                }
            }
            if (moving == 0)
            {
                moving = 1;
                mouse_x = x-rot_y;
                mouse_y = -y+rot_x;
            }
        }
        else if (state == GLUT_UP)
        {
            mouse_x = -1;
            mouse_y = -1;
            moving = 0;
        }
    }
}

void mouse_move_recall(int x, int y)
{
    if (controling == 2)
    {
        rotate_end_x = x;
        rotate_end_y = y;
        double a = sqrt((rotate_end_x - rotate_center_x) * (rotate_end_x - rotate_center_x) + (rotate_end_y - rotate_center_y) * (rotate_end_y - rotate_center_y));
        double b = sqrt((rotate_start_x - rotate_center_x) * (rotate_start_x - rotate_center_x) + (rotate_start_y - rotate_center_y) * (rotate_start_y - rotate_center_y));
        double c = sqrt((rotate_start_x - rotate_end_x) * (rotate_start_x - rotate_end_x) + (rotate_start_y - rotate_end_y) * (rotate_start_y - rotate_end_y));
        double dir = acos((a * a + b * b - c * c) / (2 * a * b))/2/3.1415926535*360;
        atk2[0].dir -= dir;
        rotate_start_x = rotate_end_x;
        rotate_start_y = rotate_end_y;
    }
    if (moving == 1)
    {
        rot_y = x - mouse_x;
        rot_x = y + mouse_y;
        glutPostRedisplay();
    }
}

void init_monster()
{
    for (int i = 0;i < N_monster;i++)
    {
        enemy[i].x_map = 1.5;
        enemy[i].y_map = 0.5;
        enemy[i].step = 0.1;
        enemy[i].nowstep = 0;
        enemy[i].mapsize = 30;
        enemy[i].blood = 1;
        enemy[i].state = 0;
        enemy[i].y_base = -7.8;
        enemy[i].deadtime = 0;
    }
}

void init_box1()
{
    for (int i = 0;i < N_box1;i++)
    {
        box1[i].state = 0;
        box1[i].mapsize = 30;
    }
    for (int i = 0;i < N_tower;i++)
    {
        tower[i].state = 0;
        tower[i].mapsize = 30;
    }
    tower[0].state = 1;
    tower[0].x_map = 4;
    tower[0].y_map = 6;
    tower[0].cal_world();
}

void init_atk1()
{
    for (int i = 0;i < N_assist;i++)
    {
        atk1[i].state = 0;
        atk1[i].maxstep = 3;
        atk1[i].step = 0.3;
        atk1[i].nowstep = 0;
        atk1[i].mapsize = 30;
    }
    for (int i = 0;i < N_cannon;i++)
    {
        atk2[i].state = 0;
        atk2[i].nowstep = 0;
        atk2[i].mapsize = 30;
        atk2[i].maxstep = 5;
        atk2[i].dir = 0;
    }
    atk2[0].state = 1;
    atk2[0].x_map = 4;
    atk2[0].y_map = 6;
    atk2[0].update();
}

void alert_land()
{
    for (int i = 0;i < land.objmodel.face_list.size();i++)
    {
        land.objmodel.face_list.at(i).at(1)= land.objmodel.face_list.at(i).at(0);
    }
    for (int i = 0;i < land.objmodel.normal_list.size()-2;i+=3)
    {
        if (land.objmodel.normal_list.at(i + 1) <0)
        {
            land.objmodel.normal_list.at(i) = -land.objmodel.normal_list.at(i);
            land.objmodel.normal_list.at(i+1) = -land.objmodel.normal_list.at(i+1);
            land.objmodel.normal_list.at(i+2) = -land.objmodel.normal_list.at(i+2);
        }
    }
}

void init_obj()
{
    cost = 0;
    number.load_texture("./operatewindow/textures/number.JPG");
    object[0].mtlfile = "./woodbox/woodbox.mtl";
    object[0].objfile = "./woodbox/woodbox.obj";
    object[1].mtlfile = "./metalbox/metalbox.mtl";
    object[1].objfile = "./metalbox/metalbox.obj";
    object[1].size = 1.8;
    object[2].mtlfile = "./gate/gate.mtl";
    object[2].objfile = "./gate/gate.obj";
    object[2].size = 2.5;
    object[3].mtlfile = "./fire/fire.mtl";
    object[3].objfile = "./fire/fire.obj";
    object[4].mtlfile = "./monster/monster.mtl";
    object[4].objfile = "./monster/monster.obj";
    object[4].size = 1;
    object[5].mtlfile = "./assit/assit.mtl";
    object[5].objfile = "./assit/assit.obj";
    object[5].size = 1;
    object[6].mtlfile = "./slime/slime.mtl";
    object[6].objfile = "./slime/slime.obj";
    object[7].mtlfile = "./archer/archer.mtl";
    object[7].objfile = "./archer/archer.obj";
    object[8].mtlfile = "./tree/tree.mtl";
    object[8].objfile = "./tree/tree.obj";
    object[8].size = 1.5;
    object[9].mtlfile = "./cannon/cannon.mtl";
    object[9].objfile = "./cannon/cannon.obj";
    object[9].size = 1.5;
    object[10].mtlfile = "./operatewindow/setting.mtl";
    object[10].objfile = "./operatewindow/setting.obj";
    object[10].size = 5;
    object[11].mtlfile = "./water/water.mtl";
    object[12].objfile = "./sky/sky.obj";
    object[12].mtlfile = "./sky/sky.mtl";
    object[12].size = 100;
    object[12].y = 12;
    object[13].objfile = "./tower/tower.obj";
    object[13].mtlfile = "./tower/tower.mtl";
    object[13].size = 3.5;
    object[14].mtlfile = "./operatewindow/control.mtl";
    object[14].objfile = "./operatewindow/control.obj";
    object[14].size = 2.5;
    object[15].mtlfile = "./operatewindow/cost.mtl";
    object[15].objfile = "./operatewindow/cost.obj";
    object[15].size = 5;
    water.height = 5;
    water.width = 5;
    water.hollow_height = 1.6;
    water.hollow_width = 0.8;
    water.size = 20;
    water.cal_vertex(0);
    //water.save_obj("./water/water.obj");
    water.cal_obj(&(object[11].objmodel));
    object[11].objmodel.applyTransfToMatrix();
    object[11].size = 100;
    object[11].y = -8.5;
    land.mtlfile = "./landscape/landscape.mtl";
    land.objfile = "./landscape/landscape.obj";
    land.size = 30;
    land.y = -8;
    landscape.height = 6;
    landscape.width = 12;
    landscape.size = 20;
    landscape.load_y_map("./landscape/ymap.jpg");
    //landscape.output_obj("./landscape/landscape.obj");
    land.mtlmodel.loadFile(land.mtlfile.c_str());
    land.objmodel.loadFile(land.objfile.c_str());
    alert_land();
   /* sky.height = 200;
    sky.R = 120;
    sky.output_obj("./sky/sky.obj");*/

    //load the material
    object[1].mtlmodel.loadFile(object[1].mtlfile.c_str());
    object[2].mtlmodel.loadFile(object[2].mtlfile.c_str());
    object[4].mtlmodel.loadFile(object[4].mtlfile.c_str());
    object[5].mtlmodel.loadFile(object[5].mtlfile.c_str());
    object[8].mtlmodel.loadFile(object[8].mtlfile.c_str());
    object[9].mtlmodel.loadFile(object[9].mtlfile.c_str());
    object[10].mtlmodel.loadFile(object[10].mtlfile.c_str());
    object[11].mtlmodel.loadFile(object[11].mtlfile.c_str());
    object[12].mtlmodel.loadFile(object[12].mtlfile.c_str());
    object[13].mtlmodel.loadFile(object[13].mtlfile.c_str());
    object[14].mtlmodel.loadFile(object[14].mtlfile.c_str());
    object[15].mtlmodel.loadFile(object[15].mtlfile.c_str());

    // load the model
    object[1].objmodel.loadFile(object[1].objfile.c_str());
    object[2].objmodel.loadFile(object[2].objfile.c_str());
    object[4].objmodel.loadFile(object[4].objfile.c_str());
    object[5].objmodel.loadFile(object[5].objfile.c_str());
    object[8].objmodel.loadFile(object[8].objfile.c_str());
    object[9].objmodel.loadFile(object[9].objfile.c_str());
    object[10].objmodel.loadFile(object[10].objfile.c_str());
    object[12].objmodel.loadFile(object[12].objfile.c_str());
    object[13].objmodel.loadFile(object[13].objfile.c_str());
    object[14].objmodel.loadFile(object[14].objfile.c_str());
    object[15].objmodel.loadFile(object[15].objfile.c_str());
}

int main(int argc, char* argv[])
{
    // GLUT初始化
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(500, 500);
    glutCreateWindow("test");
    initialize();
    glutReshapeFunc(reshape);
    glutIdleFunc(timeadd);
    glutKeyboardFunc(keyboard_recall);
    glutMouseFunc(mouse_recall);
    glutMotionFunc(mouse_move_recall);
    glutDisplayFunc(&display);
    init_obj();
    mymap.initialize();
    init_monster();
    init_box1();
    init_atk1();

    //// 开始显示
    glutMainLoop();

    return 0;
}