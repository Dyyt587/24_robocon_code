/*
 * Fashion Star 串口舵机驱动库
 * Version: v0.0.1
 * UpdateTime: 2021/02/19
 */
#include "fashion_star_uart_servo.h"
#include <rtthread.h>
#include <rtdevice.h>
#include "math.h"

#define DBG_TAG "drv.servo.fus"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

servo_t servo1;
struct rt_semaphore FSUS_sem1;
struct rt_mutex static_fsus_mutex1;

void FSUS_DelayMs(uint16_t time)
{
	rt_thread_mdelay(100);
}
// 用户实现，用于发送pkg到串口
void FSUS_UART_Send(servo_t *servo, PackageTypeDef *pkg)
{
	// 将pkg的数据拷贝到servo的tx_buf
	rt_memcpy(servo->tx_buf, &pkg->header, 2);
	rt_memcpy(servo->tx_buf + 2, &pkg->cmdId, 1);
	rt_memcpy(servo->tx_buf + 3, &pkg->size, 1);
	rt_memcpy(servo->tx_buf + 4, pkg->content, pkg->size);
	rt_memcpy(servo->tx_buf + 4 + pkg->size, &pkg->checksum, 1);
	rt_ssize_t ret = rt_device_write(servo->uart, 0, servo->tx_buf, pkg->size + 5);
	if (ret != pkg->size + 5)
	{
		LOG_D("FSUS_SendPackage: uart write error");
	}
	else
	{
	}
}

/* 接收数据回调函数 */
static rt_err_t FSUS_uart_receive_callback1(rt_device_t dev, rt_size_t size)
{
	// 发送信号量给线程
	rt_sem_release(&FSUS_sem1);
	return 0;
}

// 舵机控制相关的参数
uint8_t servoId = 0;   // 舵机的ID
float curAngle = 0;	   // 舵机当前所在的角度
float nextAngle = 0;   // 舵机的目标角度
uint16_t speed = 500;  // 舵机的转速 单位 °/s
uint16_t interval = 0; // 舵机旋转的周期
uint16_t power = 5;  // 舵机执行功率 mV 默认为0	W
uint8_t wait = 0;	   // 0:不等待 1:等待舵机旋转到特定的位置;
// 舵机角度死区, 如果舵机当前角度跟
// 目标角度相差小于死区则代表舵机到达目标角度, 舵机不再旋转
// <注意事项>
// 		死区跟舵机的型号有关系, 取决于舵机固件的设置, 不同型号的舵机会有差别
float servoDeadBlock = 1.0;
// 查询舵机的角度
uint16_t calcIntervalMs(servo_t *servo, uint8_t servoId, float nextAngle, float speed)
{
	// 读取一下舵机的角度
	FSUS_QueryServoAngle(servo, servoId, &curAngle);
	// 计算角度误差
	float dAngle = (nextAngle > curAngle) ? (nextAngle - curAngle) : (curAngle - nextAngle);
	// 计算所需的时间
	return (uint16_t)((dAngle / speed) * 1000.0);
}

// 等待舵机进入空闲状态IDLE, 即舵机到达目标角度
void waitUntilServoIDLE(servo_t *servo, uint8_t servoId, float nextAngle)
{

	while (1)
	{
		// 读取一下舵机的角度
		FSUS_QueryServoAngle(servo, servoId, &curAngle);

		// 判断舵机是否达到目标角度
		float dAngle = (nextAngle > curAngle) ? (nextAngle - curAngle) : (curAngle - nextAngle);

		// 打印一下当前的舵机角度
		LOG_D("curAngle: %f dAngle: %f\r\n", curAngle, dAngle);

		// 判断是否小于死区
		if (dAngle <= servoDeadBlock)
		{
			break;
		}
		// 等待一小段时间
		rt_thread_mdelay(5);
	}
}


void My_FSUS_up(servo_t *servo)
{
	uint8_t buf[] = {
		0x01, 0x00, 0x69, 0x12, 0x4C, 0x06, 0x21, 0x00, 0x01, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x00, 0xD3, 0x14, 0x8E, 0x23, 0x22, 0x04, 0x01, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x95, 0x02, 0xBC, 0x02, 0x40, 0xFC, 0x00, 0x00, 0x2E, 0x12, 0x4C, 0x05, 0x01, 0x00, 0x64, 0x12, 0x4C, 0x0A};
	rt_device_write(servo->uart, 0, buf, sizeof(buf));
	rt_thread_mdelay(200);
	uint8_t buf1[] = {
		0x01, 0x00, 0x69, 0x12, 0x4C, 0x06, 0x21, 0x00, 0x01, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x00, 0xD3, 0x14, 0x8E, 0x23, 0x22, 0x04, 0x01, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x95, 0x02, 0xBC, 0x02, 0x40, 0xFC, 0x00, 0x00, 0x2E, 0x12, 0x4C, 0x05, 0x01, 0x00, 0x64, 0x12, 0x4C, 0x0A};
	rt_device_write(servo->uart, 0, buf1, sizeof(buf1));
	rt_thread_mdelay(200);

	FSUS_WheelMoveTime(servo, 0, 1, 500, 1800);
	rt_thread_mdelay(2000);
}

