// Copyright 2024 WPSImageTool
// 程序入口

#include <QApplication>
#include <QDebug>
#include "ui/MainWindow.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  // 设置应用程序信息
  QApplication::setApplicationName("WPSImageTool");
  QApplication::setApplicationVersion("1.0.0");
  QApplication::setOrganizationName("WPSImageTool");
  QApplication::setOrganizationDomain("wpsimage.tool");

  qDebug() << "WPS图片批量嵌入工具启动";
  qDebug() << "Qt版本:" << QT_VERSION_STR;

  // 创建并显示主窗口
  WPSImageTool::MainWindow main_window;
  main_window.show();

  qDebug() << "主窗口已显示";

  return app.exec();
}
