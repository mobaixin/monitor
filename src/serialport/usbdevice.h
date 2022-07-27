#ifndef __USB_DEVICE_H_
#define __USB_DEVICE_H_

//定义设备信息
typedef struct _DEVICE_INFO
{
    char    FirmwareName[32];   //固件名称字符串
    char    BuildDate[32];      //固件编译时间字符串
    int     HardwareVersion;    //硬件版本号
    int     FirmwareVersion;    //固件版本号
    int     SerialNumber[3];    //适配器序列号
    int     Functions;          //适配器当前具备的功能
}DEVICE_INFO,*PDEVICE_INFO;

//定义电压输出值
#define POWER_LEVEL_NONE    0   //不输出
#define POWER_LEVEL_1V8     1   //输出1.8V
#define POWER_LEVEL_2V5     2   //输出2.5V
#define POWER_LEVEL_3V3     3   //输出3.3V
#define POWER_LEVEL_5V0     4   //输出5.0V


// #ifdef __cplusplus
extern "C"
{
// #endif
/**
  * @brief  初始化USB设备，并扫描设备连接数，必须调用
  * @param  pDevHandle 每个设备的设备号存储地址
  * @retval 扫描到的设备数量
  */
int USB_ScanDevice(int *pDevHandle);

/**
  * @brief  打开设备，必须调用
  * @param  DevHandle 设备索引号
  * @retval 打开设备的状态
  */
bool USB_OpenDevice(int DevHandle);

/**
  * @brief  关闭设备
  * @param  DevHandle 设备索引号
  * @retval 关闭设备的状态
  */
bool USB_CloseDevice(int DevHandle);

/**
  * @brief  复位设备程序，复位后需要重新调用USB_ScanDevice，USB_OpenDevice函数
  * @param  DevHandle 设备索引号
  * @retval 复位设备的状态
  */
bool USB_ResetDevice(int DevHandle);
/**
  * @brief  获取设备信息，比如设备名称，固件版本号，设备序号，设备功能说明字符串等
  * @param  DevHandle 设备索引号
  * @param  pDevInfo 设备信息存储结构体指针
  * @param  pFunctionStr 设备功能说明字符串
  * @retval 获取设备信息的状态
  */
bool DEV_GetDeviceInfo(int DevHandle,PDEVICE_INFO pDevInfo,char *pFunctionStr);

/**
  * @brief  设置可变电压输出引脚输出电压值
  * @param  DevHandle 设备索引号
  * @param  PowerLevel 输出电压值
  * @retval 设置输出电压状态
  */
bool DEV_SetPowerLevel(int DevHandle,char PowerLevel);

// #ifdef __cplusplus
}
// #endif

#endif

