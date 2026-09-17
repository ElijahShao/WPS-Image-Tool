// Copyright 2024 WPSImageTool
// 数据结构定义

#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

#include <QImage>
#include <QString>
#include <QDateTime>
#include <QSize>
#include <QPoint>

namespace WPSImageTool {

// 图片数据结构
struct ImageData {
  QImage image;              // 原始图片
  QString temp_path;         // 临时文件路径
  QDateTime add_time;        // 添加时间
  QSize original_size;       // 原始尺寸
  QString format;            // 图片格式（png, jpg等）
  int index;                 // 在列表中的索引

  ImageData() : index(-1) {}

  ImageData(const QImage& img, const QString& path = QString())
      : image(img),
        temp_path(path),
        add_time(QDateTime::currentDateTime()),
        original_size(img.size()),
        format("PNG"),
        index(-1) {}

  bool isValid() const {
    return !image.isNull();
  }
};

// Excel单元格信息
struct CellInfo {
  QString address;           // 单元格地址（如"A1"）
  QPoint position;           // 像素位置
  QSize size;                // 单元格尺寸（像素）
  int row;                   // 行号（从1开始）
  int column;                // 列号（从1开始）

  CellInfo() : row(0), column(0) {}

  CellInfo(const QString& addr, int r = 0, int c = 0)
      : address(addr), row(r), column(c) {}

  bool isValid() const {
    return !address.isEmpty() && row > 0 && column > 0;
  }
};

// 插入配置
struct InsertConfig {
  QString cell_address;      // 目标单元格
  int max_width;             // 最大宽度（像素）
  int max_height;            // 最大高度（像素）
  int spacing;               // 图片间距（像素）
  bool maintain_ratio;       // 保持宽高比
  QString layout;            // 布局方式："vertical"(垂直) 或 "horizontal"(水平)

  InsertConfig()
      : max_width(100),
        max_height(100),
        spacing(5),
        maintain_ratio(true),
        layout("vertical") {}
};

// 操作结果
struct OperationResult {
  bool success;
  QString message;
  QString error_detail;

  OperationResult() : success(false) {}

  OperationResult(bool ok, const QString& msg = QString())
      : success(ok), message(msg) {}

  static OperationResult Success(const QString& msg = "操作成功") {
    return OperationResult(true, msg);
  }

  static OperationResult Failure(const QString& msg, const QString& detail = QString()) {
    OperationResult result(false, msg);
    result.error_detail = detail;
    return result;
  }
};

}  // namespace WPSImageTool

#endif  // DATASTRUCTURES_H
