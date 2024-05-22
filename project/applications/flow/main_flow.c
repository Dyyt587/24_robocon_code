
#include "main.h"
/* USER CODE BEGIN Includes */
#include "flow_decode.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <rtdbg.h>


static rt_device_t serial;
struct rt_semaphore flow_sem1;
	uint8_t buf_flow;
#define FLOW_UART_NAME       "uart8"      /* 串口设备名称 */

void flow_protool_handle(uint8_t ch);

rt_err_t flow_uart_rx_ind(rt_device_t dev, rt_size_t size)
{
	//LOG_D("ded %d ",size);
		rt_sem_release(&flow_sem1);

		return RT_EOK;
}


 void flow_handle(void*params)
 {
	 	
//			DATA_BITS_8,				/* 8 databits */
//		STOP_BITS_2,				/* 1 stopbit */
//		PARITY_EVEN,				/* No parity  */
	struct serial_configure config = {
		BAUD_RATE_115200,						/* 115200 bits/s */
		DATA_BITS_8,				/* 8 databits */
		STOP_BITS_1,				/* 1 stopbit */
		PARITY_NONE,				/* No parity  */
		BIT_ORDER_LSB,				/* LSB first sent */
		NRZ_NORMAL,					/* Normal mode */
		4096,		/* rxBuf size */
		4096,		/* txBuf size */
		RT_SERIAL_FLOWCONTROL_NONE, /* Off flowcontrol */
		0};
	if (RT_EOK != rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config))
	{
		rt_kprintf("change %s(imu) failed!\n", serial->parent.name);
	}
		rt_sem_init(&flow_sem1, "imu_sem", 0, RT_IPC_FLAG_FIFO);

		if (rt_device_set_rx_indicate(serial, flow_uart_rx_ind) != RT_EOK)
	{
		LOG_E("%s(imu) set rx indicate failed",serial->parent.name);
		//return -1;
	}
		while(1)
		{
			//LOG_D("euwh");
			if(rt_device_read(serial,-1,&buf_flow,1)==0)
			{
				rt_sem_take(&flow_sem1,RT_WAITING_FOREVER);
				continue;
			}
						//LOG_D("data");
	flow_protool_handle(buf_flow);
			
			//rt_thread_mdelay(10);
		}
 }

