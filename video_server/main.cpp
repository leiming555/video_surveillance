#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) /* 应用程序主函数，argc是命令行参数个数，argv是命令行参数数组 */
{
    QApplication a(argc, argv); /* 创建Qt应用程序对象，初始化应用程序 */
    MainWindow w;               /* 创建主窗口对象 */
    w.show();                   /* 显示主窗口 */
    return a.exec();            /* 进入应用程序事件循环，等待用户操作 */
}                              /* 应用程序结束 */
