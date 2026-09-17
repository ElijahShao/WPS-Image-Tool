// Copyright 2024 WPSImageTool
// 图片管理模块 - 负责图片列表的增删改查

#ifndef IMAGEMANAGER_H
#define IMAGEMANAGER_H

#include <QObject>
#include <QVector>
#include "DataStructures.h"

namespace WPSImageTool {

class ImageManager : public QObject {
  Q_OBJECT

 public:
  explicit ImageManager(QObject* parent = nullptr);
  ~ImageManager() override = default;

  // 添加图片
  bool AddImage(const QImage& image);
  bool AddImage(const ImageData& image_data);

  // 删除图片
  bool RemoveImage(int index);
  void RemoveAllImages();

  // 移动图片位置
  bool MoveImage(int from_index, int to_index);
  bool MoveImageUp(int index);
  bool MoveImageDown(int index);

  // 获取图片
  ImageData GetImage(int index) const;
  QVector<ImageData> GetAllImages() const;
  QVector<QString> GetAllImagePaths() const;

  // 查询
  int GetImageCount() const;
  bool IsEmpty() const;
  bool IsValidIndex(int index) const;

  // 批量操作
  void ClearAll();
  bool ImportImages(const QStringList& file_paths);

 signals:
  void ImageAdded(int index);
  void ImageRemoved(int index);
  void ImageMoved(int from_index, int to_index);
  void AllImagesCleared();
  void ImageCountChanged(int count);

 private:
  void UpdateIndices();

  QVector<ImageData> image_list_;
  int next_index_;
};

}  // namespace WPSImageTool

#endif  // IMAGEMANAGER_H
