#include "Map.h"
int tempmap[6][12]= { {0,0,0,0,0,0,0,1,1,1,0,0},
                      {4,1,1,1,1,1,1,1,0,1,0,0},
                      {0,1,0,1,0,0,0,1,0,1,0,0},
                      {0,1,0,1,0,0,0,1,1,1,1,5},
                      {0,1,1,1,0,0,0,1,0,0,0,0},
                      {0,0,0,1,1,1,1,1,0,0,0,0} };

void myMap::initialize()
{
    for(int i=0;i<6;i++)
        for (int j = 0;j < 12;j++)
        {
            this->map[i][j] = tempmap[i][j];
         }
}

double myMap::getheight(double x_map, double y_map, Landscape* landscape)
{
    int idxx = (int)(x_map * (double)landscape->size);
    int idxy = (int)(y_map * (double)landscape->size);
    return (double)landscape->y_map[idxx][idxy] / ((double)landscape->width * (double)landscape->size / 2);
}

double myMap::getheight_box(double x_map, double y_map, Landscape* landscape)
{
    double y_max = 0;
    for(int i=0;i<landscape->size/2;i++)
        for (int j = 0;j < landscape->size/2;j++)
        {
            int idxx = (int)x_map * landscape->size+i+ landscape->size / 4;
            int idxy = (int)y_map * landscape->size+j+ landscape->size / 4;
            if (landscape->y_map[idxx][idxy] > y_max)
            {
                y_max = landscape->y_map[idxx][idxy];
            }
        }
    return y_max / ((double)landscape->width * (double)landscape->size/2);
}