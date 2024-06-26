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



extern cvdat aball;

void rbmg_handle(void *parameter)
{
    extern chassis_t chassis_mai;
		chassis_speed_t speed;
		static uint8_t mode=0;
	
						rt_thread_mdelay(1000);
////					
////					speed.y_m_s=1.4f;
////					speed.x_m_s=0.0f;
////					speed.z_rad_s=0.0f;
////					chassis_set_speed(&chassis_mai,&speed);
////					rt_thread_mdelay(14000);

////					speed.y_m_s=0.0f;
////					speed.x_m_s=0.0f;
////					speed.z_rad_s=0.7f;
////					chassis_set_speed(&chassis_mai,&speed);
////					rt_thread_mdelay(1800);
////	
////					speed.y_m_s=1.4f;
////					speed.x_m_s=0.0f;
////					speed.z_rad_s=0.0f;
////					chassis_set_speed(&chassis_mai,&speed);
////					rt_thread_mdelay(10500);
////					
////					
////					speed.y_m_s=0.0f;
////					speed.x_m_s=0.0f;
////					speed.z_rad_s=0.7f;
////					chassis_set_speed(&chassis_mai,&speed);
////					rt_thread_mdelay(1800);
//					
////					speed.y_m_s=1.4f;
////					speed.x_m_s=0.0f;
////					speed.z_rad_s=0.0f;
////					chassis_set_speed(&chassis_mai,&speed);
////					rt_thread_mdelay(9500);
////				
////					speed.y_m_s=0.0f;
////					speed.x_m_s=0.0f;
////					speed.z_rad_s=0.7f;
////					chassis_set_speed(&chassis_mai,&speed);
////					rt_thread_mdelay(1800);
////	
////	speed.y_m_s=1.4f;
////					speed.x_m_s=0.0f;
////					speed.z_rad_s=0.0f;
////					chassis_set_speed(&chassis_mai,&speed);
////					rt_thread_mdelay(9500);
////				



//					speed.y_m_s=1.4f;
//					speed.x_m_s=0.0f;
//					speed.z_rad_s=0.0f;
//					chassis_set_speed(&chassis_mai,&speed);
//					rt_thread_mdelay(9000);

//					speed.y_m_s=0.0f;
//					speed.x_m_s=0.0f;
//					speed.z_rad_s=0.7f;
//					chassis_set_speed(&chassis_mai,&speed);
//					rt_thread_mdelay(1800);
//	
//					speed.y_m_s=1.4f;
//					speed.x_m_s=0.0f;
//					speed.z_rad_s=0.0f;
//					chassis_set_speed(&chassis_mai,&speed);
//					rt_thread_mdelay(9500);
//						speed.y_m_s=0.0f;
//					speed.x_m_s=0.0f;
//					speed.z_rad_s=0.0f;
//					chassis_set_speed(&chassis_mai,&speed);
//					LOG_D("stop");
//					rt_thread_mdelay(1000);
//	
    while (1)
    {			
			
			
			
								rt_thread_mdelay(1);

//			if(mode==0)
//			{
//					speed.y_m_s=0.5f;
//					speed.x_m_s=0.0f;
//					speed.z_rad_s=0.0f;
//					chassis_set_speed(&chassis_mai,&speed);
//					rt_thread_mdelay(1000);
//				
//					speed.y_m_s=0.0f;
//					speed.x_m_s=0.0f;
//					speed.z_rad_s=0.6f;
//					chassis_set_speed(&chassis_mai,&speed);
//					rt_thread_mdelay(1000);
//				LOG_D("1mode:%d",mode);
//					mode=1;
//			}
//			else 
//			{
//				LOG_D("2mode:%d",mode);
//					speed.y_m_s=0.0f;
//					speed.x_m_s=0.0f;
//					speed.z_rad_s=0.0f;
//					chassis_set_speed(&chassis_mai,&speed);
//					rt_thread_mdelay(1000);
//			}
			
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