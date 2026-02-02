/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 2021-2030. All rights reserved.
* @projectName   video_server
* @brief         videoplayer.cpp
* @author        Deng Zhimao
* @email         dengzhimao@alientek.com
* @link          www.openedv.com
* @date          2021-11-19
*******************************************************************/
#include "videoplayer.h"

VideoPlayer::VideoPlayer(QWidget *parent)
    : QDialog(parent)
{
    currentVideoFile = nullptr;             /* 初始化视频文件对象指针 */
    currentFrame = 0;                     /* 初始化当前帧索引 */
    totalFrames = 0;                      /* 初始化总帧数 */
    frameWidth = 640;                     /* 设置帧宽度 */
    frameHeight = 480;                     /* 设置帧高度 */
    isPlaying = false;                     /* 初始化播放状态 */

    setupUI();                            /* 设置界面 */
    connectSignals();                       /* 连接信号槽 */
    loadVideoFiles();                      /* 加载视频文件 */
}

VideoPlayer::~VideoPlayer()
{
    if (currentVideoFile != nullptr) {      /* 如果视频文件对象存在 */
        if (currentVideoFile->isOpen()) {   /* 如果文件已打开 */
            currentVideoFile->close();       /* 关闭文件 */
        }
        delete currentVideoFile;             /* 删除文件对象 */
    }
}

void VideoPlayer::setupUI()
{
    this->setWindowTitle("视频播放器");       /* 设置窗口标题 */
    this->setGeometry(0, 0, 800, 480);       /* 设置窗口固定大小 */

    /* 创建视频显示标签 */
    videoLabel = new QLabel(this);
    videoLabel->setMinimumSize(320, 240); /* 设置最小尺寸 */
    videoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); /* 设置大小策略为可扩展 */
    videoLabel->setStyleSheet("QLabel {background-color: black; color: white;}");
    videoLabel->setAlignment(Qt::AlignCenter);
    videoLabel->setText("请选择要播放的视频");

    /* 创建播放控制按钮 */
    playButton = new QPushButton("播放", this);
    playButton->setMinimumSize(80, 40); /* 设置最小尺寸 */
    stopButton = new QPushButton("停止", this);
    stopButton->setMinimumSize(80, 40); /* 设置最小尺寸 */
    deleteButton = new QPushButton("删除", this);
    deleteButton->setMinimumSize(80, 40); /* 设置最小尺寸 */
    closeButton = new QPushButton("关闭", this);
    closeButton->setMinimumSize(80, 40); /* 设置最小尺寸 */

    /* 创建进度滑块 */
    progressSlider = new QSlider(Qt::Horizontal, this);
    progressSlider->setRange(0, 100);
    progressSlider->setValue(0);

    /* 创建文件列表控件 */
    fileListWidget = new QListWidget(this);
    fileListWidget->setFixedWidth(150); /* 调整文件列表宽度以适应800x480窗口 */

    /* 创建播放定时器 */
    playTimer = new QTimer(this);
    playTimer->setInterval(33);             /* 设置定时器间隔为33ms（约30fps） */

    /* 设置布局 */
    QHBoxLayout *mainLayout = new QHBoxLayout(); /* 创建主布局 */

    /* 左侧：文件列表和控制栏 */
    QVBoxLayout *leftLayout = new QVBoxLayout(); /* 创建左侧布局 */
    leftLayout->addWidget(fileListWidget);    /* 添加文件列表 */

    /* 创建控制按钮布局 */
    QHBoxLayout *controlLayout = new QHBoxLayout(); /* 创建控制按钮布局 */
    controlLayout->addWidget(playButton);     /* 添加播放按钮 */
    controlLayout->addWidget(stopButton);     /* 添加停止按钮 */
    controlLayout->addWidget(deleteButton);   /* 添加删除按钮 */
    controlLayout->addWidget(progressSlider, 1); /* 添加进度滑块，设置拉伸因子为1 */
    controlLayout->addWidget(closeButton);    /* 添加关闭按钮 */

    /* 设置控制布局的最小高度，确保控制栏可见 */
    QWidget *controlWidget = new QWidget();
    controlWidget->setLayout(controlLayout);
    controlWidget->setMinimumHeight(60); /* 设置最小高度 */
    controlWidget->setMaximumHeight(60); /* 设置最大高度 */

    leftLayout->addWidget(controlWidget);   /* 将控制布局添加到左侧布局 */

    /* 右侧：视频显示 */
    QVBoxLayout *rightLayout = new QVBoxLayout(); /* 创建右侧布局 */
    rightLayout->addWidget(videoLabel, 1);     /* 添加视频标签，设置拉伸因子为1 */

    /* 合并左右布局 */
    mainLayout->addLayout(leftLayout);       /* 添加左侧布局 */
    mainLayout->addLayout(rightLayout);      /* 添加右侧布局 */

    this->setLayout(mainLayout);             /* 应用主布局到窗口 */
}

