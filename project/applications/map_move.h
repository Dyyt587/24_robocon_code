#ifndef MAP_MOVE_H
#define MAP_MOVE_H


#include "chassis.h"
#include <Eigen/Dense> 
using namespace Eigen;

class MapMove
{
public:
	float init_angle;
	MapMove(chassis_t* chassis,float _init_angle);
	
	~MapMove(void);

	Vector3d V_trans;//平移
	Vector3d cur_pos;
	Vector3d cur_speed;
	AngleAxisd V1;//(M_PI / 4, Vector3d(0, 0, 1));//旋转

	void update(float angle);//更新角度
	void move_pos(float x_m,float y_m,float z_rad);
	void move_pos_wait(float x_m,float y_m,float z_rad);
	void move_speed(float x_m,float y_m,float z_rad);
	void move_speed_wait(float x_m,float y_m,float z_rad);
	Vector3d& get_cur_pos();
	Vector3d& get_cur_speed();	

private:
	chassis_t* chassis;//用于底盘句柄保存
};

void map_move_init(void);
void map_move_update(void);
void map_move_draw(void);

#endif