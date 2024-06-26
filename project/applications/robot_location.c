//#include "robot_location.h"
//#include <math.h>


//locate overall_position;//全局定位的结构体坐标
//Transform chassis_trans;//坐标变换的旋转矩阵

//float map_x=0,map_y=0;


//// 计算坐标变换结果
//locate transformPoint(chassis_t *chassis)
//{
//	locate pos;
//	pos.co_x=cos((float)chassis_trans.theta)*chassis_get_pos(chassis)->x_m-sin((float)chassis_trans.theta)*chassis_get_pos(chassis)->y_m+chassis_trans.tx;
//	pos.co_y=sin((float)chassis_trans.theta)*chassis_get_pos(chassis)->x_m+cos((float)chassis_trans.theta)*chassis_get_pos(chassis)->y_m+chassis_trans.ty;
//	return pos;
//}

//// 将地图坐标系中的点转换为车体坐标系中的点
//chassis_pos_t inverseTransformPoint(locate *p, Transform t) {
//    chassis_pos_t result;
//    // 计算旋转矩阵的转置（即逆矩阵）
//    float cosTheta = cos(t.theta);
//    float sinTheta = sin(t.theta);

//    // 先平移回原点，然后应用逆旋转
//    float x = p->tar_x - t.tx;
//    float y = p->tar_y - t.ty;

//    result.x_m = cosTheta * x + sinTheta * y+t.tx;
//    result.y_m = (-1)*sinTheta * x + cosTheta * y+t.tx;

//    return result;
//}


////从车体坐标到全局坐标的转换(得到当前全局坐标)
//void get_locatePos(chassis_t *chassis)
//{
//	overall_position=transformPoint(chassis);
//	//LOG_D("position:%f,%f",overall_position.co_x,overall_position.co_y);
//}


////获取陀螺仪角度未写
//void get_Angle(float an)
//{
//	chassis_trans.theta=an-chassis_trans.start;
//}




////设置地图坐标系下的目标位置
//void set_TargetPos(float x,float y,locate *dat,chassis_t *chassis)
//{
//	dat->tar_x=x;
//	dat->tar_y=y;
//	chassis->target.pos=inverseTransformPoint(dat,chassis_trans);
//}


//void locateInint(float x,float y)
//{
//	chassis_trans.tx=x;
//	chassis_trans.ty=y;
//	chassis_trans.start=0;
//}

//extern chassis_t chassis_mai;


////线程入口
///**
//功能：
//获取全局位置，全局坐标系
//**/
//void locate_entry(void *parameter)
//{
//		locateInint(0.0f,0.0f);
//    while (1)
//    {
//				get_Angle(-M_PI / 4.0f);
//				set_TargetPos(map_x,map_y,&overall_position,&chassis_mai);
//				//LOG_D("position:%f,%f",chassis_mai.target.pos.x_m,chassis_mai.target.pos.y_m);

//			//chassis_set_pos(&chassis_mai, );
//				rt_thread_mdelay(2);
//    }
//}
////初始化线程
//int locate_init(void)
//{
//	rt_thread_t locate = RT_NULL;
//	/* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
//	locate = rt_thread_create("locate",
//									   locate_entry, RT_NULL,
//									   4096, 
//										 4,
//                      1);

//	/* 线程创建成功，则启动线程 */
//	if (locate != RT_NULL)
//	{
//		rt_thread_startup(locate);
//	}

<<<<<<< Updated upstream
//	return 0;
//}
//INIT_COMPONENT_EXPORT(locate_init);
=======

//���õ�ͼ����ϵ�µ�Ŀ��λ��
void set_TargetPos(float x,float y,locate *dat,chassis_t *chassis)
{
	dat->tar_x=x;
	dat->tar_y=y;
	chassis->target.pos=inverseTransformPoint(dat,chassis_trans);
}


void locateInint(float x,float y)
{
	chassis_trans.tx=x;
	chassis_trans.ty=y;
	chassis_trans.start=0;
}

extern chassis_t chassis_mai;


//�߳����
/**
���ܣ�
��ȡȫ��λ�ã�ȫ������ϵ
**/
void locate_entry(void *parameter)
{
		locateInint(0.0f,0.0f);
    while (1)
    {
				get_Angle(-M_PI / 4.0f);
				LOG_D("position:%f,%f",chassis_mai.target.pos.x_m,chassis_mai.target.pos.y_m);
				set_TargetPos(6.0f,6.0f,&overall_position,&chassis_mai);
				rt_thread_mdelay(2);
    }
}
//��ʼ���߳�
int locate_init(void)
{
	rt_thread_t locate = RT_NULL;
	/* �����̣߳� ������ thread_test�� ����� thread_entry*/
	locate = rt_thread_create("locate",
									   locate_entry, RT_NULL,
									   4096, 
										 9,
                      1);

	/* �̴߳����ɹ����������߳� */
	if (locate != RT_NULL)
	{
		rt_thread_startup(locate);
	}

	return 0;
}
INIT_COMPONENT_EXPORT(locate_init);
>>>>>>> Stashed changes
