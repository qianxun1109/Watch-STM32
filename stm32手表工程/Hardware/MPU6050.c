#include "stm32f10x.h"                  // Device header
#include "MPU6050_Reg.h"
#include "math.h"
#include "OLED.h"


#define MPU6050_ADDRESS 0xD0		//MPU6050的I2C从机地址

float Angle=0;
float AngleAcc;
float AngleGyro;

int16_t AccX,AccY,AccZ,GyroX,GyroY,GyroZ;

//防超时死循环函数
void MPU6050_WaitEvent(I2C_TypeDef* I2Cx,uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout=10000;
	while(I2C_CheckEvent(I2Cx,I2C_EVENT)!=SUCCESS)
	{
		Timeout--;
		if(Timeout==0)
		{
			break;
		}
	}
}

//指定地址写（一个数据版）
void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data)
{
	I2C_GenerateSTART(I2C2,ENABLE);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);//直到确认起始条件发送成功
	
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Transmitter);//这个函数会自动给address最后的读写位置0或1
	//注：发送接收自带应答过程
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);//直到确认地址发送成功
	
	I2C_SendData(I2C2,RegAddress);//寄存器地址
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);//直到DR空吗，移位寄存器未必空
	
	I2C_SendData(I2C2,Data);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);//直到字节已经发送完毕，DR和移位寄存器皆空
	
	I2C_GenerateSTOP(I2C2,ENABLE);
}
//指定地址读(一字节版）
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;

	I2C_GenerateSTART(I2C2,ENABLE);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Transmitter);//这里先发送地址
	//注：发送接收自带应答过程
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C2,RegAddress);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);//这里用ed的，保证移位寄存器的数据发完再向下执行
	
	//重复起始
	I2C_GenerateSTART(I2C2,ENABLE);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Receiver);//改为接受模式
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);//注意这里的ev6和指定地址读的ev6不一样
	
	//只接收一个字节ev6事件后立刻ack给0，申请stop
	I2C_AcknowledgeConfig(I2C2,DISABLE);
	I2C_GenerateSTOP(I2C2,ENABLE);//这个函数可以自动延时直到数据都发完再stop，不会截断数据
	
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED);//ev7，直到data接收到完整数据
	Data = I2C_ReceiveData(I2C2);
	
	I2C_AcknowledgeConfig(I2C2,ENABLE);//将应答恢复为使能，为了不影响后续可能产生的读取多字节操作
	
	return Data;
}

void MPU6050_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	I2C_InitTypeDef I2CInitStructure;
	I2CInitStructure.I2C_Ack=I2C_Ack_Enable;//默认响应
	I2CInitStructure.I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit;//7位地址;
	I2CInitStructure.I2C_ClockSpeed=50000;
	I2CInitStructure.I2C_DutyCycle=I2C_DutyCycle_2;
	I2CInitStructure.I2C_Mode=I2C_Mode_I2C;
	I2CInitStructure.I2C_OwnAddress1=0x00;//自设主机地址（当被当成从机使用时才用得到）
	I2C_Init(I2C2,&I2CInitStructure);
	
	I2C_Cmd(I2C2,ENABLE);
	
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);//使用陀螺仪x轴时钟，关闭睡眠模式，关闭循环模式，不复位，温度传感器不失能
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);//不设置循环模式参数，不设置待机模式
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x07);//8分频后为1KHz
	MPU6050_WriteReg(MPU6050_CONFIG,0x00);//0x06不使用外部同步功能，最大数字低通滤波110
	//0x00不使用自带的低通滤波器，虽然加大噪声，但减小延迟acc延迟能减小19ms！！！！！！！！！ 
	
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);//最大量程11（16+8）  满量程±2000°/s
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);//最大量程11（16+8），不使用高通滤波器  满量程±16g
}
void MPU6050_GetData(int16_t *AccX,int16_t *AccY,int16_t *AccZ,int16_t *GyroX,int16_t *GyroY,int16_t *GyroZ)//指向16位寄存器
{
	//通过指针直接操作等效多返回值
	uint8_t buf[14]={0};

	I2C_GenerateSTART(I2C2,ENABLE);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Transmitter);//这里先发送地址
	//注：发送接收自带应答过程
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C2,MPU6050_ACCEL_XOUT_H);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);//这里用ed的，保证移位寄存器的数据发完再向下执行
	
	//重复起始
	I2C_GenerateSTART(I2C2,ENABLE);
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Receiver);//改为接受模式
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);//注意这里的ev6和指定地址读的ev6不一样
	
	for(uint8_t i=0;i<13;i++)
	{
		I2C_AcknowledgeConfig(I2C2,ENABLE);
		MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED);//ev7，直到data接收到完整数据
		buf[i] = I2C_ReceiveData(I2C2);
	}
	I2C_AcknowledgeConfig(I2C2,DISABLE);//先给好下次的标志位再拿数据
	I2C_GenerateSTOP(I2C2,ENABLE);//先给出暂停标志。 I2C 的I2C_GenerateSTOP只是触发 STOP 信号发送，并非立即完成。如果执行完I2C_GenerateSTOP后立刻退出函数，可能 STOP 信号还未完全发送到总线上，会导致下一次 I2C 起始信号被干扰
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED);
	buf[13] = I2C_ReceiveData(I2C2);

	
	*AccX = (buf[0]<<8) | buf[1];//会自动进行类型转换，数据不会因为移位丢失
	*AccY = (buf[2]<<8) | buf[3];
	*AccZ = (buf[4]<<8) | buf[5];
	*GyroX = (buf[8]<<8) | buf[9];
	//*GyroX+=54;
	*GyroY = (buf[10]<<8) | buf[11];
	//*GyroY+=26;
	*GyroZ = (buf[12]<<8) | buf[13];
	//*GyroZ-=5;
}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}	

float Angle_calc(int16_t Ax,int16_t Az,int16_t Gy,double a)//注：参数a和角度解算参数都要要跟着计算周期调整//
{
	AngleAcc = atan2(Ax,Az) / 3.14159 * 180;//以(Az,Ax)为坐标对应的角度,由弧度转换成角度
	float cycle = 0.005;	//周期 单位s
	float range = 2000;		//mpu6050的满量程+-2000°/s
	AngleGyro = Angle + Gy / 32768.0 * range * cycle;//即Gy/65536/2*2000°/ms 这里放中断里要调整  //注意：这个AngleGyro的计算必须基于上次滤波后的角度，不然会偏移
	return (1 - a) * AngleGyro + a * AngleAcc;//互补滤波
}


	



