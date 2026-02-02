/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 2021-2030. All rights reserved.
* @projectName   video_server
* @brief         capture_thread.cpp
* @author        Deng Zhimao
* @email         dengzhimao@alientek.com
* @link          www.openedv.com
* @date          2021-11-19
*******************************************************************/
#include "capture_thread.h"

void CaptureThread::run()
{
    /* 下面的代码请参考正点原子C应用编程V4L2章节，摄像头编程，这里不作解释 */
#ifdef linux
#ifndef __arm__
    return;
#endif
    int video_fd = -1;                      /* 视频设备文件描述符，初始化为-1表示无效状态 */
    struct v4l2_format fmt;                 /* V4L2视频格式结构体，用于设置视频捕获格式 */
    struct v4l2_requestbuffers req_bufs;    /* V4L2请求缓冲区结构体，用于向驱动申请缓冲区 */
    static struct v4l2_buffer buf;           /* V4L2缓冲区结构体，用于管理视频缓冲区，static修饰确保每次循环使用同一变量 */
    int n_buf;                              /* 缓冲区索引变量，用于遍历多个视频缓冲区 */
    struct buffer_info bufs_info[VIDEO_BUFFER_COUNT]; /* 自定义缓冲区信息数组，存储每个缓冲区的地址和长度 */
    enum v4l2_buf_type type;                /* V4L2缓冲区类型枚举，用于指定操作的缓冲区类型 */

    video_fd = open(VIDEO_DEV, O_RDWR);     /* 打开视频设备文件，O_RDWR表示以读写方式打开 */
    if (0 > video_fd) {                      /* 检查视频设备是否打开成功，文件描述符小于0表示失败 */
        printf("ERROR: failed to open video device %s\n", VIDEO_DEV); /* 打印错误信息，显示设备路径 */
        return ;                             /* 打开失败，直接返回 */
    }                                        /* 打开成功，继续执行后续操作 */

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;      /* 设置缓冲区类型为视频捕获 */
    fmt.fmt.pix.width = 640;                      /* 设置视频宽度为640像素 */
    fmt.fmt.pix.height = 480;                     /* 设置视频高度为480像素 */
    fmt.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB; /* 设置色彩空间为sRGB */
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565; /* 设置像素格式为RGB565 */

    if (0 > ioctl(video_fd, VIDIOC_S_FMT, &fmt)) { /* 应用视频格式设置 */
        printf("ERROR: failed to VIDIOC_S_FMT\n"); /* 打印格式设置失败错误信息 */
        close(video_fd);                          /* 关闭视频设备 */
        return ;                                 /* 失败返回 */
    }                                            /* 格式设置成功 */

    req_bufs.count = VIDEO_BUFFER_COUNT;          /* 设置请求的缓冲区数量 */
    req_bufs.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  /* 设置缓冲区类型为视频捕获 */
    req_bufs.memory = V4L2_MEMORY_MMAP;           /* 设置内存映射方式 */

    if (0 > ioctl(video_fd, VIDIOC_REQBUFS, &req_bufs)) { /* 请求视频缓冲区 */
        printf("ERROR: failed to VIDIOC_REQBUFS\n"); /* 打印缓冲区请求失败错误信息 */
        return ;                                 /* 失败返回 */
    }                                            /* 缓冲区请求成功 */

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;       /* 设置缓冲区类型为视频捕获 */
    buf.memory = V4L2_MEMORY_MMAP;                /* 设置内存映射方式 */
    for (n_buf = 0; n_buf < VIDEO_BUFFER_COUNT; n_buf++) { /* 遍历所有缓冲区 */

        buf.index = n_buf;                        /* 设置当前缓冲区索引 */
        if (0 > ioctl(video_fd, VIDIOC_QUERYBUF, &buf)) { /* 查询缓冲区信息 */
            printf("ERROR: failed to VIDIOC_QUERYBUF\n"); /* 打印查询失败错误信息 */
            return ;                             /* 失败返回 */
        }

        bufs_info[n_buf].length = buf.length;     /* 保存缓冲区长度 */
        bufs_info[n_buf].start = mmap(NULL, buf.length, /* 内存映射缓冲区 */
                                      PROT_READ | PROT_WRITE, MAP_SHARED,
                                      video_fd, buf.m.offset);
        if (MAP_FAILED == bufs_info[n_buf].start) { /* 检查内存映射是否成功 */
            printf("ERROR: failed to mmap video buffer, size 0x%x\n", buf.length); /* 打印映射失败错误信息 */
            return ;                             /* 失败返回 */
        }
    }                                            /* 所有缓冲区映射完成 */

    for (n_buf = 0; n_buf < VIDEO_BUFFER_COUNT; n_buf++) { /* 遍历所有缓冲区 */

        buf.index = n_buf;                        /* 设置当前缓冲区索引 */
        if (0 > ioctl(video_fd, VIDIOC_QBUF, &buf)) { /* 将缓冲区放入队列 */
            printf("ERROR: failed to VIDIOC_QBUF\n"); /* 打印入队失败错误信息 */
            return ;                             /* 失败返回 */
        }
    }                                            /* 所有缓冲区入队完成 */

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;           /* 设置缓冲区类型为视频捕获 */
    if (0 > ioctl(video_fd, VIDIOC_STREAMON, &type)) { /* 启动视频流 */
        printf("ERROR: failed to VIDIOC_STREAMON\n"); /* 打印启动失败错误信息 */
        return ;                                 /* 失败返回 */
    }                                            /* 视频流启动成功 */

    while (startFlag) {                           /* 主循环，startFlag为真时继续 */

        for (n_buf = 0; n_buf < VIDEO_BUFFER_COUNT; n_buf++) { /* 遍历所有缓冲区 */

            buf.index = n_buf;                    /* 设置当前缓冲区索引 */

            if (0 > ioctl(video_fd, VIDIOC_DQBUF, &buf)) { /* 从队列中取出缓冲区 */
                printf("ERROR: failed to VIDIOC_DQBUF\n"); /* 打印出队失败错误信息 */
                return;
            }

            QImage qImage((unsigned char*)bufs_info[n_buf].start, fmt.fmt.pix.width, fmt.fmt.pix.height, QImage::Format_RGB16); /* 将缓冲区数据转换为QImage */

            /* 更新最新的视频帧，用于拍照 */
            frameMutex.lock();                   /* 加锁保护 */
            latestFrame = qImage.copy();         /* 复制图像，避免引用同一个内存 */
            frameMutex.unlock();                 /* 解锁 */

            /* 是否开启本地显示，开启本地显示可能会导致开启广播卡顿，它们互相制约 */
            if (startLocalDisplay)
                emit imageReady(qImage);          /* 发送图像就绪信号，用于本地显示 */

            /* 是否开启视频录制 */
            if (startRecording && videoFile != nullptr) { /* 如果录制标志为true且文件对象存在 */
                videoFile->write((const char*)bufs_info[n_buf].start, buf.length); /* 将原始RGB565数据写入文件 */
                frameCount++;                     /* 帧计数器加1 */
            }

            /* 是否开启广播，开启广播会导致本地显示卡顿，它们互相制约 */
            if (startBroadcast) {
                /* udp套接字 */
                QUdpSocket udpSocket;             /* 创建UDP套接字 */

                /* QByteArray类型 */
                QByteArray byte;                  /* 创建字节数组 */

                /* 建立一个用于IO读写的缓冲区 */
                QBuffer buff(&byte);              /* 创建IO缓冲区 */

                /* image转为byte的类型，再存入buff */
                qImage.save(&buff, "JPEG", -1);   /* 将图像保存为JPEG格式到缓冲区 */

                /* 转换为base64Byte类型 */
                QByteArray base64Byte = byte.toBase64(); /* 将图像数据转换为base64编码 */

                /* 由udpSocket以广播的形式传输数据，端口号为8888 */
                //udpSocket.writeDatagram(base64Byte.data(), base64Byte.size(),  QHostAddress("192.168.1.100"), 8888);
                // 这里的 IP 必须是你 ip a 命令查出来的 ens33 的 IP
                udpSocket.writeDatagram(base64Byte.data(), base64Byte.size(), QHostAddress("192.168.10.100"), 8888); /* 发送UDP数据报 */
            }

            if (0 > ioctl(video_fd, VIDIOC_QBUF, &buf)) { /* 将处理完成的缓冲区重新入队 */
                printf("ERROR: failed to VIDIOC_QBUF\n"); /* 打印入队失败错误信息 */
                return;
            }
        }
    }                                            /* 主循环结束 */

    msleep(800);//at lease 650                    /* 延迟800ms，确保视频流完全停止 */

    for (int i = 0; i < VIDEO_BUFFER_COUNT; i++) { /* 遍历所有缓冲区 */
        munmap(bufs_info[i].start, buf.length);   /* 释放内存映射 */
    }

    close(video_fd);                              /* 关闭视频设备 */
#endif                                           /* Linux系统条件编译结束 */
}                                                /* CaptureThread::run()函数结束 */

