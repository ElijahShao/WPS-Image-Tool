// Copyright 2024 WPSImageTool
// 图片处理器实现

#include "core/ImageProcessor.h"
#include <QDir>
#include <QStandardPaths>
#include <QUuid>
#include <QBuffer>
#include <QDebug>

namespace WPSImageTool {

ImageProcessor::ImageProcessor(QObject* parent)
    : QObject(parent) {
  CreateTempDirectory();
}

QSize ImageProcessor::CalculateOptimalSize(const QImage& image,
                                          const QSize& max_size,
                                          bool maintain_ratio) const {
  if (image.isNull() || max_size.isEmpty()) {
    return QSize();
  }

  QSize original_size = image.size();

  // 如果图片已经小于最大尺寸，直接返回原尺寸
  if (original_size.width() <= max_size.width() &&
      original_size.height() <= max_size.height()) {
    return original_size;
  }

  if (!maintain_ratio) {
    return max_size;
  }

  // 保持宽高比缩放
  double width_ratio = static_cast<double>(max_size.width()) / original_size.width();
  double height_ratio = static_cast<double>(max_size.height()) / original_size.height();
  double ratio = qMin(width_ratio, height_ratio);

  int new_width = static_cast<int>(original_size.width() * ratio);
  int new_height = static_cast<int>(original_size.height() * ratio);

  return QSize(new_width, new_height);
}

QImage ImageProcessor::ResizeImage(const QImage& image,
                                  const QSize& new_size,
                                  Qt::TransformationMode mode) const {
  if (image.isNull() || new_size.isEmpty()) {
    return QImage();
  }

  return image.scaled(new_size, Qt::KeepAspectRatio, mode);
}

QString ImageProcessor::SaveImageToTemp(const QImage& image,
                                       const QString& format) {
  if (image.isNull()) {
    emit ProcessingFailed("图片无效");
    return QString();
  }

  QString file_name = GenerateTempFileName(format.toLower());
  QString file_path = temp_directory_ + QDir::separator() + file_name;

  if (image.save(file_path, format.toUtf8().constData())) {
    temp_files_.append(file_path);
    qDebug() << "图片已保存到临时文件:" << file_path;
    return file_path;
  }

  emit ProcessingFailed("保存图片失败: " + file_path);
  return QString();
}

QByteArray ImageProcessor::ConvertToFormat(const QImage& image,
                                          const QString& format) const {
  if (image.isNull()) {
    return QByteArray();
  }

  QByteArray byte_array;
  QBuffer buffer(&byte_array);
  buffer.open(QIODevice::WriteOnly);

  if (image.save(&buffer, format.toUtf8().constData())) {
    return byte_array;
  }

  return QByteArray();
}

QImage ImageProcessor::OptimizeImage(const QImage& image,
                                    int max_file_size_kb) const {
  if (image.isNull()) {
    return QImage();
  }

  QImage optimized = image;
  int quality = 95;

  while (quality > 50) {
    QByteArray data = ConvertToFormat(optimized, "JPG");
    if (data.size() / 1024 <= max_file_size_kb) {
      break;
    }

    // 如果文件太大，降低质量或缩小尺寸
    if (quality > 70) {
      quality -= 10;
    } else {
      QSize new_size = optimized.size() * 0.9;
      optimized = optimized.scaled(new_size, Qt::KeepAspectRatio,
                                   Qt::SmoothTransformation);
      quality = 85;
    }
  }

  return optimized;
}

QImage ImageProcessor::GenerateThumbnail(const QImage& image,
                                        const QSize& thumbnail_size) const {
  if (image.isNull() || thumbnail_size.isEmpty()) {
    return QImage();
  }

  return image.scaled(thumbnail_size, Qt::KeepAspectRatio,
                     Qt::SmoothTransformation);
}

void ImageProcessor::CleanupTempFiles() {
  for (const QString& file_path : temp_files_) {
    QFile file(file_path);
    if (file.exists()) {
      if (file.remove()) {
        qDebug() << "已删除临时文件:" << file_path;
      } else {
        qWarning() << "删除临时文件失败:" << file_path;
      }
    }
  }
  temp_files_.clear();
}

QString ImageProcessor::GetTempDirectory() const {
  return temp_directory_;
}

bool ImageProcessor::CreateTempDirectory() {
  QString temp_path = QStandardPaths::writableLocation(
      QStandardPaths::TempLocation);
  temp_directory_ = temp_path + QDir::separator() + "WPSImageTool";

  QDir dir;
  if (!dir.exists(temp_directory_)) {
    if (dir.mkpath(temp_directory_)) {
      qDebug() << "创建临时目录:" << temp_directory_;
      return true;
    } else {
      qWarning() << "创建临时目录失败:" << temp_directory_;
      return false;
    }
  }

  return true;
}

QString ImageProcessor::GenerateTempFileName(const QString& extension) const {
  QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
  return QString("img_%1.%2").arg(uuid).arg(extension);
}

}  // namespace WPSImageTool