void VideoPlayer::connectSignals()
{
    connect(playButton, SIGNAL(clicked()), this, SLOT(onPlayButtonClicked()));
    connect(stopButton, SIGNAL(clicked()), this, SLOT(onStopButtonClicked()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));
    connect(fileListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onFileSelected()));
    connect(playTimer, SIGNAL(timeout()), this, SLOT(onPlayTimer()));
    connect(progressSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
}

void VideoPlayer::loadVideoFiles()
{
    fileListWidget->clear();               /* 清空文件列表 */

    QDir dir("recordings");               /* 打开录制目录 */
    if (!dir.exists()) {                 /* 如果目录不存在 */
        fileListWidget->addItem("无录制文件"); /* 显示提示信息 */
        return;                         /* 直接返回 */
    }

    QStringList filters;                   /* 创建文件过滤器 */
    filters << "*.rgb";                   /* 只显示.rgb文件 */
    dir.setNameFilters(filters);            /* 设置过滤器 */
    dir.setSorting(QDir::Time | QDir::Reversed); /* 按时间倒序排列 */

    QFileInfoList fileList = dir.entryInfoList(QDir::Files); /* 获取文件列表 */

    if (fileList.isEmpty()) {             /* 如果文件列表为空 */
        fileListWidget->addItem("无录制文件"); /* 显示提示信息 */
        return;                         /* 直接返回 */
    }

    for (const QFileInfo &fileInfo : fileList) { /* 遍历文件列表 */
        fileListWidget->addItem(fileInfo.fileName()); /* 添加文件名到列表 */
    }
}

void VideoPlayer::onPlayButtonClicked()
{
    if (isPlaying) {                     /* 如果正在播放 */
        playTimer->stop();                /* 停止定时器 */
        playButton->setText("播放");       /* 更新按钮文本 */
        isPlaying = false;                /* 设置播放状态为false */
    } else {                             /* 如果未播放 */
        if (currentVideoFile == nullptr || videoData.isEmpty()) { /* 如果没有打开文件或视频数据为空 */
            return;                     /* 直接返回 */
        }
        playTimer->start();               /* 启动定时器 */
        playButton->setText("暂停");      /* 更新按钮文本 */
        isPlaying = true;                 /* 设置播放状态为true */
    }
}

void VideoPlayer::onStopButtonClicked()
{
    playTimer->stop();                    /* 停止定时器 */
    playButton->setText("播放");          /* 更新按钮文本 */
    isPlaying = false;                   /* 设置播放状态为false */
    currentFrame = 0;                    /* 重置当前帧索引 */
    progressSlider->setValue(0);          /* 重置进度条 */
    videoLabel->clear();                 /* 清空视频显示 */
}

void VideoPlayer::onCloseButtonClicked()
{
    playTimer->stop();                    /* 停止定时器 */
    if (currentVideoFile != nullptr) {      /* 如果视频文件对象存在 */
        if (currentVideoFile->isOpen()) {   /* 如果文件已打开 */
            currentVideoFile->close();       /* 关闭文件 */
        }
        delete currentVideoFile;             /* 删除文件对象 */
        currentVideoFile = nullptr;         /* 设置为空指针 */
    }
    this->close();                       /* 关闭窗口 */
}

void VideoPlayer::onDeleteButtonClicked()
{
    /* 获取当前选中的文件 */
    QListWidgetItem *item = fileListWidget->currentItem();
    if (item == nullptr) {
        return; /* 没有选中文件，直接返回 */
    }

    /* 构建文件路径 */
    QString fileName = "recordings/" + item->text();

    /* 确认删除 */
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除", 
                                  QString("确定要删除视频文件 %1 吗？").arg(item->text()),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        /* 停止播放 */
        playTimer->stop();
        playButton->setText("播放");
        isPlaying = false;

        /* 关闭并删除当前文件对象 */
        if (currentVideoFile != nullptr) {
            if (currentVideoFile->isOpen()) {
                currentVideoFile->close();
            }
            delete currentVideoFile;
            currentVideoFile = nullptr;
        }

        /* 删除文件 */
        QFile::remove(fileName);

        /* 重新加载文件列表 */
        loadVideoFiles();

        /* 清空视频显示 */
        videoLabel->clear();
        videoLabel->setText("请选择要播放的视频");

        /* 重置播放状态 */
        currentFrame = 0;
        totalFrames = 0;
        progressSlider->setValue(0);
        videoData.clear();
    }
}

