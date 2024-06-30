#include "chassis_ops_pos.h"
#include "chassis.h"
#include "apid.h"
extern chassis_t chassis_mai;
static chassis_t* chassis  = &chassis_mai;

chassis_speed_t chassis_ops_speed;
chassis_pos_t chassis_ops_pos;
chassis_pos_t chassis_ops_offeset;

apid_t ops_x,ops_y,ops_z;

#define THETAR 233.78
//单位mm
extern  float pos_x ;
extern  float pos_y ;
extern  float zangle ;
extern float last_angle;
void ops_debug(void);

void relocation_action(chassis_pos_t* standard)
{
	chassis_ops_offeset.x_m = standard->x_m-chassis_ops_pos.x_m;
	chassis_ops_offeset.y_m = standard->y_m-chassis_ops_pos.y_m;
	chassis_ops_offeset.z_rad = standard->z_rad-chassis_ops_pos.z_rad;
}

chassis_pos_t* chassis_ops_get_pos(void)
{
	return &chassis_ops_pos;
}

void chassis_ops_set_pos(chassis_pos_t* pos)
{
	chassis_ops_pos.x_m = pos->x_m-chassis_ops_offeset.x_m;
	chassis_ops_pos.y_m = pos->y_m-chassis_ops_offeset.y_m;
	chassis_ops_pos.z_rad = pos->z_rad-chassis_ops_offeset.z_rad;
}
//参数单位m
void chassis_ops_relative_set(float x,float y,float z)
{
	chassis_ops_pos.y_m +=y;//目标值
	chassis_ops_pos.x_m +=x;
	chassis_ops_pos.z_rad+=z;
}
//参数单位m
void chassis_ops_move(chassis_pos_t* pos)
{
	chassis_ops_set_pos(pos);
	
	while(1){
		if((fabs(chassis_ops_pos.x_m-pos->x_m/1000)<0.01f)&&(fabs(chassis_ops_pos.x_m-pos->y_m/1000)<0.01f))
		{
			break;
		}
		rt_thread_mdelay(10);
	}
}

void chassis_ops_relative_move(float x,float y,float z)
{
	chassis_ops_relative_set(x,y,z);
	while(1)
	{
		if((fabs(chassis_ops_pos.y_m-pos_y/1000)<0.01f)&&(fabs(chassis_ops_pos.x_m-pos_x/1000)<0.01f)&&(fabs(chassis_ops_pos.z_rad*57.2957-zangle)<1.0f))
		{
			break;
		}
		rt_thread_mdelay(10);
	}
}
extern float all_angle; 
void chassis_ops_handle(int cycle)
{
	float theta=(all_angle/180.0f)*3.141592f;//角度
	APID_Set_Target(&ops_x,chassis_ops_pos.x_m*1000);
	APID_Set_Target(&ops_y,chassis_ops_pos.y_m*1000);
	//APID_Set_Target(&ops_z,chassis_ops_pos.z_rad*57.2957804f);
	
	APID_Set_Present(&ops_x,pos_x);
	//APID_Set_Present(&ops_y,pos_y-	theta*THETAR);//减少误差，去除旋转过程中y轴
	APID_Set_Present(&ops_y,pos_y);//减少误差，去除旋转过程中y轴
	APID_Set_Present(&ops_z,all_angle);
	
	APID_Hander(&ops_x,cycle);
	APID_Hander(&ops_y,cycle);
	APID_Hander(&ops_z,cycle);
	
	chassis_ops_speed.x_m_s = APID_Get_Out(&ops_x)/1000.f;
	//chassis_ops_speed.x_m_s = 0;//APID_Get_Out(&ops_x)/1000.f;
	chassis_ops_speed.y_m_s = APID_Get_Out(&ops_y)/1000.f;
	//chassis_ops_speed.y_m_s = 0;//APID_Get_Out(&ops_y)/1000.f;
	chassis_ops_speed.z_rad_s = APID_Get_Out(&ops_z)/57.2957804f;//180/pi
	//chassis_ops_speed.z_rad_s = 0;//APID_Get_Out(&ops_z)/57.2957804f;//180/pi
	
	chassis_set_speed(chassis,&chassis_ops_speed);
	ops_debug();
}


