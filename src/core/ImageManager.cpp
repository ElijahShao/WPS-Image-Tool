// Copyright 2024 WPSImageTool
// 图片管理器实现

#include "core/ImageManager.h"
#include <QDebug>

namespace WPSImageTool {

ImageManager::ImageManager(QObject* parent)
    : QObject(parent),
      next_index_(0) {
}

bool ImageManager::AddImage(const QImage& image) {
  if (image.isNull()) {
    qWarning() << "尝试添加无效图片";
    return false;
  }

  ImageData image_data(image);
  return AddImage(image_data);
}

bool ImageManager::AddImage(const ImageData& image_data) {
  if (!image_data.isValid()) {
    qWarning() << "尝试添加无效图片数据";
    return false;
  }

  ImageData data = image_data;
  data.index = next_index_++;

  image_list_.append(data);

  int index = image_list_.size() - 1;
  emit ImageAdded(index);
  emit ImageCountChanged(image_list_.size());

  qDebug() << "图片已添加，索引:" << index << "总数:" << image_list_.size();
  return true;
}

bool ImageManager::RemoveImage(int index) {
  if (!IsValidIndex(index)) {
    qWarning() << "无效的图片索引:" << index;
    return false;
  }

  image_list_.removeAt(index);
  UpdateIndices();

  emit ImageRemoved(index);
  emit ImageCountChanged(image_list_.size());

  qDebug() << "图片已删除，索引:" << index << "剩余:" << image_list_.size();
  return true;
}

void ImageManager::RemoveAllImages() {
  if (image_list_.isEmpty()) {
    return;
  }

  image_list_.clear();
  next_index_ = 0;

  emit AllImagesCleared();
  emit ImageCountChanged(0);

  qDebug() << "所有图片已清空";
}

bool ImageManager::MoveImage(int from_index, int to_index) {
  if (!IsValidIndex(from_index) || !IsValidIndex(to_index)) {
    qWarning() << "移动图片失败：无效索引" << from_index << "->" << to_index;
    return false;
  }

  if (from_index == to_index) {
    return true;
  }

  image_list_.move(from_index, to_index);
  UpdateIndices();

  emit ImageMoved(from_index, to_index);

  qDebug() << "图片已移动:" << from_index << "->" << to_index;
  return true;
}

bool ImageManager::MoveImageUp(int index) {
  if (index <= 0 || !IsValidIndex(index)) {
    return false;
  }

  return MoveImage(index, index - 1);
}

bool ImageManager::MoveImageDown(int index) {
  if (index >= image_list_.size() - 1 || !IsValidIndex(index)) {
    return false;
  }

  return MoveImage(index, index + 1);
}

ImageData ImageManager::GetImage(int index) const {
  if (!IsValidIndex(index)) {
    qWarning() << "获取图片失败：无效索引" << index;
    return ImageData();
  }

  return image_list_.at(index);
}

QVector<ImageData> ImageManager::GetAllImages() const {
  return image_list_;
}

QVector<QString> ImageManager::GetAllImagePaths() const {
  QVector<QString> paths;
  paths.reserve(image_list_.size());

  for (const ImageData& data : image_list_) {
    if (!data.temp_path.isEmpty()) {
      paths.append(data.temp_path);
    }
  }

  return paths;
}

int ImageManager::GetImageCount() const {
  return image_list_.size();
}

bool ImageManager::IsEmpty() const {
  return image_list_.isEmpty();
}

bool ImageManager::IsValidIndex(int index) const {
  return index >= 0 && index < image_list_.size();
}

void ImageManager::ClearAll() {
  RemoveAllImages();
}

bool ImageManager::ImportImages(const QStringList& file_paths) {
  if (file_paths.isEmpty()) {
    return false;
  }

  int success_count = 0;

  for (const QString& file_path : file_paths) {
    QImage image(file_path);
    if (!image.isNull()) {
      ImageData image_data(image, file_path);
      if (AddImage(image_data)) {
        success_count++;
      }
    } else {
      qWarning() << "加载图片失败:" << file_path;
    }
  }

  qDebug() << "批量导入完成，成功:" << success_count
           << "失败:" << (file_paths.size() - success_count);

  return success_count > 0;
}

void ImageManager::UpdateIndices() {
  for (int i = 0; i < image_list_.size(); ++i) {
    image_list_[i].index = i;
  }
}

}  // namespace WPSImageTool
