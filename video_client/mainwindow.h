/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 2021-2030. All rights reserved.
* @projectName   video_client
* @brief         mainwindow.h
* @author        Deng Zhimao
* @email         dengzhimao@alientek.com
* @link          www.openedv.com
* @date          2021-11-20
*******************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QLabel>
#include <QResizeEvent>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT //Qt 宏，开启 信号槽机制（用于事件回调，比如按钮点击、数据更新）

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    /* 用于接收数据 */
    QUdpSocket *udpSocket;

    /* 显示接收的图像数据 */
    QLabel *videoLabel;

    void resizeEvent(QResizeEvent *event) override;
    QPushButton *btnSave; // 3. 声明按钮指针

private slots:
    /* 图像更新 */
    void videoUpdate();
    void saveImage(); // 2. 声明截图槽函数
};
#endif // MAINWINDOW_H
