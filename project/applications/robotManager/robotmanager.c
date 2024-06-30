#include <rtthread.h>
#define DBG_TAG "RobotManager"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include "robotManager.h"
#include "abus.h"
#include "chassis_port.h"
#include "abus.h"
#include "math.h"
// #include "PathFinder.h"
#include "aboard_power_switch.h"
#include "drv_visual.h"
#include "bus_sbus.h"
#include "intel_ros.h"
#include "chassis_ops_pos.h"

uint8_t rbmg_mode = CAB_MODE;
uint8_t chassis_dir = 0; // 车辆前进方向，以车体坐标系为主
static float line_error = 0;
static chassis_ctrl_t ctrl;



//有两种模式，一种是上三区的，一种是没上三区的

uint8_t chassis_mode=0;//默认没上三区


typedef enum line
{
    END = 0U,
    FORWARD,//前进
    ROTATIONL,//左自旋
		ROTATIONR,//右自旋
}carline;


rt_uint8_t color_type;

/*
7 8 9
4 5 6
1 2 3
*/



//相对运动函数,这个函数应该是阻塞的,形参一，底盘结构体(映射到chassis_mai)，形参二，相对运动的距离
void chassis_relative_move(chassis_t*chassis,chassis_pos_t*relative_pos)
{
	
}



carline chassisState=ROTATIONL;
//根据红区与蓝区调用不同函数
void RedMoveHandle(chassis_t*chassis,chassis_pos_t*relative_pos)
{
	if(chassis_mode==0)//未上三区
	{
		switch(chassisState)
		{
			//第一步，从出发点自旋转
			case ROTATIONL:
			{
				
				rt_thread_mdelay(1000);//第一个状态跳转就不判断了
				chassisState=FORWARD;
				break;
			}
			//第二步，直走运动距离
			case FORWARD:
			{
				
				//
				rt_thread_mdelay(1000);
				//加识别判断
				break;
			}
			case ROTATIONR:
			{
				
				rt_thread_mdelay(1000);//第一个状态跳转就不判断了
				//可能也加识别判断（包括定位轮）
				break;
			}
			case END:
			{
				
				rt_thread_mdelay(1000);//第一个状态跳转就不判断了
				break;
			}
		}
	}
	else if(chassis_mode==1)//上三区之后的逻辑
	{
		
	}
}

//根据红区与蓝区调用不同函数
void BlueMoveHandle(chassis_t*chassis,chassis_pos_t*relative_pos)
{
	if(chassis_mode==0)//未上三区
	{
		switch(chassisState)
		{
			//第一步，从出发点自旋转
			case ROTATIONL:
			{
				
				rt_thread_mdelay(1000);//第一个状态跳转就不判断了
				chassisState=FORWARD;
				break;
			}
			//第二步，直走运动距离
			case FORWARD:
			{
				
				//
				rt_thread_mdelay(1000);
				//加识别判断
				break;
			}
			case ROTATIONR:
			{
				
				rt_thread_mdelay(1000);//第一个状态跳转就不判断了
				//可能也加识别判断（包括定位轮）
				break;
			}
			case END:
			{
				
				rt_thread_mdelay(1000);//第一个状态跳转就不判断了
				break;
			}
		}
	}
	else if(chassis_mode==1)//上三区之后的逻辑
	{
		
	}
}


int rbmg_dir_callback(abus_topic_t *sub)
{
    // 接收dir变换
    return 0;
}
int rbmg_special_point_callback(abus_topic_t *sub)
{
    
    if (rbmg_mode != CAB_MODE)
    {
        rbmg_mode = ACTION_MODE;
        // LOG_D("special point! now action mode");
    }

    return 0;
}
int rbmg_chassis_ctrl_callback(abus_topic_t *sub)
{
    // 接收底盘控制数据
    return 0;
}
extern float pos_x,pos_y;

extern char id_tap[20];
float kp_line=0.0012f;
extern chassis_t chassis_mai;
static chassis_speed_t speed;
static uint8_t mode=0;
chassis_pos_t pos;
extern visual_date_t tmp;


//转向
void turn_action(uint8_t mode)
{
		speed.y_m_s=0.2f;
		speed.x_m_s=0.0f;
		speed.z_rad_s=0.0f;
		chassis_set_speed(&chassis_mai,&speed);
		rt_thread_mdelay(1500);

		if(mode==0)
		{
			pos.x_m=chassis_get_pos(&chassis_mai)->x_m;
			pos.z_rad=chassis_get_pos(&chassis_mai)->z_rad-3.1415926f/2.0f;
			pos.y_m=chassis_get_pos(&chassis_mai)->y_m;
			LOG_D("pos2:%f,%f,%f",pos.x_m,pos.y_m,pos.z_rad);
			chassis_set_pos(&chassis_mai,&pos);
			rt_thread_mdelay(1500);
		}
		else if(mode==1)
		{
			pos.x_m=chassis_get_pos(&chassis_mai)->x_m;
			pos.z_rad=chassis_get_pos(&chassis_mai)->z_rad+3.1415926f/2.0f;
			pos.y_m=chassis_get_pos(&chassis_mai)->y_m;
			LOG_D("pos2:%f,%f,%f",pos.x_m,pos.y_m,pos.z_rad);
			chassis_set_pos(&chassis_mai,&pos);
			rt_thread_mdelay(1500);
		}
		speed.y_m_s=0.2f;
		speed.x_m_s=0.0f;
		speed.z_rad_s=0.0f;
		chassis_set_speed(&chassis_mai,&speed);
		rt_thread_mdelay(1500);
}