void My_FSUS_pick(servo_t *servo)
{
	uint8_t buf[] = {
		0x01, 0x00, 0x69, 0x12, 0x4C, 0x06, 0x21, 0x00, 0x01, 0x00, 0x00, 0x01, 0x05, 0x01, 0x90, 0x01, 0xD3, 0x14, 0x8E, 0x23, 0x22, 0x04, 0x2C, 0x01, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x95, 0x02, 0xBC, 0x02, 0x40, 0xFC, 0x00, 0x00, 0xEA, 0x12, 0x4C, 0x05, 0x01, 0x00, 0x64, 0x12, 0x4C, 0x0A};
	rt_device_write(servo->uart, 0, buf, sizeof(buf));
	rt_thread_mdelay(200);
	uint8_t buf1[] = {
		0x01, 0x00, 0x69, 0x12, 0x4C, 0x06, 0x21, 0x00, 0x01, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x00, 0xD3, 0x14, 0x8E, 0x23, 0x22, 0x04, 0x2c, 0x01, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x95, 0x02, 0xBC, 0x02, 0x40, 0xFC, 0x00, 0x00, 0xBE, 0x12, 0x4C, 0x05, 0x01, 0x00, 0x64, 0x12, 0x4C, 0x0A};
	rt_device_write(servo->uart, 0, buf1, sizeof(buf1));
	rt_thread_mdelay(200);

	FSUS_WheelMoveTime(servo, 0, 0, 500, 1800);
	rt_thread_mdelay(2000);
}
void mypick(void)
{
My_FSUS_pick(&servo1);
}
void myup(void)
{
My_FSUS_up(&servo1);
}

