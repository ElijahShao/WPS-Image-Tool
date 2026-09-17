// Copyright 2024 WPSImageTool
// 主窗口实现

#include "ui/MainWindow.h"
#include <QMenuBar>
#include <QToolBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QCloseEvent>
#include <QProcess>
#include <QDebug>
#include <QGroupBox>
#include <QSplitter>

namespace WPSImageTool {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      clipboard_monitoring_enabled_(false) {

  // 创建业务逻辑组件
  clipboard_manager_ = new ClipboardManager(this);
  image_manager_ = new ImageManager(this);
  image_processor_ = new ImageProcessor(this);
  excel_controller_ = new ExcelController(this);

  SetupUI();
  ConnectSignals();
  LoadSettings();

  // 初始化Excel控制器
  if (!excel_controller_->Initialize()) {
    QMessageBox::warning(this, "警告",
        "Excel控制器初始化失败，部分功能可能不可用\n" +
        excel_controller_->GetLastError());
  }

  UpdateUIState();
}

MainWindow::~MainWindow() {
  SaveSettings();
  image_processor_->CleanupTempFiles();
}

void MainWindow::closeEvent(QCloseEvent* event) {
  SaveSettings();
  image_processor_->CleanupTempFiles();
  event->accept();
}

void MainWindow::SetupUI() {
  setWindowTitle("WPS图片批量嵌入工具");
  resize(900, 700);

  CreateMenuBar();
  CreateToolBar();
  CreateCentralWidget();
  CreateStatusBar();
}

void MainWindow::CreateMenuBar() {
  QMenuBar* menu_bar = menuBar();

  // 文件菜单
  QMenu* file_menu = menu_bar->addMenu("文件(&F)");

  QAction* open_action = file_menu->addAction("打开Excel文件(&O)");
  open_action->setShortcut(QKeySequence::Open);
  connect(open_action, &QAction::triggered, this, &MainWindow::OnOpenFile);

  file_menu->addSeparator();

  QAction* exit_action = file_menu->addAction("退出(&X)");
  exit_action->setShortcut(QKeySequence::Quit);
  connect(exit_action, &QAction::triggered, this, &MainWindow::close);

  // 编辑菜单
  QMenu* edit_menu = menu_bar->addMenu("编辑(&E)");

  QAction* paste_action = edit_menu->addAction("从剪贴板粘贴(&V)");
  paste_action->setShortcut(QKeySequence::Paste);
  connect(paste_action, &QAction::triggered,
          this, &MainWindow::OnPasteFromClipboard);

  QAction* add_files_action = edit_menu->addAction("添加图片文件(&A)");
  connect(add_files_action, &QAction::triggered,
          this, &MainWindow::OnAddImagesFromFiles);

  edit_menu->addSeparator();

  QAction* clear_action = edit_menu->addAction("清空所有图片(&C)");
  connect(clear_action, &QAction::triggered,
          this, &MainWindow::OnClearAllImages);

  // 设置菜单
  QMenu* settings_menu = menu_bar->addMenu("设置(&S)");

  QAction* settings_action = settings_menu->addAction("选项(&O)");
  connect(settings_action, &QAction::triggered,
          this, &MainWindow::OnShowSettings);

  // 帮助菜单
  QMenu* help_menu = menu_bar->addMenu("帮助(&H)");

  QAction* about_action = help_menu->addAction("关于(&A)");
  connect(about_action, &QAction::triggered, this, &MainWindow::OnAbout);
}

void MainWindow::CreateToolBar() {
  QToolBar* toolbar = addToolBar("主工具栏");
  toolbar->setMovable(false);

  QAction* open_action = toolbar->addAction("打开文件");
  connect(open_action, &QAction::triggered, this, &MainWindow::OnOpenFile);

  toolbar->addSeparator();

  QAction* paste_action = toolbar->addAction("粘贴图片");
  connect(paste_action, &QAction::triggered,
          this, &MainWindow::OnPasteFromClipboard);

  QAction* add_action = toolbar->addAction("添加文件");
  connect(add_action, &QAction::triggered,
          this, &MainWindow::OnAddImagesFromFiles);

  toolbar->addSeparator();

  QAction* insert_action = toolbar->addAction("插入图片");
  connect(insert_action, &QAction::triggered, this, &MainWindow::OnInsertImages);
}

void MainWindow::CreateCentralWidget() {
  QWidget* central_widget = new QWidget(this);
  QVBoxLayout* main_layout = new QVBoxLayout(central_widget);

  // 文件选择区域
  QGroupBox* file_group = new QGroupBox("Excel文件", central_widget);
  QHBoxLayout* file_layout = new QHBoxLayout(file_group);

  file_path_edit_ = new QLineEdit(file_group);
  file_path_edit_->setPlaceholderText("选择要操作的Excel文件...");
  file_path_edit_->setReadOnly(true);

  open_file_btn_ = new QPushButton("浏览...", file_group);
  connect(open_file_btn_, &QPushButton::clicked,
          this, &MainWindow::OnOpenFile);

  file_layout->addWidget(new QLabel("文件路径:", file_group));
  file_layout->addWidget(file_path_edit_, 1);
  file_layout->addWidget(open_file_btn_);

  main_layout->addWidget(file_group);

  // 单元格地址区域
  QGroupBox* cell_group = new QGroupBox("目标单元格", central_widget);
  QHBoxLayout* cell_layout = new QHBoxLayout(cell_group);

  cell_address_edit_ = new QLineEdit(cell_group);
  cell_address_edit_->setPlaceholderText("例如: A1, B2, C10...");
  cell_address_edit_->setText("A1");
  cell_address_edit_->setMaximumWidth(150);

  cell_layout->addWidget(new QLabel("起始单元格:", cell_group));
  cell_layout->addWidget(cell_address_edit_);
  cell_layout->addStretch();

  main_layout->addWidget(cell_group);

  // 插入模式选择
  mode_group_box_ = new QGroupBox("插入模式", central_widget);
  QVBoxLayout* mode_layout = new QVBoxLayout(mode_group_box_);

  mode_direct_edit_ = new QRadioButton("直接编辑模式（需关闭文件）", mode_group_box_);
  mode_clipboard_ = new QRadioButton("剪贴板辅助模式", mode_group_box_);
  mode_direct_edit_->setChecked(true);

  mode_layout->addWidget(mode_direct_edit_);
  mode_layout->addWidget(mode_clipboard_);

  main_layout->addWidget(mode_group_box_);

  // 图片列表区域
  QGroupBox* image_group = new QGroupBox("图片列表", central_widget);
  QVBoxLayout* image_layout = new QVBoxLayout(image_group);

  image_list_widget_ = new ImageListWidget(image_group);
  image_layout->addWidget(image_list_widget_);

  // 操作按钮
  QHBoxLayout* button_layout = new QHBoxLayout();

  paste_btn_ = new QPushButton("粘贴图片 (Ctrl+V)", image_group);
  add_files_btn_ = new QPushButton("添加文件", image_group);
  clear_all_btn_ = new QPushButton("清空所有", image_group);

  connect(paste_btn_, &QPushButton::clicked,
          this, &MainWindow::OnPasteFromClipboard);
  connect(add_files_btn_, &QPushButton::clicked,
          this, &MainWindow::OnAddImagesFromFiles);
  connect(clear_all_btn_, &QPushButton::clicked,
          this, &MainWindow::OnClearAllImages);

  button_layout->addWidget(paste_btn_);
  button_layout->addWidget(add_files_btn_);
  button_layout->addWidget(clear_all_btn_);
  button_layout->addStretch();

  image_layout->addLayout(button_layout);

  main_layout->addWidget(image_group, 1);

  // 插入按钮
  insert_images_btn_ = new QPushButton("插入图片到Excel", central_widget);
  insert_images_btn_->setMinimumHeight(40);
  insert_images_btn_->setStyleSheet(
      "QPushButton { font-size: 14pt; font-weight: bold; }");
  connect(insert_images_btn_, &QPushButton::clicked,
          this, &MainWindow::OnInsertImages);

  main_layout->addWidget(insert_images_btn_);

  central_widget->setLayout(main_layout);
  setCentralWidget(central_widget);
}

void MainWindow::CreateStatusBar() {
  status_label_ = new QLabel("就绪", this);
  statusBar()->addWidget(status_label_, 1);

  progress_bar_ = new QProgressBar(this);
  progress_bar_->setVisible(false);
  progress_bar_->setMaximumWidth(200);
  statusBar()->addPermanentWidget(progress_bar_);
}

void MainWindow::ConnectSignals() {
  // 剪贴板信号
  connect(clipboard_manager_, &ClipboardManager::ImageAvailable,
          this, &MainWindow::OnClipboardImageAvailable);

  // 图片管理器信号
  connect(image_manager_, &ImageManager::ImageCountChanged,
          this, &MainWindow::OnImageCountChanged);

  // 图片列表信号
  connect(image_list_widget_, &ImageListWidget::ImageAdded,
          [this](const QImage& image) {
            image_manager_->AddImage(image);
          });

  // Excel控制器信号
  connect(excel_controller_, &ExcelController::OperationProgress,
          this, &MainWindow::OnOperationProgress);
  connect(excel_controller_, &ExcelController::OperationCompleted,
          [this](bool success, const QString& message) {
            progress_bar_->setVisible(false);
            if (success) {
              QMessageBox::information(this, "成功", message);
              UpdateStatusBar("操作完成");
            } else {
              QMessageBox::warning(this, "失败", message);
              UpdateStatusBar("操作失败");
            }
          });
}

void MainWindow::OnOpenFile() {
  QString file_path = QFileDialog::getOpenFileName(
      this,
      "选择Excel文件",
      QString(),
      "Excel文件 (*.xlsx *.xls);;所有文件 (*.*)");

  if (file_path.isEmpty()) {
    return;
  }

  current_file_path_ = file_path;
  file_path_edit_->setText(file_path);

  // 添加到最近文件列表
  if (!recent_files_.contains(file_path)) {
    recent_files_.prepend(file_path);
    if (recent_files_.size() > 10) {
      recent_files_.removeLast();
    }
  }

  UpdateStatusBar("已选择文件: " + QFileInfo(file_path).fileName());
  UpdateUIState();

  qDebug() << "选择文件:" << file_path;
}

void MainWindow::OnPasteFromClipboard() {
  if (!clipboard_manager_->HasImage()) {
    QMessageBox::information(this, "提示", "剪贴板中没有图片");
    return;
  }

  QImage image = clipboard_manager_->GetImage();
  if (image.isNull()) {
    QMessageBox::warning(this, "错误", "无法获取剪贴板图片");
    return;
  }

  // 保存到临时文件
  QString temp_path = image_processor_->SaveImageToTemp(image, "PNG");
  if (temp_path.isEmpty()) {
    QMessageBox::warning(this, "错误", "保存临时文件失败");
    return;
  }

  ImageData image_data(image, temp_path);
  image_manager_->AddImage(image_data);
  image_list_widget_->AddImage(image);

  UpdateStatusBar(QString("已添加图片，共 %1 张")
                 .arg(image_manager_->GetImageCount()));
}

void MainWindow::OnAddImagesFromFiles() {
  QStringList file_paths = QFileDialog::getOpenFileNames(
      this,
      "选择图片文件",
      QString(),
      "图片文件 (*.png *.jpg *.jpeg *.bmp *.gif);;所有文件 (*.*)");

  if (file_paths.isEmpty()) {
    return;
  }

  int success_count = 0;
  for (const QString& file_path : file_paths) {
    QImage image(file_path);
    if (!image.isNull()) {
      ImageData image_data(image, file_path);
      image_manager_->AddImage(image_data);
      image_list_widget_->AddImage(image);
      success_count++;
    }
  }

  if (success_count > 0) {
    UpdateStatusBar(QString("已添加 %1 张图片，共 %2 张")
                   .arg(success_count)
                   .arg(image_manager_->GetImageCount()));
  } else {
    QMessageBox::warning(this, "错误", "没有成功添加任何图片");
  }
}

void MainWindow::OnClearAllImages() {
  image_list_widget_->RemoveAllImages();
  image_manager_->ClearAll();
  UpdateStatusBar("已清空所有图片");
}

void MainWindow::OnInsertImages() {
  // 验证输入
  if (current_file_path_.isEmpty()) {
    QMessageBox::warning(this, "错误", "请先选择Excel文件");
    return;
  }

  if (!QFile::exists(current_file_path_)) {
    QMessageBox::warning(this, "错误", "Excel文件不存在");
    return;
  }

  if (image_manager_->GetImageCount() == 0) {
    QMessageBox::warning(this, "错误", "请先添加要插入的图片");
    return;
  }

  QString cell_address = cell_address_edit_->text().trimmed().toUpper();
  if (!ValidateCellAddress(cell_address)) {
    QMessageBox::warning(this, "错误",
        "单元格地址格式无效\n请输入正确的格式，例如: A1, B2, C10");
    return;
  }

  if (mode_clipboard_->isChecked()) {
    OnClipboardModeInsert();
    return;
  }

  // 直接编辑模式
  if (CheckFileOpen(current_file_path_)) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "文件正在使用",
        "检测到文件正在被WPS使用\n需要先关闭文件才能继续操作\n\n是否继续？",
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) {
      return;
    }