void chassis_ops_rotate(void)
{
	APID_STOP(&ops_y);
	APID_Set_Target(&ops_z,chassis_ops_pos.z_rad*57.2957804f);

	while(1)
	{
		if(fabs(ops_z.parameter.target-ops_z.parameter.present)<0.1)
		{
			break;
		}
		rt_thread_mdelay(20);
	}
	rt_thread_mdelay(100);
	float error_offset = chassis_ops_pos.y_m*1000 - pos_y;
	chassis_ops_offeset.y_m -=error_offset;
	
	chassis_ops_pos.y_m = APID_Get_Present(&ops_y);
	
APID_Enable(&ops_y);
}
void ops_debug(void)
{
	LOG_D("ops:%.2f,%.2f,%.2f",pos_x,pos_y,all_angle);

	//LOG_D("t_x:%.2f,%.2f,%.2f",APID_Get_Target(&ops_x),APID_Get_Present(&ops_x),APID_Get_Out(&ops_x)/1000.f);
	//LOG_D("t_y:%f,%f,%f",APID_Get_Target(&ops_y),APID_Get_Present(&ops_y),APID_Get_Out(&ops_y)/1000.f);
	//LOG_D("t_z:%.2f,%.2f,%.2f",APID_Get_Target(&ops_z),APID_Get_Present(&ops_z),APID_Get_Out(&ops_z)/57.2957804f);
//	LOG_D("t_y:%.2f,p_y:%.2f",APID_Get_Target(&ops_y),APID_Get_Present(&ops_y));
//	LOG_D("t_z:%.2f,p_z:%.2f",APID_Get_Target(&ops_z),APID_Get_Present(&ops_z));
}
int chassis_ops_init(void)
{
//				var_register(&(chassis_ops_pos.x_m), "tarspeed", _f);
//				var_register(&(ops_x.parameter.target), "tarspeed", _f);
//				var_register(&(ops_x.parameter.kp), "kp", _f);
//				var_register(&(ops_x.parameter.ki), "ki", _f);
//				var_register(&(ops_x.parameter.kd), "kd", _f);
//  
//				var_register(&(chassis_ops_pos.y_m), "tarspeed", _f);
//				var_register(&(ops_y.parameter.target), "tarspeed", _f);
//				var_register(&(ops_y.parameter.kp), "kp", _f);
//				var_register(&(ops_y.parameter.ki), "ki", _f);
//				var_register(&(ops_y.parameter.kd), "kd", _f);
	
////				var_register(&(chassis_ops_pos.z_rad), "tarspeed", _f);
//				var_register(&(ops_z.parameter.target), "tarspeed", _f);
//				var_register(&(ops_z.parameter.kp), "kp", _f);
//				var_register(&(ops_z.parameter.ki), "ki", _f);
//				var_register(&(ops_z.parameter.kd), "kd", _f);

	
	APID_Init(&ops_x,PID_POSITION,1.91,0.000,13.058);
	APID_Init(&ops_y,PID_POSITION,4.91,0.0001,13.085);
	APID_Init(&ops_z,PID_POSITION,8.75,0.0,900);
	
	APID_Set_Target(&ops_x,0);
	APID_Set_Target(&ops_y,0);
	APID_Set_Target(&ops_z,0);

  APID_Set_Out_Limit(&ops_x, 1000);    
  APID_Set_Integral_Limit(&ops_x, 20); 
  APID_Set_Bias_Dead_Zone(&ops_x, 0.2);  
       // APID_Set_Bias_Limit(&ops_x, 2000);    
	APID_Set_Out_Limit(&ops_y, 1000);    
  APID_Set_Integral_Limit(&ops_y, 20); 
        //APID_Set_Bias_Dead_Zone(&ops_y, 0.2);		

	APID_Set_Out_Limit(&ops_z, 57.296f*4);    
  APID_Set_Integral_Limit(&ops_z, 20); 
  APID_Set_Bias_Dead_Zone(&ops_z, 0.02);
  return 0;
}

//
void tansform_angle()
{
	
}

INIT_ENV_EXPORT(chassis_ops_init);