void VideoPlayer::onFileSelected()
{
    QListWidgetItem *item = fileListWidget->currentItem(); /* 获取当前选中的项 */
    if (item == nullptr) {               /* 如果没有选中项 */
        return;                         /* 直接返回 */
    }

    QString fileName = "recordings/" + item->text(); /* 构造完整文件路径 */

    /* 关闭之前的文件 */
    if (currentVideoFile != nullptr) {      /* 如果视频文件对象存在 */
        if (currentVideoFile->isOpen()) {   /* 如果文件已打开 */
            currentVideoFile->close();       /* 关闭文件 */
        }
        delete currentVideoFile;             /* 删除文件对象 */
    }

    /* 打开新文件 */
    currentVideoFile = new QFile(fileName);  /* 创建文件对象 */
    if (!currentVideoFile->open(QIODevice::ReadOnly)) { /* 以只读方式打开文件 */
        qDebug() << "ERROR: failed to open video file:" << fileName; /* 打印错误信息 */
        delete currentVideoFile;             /* 删除文件对象 */
        currentVideoFile = nullptr;         /* 设置为空指针 */
        return;                         /* 直接返回 */
    }

    /* 读取视频数据 */
    qint64 fileSize = currentVideoFile->size(); /* 获取文件大小 */
    qint64 frameSize = frameWidth * frameHeight * 2; /* 计算每帧大小（RGB565格式，每像素2字节） */
    totalFrames = fileSize / frameSize;     /* 计算总帧数 */

    /* 限制读取的数据量，避免内存不足 */
    qint64 maxDataSize = 100 * 1024 * 1024; /* 最大100MB */
    qint64 dataSize = qMin(fileSize, maxDataSize); /* 取较小值 */
    videoData = currentVideoFile->read(dataSize); /* 读取视频数据（限制大小） */

    currentVideoFile->close();               /* 关闭文件，释放资源 */

    /* 重置播放状态 */
    currentFrame = 0;                    /* 重置当前帧索引 */
    progressSlider->setRange(0, totalFrames - 1); /* 设置进度条范围 */
    progressSlider->setValue(0);          /* 重置进度条 */

    /* 显示第一帧 */
    playNextFrame();                     /* 播放第一帧 */
}

void VideoPlayer::resizeEvent(QResizeEvent *event) /* 窗口大小改变事件 */
{
    QDialog::resizeEvent(event);         /* 调用父类的resizeEvent */

    /* 设置视频标签为固定大小(320, 240) */
    videoLabel->setFixedSize(320, 240); /* 设置视频标签固定大小 */
}

void VideoPlayer::onPlayTimer()
{
    playNextFrame();                     /* 播放下一帧 */
}

void VideoPlayer::onSliderValueChanged(int value)
{
    currentFrame = value;                 /* 更新当前帧索引 */
    playNextFrame();                     /* 播放指定帧 */
}

void VideoPlayer::playNextFrame()
{
    if (currentVideoFile == nullptr || videoData.isEmpty()) { /* 如果没有文件或视频数据为空 */
        return;                         /* 直接返回 */
    }

    if (currentFrame >= totalFrames) {     /* 如果已经播放完所有帧 */
        playTimer->stop();                /* 停止定时器 */
        playButton->setText("播放");       /* 更新按钮文本 */
        isPlaying = false;                /* 设置播放状态为false */
        currentFrame = 0;                /* 重置当前帧索引 */
        progressSlider->setValue(0);      /* 重置进度条 */
        return;                         /* 直接返回 */
    }

    /* 计算当前帧的数据位置 */
    qint64 frameSize = frameWidth * frameHeight * 2; /* 计算每帧大小 */
    qint64 frameOffset = currentFrame * frameSize; /* 计算当前帧的偏移量 */

    if (frameOffset + frameSize > videoData.size()) { /* 检查偏移量是否越界 */
        return;                         /* 直接返回 */
    }

    /* 创建QImage并显示 */
    QImage image((unsigned char*)videoData.data() + frameOffset, /* 指向当前帧数据 */
                frameWidth, frameHeight, QImage::Format_RGB16); /* 创建RGB565格式的QImage */

    /* 缩放图像以适应视频标签大小 */
    QImage scaledImage = image.scaled(videoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation); /* 保持比例缩放 */

    videoLabel->setPixmap(QPixmap::fromImage(scaledImage)); /* 显示缩放后的图像 */

    /* 更新进度条 */
    progressSlider->setValue(currentFrame); /* 更新进度条位置 */

    /* 增加帧索引 */
    if (isPlaying) {                     /* 如果正在播放 */
        currentFrame++;                   /* 增加帧索引 */
    }
}