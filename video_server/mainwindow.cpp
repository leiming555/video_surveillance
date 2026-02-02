/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 2021-2030. All rights reserved.
* @projectName   video_server
* @brief         mainwindow.cpp
* @author        Deng Zhimao
* @email         dengzhimao@alientek.com
* @link          www.openedv.com
* @date          2021-11-19
*******************************************************************/
#include "mainwindow.h"                   /* 包含主窗口头文件 */

MainWindow::MainWindow(QWidget *parent)   /* 主窗口构造函数，parent为父窗口指针 */
    : QMainWindow(parent)                 /* 调用父类QMainWindow的构造函数 */
{
    this->setGeometry(0, 0, 800, 480);     /* 设置主窗口位置和大小：(x,y,width,height) */

    videoLabel = new QLabel(this);         /* 创建视频显示标签，父窗口为this */
    videoLabel->setText("未获取到图像数据或未开启本地显示"); /* 设置标签初始文本 */
    videoLabel->setStyleSheet("QWidget {color: white;}"); /* 设置标签样式：文字颜色为白色 */
    videoLabel->setAlignment(Qt::AlignCenter); /* 设置文本居中对齐 */
    videoLabel->resize(640, 480);          /* 设置标签大小为视频分辨率 */

    checkBox1 = new QCheckBox(this);       /* 创建第一个复选框，用于控制本地显示 */
    checkBox2 = new QCheckBox(this);       /* 创建第二个复选框，用于控制广播 */
    checkBox3 = new QCheckBox(this);       /* 创建第三个复选框，用于控制视频录制 */

    checkBox1->resize(120, 50);            /* 设置第一个复选框大小 */
    checkBox2->resize(120, 50);            /* 设置第二个复选框大小 */
    checkBox3->resize(120, 50);            /* 设置第三个复选框大小 */

    checkBox1->setText("画面");        /* 设置第一个复选框文本 */
    checkBox2->setText("上传");        /* 设置第二个复选框文本 */
    checkBox3->setText("录制");        /* 设置第三个复选框文本 */

    checkBox1->setStyleSheet("QCheckBox {color: yellow;}"
                             "QCheckBox:indicator {width: 40; height: 40;}"); /* 设置第一个复选框样式：文字黄色，指示器大小40x40 */
    checkBox2->setStyleSheet("QCheckBox {color: yellow;}"
                             "QCheckBox:indicator {width: 40; height: 40}"); /* 设置第二个复选框样式：文字黄色，指示器大小40x40 */
    checkBox3->setStyleSheet("QCheckBox {color: green;}"
                             "QCheckBox:indicator {width: 40; height: 40}"); /* 设置第三个复选框样式：文字绿色，指示器大小40x40 */

    /* 按钮 */
    startCaptureButton = new QPushButton(this); /* 创建开始/停止录像按钮 */
    startCaptureButton->setCheckable(true);     /* 设置按钮为可选中状态 */
    startCaptureButton->setText("开始");     /* 设置按钮初始文本 */

    playVideoButton = new QPushButton(this);  /* 创建播放视频按钮 */
    playVideoButton->setText("视频相册");     /* 设置按钮文本 */

    captureButton = new QPushButton(this);     /* 创建拍照按钮 */
    captureButton->setText("拍照");          /* 设置按钮文本 */

    viewPhotoButton = new QPushButton(this);   /* 创建查看照片按钮 */
    viewPhotoButton->setText("照片相册");      /* 设置按钮文本 */

    /* 设置背景颜色为黑色 */
    QColor color = QColor(Qt::black);           /* 创建黑色颜色对象 */
    QPalette p;                                 /* 创建调色板对象 */
    p.setColor(QPalette::Window, color);        /* 设置窗口背景颜色为黑色 */
    this->setPalette(p);                        /* 应用调色板到主窗口 */

    /* 样式表 */
    startCaptureButton->setStyleSheet("QPushButton {background-color: white; border-radius: 30}"
                                      "QPushButton:pressed  {background-color: red;}"); /* 设置按钮样式：白色背景，圆角30，按下时红色 */
    playVideoButton->setStyleSheet("QPushButton {background-color: cyan; border-radius: 30}"
                                      "QPushButton:pressed  {background-color: blue;}"); /* 设置按钮样式：青色背景，圆角30，按下时蓝色 */
    captureButton->setStyleSheet("QPushButton {background-color: green; border-radius: 30}"
                                      "QPushButton:pressed  {background-color: darkgreen;}"); /* 设置按钮样式：绿色背景，圆角30，按下时深绿色 */
    viewPhotoButton->setStyleSheet("QPushButton {background-color: yellow; border-radius: 30}"
                                      "QPushButton:pressed  {background-color: orange;}"); /* 设置按钮样式：黄色背景，圆角30，按下时橙色 */

    captureThread = new CaptureThread(this);    /* 创建视频捕获线程对象 */
    videoPlayer = new VideoPlayer(this);        /* 创建视频播放器窗口对象 */
    photoViewer = new PhotoViewer(this);        /* 创建照片查看器窗口对象 */

    connect(startCaptureButton, SIGNAL(clicked(bool)), captureThread, SLOT(setThreadStart(bool))); /* 连接开始/停止按钮信号到线程的启动/停止槽 */
    connect(startCaptureButton, SIGNAL(clicked(bool)), this, SLOT(startCaptureButtonClicked(bool))); /* 连接开始/停止按钮信号到自身的槽，用于更新按钮文本 */
    connect(captureThread, SIGNAL(imageReady(QImage)), this, SLOT(showImage(QImage))); /* 连接线程的图像就绪信号到显示槽 */
    connect(checkBox1, SIGNAL(clicked(bool)), captureThread, SLOT(setLocalDisplay(bool))); /* 连接本地显示复选框信号到线程的本地显示控制槽 */
    connect(checkBox2, SIGNAL(clicked(bool)), captureThread, SLOT(setBroadcast(bool))); /* 连接广播复选框信号到线程的广播控制槽 */
    connect(checkBox3, SIGNAL(clicked(bool)), captureThread, SLOT(setRecording(bool))); /* 连接视频录制复选框信号到线程的录制控制槽 */
    connect(playVideoButton, SIGNAL(clicked()), this, SLOT(openVideoPlayer())); /* 连接播放视频按钮信号到打开播放器槽 */
    connect(captureButton, SIGNAL(clicked()), this, SLOT(onCaptureButtonClicked())); /* 连接拍照按钮信号到拍照槽 */
    connect(viewPhotoButton, SIGNAL(clicked()), this, SLOT(openPhotoViewer())); /* 连接查看照片按钮信号到打开照片查看器槽 */
}

