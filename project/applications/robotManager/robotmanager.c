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

uint8_t rbmg_mode = CAB_MODE;
uint8_t chassis_dir = 0; // 车辆前进方向，以车体坐标系为主
static float line_error = 0;
static chassis_ctrl_t ctrl;



//有两种模式，一种是上三区的，一种是没上三区的
uint8_t chassis_mode=0;//默认没上三区


typedef enum line
{
    END = 0U,
    FORWARD,
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
float kp_line=0.0001f;
extern chassis_t chassis_mai;
static chassis_speed_t speed;
static uint8_t mode=0;
chassis_pos_t pos;
extern visual_date_t tmp;


//转向
void turn_action(uint8_t mode)
{
//	while(1)
//	{
//		if(posy>5.0f)//转向
//		{
//			if(mode==0)
//			{
//				pos.x_m=chassis_get_pos(&chassis_mai)->x_m;
//				pos.z_rad=chassis_get_pos(&chassis_mai)->z_rad+3.1415926f/2.0f;
//				pos.y_m=chassis_get_pos(&chassis_mai)->y_m;
//				chassis_set_pos(&chassis_mai,&pos);
//				rt_thread_mdelay(50);
//			}
//			else if(mode==1)
//			{
//				pos.x_m=chassis_get_pos(&chassis_mai)->x_m;
//				pos.z_rad=chassis_get_pos(&chassis_mai)->z_rad-3.1415926f/2.0f;
//				pos.y_m=chassis_get_pos(&chassis_mai)->y_m;
//				chassis_set_pos(&chassis_mai,&pos);
//				rt_thread_mdelay(50);
//			}
//		}
//		else 
//		{
//			break;
//		}
//	}
		if(mode==0)
		{
			pos.x_m=chassis_get_pos(&chassis_mai)->x_m;
			pos.z_rad=chassis_get_pos(&chassis_mai)->z_rad+3.1415926f/8.0f;
			pos.y_m=chassis_get_pos(&chassis_mai)->y_m;
			LOG_D("pos2:%f,%f,%f",pos.x_m,pos.y_m,pos.z_rad);
			chassis_set_pos(&chassis_mai,&pos);
			rt_thread_mdelay(150000);
		}
		else if(mode==1)
		{
			pos.x_m=chassis_get_pos(&chassis_mai)->x_m;
			pos.z_rad=chassis_get_pos(&chassis_mai)->z_rad-3.1415926f/8.0f;
			pos.y_m=chassis_get_pos(&chassis_mai)->y_m;
			LOG_D("pos2:%f,%f,%f",pos.x_m,pos.y_m,pos.z_rad);
			chassis_set_pos(&chassis_mai,&pos);
			rt_thread_mdelay(150000);
		}
		speed.y_m_s=0.1f;
		speed.x_m_s=0.0f;
		speed.z_rad_s=0.0f;
		chassis_set_speed(&chassis_mai,&speed);
		rt_thread_mdelay(1500);
}



//巡线
void findline(void)
{
	while(1)
	{
//		if(strcmp(tmp.id_tap,(char *)"line")==0&&posy<4.0f)
		if(strcmp(tmp.id_tap,(char *)"line")==0)
		{
				speed.x_m_s = 0.0f;
				speed.y_m_s = 0.1f;
				speed.z_rad_s =kp_line*(tmp.x  -320.f) ;
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
			break;
		}
	}
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
		rt_thread_mdelay(2000);
		while(1)
		{
			rt_thread_mdelay(100);
		}
		findline();
		rt_thread_mdelay(100);
		if(strcmp(tmp.id_tap,(char *)"LR")==0)
		{
			turn_action(0);
		}
		else if(strcmp(tmp.id_tap,(char *)"LL")==0)
		{
			turn_action(1);
		}
		findline();
		
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