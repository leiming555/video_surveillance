/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 2021-2030. All rights reserved.
* @projectName   video_server
* @brief         videoplayer.h
* @author        Deng Zhimao
* @email         dengzhimao@alientek.com
* @link          www.openedv.com
* @date          2021-11-19
*******************************************************************/
#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QFile>
#include <QDir>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QtGlobal>
#include <QSizePolicy>
#include <QResizeEvent>
#include <QPixmap>
#include <QImage>
#include <QMessageBox>

class VideoPlayer : public QDialog
{
    Q_OBJECT

public:
    explicit VideoPlayer(QWidget *parent = nullptr);
    ~VideoPlayer();

    void loadVideoFiles();                   /* 加载录制目录中的视频文件 */

private:
    QLabel *videoLabel;                     /* 视频显示标签 */
    QPushButton *playButton;                /* 播放/暂停按钮 */
    QPushButton *stopButton;                /* 停止按钮 */
    QPushButton *deleteButton;              /* 删除按钮 */
    QPushButton *closeButton;               /* 关闭按钮 */
    QSlider *progressSlider;                /* 进度滑块 */
    QListWidget *fileListWidget;            /* 文件列表控件 */
    QTimer *playTimer;                     /* 播放定时器 */

    QFile *currentVideoFile;                /* 当前视频文件对象 */
    QByteArray videoData;                   /* 视频数据缓冲区 */
    int currentFrame;                       /* 当前帧索引 */
    int totalFrames;                        /* 总帧数 */
    int frameWidth;                         /* 帧宽度 */
    int frameHeight;                        /* 帧高度 */
    bool isPlaying;                         /* 播放状态标志 */

    void setupUI();                         /* 设置界面 */
    void connectSignals();                   /* 连接信号槽 */
    void playNextFrame();                   /* 播放下一帧 */
    void updateProgress();                   /* 更新进度条 */
    void resizeEvent(QResizeEvent *event) override; /* 窗口大小改变事件 */

private slots:
    void onPlayButtonClicked();             /* 播放按钮点击 */
    void onStopButtonClicked();              /* 停止按钮点击 */
    void onDeleteButtonClicked();            /* 删除按钮点击 */
    void onCloseButtonClicked();             /* 关闭按钮点击 */
    void onFileSelected();                  /* 文件列表项选中 */
    void onPlayTimer();                     /* 播放定时器超时 */
    void onSliderValueChanged(int value);    /* 滑块值改变 */
};

#endif // VIDEOPLAYER_H