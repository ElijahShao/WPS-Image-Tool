// Copyright 2024 WPSImageTool
// Excel控制器模块 - 负责Excel文件操作

#ifndef EXCELCONTROLLER_H
#define EXCELCONTROLLER_H

#include <QObject>
#include <QString>
#include <QVector>
#include "DataStructures.h"
#include "python/PythonBridge.h"

namespace WPSImageTool {

class ExcelController : public QObject {
  Q_OBJECT

 public:
  explicit ExcelController(QObject* parent = nullptr);
  ~ExcelController() override;

  // 初始化
  bool Initialize();

  // 文件检查
  bool CheckFileExists(const QString& file_path) const;
  bool IsFileOpen(const QString& file_path);

  // 单元格信息
  CellInfo GetCellInfo(const QString& file_path,
                      const QString& cell_address);
  QSize GetCellSize(const QString& file_path,
                   const QString& cell_address);

  // 插入图片
  OperationResult InsertImages(const QString& file_path,
                              const QVector<QString>& image_paths,
                              const QString& start_cell,
                              const InsertConfig& config = InsertConfig());

  // 单张图片插入
  OperationResult InsertSingleImage(const QString& file_path,
                                   const QString& image_path,
                                   const QString& cell_address);

  // 批量插入（垂直布局）
  OperationResult InsertImagesVertical(const QString& file_path,
                                      const QVector<QString>& image_paths,
                                      const QString& start_cell);

  // 批量插入（水平布局）
  OperationResult InsertImagesHorizontal(const QString& file_path,
                                        const QVector<QString>& image_paths,
                                        const QString& start_cell);

  // 获取最后的错误信息
  QString GetLastError() const { return last_error_; }

 signals:
  void OperationProgress(int current, int total);
  void OperationCompleted(bool success, const QString& message);
  void ErrorOccurred(const QString& error);

 private:
  // 解析单元格地址（如"A1" -> row=1, col=1）
  bool ParseCellAddress(const QString& address, int& row, int& column) const;

  // 列字母转数字（A->1, B->2, ..., Z->26, AA->27）
  int ColumnLetterToNumber(const QString& column_letter) const;

  // 数字转列字母
  QString NumberToColumnLetter(int number) const;

  // 计算下一个单元格地址
  QString GetNextCellAddress(const QString& current_cell,
                            const QString& direction) const;

  PythonBridge* python_bridge_;
  QString last_error_;
  bool initialized_;
};

}  // namespace WPSImageTool

#endif  // EXCELCONTROLLER_H