void CaptureThread::setRecording(bool start)      /* 设置视频录制槽函数 */
{
    startRecording = start;                       /* 设置录制标志 */

    if (start) {                                  /* 如果开始录制 */
        frameCount = 0;                           /* 重置帧计数器 */

        /* 创建录制目录 */
        QDir dir;                                /* 创建目录对象 */
        if (!dir.exists("recordings")) {           /* 如果recordings目录不存在 */
            dir.mkpath("recordings");             /* 创建recordings目录 */
        }

        /* 生成文件名：video_YYYYMMDD_HHMMSS.rgb */
        QString fileName = QString("recordings/video_%1.rgb")
                          .arg(QDateTime::currentDateTime()
                          .toString("yyyyMMdd_HHmmss")); /* 生成带时间戳的文件名 */

        videoFile = new QFile(fileName);          /* 创建文件对象 */
        if (!videoFile->open(QIODevice::WriteOnly)) { /* 以只写方式打开文件 */
            qDebug() << "ERROR: failed to open recording file:" << fileName; /* 打印错误信息 */
            delete videoFile;                      /* 删除文件对象 */
            videoFile = nullptr;                   /* 设置为空指针 */
            startRecording = false;                /* 设置录制标志为false */
        } else {
            qDebug() << "Recording started:" << fileName; /* 打印录制开始信息 */
        }
    } else {                                      /* 如果停止录制 */
        if (videoFile != nullptr) {               /* 如果文件对象存在 */
            videoFile->close();                   /* 关闭文件 */
            delete videoFile;                      /* 删除文件对象 */
            videoFile = nullptr;                   /* 设置为空指针 */
            qDebug() << "Recording stopped, total frames:" << frameCount; /* 打印录制停止信息 */
        }
    }
}

