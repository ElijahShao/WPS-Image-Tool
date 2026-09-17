// Copyright 2024 WPSImageTool
// 图片处理模块 - 负责图片缩放、格式转换等操作

#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QImage>
#include <QSize>
#include <QString>
#include "DataStructures.h"

namespace WPSImageTool {

class ImageProcessor : public QObject {
  Q_OBJECT

 public:
  explicit ImageProcessor(QObject* parent = nullptr);
  ~ImageProcessor() override = default;

  // 计算图片最佳尺寸（保持宽高比）
  QSize CalculateOptimalSize(const QImage& image,
                            const QSize& max_size,
                            bool maintain_ratio = true) const;

  // 缩放图片
  QImage ResizeImage(const QImage& image,
                    const QSize& new_size,
                    Qt::TransformationMode mode = Qt::SmoothTransformation) const;

  // 保存图片到临时文件
  QString SaveImageToTemp(const QImage& image,
                         const QString& format = "PNG");

  // 转换图片格式
  QByteArray ConvertToFormat(const QImage& image,
                            const QString& format) const;

  // 优化图片质量和大小
  QImage OptimizeImage(const QImage& image,
                      int max_file_size_kb = 500) const;

  // 生成缩略图
  QImage GenerateThumbnail(const QImage& image,
                          const QSize& thumbnail_size) const;

  // 清理临时文件
  void CleanupTempFiles();

  // 获取临时目录路径
  QString GetTempDirectory() const;

 signals:
  void ProcessingProgress(int percent);
  void ProcessingCompleted();
  void ProcessingFailed(const QString& error);

 private:
  // 创建临时目录
  bool CreateTempDirectory();

  // 生成唯一的临时文件名
  QString GenerateTempFileName(const QString& extension) const;

  QVector<QString> temp_files_;
  QString temp_directory_;
};

}  // namespace WPSImageTool

#endif  // IMAGEPROCESSOR_H
