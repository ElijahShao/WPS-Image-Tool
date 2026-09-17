// Copyright 2024 WPSImageTool
// Python桥接模块 - 负责C++与Python交互

#ifndef PYTHONBRIDGE_H
#define PYTHONBRIDGE_H

#include <Python.h>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>
#include <QMap>

namespace WPSImageTool {

class PythonBridge : public QObject {
  Q_OBJECT

 public:
  explicit PythonBridge(QObject* parent = nullptr);
  ~PythonBridge() override;

  // 初始化Python解释器
  bool Initialize();

  // 加载Python脚本模块
  bool LoadModule(const QString& module_name, const QString& module_path);

  // 调用Python函数
  QVariant CallFunction(const QString& module_name,
                       const QString& function_name,
                       const QVariantList& args = QVariantList());

  // 获取最后的错误信息
  QString GetLastError() const { return last_error_; }

  // 检查是否已初始化
  bool IsInitialized() const { return initialized_; }

 signals:
  void ErrorOccurred(const QString& error);
  void LogMessage(const QString& message);

 private:
  // 类型转换辅助函数
  PyObject* ConvertQVariantToPython(const QVariant& value);
  QVariant ConvertPythonToQVariant(PyObject* obj);

  // 错误处理
  void HandlePythonError();
  void SetLastError(const QString& error);

  // 成员变量
  bool initialized_;
  PyObject* main_module_;
  QMap<QString, PyObject*> loaded_modules_;
  QString last_error_;
  QString script_path_;
};

}  // namespace WPSImageTool

#endif  // PYTHONBRIDGE_H