MainWindow::~MainWindow()                    /* 主窗口析构函数 */
{
}

void MainWindow::showImage(QImage image)      /* 显示图像槽函数，接收QImage参数 */
{
    videoLabel->setPixmap(QPixmap::fromImage(image)); /* 将QImage转换为QPixmap并显示在标签上 */
}

void MainWindow::resizeEvent(QResizeEvent *event) /* 窗口大小改变事件处理函数 */
{
    Q_UNUSED(event)                           /* 未使用的事件参数 */

    startCaptureButton->move(50, this->height() - 80); /* 设置开始/停止按钮位置：左侧50px，底部80px */
    playVideoButton->move(270, this->height() - 80); /* 设置播放视频按钮位置：左侧270px，底部80px */
    captureButton->move(490, this->height() - 80); /* 设置拍照按钮位置：左侧490px，底部80px */
    viewPhotoButton->move(710, this->height() - 80); /* 设置查看照片按钮位置：左侧710px，底部80px */

//startCaptureButton->move((this->width() - 200) / 2, this->height() - 80); /* 居中显示按钮的备选方案（注释掉） */
    startCaptureButton->resize(200, 60);      /* 设置开始/停止按钮大小 */
    playVideoButton->resize(200, 60);        /* 设置播放视频按钮大小 */
    captureButton->resize(200, 60);          /* 设置拍照按钮大小 */
    viewPhotoButton->resize(200, 60);        /* 设置查看照片按钮大小 */
    videoLabel->move((this->width() - 640) / 2, (this->height() - 480) / 2); /* 视频标签居中显示 */
    checkBox1->move(this->width() - 120, this->height() / 2 - 75); /* 第一个复选框位置：右侧，垂直居中偏上 */
    checkBox2->move(this->width() - 120, this->height() / 2); /* 第二个复选框位置：右侧，垂直居中 */
    checkBox3->move(this->width() - 120, this->height() / 2 + 75); /* 第三个复选框位置：右侧，垂直居中偏下 */
}

void MainWindow::startCaptureButtonClicked(bool start) /* 开始/停止按钮点击槽函数 */
{
    if (start)
        startCaptureButton->setText("停止录像"); /* 按钮被选中时，文本改为"停止录像" */
    else
        startCaptureButton->setText("开始录像"); /* 按钮未被选中时，文本改为"开始录像" */
}

void MainWindow::openVideoPlayer()        /* 打开视频播放器槽函数 */
{
    videoPlayer->loadVideoFiles();        /* 加载视频文件列表 */
    videoPlayer->show();                /* 显示播放器窗口 */
}

void MainWindow::onCaptureButtonClicked() /* 拍照按钮点击槽函数 */
{
    captureThread->capturePhoto(); /* 调用线程的拍照函数 */
}

void MainWindow::openPhotoViewer()        /* 打开照片查看器槽函数 */
{
    photoViewer->loadPhotoFiles();        /* 加载照片文件列表 */
    photoViewer->show();                /* 显示照片查看器窗口 */
}

