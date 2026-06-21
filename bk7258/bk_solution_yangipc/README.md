# yangipc项目开发指南

* [English](./README.md)

## 1 项目概述

本项目是一个基于BK7258芯片ipc解决方案，实现了通过WiFi传输H264图像数据，实时双向/单向PCMA音频的功能。


## 2 快速开始

### 2.1 硬件准备
- BK7258开发板
- LCD屏幕
- UVC/DVP摄像头模块
- 板载speaker/mic，或UAC
- 电源和连接线

### 2.2 编译和烧录

烧录流程参考 `烧录代码 <https://docs.bekencorp.com/arminodoc/bk_avdk_smp/smp_doc/bk7258/zh_CN/v3.0.1/get-started/index.html#id7>`_

在本工程目录下编译：

```bash
cd bk_avdk_smp/bk_solution_yangipc/projects/yangipc
make clean
make bk7258
```

编译时会自动应用 `sdk_patch` 目录下的补丁。编译生成的烧录 bin 文件路径：``projects/yangipc/build/bk7258/yangipc/package/all-app.bin``

### 2.3 基本操作流程
1. 设备上电启动
2. 测试机（Android）下载IOT应用到设备，下载地址： <https://dl.bekencorp.com/apk/BekenIot.apk>
3. 自行创建账号，并完成登录
4. 测试机打开IOT应用，添加设备，选择： `可视门铃` ，DL设备存在01-18，和DEBUG，建议先选择 `BK7258_DL_01` 进行尝试使用。点进去后里面详细介绍了使用的外设，包括UVC摄像头、H.264编码器、SD卡存储、LCD屏幕、语音功能等。
5. `开始添加`，选择非5G的WiFi，连接成功后，点击下一步，开始通过蓝牙进行配网
6. 检查扫描到的设备蓝牙广播，点击IP地址匹配的进行连接，会自动完成100%的配网
7. 配网完成之后会自动打开UVC摄像头，且打开网络图传，传输的格式是H.264，图像的分辨率为864X480
8. 打开其他外设，可以在IOT应用上进行控制。

## 3 yangipc

### 3.1 bk_avdk_smp 配置

打上 `sdk_patch` 目录的补丁；若需手动核对，主要涉及以下修改。

#### 3.1.1 ap/include/posix/time.h → b/ap/include/posix/time.h

```c
#define CLOCK_REALTIME     0
```
修改为：
```c
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME     0
#endif
```

```c
#define CLOCKS_PER_SEC    ( ( clock_t ) configTICK_RATE_HZ )
```
修改为：
```c
#ifdef CLOCKS_PER_SEC
#undef CLOCKS_PER_SEC
#endif
#define CLOCKS_PER_SEC    ( ( clock_t ) configTICK_RATE_HZ )
```

```c
#define TIMER_ABSTIME    0x01
```
修改为：
```c
#ifdef TIMER_ABSTIME
#undef TIMER_ABSTIME
#endif
#define TIMER_ABSTIME    0x01
```

#### 3.1.2 ap/components/psa_mbedtls/mbedtls_port/configs/mbedtls_psa_crypto_config.h

第 1996 行，去掉注释并启用 DTLS SRTP：

```c
//#define MBEDTLS_SSL_DTLS_SRTP
```
修改为：
```c
#define MBEDTLS_SSL_DTLS_SRTP
```

### 3.2 yangipc 配置文件

**ap_main.c：**

```c
//是否支持datachannel
void yang_init_ipcConfig(YangIpcConfig* config){
    config->audioDirection=1;// 0:单向直播 1:对讲
    
    config->width=640;
    config->height=480;
    config->fps=30;

    strcpy(config->deviceName, "test1001");//客户端需配置devicdeName才能联通设备

//coturn配置
    config->icePort=3478;
    strcpy(config->iceServerIP, "192.168.0.104");
    strcpy(config->iceUserName, "metartc");
    strcpy(config->icePassword, "metartc");
//mqtt配置
    config->mqttPort=1883;
    strcpy(config->mqttServerIP, "192.168.0.104");
    memset(config->mqttUserName,0,sizeof(config->mqttUserName));
    memset(config->mqttPassword,0,sizeof(config->mqttPassword));
}

yangipc_wifi_sta_connect("ssid", "password");//连接wifi网络
```

### 3.3 yangipc启动

```bash
ap_cmd yang_ipc
```
### 3.4 客户端下载
```bash
git clone https://github.com/metartc/yangipcclient.git
or 
git clone https://gitee.com/metartc/yangipcclient.git
```
### 3.5 客户端浏览器html_freertos
```
index_mqtt_hd.html
107行：yang_pubTopic="test1001";//和设备配置yang_init_ipcConfig中deviceName相同
配置mqtt参数和ice server(coturn)参数
```
### 3.6 客户端
```
class YangIpcPlayer;
startMqtt((char*)"test1001");//和设备配置yang_init_ipcConfig中deviceName相同
配置mqtt参数和ice server(coturn)参数
void setIceServer(char* ip,int32_t port,char* username,char* passwork);
void setMqttServer(bool isTls,char* ip,int32_t port,char* username,char* password);

```