    CloseWPSFile(current_file_path_);
  }

  // 准备图片路径列表
  QVector<QString> image_paths;
  QVector<ImageData> images = image_manager_->GetAllImages();

  for (ImageData& image_data : images) {
    if (image_data.temp_path.isEmpty()) {
      QString temp_path = image_processor_->SaveImageToTemp(
          image_data.image, "PNG");
      if (!temp_path.isEmpty()) {
        image_data.temp_path = temp_path;
        image_paths.append(temp_path);
      }
    } else {
      image_paths.append(image_data.temp_path);
    }
  }

  if (image_paths.isEmpty()) {
    QMessageBox::warning(this, "错误", "准备图片文件失败");
    return;
  }

  // 显示进度条
  progress_bar_->setVisible(true);
  progress_bar_->setMaximum(image_paths.size());
  progress_bar_->setValue(0);

  UpdateStatusBar("正在插入图片...");

  // 执行插入操作
  InsertConfig config;
  config.cell_address = cell_address;
  config.layout = "vertical";

  OperationResult result = excel_controller_->InsertImages(
      current_file_path_, image_paths, cell_address, config);

  progress_bar_->setVisible(false);

  if (result.success) {
    QMessageBox::information(this, "成功", result.message);
    UpdateStatusBar("插入完成");

    // 重新打开文件
    ReopenWPSFile(current_file_path_);
  } else {
    QMessageBox::critical(this, "失败",
        result.message + "\n\n" + result.error_detail);
    UpdateStatusBar("插入失败");
  }
}