void FSUS_process(void *parameter)
{
	servo_t *servo = (servo_t *)parameter;
	uint8_t ret = FSUS_Ping(&servo1, 0);
	if (ret != FSUS_STATUS_SUCCESS)
	{
		LOG_E("FSUS_Init: Ping error%d", ret);
	}
	else
	{
		LOG_D("FSUS_Init: Ping success");
		rt_thread_mdelay(100);
	}
	My_FSUS_up(&servo1);

	while (1)
	{

		// My_FSUS_pick(&servo1);

		// // 设置舵机的目标角度
		// nextAngle = 60.0;
		// // 根据转速还有角度误差计算周期
		// interval = calcIntervalMs(servo, servoId, nextAngle, speed);
		// LOG_D("Set Servo %f-> %f", curAngle, nextAngle);
		// // 控制舵机角度
		// FSUS_SetServoAngle(servo, servoId, nextAngle, interval, power, wait);
		// rt_thread_mdelay(interval);
		// // rt_thread_mdelay(5);
		// //waitUntilServoIDLE(servo,servoId, nextAngle);

		// // 等待1s 看舵机死区范围
		// rt_thread_mdelay(1000);
		// // 读取一下舵机的角度
		// FSUS_QueryServoAngle(servo, servoId, &curAngle);
		// LOG_D("Final Angle: %f", curAngle);
		// rt_thread_mdelay(1000);

		// // 设置舵机的目标角度
		// nextAngle = -60;
		// // 根据转速还有角度误差计算周期
		// interval = calcIntervalMs(servo, servoId, nextAngle, speed);
		// // 控制舵机角度
		// FSUS_SetServoAngle(servo, servoId, nextAngle, interval, power, wait);
		// // 需要延时一会儿，确保舵机接收并开始执行舵机控制指令
		// // 如果马上发送舵机角度查询信息,新发送的这条指令可能会覆盖舵机角度控制信息
		// rt_thread_mdelay(5);
		// rt_thread_mdelay(interval);

		// //waitUntilServoIDLE(servo, servoId, nextAngle);

		// // 等待1s 看舵机死区范围
		// rt_thread_mdelay(1000);
		// // 读取一下舵机的角度
		// FSUS_QueryServoAngle(servo, servoId, &curAngle);
		// LOG_D("Final Angle: %f", curAngle);
		 rt_thread_mdelay(10);
	}
}
int FSUS_Init(void)
{
	rt_thread_t fsus_thread = RT_NULL;
	// 创建信号量
	rt_sem_init(&FSUS_sem1, "fsus_sem", 0, RT_IPC_FLAG_FIFO);
	servo1.rx_sem = &FSUS_sem1;
	/* 初始化静态互斥量 */
	rt_err_t result = rt_mutex_init(&static_fsus_mutex1, "fsus_mutex", RT_IPC_FLAG_FIFO);
	if (result != RT_EOK)
	{
		rt_kprintf("init static mutex failed.\n");
		return -1;
	}
	servo1.mutex = &static_fsus_mutex1;

	// 初始化串口
	servo1.uart = rt_device_find("uart3");
	if (servo1.uart == RT_NULL)
	{
		rt_kprintf("Can't find uart device\n");
		return -1;
	}
	rt_device_open(servo1.uart, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
	rt_device_set_rx_indicate(servo1.uart, FSUS_uart_receive_callback1);
	servo1.rx_size = 0;
	// 创建线程
	fsus_thread = rt_thread_create("drv_fsus",
								   FSUS_process, &servo1,
								   1024,
								   15, 1);

	if (fsus_thread != RT_NULL)
	{
		rt_thread_startup(fsus_thread);
	}

	return 0;
}
INIT_COMPONENT_EXPORT(FSUS_Init);

// 计算Package的校验和
uint8_t FSUS_CalcChecksum(PackageTypeDef *pkg)
{
	uint8_t checksum;
	// // 初始化环形队列
	// RingBufferTypeDef ringBuf;
	// uint8_t pkgBuf[FSUS_PACK_RESPONSE_MAX_SIZE+1];
	// RingBuffer_Init(&ringBuf, FSUS_PACK_RESPONSE_MAX_SIZE, pkgBuf);
	// // 将Package转换为ringbuffer
	// // 在转换的时候,会自动的计算checksum
	// FSUS_Package2RingBuffer(pkg, &ringBuf);
	// // 获取环形队列队尾的元素(即校验和的位置)
	// checksum = RingBuffer_GetValueByIndex(&ringBuf, RingBuffer_GetByteUsed(&ringBuf)-1);
	checksum += (uint8_t)pkg->header;
	checksum += (uint8_t)(pkg->header >> 8);
	checksum += pkg->cmdId;
	checksum += pkg->size;
	for (int i = 0; i < pkg->size; i++)
	{
		checksum += pkg->content[i];
	}
	return checksum;
}

// 判断是否为有效的请求头的
FSUS_STATUS FSUS_IsValidResponsePackage(PackageTypeDef *pkg)
{
	// 帧头数据不对
	if (pkg->header != FSUS_PACK_RESPONSE_HEADER)
	{
		// 帧头不对
		return FSUS_STATUS_WRONG_RESPONSE_HEADER;
	}
	// 判断控制指令是否有效 指令范围超出
	if (pkg->cmdId > FSUS_CMD_NUM)
	{
		return FSUS_STATUS_UNKOWN_CMD_ID;
	}
	// 参数的size大于FSUS_PACK_RESPONSE_MAX_SIZE里面的限制
	if (pkg->size > (FSUS_PACK_RESPONSE_MAX_SIZE - 5))
	{
		return FSUS_STATUS_SIZE_TOO_BIG;
	}
	// 校验和不匹配
	if (FSUS_CalcChecksum(pkg) != pkg->checksum)
	{
		return FSUS_STATUS_CHECKSUM_ERROR;
	}
	// 数据有效
	return FSUS_STATUS_SUCCESS;
}

// 构造发送数据帧
void FSUS_SendPackage(servo_t *servo, uint8_t cmdId, uint8_t size, uint8_t *content)
{
	// 申请内存
	// LOG_D("[Package] malloc for pkg\r\n");
	PackageTypeDef pkg;

	// 设置帧头
	pkg.header = FSUS_PACK_REQUEST_HEADER;
	// 设置指令ID
	pkg.cmdId = cmdId;
	// 设置尺寸
	pkg.size = size;
	// 逐一拷贝数组里面的内容
	for (int i = 0; i < size; i++)
	{
		pkg.content[i] = content[i];
	}
	uint8_t checksum = FSUS_CalcChecksum(&pkg);
	// pkg.content[size] = checksum;
	pkg.checksum = checksum;
	// 将pkg发送到发送缓冲区sendBuf里面
	// FSUS_Package2RingBuffer(&pkg, servo->sendBuf);
	// 通过串口将数据发送出去
	FSUS_UART_Send(servo, &pkg);
}

// 接收数据帧 (在接收的时候动态的申请内存)
// TODO:函数返回时候即表示已经接收完毕或者接收失败
//		该函数内部需要等待rtthread接收到足够的数据，通常使用在rx回调函数中一次次读取数据进行判断
FSUS_STATUS FSUS_RecvPackage(servo_t *servo, PackageTypeDef *pkg)
{

	pkg->status = 0; // Package状态初始化

	uint8_t bIdx = 0;	 // 接收的参数字节索引号
	uint16_t header = 0; // 帧头
	uint8_t tmpdata = 0;

	// 如果没有超时
	// while (!SysTick_CountdownIsTimeout())
	while (1)
	{

		// 尝试有数据读取
		if (rt_device_read(servo->uart, 0, &tmpdata, 1) == 0)
		{
			// 没有数据了
			// 等待信号量,等待有数据
			if (rt_sem_take(servo->rx_sem, FSUS_TIMEOUT_MS) != RT_EOK)
			{
				// 超时啦

				// TODO:应该做一些清理工作，比如清空接收缓冲区等等
				// rt_device_read()

				return FSUS_STATUS_TIMEOUT;
			}
			continue;
		}
		// 真的有数据了

		// 查看校验码是否已经接收到
		if (pkg->status & FSUS_RECV_FLAG_CONTENT)
		{
			// 参数内容接收完成
			// 接收校验码
			pkg->checksum = tmpdata;
			// 设置状态码-校验码已经接收
			pkg->status = pkg->status | FSUS_RECV_FLAG_CHECKSUM;
			// 直接进行校验码检查
			if (FSUS_CalcChecksum(pkg) != pkg->checksum)
			{

				// 校验码错误
				return FSUS_STATUS_CHECKSUM_ERROR;
			}
			else
			{
				// 数据帧接收成功
				return FSUS_STATUS_SUCCESS;
			}
		}
		else if (pkg->status & FSUS_RECV_FLAG_SIZE)
		{
			// Size已经接收完成
			// 接收参数字节
			pkg->content[bIdx] = tmpdata;
			bIdx++;
			// 判断是否接收完成
			if (bIdx == pkg->size)
			{
				// 标记为参数接收完成
				pkg->status = pkg->status | FSUS_RECV_FLAG_CONTENT;
			}
		}
		else if (pkg->status & FSUS_RECV_FLAG_CMD_ID)
		{
			// 指令接收完成
			// 接收尺寸信息
			pkg->size = tmpdata;
			// 判断长度size是否合法
			// 参数的size大于FSUS_PACK_RESPONSE_MAX_SIZE里面的限制
			if (pkg->size > (FSUS_PACK_RESPONSE_MAX_SIZE - 5))
			{

				return FSUS_STATUS_SIZE_TOO_BIG;
			}
			// 设置尺寸接收完成的标志位
			pkg->status = pkg->status | FSUS_RECV_FLAG_SIZE;
		}
		else if (pkg->status & FSUS_RECV_FLAG_HEADER)
		{
			// 帧头已接收
			// 接收指令ID
			pkg->cmdId = tmpdata;
			// 判断指令是否合法
			// 判断控制指令是否有效 指令范围超出
			if (pkg->cmdId > FSUS_CMD_NUM)
			{

				return FSUS_STATUS_UNKOWN_CMD_ID;
			}
			// 设置cmdId已经接收到标志位
			pkg->status = pkg->status | FSUS_RECV_FLAG_CMD_ID;
		}
		else
		{
			// 接收帧头
			if (header == 0)
			{
				// 接收第一个字节
				header = tmpdata;
				// 判断接收的第一个字节对不对
				if (header != (FSUS_PACK_RESPONSE_HEADER & 0x0F))
				{
					// 第一个字节不对 header重置为0
					header = 0;
				}
			}
			else if (header == (FSUS_PACK_RESPONSE_HEADER & 0x0F))
			{
				// 接收帧头第二个字节
				header = header | (tmpdata << 8);
				// 检查第二个字节是否正确
				if (header != FSUS_PACK_RESPONSE_HEADER)
				{
					header = 0;
				}
				else
				{
					pkg->header = header;
					// 帧头接收成功
					pkg->status = pkg->status | FSUS_RECV_FLAG_HEADER;
				}
			}
			else
			{
				header = 0;
			}
		}
	}
	// 等待超时
	return FSUS_STATUS_TIMEOUT;
}

// 舵机通讯检测
// 注: 如果没有舵机响应这个Ping指令的话, 就会超时
FSUS_STATUS FSUS_Ping(servo_t *servo, uint8_t servo_id)
{
	// TODO:加锁！！！！！！！！！！！
	uint8_t statusCode;	 // 状态码
	uint8_t ehcoServoId; // PING得到的舵机ID
						 // LOG_D("[PING]Send Ping Package\r\n");
						 // 发送请求包
	if (rt_mutex_take(servo->mutex, FSUS_TIMEOUT_MS) != RT_EOK)
	{
		return FSUS_STATUS_TIMEOUT;
	}
	FSUS_SendPackage(servo, FSUS_CMD_PING, 1, &servo_id);
	// 接收返回的Ping
	PackageTypeDef pkg;
	statusCode = FSUS_RecvPackage(servo, &pkg);
	rt_mutex_release(servo->mutex);

	if (statusCode == FSUS_STATUS_SUCCESS)
	{
		// 进一步检查ID号是否匹配
		ehcoServoId = (uint8_t)pkg.content[0];
		if (ehcoServoId != servo_id)
		{
			// 反馈得到的舵机ID号不匹配
			return FSUS_STATUS_ID_NOT_MATCH;
		}
	}
	return statusCode;
}

// 重置舵机的用户资料
FSUS_STATUS FSUS_ResetUserData(servo_t *servo, uint8_t servo_id)
{
	const uint8_t size = 1;
	FSUS_STATUS statusCode;
	if (rt_mutex_take(servo->mutex, FSUS_TIMEOUT_MS) != RT_EOK)
	{
		return FSUS_STATUS_TIMEOUT;
	}
	// 发送请求包
	FSUS_SendPackage(servo, FSUS_CMD_RESET_USER_DATA, size, &servo_id);
	// 接收重置结果
	PackageTypeDef pkg;
	statusCode = FSUS_RecvPackage(servo, &pkg);
	rt_mutex_release(servo->mutex);

	if (statusCode == FSUS_STATUS_SUCCESS)
	{
		// 成功的接收到反馈数据
		// 读取反馈数据中的result
		uint8_t result = (uint8_t)pkg.content[1];
		if (result == 1)
		{
			return FSUS_STATUS_SUCCESS;
		}
		else
		{
			return FSUS_STATUS_FAIL;
		}
	}
	return statusCode;
}

// 读取数据
FSUS_STATUS FSUS_ReadData(servo_t *servo, uint8_t servo_id, uint8_t address, uint8_t *value, uint8_t *size)
{
	FSUS_STATUS statusCode;
	// 构造content
	uint8_t buffer[2] = {servo_id, address};
	// 发送请求数据
	FSUS_SendPackage(servo, FSUS_CMD_READ_DATA, 2, buffer);
	// 接收返回信息
	PackageTypeDef pkg;
	statusCode = FSUS_RecvPackage(servo, &pkg);
	if (statusCode == FSUS_STATUS_SUCCESS)
	{
		// 读取数据
		// 读取数据是多少个位
		*size = pkg.size - 2; // content的长度减去servo_id跟address的长度
		// 数据拷贝
		for (int i = 0; i < *size; i++)
		{
			value[i] = pkg.content[i + 2];
		}
	}
	return statusCode;
}

// 写入数据
FSUS_STATUS FSUS_WriteData(servo_t *servo, uint8_t servo_id, uint8_t address, uint8_t *value, uint8_t size)
{
	FSUS_STATUS statusCode;
	// 构造content
	uint8_t buffer[size + 2]; // 舵机ID + 地址位Address + 数据byte数
	buffer[0] = servo_id;
	buffer[1] = address;
	// 拷贝数据
	for (int i = 0; i < size; i++)
	{
		buffer[i + 2] = value[i];
	}
	// 发送请求数据
	FSUS_SendPackage(servo, FSUS_CMD_WRITE_DATA, size + 2, buffer);
	// 接收返回信息
	PackageTypeDef pkg;
	statusCode = FSUS_RecvPackage(servo, &pkg);
	if (statusCode == FSUS_STATUS_SUCCESS)
	{
		uint8_t result = pkg.content[2];
		if (result == 1)
		{
			statusCode = FSUS_STATUS_SUCCESS;
		}
		else
		{
			statusCode = FSUS_STATUS_FAIL;
		}
	}
	return statusCode;
}

/*
 * 轮转控制模式
 * speed单位 °/s
 */
FSUS_STATUS FSUS_WheelMove(servo_t *servo, uint8_t servo_id, uint8_t method, uint16_t speed, uint16_t value)
{
	// 创建环形缓冲队列
	const uint8_t size = 6;
	uint8_t buffer[size + 1];
	// RingBufferTypeDef ringBuf;
	// RingBuffer_Init(&ringBuf, size, buffer);
	// 写入content
	// RingBuffer_WriteByte(&ringBuf, servo_id); // 舵机ID
	// RingBuffer_WriteByte(&ringBuf, method);	  // 写入执行方式与旋转方向
	// RingBuffer_WriteUShort(&ringBuf, speed);  // 设置舵机的旋转速度 °/s
	// RingBuffer_WriteUShort(&ringBuf, value);

	buffer[0] = servo_id;
	buffer[1] = method;
	buffer[2] = speed & 0xFF;
	buffer[3] = (speed >> 8) & 0xFF;
	buffer[4] = value & 0xFF;
	buffer[5] = (value >> 8) & 0xFF;

	if (rt_mutex_take(servo->mutex, FSUS_TIMEOUT_MS) != RT_EOK)
	{
		return FSUS_STATUS_TIMEOUT;
	}
	// 发送请求包
	// FSUS_SendPackage(servo, FSUS_CMD_SPIN, size, buffer + 1);
	FSUS_SendPackage(servo, FSUS_CMD_SPIN, size, buffer);
	rt_mutex_release(servo->mutex);

	return FSUS_STATUS_SUCCESS;
}

// 轮转模式, 舵机停止转动
FSUS_STATUS FSUS_WheelStop(servo_t *servo, uint8_t servo_id)
{
	uint8_t method = 0x00;
	uint16_t speed = 0;
	uint16_t value = 0;
	return FSUS_WheelMove(servo, servo_id, method, speed, value);
}

// 轮转模式 不停旋转
FSUS_STATUS FSUS_WheelKeepMove(servo_t *servo, uint8_t servo_id, uint8_t is_cw, uint16_t speed)
{
	uint8_t method = 0x01; // 持续旋转
	if (is_cw)
	{
		// 顺时针旋转
		method = method | 0x80;
	}
	uint16_t value = 0;
	return FSUS_WheelMove(servo, servo_id, method, speed, value);
}

// 轮转模式 按照特定的速度旋转特定的时间
FSUS_STATUS FSUS_WheelMoveTime(servo_t *servo, uint8_t servo_id, uint8_t is_cw, uint16_t speed, uint16_t nTime)
{
	uint8_t method = 0x03; // 旋转一段时间
	if (is_cw)
	{
		// 顺时针旋转
		method = method | 0x80;
	}
	return FSUS_WheelMove(servo, servo_id, method, speed, nTime);
}

// 轮转模式 旋转特定的圈数
FSUS_STATUS FSUS_WheelMoveNCircle(servo_t *servo, uint8_t servo_id, uint8_t is_cw, uint16_t speed, uint16_t nCircle)
{
	uint8_t method = 0x02; // 旋转特定的圈数
	if (is_cw)
	{
		// 顺时针旋转
		method = method | 0x80;
	}
	return FSUS_WheelMove(servo, servo_id, method, speed, nCircle);
}

// 设置舵机的角度
// @angle 单位度
// @interval 单位ms
// @power 舵机执行功率 单位mW
//        若power=0或者大于保护值
FSUS_STATUS FSUS_SetServoAngle(servo_t *servo, uint8_t servo_id, float angle, uint16_t interval, uint16_t power, uint8_t wait)
{

	// 创建环形缓冲队列
	const uint8_t size = 7;
	uint8_t buffer[size];
	// RingBufferTypeDef ringBuf;
	// RingBuffer_Init(&ringBuf, size, buffer);
	// 数值约束
	if (angle > 180.0f)
	{
		angle = 180.0f;
	}
	else if (angle < -180.0f)
	{
		angle = -180.0f;
	}
	// 构造content
	// RingBuffer_WriteByte(&ringBuf, servo_id);
	// RingBuffer_WriteShort(&ringBuf, (int16_t)(10 * angle));
	// RingBuffer_WriteUShort(&ringBuf, interval);
	// RingBuffer_WriteUShort(&ringBuf, power);

	buffer[0] = servo_id;
	buffer[1] = (int16_t)(10 * angle) & 0xFF;
	buffer[2] = ((int16_t)(10 * angle) >> 8) & 0xFF;
	buffer[3] = interval & 0xFF;
	buffer[4] = (interval >> 8) & 0xFF;
	buffer[5] = power & 0xFF;
	buffer[6] = (power >> 8) & 0xFF;

	// 发送请求包
	if (rt_mutex_take(servo->mutex, FSUS_TIMEOUT_MS) != RT_EOK)
	{
		return FSUS_STATUS_TIMEOUT;
	}
	FSUS_SendPackage(servo, FSUS_CMD_ROTATE, size, buffer);
	rt_mutex_release(servo->mutex);

	if (wait)
	{
		return FSUS_Wait(servo, servo_id, angle, 0);
	}
	else
	{
		return FSUS_STATUS_SUCCESS;
	}
}

/* 设置舵机的角度(指定周期) */
FSUS_STATUS FSUS_SetServoAngleByInterval(servo_t *servo, uint8_t servo_id,
										 float angle, uint16_t interval, uint16_t t_acc,
										 uint16_t t_dec, uint16_t power, uint8_t wait)
{
	// 创建环形缓冲队列
	const uint8_t size = 11;
	uint8_t buffer[size];
	// RingBufferTypeDef ringBuf;
	// RingBuffer_Init(&ringBuf, size, buffer);
	// 数值约束
	if (angle > 180.0f)
	{
		angle = 180.0f;
	}
	else if (angle < -180.0f)
	{
		angle = -180.0f;
	}
	if (t_acc < 20)
	{
		t_acc = 20;
	}
	if (t_dec < 20)
	{
		t_dec = 20;
	}

	// 协议打包
	// RingBuffer_WriteByte(&ringBuf, servo_id);
	// RingBuffer_WriteShort(&ringBuf, (int16_t)(10 * angle));
	// RingBuffer_WriteUShort(&ringBuf, interval);
	// RingBuffer_WriteUShort(&ringBuf, t_acc);
	// RingBuffer_WriteUShort(&ringBuf, t_dec);
	// RingBuffer_WriteUShort(&ringBuf, power);

	buffer[0] = servo_id;
	buffer[1] = (int16_t)(10 * angle) & 0xFF;
	buffer[2] = ((int16_t)(10 * angle) >> 8) & 0xFF;
	buffer[3] = interval & 0xFF;
	buffer[4] = (interval >> 8) & 0xFF;
	buffer[5] = t_acc & 0xFF;
	buffer[6] = (t_acc >> 8) & 0xFF;
	buffer[7] = t_dec & 0xFF;
	buffer[8] = (t_dec >> 8) & 0xFF;
	buffer[9] = power & 0xFF;
	buffer[10] = (power >> 8) & 0xFF;

	// 发送请求包
	if (rt_mutex_take(servo->mutex, FSUS_TIMEOUT_MS) != RT_EOK)
	{
		return FSUS_STATUS_TIMEOUT;
	}
	FSUS_SendPackage(servo, FSUS_CMD_SET_SERVO_ANGLE_BY_INTERVAL, size, buffer);
	rt_mutex_release(servo->mutex);

	if (wait)
	{
		return FSUS_Wait(servo, servo_id, angle, 0);
	}
	else
	{
		return FSUS_STATUS_SUCCESS;
	}
}

/* 设置舵机的角度(指定转速) */
FSUS_STATUS FSUS_SetServoAngleByVelocity(servo_t *servo, uint8_t servo_id,
										 float angle, float velocity, uint16_t t_acc,
										 uint16_t t_dec, uint16_t power, uint8_t wait)
{
	// 创建环形缓冲队列
	const uint8_t size = 11;
	uint8_t buffer[size];
	// RingBufferTypeDef ringBuf;
	// RingBuffer_Init(&ringBuf, size, buffer);

	// 数值约束
	if (angle > 180.0f)
	{
		angle = 180.0f;
	}
	else if (angle < -180.0f)
	{
		angle = -180.0f;
	}
	if (velocity < 1.0f)
	{
		velocity = 1.0f;
	}
	else if (velocity > 750.0f)
	{
		velocity = 750.0f;
	}
	if (t_acc < 20)
	{
		t_acc = 20;
	}
	if (t_dec < 20)
	{
		t_dec = 20;
	}

	// 协议打包
	// RingBuffer_WriteByte(&ringBuf, servo_id);
	// RingBuffer_WriteShort(&ringBuf, (int16_t)(10.0f * angle));
	// RingBuffer_WriteUShort(&ringBuf, (uint16_t)(10.0f * velocity));
	// RingBuffer_WriteUShort(&ringBuf, t_acc);
	// RingBuffer_WriteUShort(&ringBuf, t_dec);
	// RingBuffer_WriteUShort(&ringBuf, power);

	buffer[0] = servo_id;
	buffer[1] = (int16_t)(10 * angle) & 0xFF;
	buffer[2] = ((int16_t)(10 * angle) >> 8) & 0xFF;
	buffer[3] = (uint16_t)(10 * velocity) & 0xFF;
	buffer[4] = ((uint16_t)(10 * velocity) >> 8) & 0xFF;
	buffer[5] = t_acc & 0xFF;
	buffer[6] = (t_acc >> 8) & 0xFF;
	buffer[7] = t_dec & 0xFF;
	buffer[8] = (t_dec >> 8) & 0xFF;
	buffer[9] = power & 0xFF;
	buffer[10] = (power >> 8) & 0xFF;

	// 发送请求包
	if (rt_mutex_take(servo->mutex, FSUS_TIMEOUT_MS) != RT_EOK)
	{
		return FSUS_STATUS_TIMEOUT;
	}
	FSUS_SendPackage(servo, FSUS_CMD_SET_SERVO_ANGLE_BY_VELOCITY, size, buffer);
	rt_mutex_release(servo->mutex);

	if (wait)
	{
		return FSUS_Wait(servo, servo_id, angle, 0);
	}
	else
	{
		return FSUS_STATUS_SUCCESS;
	}
}

/* 查询单个舵机的角度信息 angle 单位度 */
FSUS_STATUS FSUS_QueryServoAngle(servo_t *servo, uint8_t servo_id, float *angle)
{
	const uint8_t size = 1; // 请求包content的长度
	uint8_t ehcoServoId;
	int16_t echoAngle;
	if (rt_mutex_take(servo->mutex, FSUS_TIMEOUT_MS) != RT_EOK)
	{
		return FSUS_STATUS_TIMEOUT;
	}
	// 发送舵机角度请求包
	FSUS_SendPackage(servo, FSUS_CMD_READ_ANGLE, size, &servo_id);
	// 接收返回的Ping
	PackageTypeDef pkg;
	uint8_t statusCode = FSUS_RecvPackage(servo, &pkg);
	rt_mutex_release(servo->mutex);

	if (statusCode == FSUS_STATUS_SUCCESS)
	{
		// 成功的获取到舵机角度回读数据
		ehcoServoId = (uint8_t)pkg.content[0];
		// 检测舵机ID是否匹配
		if (ehcoServoId != servo_id)
		{
			// 反馈得到的舵机ID号不匹配
			return FSUS_STATUS_ID_NOT_MATCH;
		}

		// 提取舵机角度
		echoAngle = (int16_t)(pkg.content[1] | (pkg.content[2] << 8));
		*angle = (float)(echoAngle / 10.0);
	}
	return statusCode;
}

/* 设置舵机的角度(多圈模式) */
FSUS_STATUS FSUS_SetServoAngleMTurn(servo_t *servo, uint8_t servo_id, float angle,
									uint32_t interval, uint16_t power, uint8_t wait)
{
	// 创建环形缓冲队列
	const uint8_t size = 11;
	uint8_t buffer[size];
	// RingBufferTypeDef ringBuf;
	// RingBuffer_Init(&ringBuf, size, buffer);
	// 数值约束
	if (angle > 368640.0f)
	{
		angle = 368640.0f;
	}
	else if (angle < -368640.0f)
	{
		angle = -368640.0f;
	}
	if (interval > 4096000)
	{
		angle = 4096000;
	}
	// 协议打包
	// RingBuffer_WriteByte(&ringBuf, servo_id);
	// RingBuffer_WriteLong(&ringBuf, (int32_t)(10 * angle));
	// RingBuffer_WriteULong(&ringBuf, interval);
	// RingBuffer_WriteShort(&ringBuf, power);

	buffer[0] = servo_id;
	buffer[1] = (int32_t)(10 * angle) & 0xFF;
	buffer[2] = ((int32_t)(10 * angle) >> 8) & 0xFF;
	buffer[3] = ((int32_t)(10 * angle) >> 16) & 0xFF;
	buffer[4] = ((int32_t)(10 * angle) >> 24) & 0xFF;
	buffer[5] = interval & 0xFF;
	buffer[6] = (interval >> 8) & 0xFF;
	buffer[7] = (interval >> 16) & 0xFF;
	buffer[8] = (interval >> 24) & 0xFF;
	buffer[9] = power & 0xFF;
	buffer[10] = (power >> 8) & 0xFF;

	// 发送请求包
	if (rt_mutex_take(servo->mutex, FSUS_TIMEOUT_MS) != RT_EOK)
	{
		return FSUS_STATUS_TIMEOUT;
	}
	FSUS_SendPackage(servo, FSUS_CMD_SET_SERVO_ANGLE_MTURN, size, buffer);
	rt_mutex_release(servo->mutex);

	if (wait)
	{
		return FSUS_Wait(servo, servo_id, angle, 1);
	}
	else
	{
		return FSUS_STATUS_SUCCESS;
	}
}

/* 设置舵机的角度(多圈模式, 指定周期) */
FSUS_STATUS FSUS_SetServoAngleMTurnByInterval(servo_t *servo, uint8_t servo_id, float angle,
											  uint32_t interval, uint16_t t_acc, uint16_t t_dec, uint16_t power, uint8_t wait)
{
	// 创建环形缓冲队列
	const uint8_t size = 15;
	uint8_t buffer[size];
	// RingBufferTypeDef ringBuf;
	// RingBuffer_Init(&ringBuf, size, buffer);

	// 数值约束
	if (angle > 368640.0f)
	{
		angle = 368640.0f;
	}
	else if (angle < -368640.0f)
	{
		angle = -368640.0f;
	}
	if (interval > 4096000)
	{
		interval = 4096000;
	}
	if (t_acc < 20)
	{
		t_acc = 20;
	}
	if (t_dec < 20)
	{
		t_dec = 20;
	}
	// 协议打包
	// RingBuffer_WriteByte(&ringBuf, servo_id);
	// RingBuffer_WriteLong(&ringBuf, (int32_t)(10 * angle));
	// RingBuffer_WriteULong(&ringBuf, interval);
	// RingBuffer_WriteUShort(&ringBuf, t_acc);
	// RingBuffer_WriteUShort(&ringBuf, t_dec);
	// RingBuffer_WriteShort(&ringBuf, power);

	buffer[0] = servo_id;
	buffer[1] = (int32_t)(10 * angle) & 0xFF;
	buffer[2] = ((int32_t)(10 * angle) >> 8) & 0xFF;
	buffer[3] = ((int32_t)(10 * angle) >> 16) & 0xFF;
	buffer[4] = ((int32_t)(10 * angle) >> 24) & 0xFF;
	buffer[5] = interval & 0xFF;
	buffer[6] = (interval >> 8) & 0xFF;
	buffer[7] = (interval >> 16) & 0xFF;
	buffer[8] = (interval >> 24) & 0xFF;
	buffer[9] = t_acc & 0xFF;
	buffer[10] = (t_acc >> 8) & 0xFF;
	buffer[11] = t_dec & 0xFF;
	buffer[12] = (t_dec >> 8) & 0xFF;
	buffer[13] = power & 0xFF;
	buffer[14] = (power >> 8) & 0xFF;

	// 发送请求包
	if (rt_mutex_take(servo->mutex, FSUS_TIMEOUT_MS) != RT_EOK)
	{
		return FSUS_STATUS_TIMEOUT;
	}
	FSUS_SendPackage(servo, FSUS_CMD_SET_SERVO_ANGLE_MTURN_BY_INTERVAL, size, buffer);
	rt_mutex_release(servo->mutex);

	if (wait)
	{
		return FSUS_Wait(servo, servo_id, angle, 1);
	}
	else
	{
		return FSUS_STATUS_SUCCESS;
	}
}

/* 设置舵机的角度(多圈模式, 指定转速) */
FSUS_STATUS FSUS_SetServoAngleMTurnByVelocity(servo_t *servo, uint8_t servo_id, float angle,
											  float velocity, uint16_t t_acc, uint16_t t_dec, uint16_t power, uint8_t wait)
{
	// 创建环形缓冲队列
	const uint8_t size = 13;
	uint8_t buffer[size];
	// RingBufferTypeDef ringBuf;
	// RingBuffer_Init(&ringBuf, size, buffer);
	// 数值约束
	if (angle > 368640.0f)
	{
		angle = 368640.0f;
	}
	else if (angle < -368640.0f)
	{
		angle = -368640.0f;
	}
	if (velocity < 1.0f)
	{
		velocity = 1.0f;
	}
	else if (velocity > 750.0f)
	{
		velocity = 750.0f;
	}
	if (t_acc < 20)
	{
		t_acc = 20;
	}
	if (t_dec < 20)
	{
		t_dec = 20;
	}
	// 协议打包
	// RingBuffer_WriteByte(&ringBuf, servo_id);
	// RingBuffer_WriteLong(&ringBuf, (int32_t)(10.0f * angle));
	// RingBuffer_WriteUShort(&ringBuf, (uint16_t)(10.0f * velocity));
	// RingBuffer_WriteUShort(&ringBuf, t_acc);
	// RingBuffer_WriteUShort(&ringBuf, t_dec);
	// RingBuffer_WriteShort(&ringBuf, power);

	buffer[0] = servo_id;
	buffer[1] = (int32_t)(10 * angle) & 0xFF;
	buffer[2] = ((int32_t)(10 * angle) >> 8) & 0xFF;
	buffer[3] = ((int32_t)(10 * angle) >> 16) & 0xFF;
	buffer[4] = ((int32_t)(10 * angle) >> 24) & 0xFF;
	buffer[5] = (uint16_t)(10 * velocity) & 0xFF;
	buffer[6] = ((uint16_t)(10 * velocity) >> 8) & 0xFF;
	buffer[7] = t_acc & 0xFF;
	buffer[8] = (t_acc >> 8) & 0xFF;
	buffer[9] = t_dec & 0xFF;
	buffer[10] = (t_dec >> 8) & 0xFF;
	buffer[11] = power & 0xFF;
	buffer[12] = (power >> 8) & 0xFF;

	// 发送请求包
	if (rt_mutex_take(servo->mutex, FSUS_TIMEOUT_MS) != RT_EOK)
	{
		return FSUS_STATUS_TIMEOUT;
	}
	FSUS_SendPackage(servo, FSUS_CMD_SET_SERVO_ANGLE_MTURN_BY_VELOCITY, size, buffer);
	rt_mutex_release(servo->mutex);

	if (wait)
	{
		return FSUS_Wait(servo, servo_id, angle, 1);
	}
	else
	{
		return FSUS_STATUS_SUCCESS;
	}
}

/* 查询舵机的角度(多圈模式) */
FSUS_STATUS FSUS_QueryServoAngleMTurn(servo_t *servo, uint8_t servo_id, float *angle)
{
	// 创建环形缓冲队列
	const uint8_t size = 1; // 请求包content的长度
	uint8_t ehcoServoId;
	int32_t echoAngle;
	if (rt_mutex_take(servo->mutex, FSUS_TIMEOUT_MS) != RT_EOK)
	{
		return FSUS_STATUS_TIMEOUT;
	}
	// 发送舵机角度请求包
	FSUS_SendPackage(servo, FSUS_CMD_QUERY_SERVO_ANGLE_MTURN, size, &servo_id);
	// 接收返回的Ping
	PackageTypeDef pkg;
	uint8_t statusCode = FSUS_RecvPackage(servo, &pkg);
	if (statusCode == FSUS_STATUS_SUCCESS)
	{
		// 成功的获取到舵机角度回读数据
		ehcoServoId = (uint8_t)pkg.content[0];
		// 检测舵机ID是否匹配
		if (ehcoServoId != servo_id)
		{
			// 反馈得到的舵机ID号不匹配
			return FSUS_STATUS_ID_NOT_MATCH;
		}

		// 提取舵机角度
		echoAngle = (int32_t)(pkg.content[1] | (pkg.content[2] << 8) | (pkg.content[3] << 16) | (pkg.content[4] << 24));
		*angle = (float)(echoAngle / 10.0);
	}
	rt_mutex_release(servo->mutex);
	return statusCode;
}

/* 舵机阻尼模式 */
FSUS_STATUS FSUS_DampingMode(servo_t *servo, uint8_t servo_id, uint16_t power)
{
	const uint8_t size = 3; // 请求包content的长度
	uint8_t buffer[size];	// content缓冲区
	// RingBufferTypeDef ringBuf;				 // 创建环形缓冲队列
	// RingBuffer_Init(&ringBuf, size, buffer); // 缓冲队列初始化
	// 构造content
	// RingBuffer_WriteByte(&ringBuf, servo_id);
	// RingBuffer_WriteUShort(&ringBuf, power);

	buffer[0] = servo_id;
	buffer[1] = power & 0xFF;
	buffer[2] = (power >> 8) & 0xFF;

	// 发送请求包
	if (rt_mutex_take(servo->mutex, FSUS_TIMEOUT_MS) != RT_EOK)
	{
		return FSUS_STATUS_TIMEOUT;
	}
	FSUS_SendPackage(servo, FSUS_CMD_DAMPING, size, buffer);
	rt_mutex_release(servo->mutex);

	return FSUS_STATUS_SUCCESS;
}

/* 等待电机旋转到特定的位置 */
FSUS_STATUS FSUS_Wait(servo_t *servo, uint8_t servo_id, float target_angle, uint8_t is_mturn)
{
	float angle_read;	// 读取出来的电机角度
	uint16_t count = 0; // 计数
	float angle_error;
	// 循环等待
	while (1)
	{
		if (is_mturn)
		{
			FSUS_QueryServoAngleMTurn(servo, servo_id, &angle_read);
		}
		else
		{
			FSUS_QueryServoAngle(servo, servo_id, &angle_read);
		}

		angle_error = fabsf(target_angle - angle_read);
		// LOG_D("status:%d, angle:%.1f ,angle_error: %.1f\r\n", status, angle_read, angle_error);
		if (angle_error <= FSUS_ANGLE_DEADAREA)
		{
			return FSUS_STATUS_SUCCESS;
		}

		// 超时判断机制
		count += 1;
		if (count >= FSUS_WAIT_COUNT_MAX)
		{
			return FSUS_STATUS_FAIL;
		}
		// 延时100ms
		FSUS_DelayMs(10);
	}
}
