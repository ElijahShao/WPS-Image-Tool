// Copyright 2024 WPSImageTool
// 主窗口 - 应用程序主界面

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include "ImageListWidget.h"
#include "ClipboardManager.h"
#include "core/ImageManager.h"
#include "core/ImageProcessor.h"
#include "core/ExcelController.h"

namespace WPSImageTool {

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;

 protected:
  void closeEvent(QCloseEvent* event) override;

 private slots:
  // 文件操作
  void OnOpenFile();
  void OnRecentFileSelected();

  // 图片操作
  void OnPasteFromClipboard();
  void OnAddImagesFromFiles();
  void OnClearAllImages();

  // 插入操作
  void OnInsertImages();
  void OnClipboardModeInsert();

  // 剪贴板监听
  void OnClipboardImageAvailable(const QImage& image);

  // UI更新
  void OnImageCountChanged(int count);
  void OnOperationProgress(int current, int total);

  // 设置
  void OnShowSettings();
  void OnAbout();

 private:
  void SetupUI();
  void CreateMenuBar();
  void CreateToolBar();
  void CreateStatusBar();
  void CreateCentralWidget();
  void ConnectSignals();

  // 文件检查
  bool CheckFileOpen(const QString& file_path);
  void CloseWPSFile(const QString& file_path);
  void ReopenWPSFile(const QString& file_path);

  // 单元格地址解析
  bool ValidateCellAddress(const QString& address);

  // 界面更新
  void UpdateUIState();
  void UpdateStatusBar(const QString& message);

  // 加载/保存设置
  void LoadSettings();
  void SaveSettings();

  // UI组件
  ImageListWidget* image_list_widget_;
  QLineEdit* file_path_edit_;
  QLineEdit* cell_address_edit_;
  QPushButton* open_file_btn_;
  QPushButton* insert_images_btn_;
  QPushButton* paste_btn_;
  QPushButton* add_files_btn_;
  QPushButton* clear_all_btn_;
  QLabel* status_label_;
  QProgressBar* progress_bar_;

  // 模式选择
  QRadioButton* mode_direct_edit_;
  QRadioButton* mode_clipboard_;
  QGroupBox* mode_group_box_;

  // 业务逻辑组件
  ClipboardManager* clipboard_manager_;
  ImageManager* image_manager_;
  ImageProcessor* image_processor_;
  ExcelController* excel_controller_;

  // 状态变量
  QString current_file_path_;
  QString target_cell_address_;
  QStringList recent_files_;
  bool clipboard_monitoring_enabled_;
};

}  // namespace WPSImageTool

#endif  // MAINWINDOW_H
