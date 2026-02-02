/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 2021-2030. All rights reserved.
* @projectName   video_server
* @brief         photoviewer.cpp
* @author        Deng Zhimao
* @email         dengzhimao@alientek.com
* @link          www.openedv.com
* @date          2021-11-19
*******************************************************************/
#include "photoviewer.h"
#include <QMessageBox>

PhotoViewer::PhotoViewer(QWidget *parent)
    : QDialog(parent)
{
    currentIndex = -1; /* 初始化当前索引为-1，表示无选中照片 */

    setupUI(); /* 设置界面 */
    connectSignals(); /* 连接信号槽 */
    loadPhotoFiles(); /* 加载照片文件 */
}

PhotoViewer::~PhotoViewer()
{
}

void PhotoViewer::setupUI()
{
    this->setWindowTitle("照片查看器"); /* 设置窗口标题 */
    this->setGeometry(0, 0, 800, 480); /* 设置窗口固定大小 */

    /* 创建照片显示标签 */
    photoLabel = new QLabel(this);
    photoLabel->setMinimumSize(320, 240); /* 设置最小尺寸 */
    photoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); /* 设置大小策略为可扩展 */
    photoLabel->setStyleSheet("QLabel {background-color: black; color: white;}");
    photoLabel->setAlignment(Qt::AlignCenter);
    photoLabel->setText("请选择要查看的照片");

    /* 创建控制按钮 */
    prevButton = new QPushButton("上一张", this);
    prevButton->setMinimumSize(80, 40); /* 设置最小尺寸 */
    nextButton = new QPushButton("下一张", this);
    nextButton->setMinimumSize(80, 40); /* 设置最小尺寸 */
    deleteButton = new QPushButton("删除", this);
    deleteButton->setMinimumSize(80, 40); /* 设置最小尺寸 */
    closeButton = new QPushButton("关闭", this);
    closeButton->setMinimumSize(80, 40); /* 设置最小尺寸 */

    /* 创建文件列表控件 */
    fileListWidget = new QListWidget(this);
    fileListWidget->setFixedWidth(150); /* 设置文件列表宽度 */

    /* 设置布局 */
    QHBoxLayout *mainLayout = new QHBoxLayout(); /* 创建主布局 */

    /* 左侧：文件列表和控制栏 */
    QVBoxLayout *leftLayout = new QVBoxLayout(); /* 创建左侧布局 */
    leftLayout->addWidget(fileListWidget); /* 添加文件列表 */

    /* 创建控制按钮布局 */
    QHBoxLayout *controlLayout = new QHBoxLayout(); /* 创建控制按钮布局 */
    controlLayout->addWidget(prevButton); /* 添加上一张按钮 */
    controlLayout->addWidget(nextButton); /* 添加下一张按钮 */
    controlLayout->addWidget(deleteButton); /* 添加删除按钮 */
    controlLayout->addWidget(closeButton); /* 添加关闭按钮 */

    /* 设置控制布局的最小高度，确保控制栏可见 */
    QWidget *controlWidget = new QWidget();
    controlWidget->setLayout(controlLayout);
    controlWidget->setMinimumHeight(60); /* 设置最小高度 */
    controlWidget->setMaximumHeight(60); /* 设置最大高度 */

    leftLayout->addWidget(controlWidget); /* 将控制布局添加到左侧布局 */

    /* 右侧：照片显示 */
    QVBoxLayout *rightLayout = new QVBoxLayout(); /* 创建右侧布局 */
    rightLayout->addWidget(photoLabel, 1); /* 添加照片标签，设置拉伸因子为1 */

    /* 合并左右布局 */
    mainLayout->addLayout(leftLayout); /* 添加左侧布局 */
    mainLayout->addLayout(rightLayout); /* 添加右侧布局 */

    this->setLayout(mainLayout); /* 应用主布局到窗口 */
}

void PhotoViewer::connectSignals()
{
    connect(prevButton, SIGNAL(clicked()), this, SLOT(onPrevButtonClicked()));
    connect(nextButton, SIGNAL(clicked()), this, SLOT(onNextButtonClicked()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));
    connect(fileListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onFileSelected()));
}

void PhotoViewer::loadPhotoFiles()
{
    fileListWidget->clear(); /* 清空文件列表 */
    photoFiles.clear(); /* 清空照片文件列表 */
    currentIndex = -1; /* 重置当前索引 */
    currentPhotoFile.clear(); /* 重置当前照片文件路径 */

    QDir dir("photos"); /* 打开照片目录 */
    if (!dir.exists()) { /* 如果目录不存在 */
        fileListWidget->addItem("无照片文件"); /* 显示提示信息 */
        return; /* 直接返回 */
    }

    QStringList filters; /* 创建文件过滤器 */
    filters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp"; /* 支持的图片格式 */
    dir.setNameFilters(filters); /* 设置过滤器 */
    dir.setSorting(QDir::Time | QDir::Reversed); /* 按时间倒序排列 */

    QFileInfoList fileList = dir.entryInfoList(QDir::Files); /* 获取文件列表 */

    if (fileList.isEmpty()) { /* 如果文件列表为空 */
        fileListWidget->addItem("无照片文件"); /* 显示提示信息 */
        return; /* 直接返回 */
    }

    for (const QFileInfo &fileInfo : fileList) { /* 遍历文件列表 */
        QString fileName = fileInfo.fileName(); /* 获取文件名 */
        fileListWidget->addItem(fileName); /* 添加文件名到列表 */
        photoFiles.append(dir.absoluteFilePath(fileName)); /* 添加文件绝对路径到照片文件列表 */
    }
}

