/******************************************************************
Copyright © Deng Zhimao Co., Ltd. 2021-2030. All rights reserved.
* @projectName   video_server
* @brief         photoviewer.h
* @author        Deng Zhimao
* @email         dengzhimao@alientek.com
* @link          www.openedv.com
* @date          2021-11-19
*******************************************************************/
#ifndef PHOTOVIEWER_H
#define PHOTOVIEWER_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QImage>
#include <QPixmap>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QDebug>

class PhotoViewer : public QDialog
{
    Q_OBJECT

public:
    explicit PhotoViewer(QWidget *parent = nullptr);
    ~PhotoViewer();

    void loadPhotoFiles(); /* 加载照片文件 */

private:
    QLabel *photoLabel; /* 照片显示标签 */
    QPushButton *prevButton; /* 上一张按钮 */
    QPushButton *nextButton; /* 下一张按钮 */
    QPushButton *deleteButton; /* 删除按钮 */
    QPushButton *closeButton; /* 关闭按钮 */
    QListWidget *fileListWidget; /* 文件列表控件 */

    QString currentPhotoFile; /* 当前照片文件路径 */
    QList<QString> photoFiles; /* 照片文件列表 */
    int currentIndex; /* 当前照片索引 */

    void setupUI(); /* 设置界面 */
    void connectSignals(); /* 连接信号槽 */
    void showCurrentPhoto(); /* 显示当前照片 */
    void resizeEvent(QResizeEvent *event) override; /* 窗口大小改变事件 */

private slots:
    void onPrevButtonClicked(); /* 上一张按钮点击 */
    void onNextButtonClicked(); /* 下一张按钮点击 */
    void onDeleteButtonClicked(); /* 删除按钮点击 */
    void onCloseButtonClicked(); /* 关闭按钮点击 */
    void onFileSelected(); /* 文件列表项选中 */
};

#endif // PHOTOVIEWER_H
