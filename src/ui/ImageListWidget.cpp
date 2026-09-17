// Copyright 2024 WPSImageTool
// 图片列表控件实现

#include "ui/ImageListWidget.h"
#include <QMessageBox>
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QDebug>

namespace WPSImageTool {

ImageListWidget::ImageListWidget(QWidget* parent)
    : QWidget(parent) {
  SetupUI();
  CreateContextMenu();
}

void ImageListWidget::AddImage(const QImage& image) {
  if (image.isNull()) {
    qWarning() << "尝试添加无效图片";
    return;
  }

  ImageData image_data(image);
  images_.append(image_data);

  // 创建缩略图
  QPixmap thumbnail = CreateThumbnail(image, QSize(100, 100));

  // 创建列表项
  QListWidgetItem* item = new QListWidgetItem(list_widget_);
  item->setIcon(QIcon(thumbnail));
  item->setText(QString("图片 %1 (%2x%3)")
               .arg(images_.size())
               .arg(image.width())
               .arg(image.height()));
  item->setToolTip(QString("尺寸: %1x%2\n添加时间: %3")
                  .arg(image.width())
                  .arg(image.height())
                  .arg(image_data.add_time.toString("yyyy-MM-dd hh:mm:ss")));

  list_widget_->addItem(item);

  emit ImageAdded(image);
  emit ImageCountChanged(images_.size());

  qDebug() << "图片已添加到列表，总数:" << images_.size();
}

void ImageListWidget::RemoveSelectedImage() {
  int current_index = GetSelectedIndex();
  if (current_index < 0) {
    QMessageBox::information(this, "提示", "请先选择要删除的图片");
    return;
  }

  images_.removeAt(current_index);
  delete list_widget_->takeItem(current_index);

  emit ImageRemoved(current_index);
  emit ImageCountChanged(images_.size());

  UpdateImageList();

  qDebug() << "图片已删除，索引:" << current_index;
}

void ImageListWidget::RemoveAllImages() {
  if (images_.isEmpty()) {
    return;
  }

  QMessageBox::StandardButton reply = QMessageBox::question(
      this,
      "确认",
      QString("确定要清空所有图片吗？共有 %1 张图片。").arg(images_.size()),
      QMessageBox::Yes | QMessageBox::No);

  if (reply != QMessageBox::Yes) {
    return;
  }

  images_.clear();
  list_widget_->clear();

  emit ImageCountChanged(0);

  qDebug() << "所有图片已清空";
}

void ImageListWidget::MoveImageUp() {
  int current_index = GetSelectedIndex();
  if (current_index <= 0) {
    return;
  }

  images_.move(current_index, current_index - 1);

  QListWidgetItem* item = list_widget_->takeItem(current_index);
  list_widget_->insertItem(current_index - 1, item);
  list_widget_->setCurrentRow(current_index - 1);

  UpdateImageList();

  qDebug() << "图片上移:" << current_index << "->" << (current_index - 1);
}

void ImageListWidget::MoveImageDown() {
  int current_index = GetSelectedIndex();
  if (current_index < 0 || current_index >= images_.size() - 1) {
    return;
  }

  images_.move(current_index, current_index + 1);

  QListWidgetItem* item = list_widget_->takeItem(current_index);
  list_widget_->insertItem(current_index + 1, item);
  list_widget_->setCurrentRow(current_index + 1);

  UpdateImageList();

  qDebug() << "图片下移:" << current_index << "->" << (current_index + 1);
}

QVector<ImageData> ImageListWidget::GetAllImages() const {
  return images_;
}

QVector<QString> ImageListWidget::GetAllImagePaths() const {
  QVector<QString> paths;
  paths.reserve(images_.size());

  for (const ImageData& data : images_) {
    if (!data.temp_path.isEmpty()) {
      paths.append(data.temp_path);
    }
  }

  return paths;
}

int ImageListWidget::GetImageCount() const {
  return images_.size();
}

int ImageListWidget::GetSelectedIndex() const {
  return list_widget_->currentRow();
}

void ImageListWidget::SelectImage(int index) {
  if (index >= 0 && index < list_widget_->count()) {
    list_widget_->setCurrentRow(index);
  }
}

void ImageListWidget::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Delete) {
    RemoveSelectedImage();
  } else if (event->key() == Qt::Key_Up && event->modifiers() & Qt::ControlModifier) {
    MoveImageUp();
  } else if (event->key() == Qt::Key_Down && event->modifiers() & Qt::ControlModifier) {
    MoveImageDown();
  } else {
    QWidget::keyPressEvent(event);
  }
}

void ImageListWidget::OnItemDoubleClicked(QListWidgetItem* item) {
  int index = list_widget_->row(item);
  if (index >= 0 && index < images_.size()) {
    ShowImagePreview(index);
    emit ImageDoubleClicked(index);
  }
}

void ImageListWidget::OnItemSelectionChanged() {
  int index = GetSelectedIndex();
  if (index >= 0) {
    emit ImageSelected(index);
  }
}