void PhotoViewer::showCurrentPhoto()
{
    if (currentIndex < 0 || currentIndex >= photoFiles.size()) { /* 索引越界检查 */
        photoLabel->setText("请选择要查看的照片"); /* 显示提示信息 */
        return; /* 直接返回 */
    }

    currentPhotoFile = photoFiles[currentIndex]; /* 获取当前照片文件路径 */

    /* 加载并显示照片 */
    QImage image(currentPhotoFile); /* 加载照片 */
    if (image.isNull()) { /* 如果加载失败 */
        photoLabel->setText(QString("无法加载照片: %1").arg(currentPhotoFile)); /* 显示错误信息 */
        return; /* 直接返回 */
    }

    /* 缩放图像以适应标签大小，保持宽高比 */
    QImage scaledImage = image.scaled(photoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    photoLabel->setPixmap(QPixmap::fromImage(scaledImage)); /* 显示缩放后的图像 */

    /* 更新文件列表选中项 */
    if (fileListWidget->count() > 0) {
        fileListWidget->setCurrentRow(currentIndex); /* 设置当前选中项 */
    }
}

void PhotoViewer::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event); /* 调用父类的resizeEvent */

    /* 设置照片标签为固定大小(320, 240) */
    photoLabel->setFixedSize(320, 240); /* 设置照片标签固定大小 */

    /* 如果当前有照片显示，则重新显示 */
    if (!currentPhotoFile.isEmpty()) {
        showCurrentPhoto(); /* 重新显示当前照片 */
    }
}

void PhotoViewer::onPrevButtonClicked()
{
    if (photoFiles.isEmpty()) { /* 照片列表为空 */
        return; /* 直接返回 */
    }

    currentIndex--; /* 索引减1 */
    if (currentIndex < 0) { /* 如果索引小于0 */
        currentIndex = photoFiles.size() - 1; /* 循环到最后一张 */
    }

    showCurrentPhoto(); /* 显示当前照片 */
}

void PhotoViewer::onNextButtonClicked()
{
    if (photoFiles.isEmpty()) { /* 照片列表为空 */
        return; /* 直接返回 */
    }

    currentIndex++; /* 索引加1 */
    if (currentIndex >= photoFiles.size()) { /* 如果索引超出范围 */
        currentIndex = 0; /* 循环到第一张 */
    }

    showCurrentPhoto(); /* 显示当前照片 */
}

void PhotoViewer::onDeleteButtonClicked()
{
    if (currentPhotoFile.isEmpty()) { /* 没有选中照片 */
        return; /* 直接返回 */
    }

    /* 确认删除 */
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "确认删除",
                                  QString("确定要删除照片 %1 吗？").arg(QFileInfo(currentPhotoFile).fileName()),
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        /* 删除文件 */
        if (QFile::remove(currentPhotoFile)) { /* 删除成功 */
            /* 重新加载照片文件列表 */
            loadPhotoFiles(); /* 重新加载文件列表 */

            /* 清空照片显示 */
            photoLabel->clear(); /* 清空照片显示 */
            photoLabel->setText("请选择要查看的照片"); /* 显示提示信息 */

            /* 重置当前照片文件路径 */
            currentPhotoFile.clear(); /* 清空当前照片文件路径 */
        } else {
            qDebug() << "ERROR: failed to delete photo:" << currentPhotoFile; /* 打印错误信息 */
        }
    }
}

void PhotoViewer::onCloseButtonClicked()
{
    this->close(); /* 关闭窗口 */
}

void PhotoViewer::onFileSelected()
{
    QListWidgetItem *item = fileListWidget->currentItem(); /* 获取当前选中的项 */
    if (item == nullptr) { /* 如果没有选中项 */
        return; /* 直接返回 */
    }

    QString fileName = item->text(); /* 获取文件名 */
    int index = -1; /* 初始化索引为-1 */

    /* 查找文件名对应的索引 */
    for (int i = 0; i < photoFiles.size(); i++) {
        if (QFileInfo(photoFiles[i]).fileName() == fileName) { /* 找到匹配的文件名 */
            index = i; /* 记录索引 */
            break; /* 跳出循环 */
        }
    }

    if (index >= 0) { /* 找到匹配的索引 */
        currentIndex = index; /* 设置当前索引 */
        showCurrentPhoto(); /* 显示当前照片 */
    }
}