//巡线
void findline(void)
{
	int cnt=0;
	while(1)
	{
//		if(strcmp(tmp.id_tap,(char *)"line")==0&&posy<4.0f)
		if(strcmp(tmp.id_tap,(char *)"line")==0)
		{
				speed.x_m_s = 0.0f;
				speed.y_m_s = 0.6f;
				speed.z_rad_s =-kp_line*(tmp.x  -320.f) ;
				if(speed.z_rad_s>2.0f)
				{
						speed.z_rad_s=2.0f;
						LOG_E("line error");
				}
				else if(speed.z_rad_s<-2.0f)
				{
						speed.z_rad_s=-2.0f;
						LOG_E("line error");
				}
				chassis_set_speed(&chassis_mai,&speed);
				rt_thread_mdelay(50);
		}
		else
		{
			cnt++;
			if(cnt>3){
			break;
			}
		}
	}
}

#define GETBALLSPEED 0.3

/*
@brief: 进入三区放球区域，开始寻球
@param：：0表示无球，1表示有球
*/


int LineTracking(float start_posx,float start_posy,float start_posz)
{
	pos.x_m=start_posx;//pos.x_s 目标点数值储存变量
	pos.y_m=start_posy;
	pos.z_rad=start_posz;
	while(1)
	{
		if(pos.y_m-start_posy<2.7f)//如果车所在的线未循完
		{
			pos.y_m=pos.y_m+0.02*GETBALLSPEED;
			chassis_set_pos(&chassis_mai,&pos); 
			rt_thread_mdelay(20);
		}
		else
		{
			
			
			return 0;
		}
	}
	return 1;
}

void get_ball(void) //取球
{
	float start_posx=chassis_ops_get_pos()->x_m;//获取当前位置
	float start_posy=chassis_ops_get_pos()->y_m;//
	float start_posz=chassis_ops_get_pos()->z_rad;//
		
	while(LineTracking(start_posx,start_posy,start_posz)==0)//当车并未找到球
	{
		if(pos.x_m-start_posx>2.25)//并且移动距离超过放球区域
		{
			while(1)
			{
				rt_thread_mdelay(200);
				LOG_D("finding ball error");//卡死，输出日志
			}
		}

		pos.x_m=start_posx;
		pos.y_m=start_posy;
		pos.z_rad=start_posz;
		chassis_set_pos(&chassis_mai,&pos); //绝对运动函数
		start_posx+=0.8;
		chassis_set_pos(&chassis_mai,&pos); 
		rt_thread_mdelay(20);

	}
	//已经抓到球
  chassis_ops_relative_move(0,-0.1,0);//往后退一小步
	chassis_ops_relative_move(0,0,-3.14159/2);//旋转90°
  chassis_ops_relative_move(0,0.1,0);//再往前走一小步
	
	chassis_ops_relative_move(0,2,0);//车向前走

}




//ops行走第二段
void goops_action(void)
{
	while(1)
	{
		if(pos_y>4.0f&&pos_y<=5.0f)
		{
				speed.x_m_s = 0.0f;
				speed.y_m_s = 0.1f;
				chassis_set_speed(&chassis_mai,&speed);
				rt_thread_mdelay(50);
		}
		else
		{
			break;
		}
	}
}

void rbmg_handle(void *parameter)
{
	
//	motor_set_speed(M3508_5_CAN1,-100);
//	motor_set_speed(M3508_6_CAN1,100);
	rt_thread_mdelay(2000);
	


//		if(strcmp(tmp.id_tap,(char *)"LR")==0)
//		{
//			turn_action(0);
//		}
//		else if(strcmp(tmp.id_tap,(char *)"LL")==0)
//		{
//			turn_action(1);
//		}
//		findline();
	extern float all_angle;
	rt_thread_mdelay(15000);
	chassis_pos_t pos1;
	pos1.x_m=0.0f;
	pos1.y_m=0.0f;
	pos1.z_rad=all_angle/57.2957804f+3.1415926/2;
	chassis_ops_set_pos(&pos1);
	chassis_ops_rotate();
	while(1)
	{
		
		rt_thread_mdelay(15);
	}
chassis_pos_t pos;
		pos.x_m=0.0f;
		pos.y_m=0.0f;
		extern  float zangle ;
		pos.z_rad=zangle/57.2957804f;
//chassis_ops_set_pos(&pos);
		//chassis_ops_relative_move(0.0f,6.15f,0.0f);
		chassis_ops_relative_move(0.0f,0.f,-3.1415926f/2.f);
		rt_thread_mdelay(10000);//
//		chassis_ops_relative_move(3.75f,0.0f,0.0f);
//		rt_thread_mdelay(100);
//		//进入三区
//		chassis_ops_relative_move(0.00f,1.75f,0.0f);
//		rt_thread_mdelay(100);
//		chassis_ops_relative_move(-3.00f,0.0f,0.0f);
//		 
		
		
    while (1)
    {			
			
			rt_thread_mdelay(1);
			
    }
}

int rbmg_init(void)
{
    rt_thread_t tid_rbmg = RT_NULL;

    /* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
    tid_rbmg = rt_thread_create("robotmanger",
                                rbmg_handle, RT_NULL,
                                4096*2,
                                14, 1);

    /* 线程创建成功，则启动线程 */
    if (tid_rbmg != RT_NULL)
    {
        rt_thread_startup(tid_rbmg);
    }
    return 0;
}
INIT_APP_EXPORT(rbmg_init);