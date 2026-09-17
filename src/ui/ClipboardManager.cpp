// Copyright 2024 WPSImageTool
// 剪贴板管理器实现

#include "ui/ClipboardManager.h"
#include <QApplication>
#include <QMimeData>
#include <QFileInfo>
#include <QDebug>

namespace WPSImageTool {

ClipboardManager::ClipboardManager(QObject* parent)
    : QObject(parent),
      monitoring_(false) {
  clipboard_ = QApplication::clipboard();

  connect(clipboard_, &QClipboard::dataChanged,
          this, &ClipboardManager::OnClipboardChanged);
}

bool ClipboardManager::HasImage() const {
  const QMimeData* mime_data = clipboard_->mimeData();
  return mime_data && mime_data->hasImage();
}

QImage ClipboardManager::GetImage() const {
  if (!HasImage()) {
    return QImage();
  }

  QImage image = clipboard_->image();
  return image;
}

bool ClipboardManager::HasFilePath() const {
  const QMimeData* mime_data = clipboard_->mimeData();
  return mime_data && mime_data->hasUrls();
}

QStringList ClipboardManager::GetFilePaths() const {
  QStringList paths;

  if (!HasFilePath()) {
    return paths;
  }

  const QMimeData* mime_data = clipboard_->mimeData();
  QList<QUrl> urls = mime_data->urls();

  for (const QUrl& url : urls) {
    if (url.isLocalFile()) {
      QString path = url.toLocalFile();
      if (IsImageFile(path)) {
        paths.append(path);
      }
    }
  }

  return paths;
}

void ClipboardManager::SetImage(const QImage& image) {
  if (image.isNull()) {
    qWarning() << "尝试设置无效图片到剪贴板";
    return;
  }

  clipboard_->setImage(image);
  qDebug() << "图片已设置到剪贴板";
}

void ClipboardManager::StartMonitoring() {
  if (monitoring_) {
    qDebug() << "剪贴板监听已启动";
    return;
  }

  monitoring_ = true;
  qDebug() << "开始监听剪贴板";
}

void ClipboardManager::StopMonitoring() {
  if (!monitoring_) {
    return;
  }

  monitoring_ = false;
  qDebug() << "停止监听剪贴板";
}

void ClipboardManager::OnClipboardChanged() {
  emit ClipboardChanged();

  if (!monitoring_) {
    return;
  }

  // 检查是否有图片
  if (HasImage()) {
    QImage image = GetImage();
    if (!image.isNull()) {
      // 避免重复通知相同图片
      if (image != last_image_) {
        last_image_ = image;
        qDebug() << "剪贴板检测到新图片";
        emit ImageAvailable(image);
      }
    }
  }

  // 检查是否有文件路径
  if (HasFilePath()) {
    QStringList paths = GetFilePaths();
    if (!paths.isEmpty()) {
      qDebug() << "剪贴板检测到图片文件:" << paths.size() << "个";
      emit FilePathsAvailable(paths);
    }
  }
}

bool ClipboardManager::IsImageFile(const QString& file_path) const {
  QFileInfo file_info(file_path);

  if (!file_info.exists() || !file_info.isFile()) {
    return false;
  }

  QString suffix = file_info.suffix().toLower();

  // 支持的图片格式
  QStringList image_extensions = {
    "png", "jpg", "jpeg", "bmp", "gif", "tiff", "tif", "ico", "webp"
  };

  return image_extensions.contains(suffix);
}

}  // namespace WPSImageTool
