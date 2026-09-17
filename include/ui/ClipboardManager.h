// Copyright 2024 WPSImageTool
// 剪贴板管理器 - 负责监听和处理剪贴板图片

#ifndef CLIPBOARDMANAGER_H
#define CLIPBOARDMANAGER_H

#include <QObject>
#include <QClipboard>
#include <QImage>
#include <QMimeData>

namespace WPSImageTool {

class ClipboardManager : public QObject {
  Q_OBJECT

 public:
  explicit ClipboardManager(QObject* parent = nullptr);
  ~ClipboardManager() override = default;

  // 检查剪贴板是否有图片
  bool HasImage() const;

  // 获取剪贴板中的图片
  QImage GetImage() const;

  // 检查剪贴板是否有文件路径
  bool HasFilePath() const;

  // 获取剪贴板中的文件路径
  QStringList GetFilePaths() const;

  // 设置图片到剪贴板
  void SetImage(const QImage& image);

  // 开始/停止监听剪贴板
  void StartMonitoring();
  void StopMonitoring();

  bool IsMonitoring() const { return monitoring_; }

 signals:
  void ImageAvailable(const QImage& image);
  void FilePathsAvailable(const QStringList& paths);
  void ClipboardChanged();

 private slots:
  void OnClipboardChanged();

 private:
  // 判断文件是否为图片
  bool IsImageFile(const QString& file_path) const;

  QClipboard* clipboard_;
  bool monitoring_;
  QImage last_image_;
};

}  // namespace WPSImageTool

#endif  // CLIPBOARDMANAGER_H