void MainWindow::OnClipboardModeInsert() {
  QMessageBox::information(this, "剪贴板辅助模式",
      "将按顺序把每张图片放入剪贴板\n"
      "请在WPS中手动粘贴到对应的单元格\n\n"
      "点击确定开始...");

  QVector<ImageData> images = image_manager_->GetAllImages();

  for (int i = 0; i < images.size(); ++i) {
    clipboard_manager_->SetImage(images[i].image);

    QMessageBox::information(this, "剪贴板辅助",
        QString("已将第 %1/%2 张图片放入剪贴板\n"
                "请在WPS中按 Ctrl+V 粘贴\n\n"
                "粘贴完成后点击确定继续...")
        .arg(i + 1).arg(images.size()));
  }

  QMessageBox::information(this, "完成", "所有图片已处理完成");
}

void MainWindow::OnClipboardImageAvailable(const QImage& image) {
  if (clipboard_monitoring_enabled_) {
    image_list_widget_->AddImage(image);
    image_manager_->AddImage(image);
  }
}

void MainWindow::OnImageCountChanged(int count) {
  UpdateUIState();
  UpdateStatusBar(QString("图片列表: %1 张").arg(count));
}

void MainWindow::OnOperationProgress(int current, int total) {
  progress_bar_->setMaximum(total);
  progress_bar_->setValue(current);
}