void CaptureThread::capturePhoto() /* 拍照函数实现 */
{
    /* 检查是否有有效的视频帧 */
    frameMutex.lock(); /* 加锁保护 */
    if (latestFrame.isNull()) { /* 如果帧为空 */
        frameMutex.unlock(); /* 解锁 */
        qDebug() << "ERROR: no valid video frame to capture"; /* 打印错误信息 */
        return; /* 直接返回 */
    }

    QImage photo = latestFrame.copy(); /* 复制最新的视频帧 */
    frameMutex.unlock(); /* 解锁 */

    /* 创建照片保存目录 */
    QDir dir; /* 创建目录对象 */
    if (!dir.exists("photos")) { /* 如果photos目录不存在 */
        if (!dir.mkpath("photos")) { /* 创建photos目录 */
            qDebug() << "ERROR: failed to create photos directory"; /* 打印错误信息 */
            return; /* 目录创建失败，返回 */
        }
    }

    /* 生成文件名：photo_YYYYMMDD_HHMMSS.jpg */
    QString fileName = QString("photos/photo_%1.jpg")
                      .arg(QDateTime::currentDateTime()
                      .toString("yyyyMMdd_HHmmss")); /* 生成带时间戳的文件名 */

    /* 保存照片为JPEG格式 */
    if (!photo.save(fileName, "JPEG", 95)) { /* 保存照片，质量95 */
        qDebug() << "ERROR: failed to save photo:" << fileName; /* 打印错误信息 */
        return; /* 保存失败，返回 */
    }

    qDebug() << "Photo captured and saved:" << fileName; /* 打印保存成功信息 */
}