void ImageListWidget::ShowContextMenu(const QPoint& pos) {
  QPoint global_pos = list_widget_->mapToGlobal(pos);

  bool has_selection = GetSelectedIndex() >= 0;
  bool has_images = !images_.isEmpty();

  action_preview_->setEnabled(has_selection);
  action_remove_->setEnabled(has_selection);
  action_move_up_->setEnabled(has_selection && GetSelectedIndex() > 0);
  action_move_down_->setEnabled(has_selection &&
                               GetSelectedIndex() < images_.size() - 1);
  action_clear_all_->setEnabled(has_images);

  context_menu_->exec(global_pos);
}

void ImageListWidget::OnPreviewImage() {
  int index = GetSelectedIndex();
  if (index >= 0 && index < images_.size()) {
    ShowImagePreview(index);
  }
}

void ImageListWidget::OnRemoveImage() {
  RemoveSelectedImage();
}

void ImageListWidget::OnClearAll() {
  RemoveAllImages();
}

void ImageListWidget::OnMoveUp() {
  MoveImageUp();
}

void ImageListWidget::OnMoveDown() {
  MoveImageDown();
}

void ImageListWidget::SetupUI() {
  layout_ = new QVBoxLayout(this);
  layout_->setContentsMargins(0, 0, 0, 0);

  list_widget_ = new QListWidget(this);
  list_widget_->setIconSize(QSize(100, 100));
  list_widget_->setViewMode(QListWidget::IconMode);
  list_widget_->setResizeMode(QListWidget::Adjust);
  list_widget_->setMovement(QListWidget::Static);
  list_widget_->setContextMenuPolicy(Qt::CustomContextMenu);

  connect(list_widget_, &QListWidget::itemDoubleClicked,
          this, &ImageListWidget::OnItemDoubleClicked);
  connect(list_widget_, &QListWidget::itemSelectionChanged,
          this, &ImageListWidget::OnItemSelectionChanged);
  connect(list_widget_, &QListWidget::customContextMenuRequested,
          this, &ImageListWidget::ShowContextMenu);

  layout_->addWidget(list_widget_);
  setLayout(layout_);
}

void ImageListWidget::CreateContextMenu() {
  context_menu_ = new QMenu(this);

  action_preview_ = context_menu_->addAction("预览图片");
  connect(action_preview_, &QAction::triggered,
          this, &ImageListWidget::OnPreviewImage);

  context_menu_->addSeparator();

  action_move_up_ = context_menu_->addAction("上移 (Ctrl+↑)");
  connect(action_move_up_, &QAction::triggered,
          this, &ImageListWidget::OnMoveUp);

  action_move_down_ = context_menu_->addAction("下移 (Ctrl+↓)");
  connect(action_move_down_, &QAction::triggered,
          this, &ImageListWidget::OnMoveDown);

  context_menu_->addSeparator();

  action_remove_ = context_menu_->addAction("删除图片 (Delete)");
  connect(action_remove_, &QAction::triggered,
          this, &ImageListWidget::OnRemoveImage);

  action_clear_all_ = context_menu_->addAction("清空所有");
  connect(action_clear_all_, &QAction::triggered,
          this, &ImageListWidget::OnClearAll);
}

QPixmap ImageListWidget::CreateThumbnail(const QImage& image,
                                        const QSize& size) const {
  if (image.isNull()) {
    return QPixmap();
  }

  return QPixmap::fromImage(
      image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ImageListWidget::ShowImagePreview(int index) {
  if (index < 0 || index >= images_.size()) {
    return;
  }

  const ImageData& image_data = images_[index];

  QDialog* dialog = new QDialog(this);
  dialog->setWindowTitle(QString("图片预览 - 图片 %1").arg(index + 1));
  dialog->resize(800, 600);

  QVBoxLayout* layout = new QVBoxLayout(dialog);

  QScrollArea* scroll_area = new QScrollArea(dialog);
  scroll_area->setWidgetResizable(true);

  QLabel* image_label = new QLabel(scroll_area);
  image_label->setPixmap(QPixmap::fromImage(image_data.image));
  image_label->setAlignment(Qt::AlignCenter);

  scroll_area->setWidget(image_label);
  layout->addWidget(scroll_area);

  QLabel* info_label = new QLabel(dialog);
  info_label->setText(QString("尺寸: %1 x %2 | 格式: %3 | 添加时间: %4")
                     .arg(image_data.image.width())
                     .arg(image_data.image.height())
                     .arg(image_data.format)
                     .arg(image_data.add_time.toString("yyyy-MM-dd hh:mm:ss")));
  layout->addWidget(info_label);

  dialog->setLayout(layout);
  dialog->exec();
  dialog->deleteLater();
}

void ImageListWidget::UpdateImageList() {
  // 更新所有项的显示文本
  for (int i = 0; i < list_widget_->count(); ++i) {
    QListWidgetItem* item = list_widget_->item(i);
    if (i < images_.size()) {
      const ImageData& data = images_[i];
      item->setText(QString("图片 %1 (%2x%3)")
                   .arg(i + 1)
                   .arg(data.image.width())
                   .arg(data.image.height()));
    }
  }
}

}  // namespace WPSImageTool
