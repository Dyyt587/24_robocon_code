#include "chassis_ops_pos.h"
#include "chassis.h"
#include "apid.h"
extern chassis_t chassis_mai;
static chassis_t* chassis  = &chassis_mai;

chassis_speed_t chassis_ops_speed;
chassis_pos_t chassis_ops_pos;

apid_t ops_x,ops_y,ops_z;


extern  float pos_x ;
extern  float pos_y ;
extern  float zangle ;



void chassis_ops_set_pos(chassis_pos_t* pos)
{
	chassis_ops_pos = *pos;
}

void chassis_ops_handle(int cycle)
{
	
	APID_Set_Present(&ops_x,pos_x);
	APID_Set_Present(&ops_y,pos_y);
	APID_Set_Present(&ops_z,zangle);
	
	APID_Hander(&ops_x,cycle);
	APID_Hander(&ops_y,cycle);
	APID_Hander(&ops_z,cycle);
	
	chassis_ops_speed.x_m_s = APID_Get_Out(&ops_x)/1000.f;
	chassis_ops_speed.y_m_s = APID_Get_Out(&ops_y)/1000.f;
	chassis_ops_speed.z_rad_s = APID_Get_Out(&ops_z)/57.2957804f;//180/pi
	
	chassis_set_speed(chassis,&chassis_ops_speed);
	
}
void ops_debug(void)
{
	LOG_D("t_x:%.2f,p_x:%.2f",APID_Get_Target(&ops_x),APID_Get_Present(&ops_x));
//	LOG_D("t_y:%.2f,p_y:%.2f",APID_Get_Target(&ops_y),APID_Get_Present(&ops_y));
//	LOG_D("t_z:%.2f,p_z:%.2f",APID_Get_Target(&ops_z),APID_Get_Present(&ops_z));
}
int chassis_ops_init(void)
{
				var_register(&(chassis_ops_pos.x_m), "tarspeed", _f);
				var_register(&(ops_x.parameter.kp), "kp", _f);
				var_register(&(ops_y.parameter.ki), "ki", _f);
				var_register(&(ops_z.parameter.kd), "kd", _f);
           
        APID_Set_Out_Limit(&ops_x, 1000);    
        APID_Set_Integral_Limit(&ops_x, 20); 
        APID_Set_Bias_Dead_Zone(&ops_x, 0.2);  
       // APID_Set_Bias_Limit(&ops_x, 2000);    
	        APID_Set_Out_Limit(&ops_y, 1000);    
        APID_Set_Integral_Limit(&ops_y, 20); 
        APID_Set_Bias_Dead_Zone(&ops_y, 0.2);		
 
	        APID_Set_Out_Limit(&ops_z, 57.296f*2);    
        APID_Set_Integral_Limit(&ops_z, 20); 
        //APID_Set_Bias_Dead_Zone(&ops_z, 0.2);
    return 0;
}
INIT_ENV_EXPORT(chassis_ops_init);