void flow_protool_handle(uint8_t ch)
{

  PROTOCOL protocol = UPIXELS; //需解析哪种协议，319和306GS支持MAVLINK_PX4、MAVLINK_APM、MSP和UPIXELS协议，302GS支持MAVLINK_PX4_NO_TOF、MSP_NO_TOF和UPIXELS_NO_TOF协议


		int ret;
		//HAL_Delay(100);
		//LOG_D("running...\n");
		//HAL_UART_Receive(&huart3, &ch, 1, 25);

		switch (protocol) {
			case MAVLINK_PX4_NO_TOF: //302GS使用的MAVLINK_PX4纯光流协议
				ret = px4notof_parse_char(ch);
				if(!ret){
					static float integrated_x = 0;
					static float integrated_y = 0;
					static uint8_t quality = 0;
					integrated_x = px4_flow_data.integrated_x;
					integrated_y = px4_flow_data.integrated_y;
					quality = px4_flow_data.quality;
					LOG_D("integrated_x=%f,integrated_y=%f,quality=%d\n",integrated_x,integrated_y,quality);
				}
				break;			
			case MAVLINK_PX4: //319和306GS使用的MAVLINK_PX4协议
				ret = px4_parse_char(ch);
				if(!ret){
					static float integrated_x = 0;
					static float integrated_y = 0;
					static float distance = 0;
					static uint8_t quality = 0;
					integrated_x = px4_flow_data.integrated_x;
					integrated_y = px4_flow_data.integrated_y;
					distance = px4_flow_data.distance;
					quality = px4_flow_data.quality;
					LOG_D("integrated_x=%f,integrated_y=%f,distance=%f,quality=%d\n",integrated_x,integrated_y,distance,quality);
					
					static uint16_t current_distance = 0;
					current_distance = px4_dis_data.current_distance;
					LOG_D("current_distance=%d\n",current_distance);
				}
				break;	
			case MAVLINK_APM: //319和306GS使用的MAVLINK_APM协议
				ret = apm_parse_char(ch);
				if(!ret){
					static float flow_comp_x = 0;
					static float flow_comp_y = 0;
					static float ground_distance = 0;
					static uint8_t quality = 0;
					flow_comp_x = apm_flow_data.flow_comp_x;
					flow_comp_y = apm_flow_data.flow_comp_y;
					ground_distance = apm_flow_data.ground_distance;
					quality = apm_flow_data.quality;
					LOG_D("flow_comp_x=%f,flow_comp_y=%f,ground_distance=%f,quality=%d\n",flow_comp_x,flow_comp_y,ground_distance,quality);

					static uint16_t current_distance = 0;
					current_distance = apm_dis_data.current_distance;
					LOG_D("current_distance=%d\n",current_distance);
				}
				break;			
			case MSP_NO_TOF: //302GS使用的MSP纯光流协议
				ret = mspnotof_parse_char(ch);
				if(!ret){
					static uint8_t flow_quality = 0;
					static int32_t motionX = 0;
					static int32_t motionY = 0;
					flow_quality = msp_flow_data.flow_quality;
					motionX = msp_flow_data.motionX;
					motionY = msp_flow_data.motionY;
					LOG_D("flow_quality=%d,motionX=%d,motionY=%d\n",flow_quality,motionX,motionY);				
				}
				break;
			case MSP: //319和306GS使用的MSP协议
				ret = msp_parse_char(ch);
				if(!ret){
					static uint8_t flow_quality = 0;
					static int32_t motionX = 0;
					static int32_t motionY = 0;
					flow_quality = msp_flow_data.flow_quality;
					motionX = msp_flow_data.motionX;
					motionY = msp_flow_data.motionY;
					LOG_D("flow_quality=%d,motionX=%d,motionY=%d\n",flow_quality,motionX,motionY);		
					
					static uint8_t quality = 0;
					static uint32_t distance = 0;
					quality = msp_dis_data.quality;
					distance = msp_dis_data.distance;
					LOG_D("quality=%d,distance=%d\n",quality,distance);
				}
				break;
			case UPIXELS_NO_TOF: //302GS使用的UPIXELS纯光流协议
				ret = upnotof_parse_char(ch);
				if(!ret){
					static int16_t flow_x_integral = 0;
					static int16_t flow_y_integral = 0;
					static uint8_t valid = 0;
					flow_x_integral = up_flow_data.flow_x_integral;
					flow_y_integral = up_flow_data.flow_y_integral;
					valid = up_flow_data.valid;
					LOG_D("flow_x_integral=%d,flow_y_integral=%d,valid=%d\n",flow_x_integral,flow_y_integral,valid);
				}
				break;
			case UPIXELS: //319和306GS使用的UPIXELS协议
				ret = up_parse_char(ch);
				if(!ret){
					static int16_t flow_x_integral = 0;
					static int16_t flow_y_integral = 0;
					static uint16_t ground_distance = 0;
					static uint8_t valid = 0;
					static uint8_t tof_confidence = 0;
					flow_x_integral = up_data.flow_x_integral;
					flow_y_integral = up_data.flow_y_integral;
					ground_distance = up_data.ground_distance;
					valid = up_data.valid;
					tof_confidence = up_data.tof_confidence;
					LOG_D("flow_x_integral=%d,flow_y_integral=%d,ground_distance=%d,valid=%d,tof_confidence=%d\n",flow_x_integral,flow_y_integral,ground_distance,valid,tof_confidence);
				}
				break;
			break;
			default:
				LOG_D("Wrong Protocol\n");
			break;
		}
		
 }


int Drv_flow_init(void)
{

		/* 查找串口设备 */
    serial = rt_device_find(FLOW_UART_NAME);
    if (!serial)
    {
        rt_kprintf("find %s(flow) failed!\n", FLOW_UART_NAME);
        return 0;
    }

	if (rt_device_open(serial, RT_DEVICE_OFLAG_RDWR ) != RT_EOK)
	{
		LOG_E("%s(flow) open failed",FLOW_UART_NAME);
		return 0;
	}
  rt_thread_t tid_chassis = RT_NULL;

    /* 创建线程， 名称是 thread_test， 入口是 thread_entry*/
    tid_chassis = rt_thread_create("flow",
                                   flow_handle, RT_NULL,
                                   4096,
                                   15, 1);

    /* 线程创建成功，则启动线程 */
    if (tid_chassis != RT_NULL)
    {
        rt_thread_startup(tid_chassis);
    }
    return 0;
}INIT_DEVICE_EXPORT(Drv_flow_init);


