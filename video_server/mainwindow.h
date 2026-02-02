/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 2021-2030. All rights reserved.
* @projectName   video_server
* @brief         mainwindow.h
* @author        Deng Zhimao
* @email         dengzhimao@alientek.com
* @link          www.openedv.com
* @date          2021-11-19
*******************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QImage>
#include <QPushButton>
#include <QHBoxLayout>
#include <QCheckBox>

#include "capture_thread.h"
#include "videoplayer.h"
#include "photoviewer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    /* 用于显示捕获到的图像 */
    QLabel *videoLabel;

    /* 摄像头线程 */
    CaptureThread *captureThread;

    /* 开始捕获图像按钮 */
    QPushButton *startCaptureButton;

    /* 用于开启本地图像显示 */
    QCheckBox *checkBox1;

    /* 用于开启网络广播 */
    QCheckBox *checkBox2;

    /* 用于开启视频录制 */
    QCheckBox *checkBox3;

    /* 用于打开视频播放器 */
    QPushButton *playVideoButton;

    /* 用于拍照 */
    QPushButton *captureButton;

    /* 用于查看照片 */
    QPushButton *viewPhotoButton;

    /* 视频播放器窗口 */
    VideoPlayer *videoPlayer;

    /* 照片查看器窗口 */
    PhotoViewer *photoViewer;

    /* 重写大小事件 */
    void resizeEvent(QResizeEvent *event) override;

private slots:
    /* 显示图像 */
    void showImage(QImage);

    /* 开始采集按钮被点击 */
    void startCaptureButtonClicked(bool);

    /* 打开视频播放器 */
    void openVideoPlayer();

    /* 拍照按钮被点击 */
    void onCaptureButtonClicked();

    /* 查看照片按钮被点击 */
    void openPhotoViewer();
};
#endif // MAINWINDOW_H
