// Copyright 2024 WPSImageTool
// Excel控制器实现

#include "core/ExcelController.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QRegularExpression>

namespace WPSImageTool {

ExcelController::ExcelController(QObject* parent)
    : QObject(parent),
      python_bridge_(nullptr),
      initialized_(false) {
  python_bridge_ = new PythonBridge(this);

  connect(python_bridge_, &PythonBridge::ErrorOccurred,
          this, &ExcelController::ErrorOccurred);
}

ExcelController::~ExcelController() {
  // PythonBridge会被Qt的父子关系自动删除
}

bool ExcelController::Initialize() {
  if (initialized_) {
    return true;
  }

  // 初始化Python桥接
  if (!python_bridge_->Initialize()) {
    last_error_ = python_bridge_->GetLastError();
    return false;
  }

  // 加载Excel操作模块
  if (!python_bridge_->LoadModule("excel_handler", "")) {
    last_error_ = "加载Excel处理模块失败";
    return false;
  }

  initialized_ = true;
  qDebug() << "ExcelController初始化成功";
  return true;
}

bool ExcelController::CheckFileExists(const QString& file_path) const {
  QFileInfo file_info(file_path);
  return file_info.exists() && file_info.isFile();
}

bool ExcelController::IsFileOpen(const QString& file_path) {
  if (!initialized_ && !Initialize()) {
    return false;
  }

  QVariantList args;
  args << file_path;

  QVariant result = python_bridge_->CallFunction("excel_handler",
                                                "is_file_open",
                                                args);

  return result.toBool();
}

CellInfo ExcelController::GetCellInfo(const QString& file_path,
                                     const QString& cell_address) {
  CellInfo info;

  if (!initialized_ && !Initialize()) {
    return info;
  }

  int row = 0;
  int column = 0;
  if (!ParseCellAddress(cell_address, row, column)) {
    last_error_ = "单元格地址格式无效: " + cell_address;
    return info;
  }

  info.address = cell_address;
  info.row = row;
  info.column = column;

  // 调用Python获取单元格尺寸
  QVariantList args;
  args << file_path << cell_address;

  QVariant result = python_bridge_->CallFunction("excel_handler",
                                                "get_cell_info",
                                                args);

  if (result.isValid() && result.type() == QVariant::Map) {
    QVariantMap map = result.toMap();
    info.size = QSize(map["width"].toInt(), map["height"].toInt());
    info.position = QPoint(map["x"].toInt(), map["y"].toInt());
  }

  return info;
}

QSize ExcelController::GetCellSize(const QString& file_path,
                                  const QString& cell_address) {
  CellInfo info = GetCellInfo(file_path, cell_address);
  return info.size;
}

OperationResult ExcelController::InsertImages(
    const QString& file_path,
    const QVector<QString>& image_paths,
    const QString& start_cell,
    const InsertConfig& config) {

  if (!initialized_ && !Initialize()) {
    return OperationResult::Failure("控制器未初始化", last_error_);
  }

  if (!CheckFileExists(file_path)) {
    return OperationResult::Failure("Excel文件不存在: " + file_path);
  }

  if (image_paths.isEmpty()) {
    return OperationResult::Failure("没有要插入的图片");
  }

  // 检查文件是否打开
  if (IsFileOpen(file_path)) {
    return OperationResult::Failure(
        "文件正在被使用，请先关闭文件: " + file_path);
  }

  // 根据布局方式选择插入方法
  if (config.layout == "vertical") {
    return InsertImagesVertical(file_path, image_paths, start_cell);
  } else if (config.layout == "horizontal") {
    return InsertImagesHorizontal(file_path, image_paths, start_cell);
  } else {
    return OperationResult::Failure("不支持的布局方式: " + config.layout);
  }
}

OperationResult ExcelController::InsertSingleImage(
    const QString& file_path,
    const QString& image_path,
    const QString& cell_address) {

  if (!initialized_ && !Initialize()) {
    return OperationResult::Failure("控制器未初始化", last_error_);
  }

  QVariantList args;
  args << file_path << image_path << cell_address;

  QVariant result = python_bridge_->CallFunction("excel_handler",
                                                "insert_image",
                                                args);

  if (result.isValid() && result.type() == QVariant::Map) {
    QVariantMap map = result.toMap();
    bool success = map["success"].toBool();
    QString message = map["message"].toString();

    if (success) {
      return OperationResult::Success(message);
    } else {
      return OperationResult::Failure(message, map["error"].toString());
    }
  }

  return OperationResult::Failure("插入图片失败");
}

OperationResult ExcelController::InsertImagesVertical(
    const QString& file_path,
    const QVector<QString>& image_paths,
    const QString& start_cell) {

  if (image_paths.isEmpty()) {
    return OperationResult::Failure("图片列表为空");
  }

  int total = image_paths.size();
  int current = 0;
  QString current_cell = start_cell;

  for (const QString& image_path : image_paths) {
    emit OperationProgress(++current, total);

    OperationResult result = InsertSingleImage(file_path, image_path,
                                              current_cell);

    if (!result.success) {
      QString error_msg = QString("第%1张图片插入失败: %2")
                         .arg(current).arg(result.message);
      emit OperationCompleted(false, error_msg);
      return OperationResult::Failure(error_msg, result.error_detail);
    }

    // 移动到下一行
    current_cell = GetNextCellAddress(current_cell, "down");
  }

  emit OperationCompleted(true, QString("成功插入%1张图片").arg(total));
  return OperationResult::Success(QString("成功插入%1张图片").arg(total));
}

OperationResult ExcelController::InsertImagesHorizontal(
    const QString& file_path,
    const QVector<QString>& image_paths,
    const QString& start_cell) {

  if (image_paths.isEmpty()) {
    return OperationResult::Failure("图片列表为空");
  }

  int total = image_paths.size();
  int current = 0;
  QString current_cell = start_cell;

  for (const QString& image_path : image_paths) {
    emit OperationProgress(++current, total);

    OperationResult result = InsertSingleImage(file_path, image_path,
                                              current_cell);

    if (!result.success) {
      QString error_msg = QString("第%1张图片插入失败: %2")
                         .arg(current).arg(result.message);
      emit OperationCompleted(false, error_msg);
      return OperationResult::Failure(error_msg, result.error_detail);
    }

    // 移动到下一列
    current_cell = GetNextCellAddress(current_cell, "right");
  }

  emit OperationCompleted(true, QString("成功插入%1张图片").arg(total));
  return OperationResult::Success(QString("成功插入%1张图片").arg(total));
}

bool ExcelController::ParseCellAddress(const QString& address,
                                      int& row,
                                      int& column) const {
  // 匹配单元格地址格式：A1, B2, AA10等
  QRegularExpression regex("^([A-Z]+)(\\d+)$");
  QRegularExpressionMatch match = regex.match(address.toUpper());

  if (!match.hasMatch()) {
    return false;
  }

  QString column_letter = match.captured(1);
  QString row_number = match.captured(2);

  column = ColumnLetterToNumber(column_letter);
  row = row_number.toInt();

  return row > 0 && column > 0;
}

int ExcelController::ColumnLetterToNumber(const QString& column_letter) const {
  int result = 0;
  int length = column_letter.length();

  for (int i = 0; i < length; ++i) {
    result = result * 26 + (column_letter[i].unicode() - 'A' + 1);
  }

  return result;
}

QString ExcelController::NumberToColumnLetter(int number) const {
  QString result;

  while (number > 0) {
    int remainder = (number - 1) % 26;
    result = QChar('A' + remainder) + result;
    number = (number - 1) / 26;
  }

  return result;
}

QString ExcelController::GetNextCellAddress(const QString& current_cell,
                                           const QString& direction) const {
  int row = 0;
  int column = 0;

  if (!ParseCellAddress(current_cell, row, column)) {
    return current_cell;
  }

  if (direction == "down") {
    row++;
  } else if (direction == "up" && row > 1) {
    row--;
  } else if (direction == "right") {
    column++;
  } else if (direction == "left" && column > 1) {
    column--;
  }

  return NumberToColumnLetter(column) + QString::number(row);
}

}  // namespace WPSImageTool