void MainWindow::OnShowSettings() {
  QMessageBox::information(this, "设置", "设置功能开发中...");
}

void MainWindow::OnAbout() {
  QMessageBox::about(this, "关于",
      "<h2>WPS图片批量嵌入工具 v1.0</h2>"
      "<p>用于将图片批量插入到WPS Excel文件中</p>"
      "<p>技术栈: Qt 5.12 + C++ + Python</p>"
      "<p>Copyright © 2024</p>");
}

bool MainWindow::CheckFileOpen(const QString& file_path) {
  return excel_controller_->IsFileOpen(file_path);
}

void MainWindow::CloseWPSFile(const QString& file_path) {
  // 尝试通过进程关闭WPS
  UpdateStatusBar("正在关闭文件...");
  QMessageBox::information(this, "提示",
      "请手动保存并关闭WPS中的文件\n关闭后点击确定继续");
}

void MainWindow::ReopenWPSFile(const QString& file_path) {
  QMessageBox::StandardButton reply = QMessageBox::question(
      this,
      "打开文件",
      "是否打开Excel文件查看结果？",
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    QProcess::startDetached("cmd.exe", QStringList() << "/c" << "start"
                           << "" << QDir::toNativeSeparators(file_path));
  }
}

bool MainWindow::ValidateCellAddress(const QString& address) {
  if (address.isEmpty()) {
    return false;
  }

  QRegularExpression regex("^[A-Z]+\\d+$");
  return regex.match(address).hasMatch();
}

void MainWindow::UpdateUIState() {
  bool has_file = !current_file_path_.isEmpty();
  bool has_images = image_manager_->GetImageCount() > 0;

  insert_images_btn_->setEnabled(has_file && has_images);
}

void MainWindow::UpdateStatusBar(const QString& message) {
  status_label_->setText(message);
}

void MainWindow::LoadSettings() {
  QSettings settings("WPSImageTool", "WPSImageTool");

  // 加载窗口位置和大小
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("windowState").toByteArray());

  // 加载最近文件列表
  recent_files_ = settings.value("recentFiles").toStringList();

  // 加载默认单元格地址
  QString default_cell = settings.value("defaultCell", "A1").toString();
  cell_address_edit_->setText(default_cell);
}

void MainWindow::SaveSettings() {
  QSettings settings("WPSImageTool", "WPSImageTool");

  // 保存窗口位置和大小
  settings.setValue("geometry", saveGeometry());
  settings.setValue("windowState", saveState());

  // 保存最近文件列表
  settings.setValue("recentFiles", recent_files_);

  // 保存默认单元格地址
  settings.setValue("defaultCell", cell_address_edit_->text());
}

void MainWindow::OnRecentFileSelected() {
  // 待实现
}

}  // namespace WPSImageTool
