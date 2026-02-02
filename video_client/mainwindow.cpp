/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 2021-2030. All rights reserved.
* @projectName   video_client
* @brief         mainwindow.cpp
* @author        Deng Zhimao
* @email         dengzhimao@alientek.com
* @link          www.openedv.com
* @date          2021-11-20
*******************************************************************/
#include "mainwindow.h"
#include <QDebug>
#include <QDateTime> // 用于生成文件名的时间戳
#include <QMessageBox> // 用于提示保存成功
#include <QDir>        // 用于操作路径
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    /* 设置背景颜色为黑色 */
    QColor color = QColor(Qt::black);
    QPalette p;
    p.setColor(QPalette::Window, color);
    this->setPalette(p);

    udpSocket = new QUdpSocket(this);
    /* 绑定端口号 */
    udpSocket->bind(QHostAddress::Any, 8888);

    videoLabel = new QLabel(this);
    videoLabel->resize(640, 480);
    videoLabel->setText("未获取到图像数据");
    videoLabel->setStyleSheet("QWidget {color: white;}");
    videoLabel->setAlignment(Qt::AlignCenter);
    connect(udpSocket, SIGNAL(readyRead()), this,SLOT(videoUpdate()));
    // ================== 新增代码：截图按钮 ==================
       btnSave = new QPushButton(this);
       btnSave->setText("截图保存");
       btnSave->resize(100, 50);
       // 将按钮放在右侧空白处 (窗口宽800，视频宽640，右边有空余)
       btnSave->move(680, 200);

       // 连接信号槽：点击按钮 -> 执行 saveImage 函数
       connect(btnSave, SIGNAL(clicked()), this, SLOT(saveImage()));
       // ======================================================

    this->setGeometry(0, 0, 800, 480);
}

MainWindow::~MainWindow()
{
}

void MainWindow::videoUpdate()
{
    QByteArray datagram;

    /* 数据大小重置 */
    datagram.resize(udpSocket->pendingDatagramSize());

    /* 数据存放到datagram中 */
    udpSocket->readDatagram(datagram.data(), datagram.size());

    QByteArray decryptedByte;
    decryptedByte = QByteArray::fromBase64(datagram.data());

    QImage image;
    image.loadFromData(decryptedByte);

    /* 显示图像 */
    videoLabel->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    videoLabel->move((this->width() - 640) / 2, (this->height() - 480) / 2);
}

void MainWindow::saveImage()
{
    // 1. 获取当前 Label 显示的图像
    const QPixmap *pixmap = videoLabel->pixmap();

    if (pixmap) {
        // 2. 将 Pixmap 转为 Image
        QImage image = pixmap->toImage();

        // 3. 生成文件名（使用当前时间，精确到秒，防止覆盖）
        // 格式例如：2023-10-25_14-30-05.jpg
        QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
        QString fileName = currentTime + ".jpg";

        // 4. 保存图片
        if (image.save(fileName)) {
            // 获取当前程序运行的目录
            QString currentPath = QDir::currentPath();
            qDebug() << "截图已保存到：" << currentPath << "/" << fileName;

//             弹窗提示用户（可选）
             QMessageBox::information(this, "成功", "截图已保存：\n" + fileName);

//             为了视觉反馈，可以让按钮变一下文字
          //  btnSave->setText("保存成功!");
            // 1秒后变回来（使用 QTimer 需要 include <QTimer>，这里简单处理就不加了，手动改回即可）
        } else {
            qDebug() << "保存失败！";
        }
    } else {
        qDebug() << "当前没有图像，无法截图！";
    }
}

