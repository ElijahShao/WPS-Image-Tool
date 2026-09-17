// Copyright 2024 WPSImageTool
// 图片列表控件 - 显示和管理图片列表

#ifndef IMAGELISTWIDGET_H
#define IMAGELISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QVector>
#include "core/DataStructures.h"

namespace WPSImageTool {

class ImageListWidget : public QWidget {
  Q_OBJECT

 public:
  explicit ImageListWidget(QWidget* parent = nullptr);
  ~ImageListWidget() override = default;

  // 图片操作
  void AddImage(const QImage& image);
  void RemoveSelectedImage();
  void RemoveAllImages();
  void MoveImageUp();
  void MoveImageDown();

  // 获取图片数据
  QVector<ImageData> GetAllImages() const;
  QVector<QString> GetAllImagePaths() const;
  int GetImageCount() const;

  // 选择相关
  int GetSelectedIndex() const;
  void SelectImage(int index);

 signals:
  void ImageAdded(const QImage& image);
  void ImageRemoved(int index);
  void ImageSelected(int index);
  void ImageDoubleClicked(int index);
  void ImageCountChanged(int count);

 protected:
  void keyPressEvent(QKeyEvent* event) override;

 private slots:
  void OnItemDoubleClicked(QListWidgetItem* item);
  void OnItemSelectionChanged();
  void ShowContextMenu(const QPoint& pos);
  void OnPreviewImage();
  void OnRemoveImage();
  void OnClearAll();
  void OnMoveUp();
  void OnMoveDown();

 private:
  void SetupUI();
  void CreateContextMenu();
  QPixmap CreateThumbnail(const QImage& image, const QSize& size) const;
  void ShowImagePreview(int index);
  void UpdateImageList();

  QListWidget* list_widget_;
  QVBoxLayout* layout_;
  QMenu* context_menu_;

  // 菜单动作
  QAction* action_preview_;
  QAction* action_remove_;
  QAction* action_clear_all_;
  QAction* action_move_up_;
  QAction* action_move_down_;

  QVector<ImageData> images_;
};

}  // namespace WPSImageTool

#endif  // IMAGELISTWIDGET_H
