/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 2021-2030. All rights reserved.
* @projectName   video_server
* @brief         capture_thread.h
* @author        Deng Zhimao
* @email         dengzhimao@alientek.com
* @link          www.openedv.com
* @date          2021-11-19
*******************************************************************/
#ifndef CAPTURE_THREAD_H                   /* 头文件保护宏，防止重复包含 */
#define CAPTURE_THREAD_H

#include <sys/types.h>                     /* 系统类型定义头文件 */
#include <sys/stat.h>                      /* 文件状态头文件 */
#include <fcntl.h>                         /* 文件控制头文件，包含open等函数 */
#include <stdio.h>                         /* 标准输入输出头文件 */
#include <unistd.h>                        /* 系统调用头文件，包含close等函数 */
#include <string.h>                        /* 字符串处理头文件 */
#include <pthread.h>                       /* 线程头文件 */
#ifdef linux                               /* Linux系统条件编译 */
#include <linux/fb.h>                      /* 帧缓冲头文件 */
#include <sys/ioctl.h>                     /* IO控制头文件，包含ioctl函数 */
#include <sys/mman.h>                      /* 内存映射头文件，包含mmap函数 */
#include <linux/videodev2.h>               /* V4L2视频设备头文件 */
#include <linux/input.h>                   /* 输入设备头文件 */
#endif

#include <QThread>                         /* Qt线程类头文件 */
#include <QDebug>                          /* Qt调试头文件 */
#include <QPushButton>                     /* Qt按钮类头文件 */
#include <QImage>                          /* Qt图像类头文件 */
#include <QByteArray>                      /* Qt字节数组类头文件 */
#include <QBuffer>                         /* Qt缓冲区类头文件 */
#include <QTime>                           /* Qt时间类头文件 */
#include <QUdpSocket>                      /* Qt UDP套接字头文件 */
#include <QFile>                           /* Qt文件类头文件 */
#include <QDir>                            /* Qt目录类头文件 */
#include <QDateTime>                       /* Qt日期时间类头文件 */
#include <QMutex>                           /* Qt互斥锁类头文件 */

#define VIDEO_DEV			"/dev/video1"   /* 视频设备路径 */
#define FB_DEV				"/dev/fb0"      /* 帧缓冲设备路径 */
#define VIDEO_BUFFER_COUNT	3               /* 视频缓冲区数量 */

struct buffer_info {                       /* 缓冲区信息结构体 */
    void *start;                           /* 缓冲区起始地址 */
    unsigned int length;                   /* 缓冲区长度 */
};

class CaptureThread : public QThread       /* 视频捕获线程类，继承自QThread */
{
    Q_OBJECT                              /* Qt元对象宏，启用信号槽机制 */

signals:
    /* 准备图片 */
    void imageReady(QImage);               /* 图像就绪信号，用于传递捕获的图像 */
    void sendImage(QImage);                /* 发送图像信号，预留接口 */

private:
    /* 线程开启flag */
    bool startFlag = false;                /* 线程启动标志，默认为false */

    /* 开启广播flag */
    bool startBroadcast = false;           /* 广播启动标志，默认为false */

    /* 本地显示flag  */
    bool startLocalDisplay = false;        /* 本地显示启动标志，默认为false */

    /* 视频录制相关 */
    bool startRecording = false;           /* 视频录制标志，默认为false */
    QFile *videoFile = nullptr;            /* 录制文件对象指针 */
    int frameCount = 0;                     /* 录制帧计数器 */

    /* 最新的视频帧，用于拍照 */
    QImage latestFrame;                     /* 存储最新的视频帧 */
    QMutex frameMutex;                      /* 保护latestFrame的互斥锁 */

    void run() override;                   /* 重写run()方法，线程执行的核心函数 */

public:
    CaptureThread(QObject *parent = nullptr) {
        Q_UNUSED(parent);                  /* 未使用的父对象参数 */
    }

public slots:
    /* 设置线程 */
    void setThreadStart(bool start) {
        startFlag = start;                 /* 设置线程启动标志 */
        if (start) {
            if (!this->isRunning())        /* 如果线程未运行 */
                this->start();             /* 启动线程 */
        } else {
            this->quit();                  /* 停止线程 */
        }
    }

    /* 设置广播 */
    void setBroadcast(bool start) {
        startBroadcast = start;            /* 设置广播启动标志 */
    }

    /* 设置本地显示 */
    void setLocalDisplay(bool start) {
        startLocalDisplay = start;         /* 设置本地显示启动标志 */
    }

    /* 设置视频录制 */
    void setRecording(bool start);         /* 设置视频录制标志，声明函数 */

    /* 拍照 */
    void capturePhoto();                   /* 拍照函数，声明函数 */
};

#endif // CAPTURE_THREAD_H                 /* 头文件保护宏结束 */
