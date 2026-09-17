// Copyright 2024 WPSImageTool
// Python桥接实现

#include "python/PythonBridge.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

namespace WPSImageTool {

PythonBridge::PythonBridge(QObject* parent)
    : QObject(parent),
      initialized_(false),
      main_module_(nullptr) {
}

PythonBridge::~PythonBridge() {
  // 清理加载的模块
  for (auto it = loaded_modules_.begin(); it != loaded_modules_.end(); ++it) {
    Py_XDECREF(it.value());
  }
  loaded_modules_.clear();

  if (initialized_) {
    Py_Finalize();
    qDebug() << "Python解释器已关闭";
  }
}

bool PythonBridge::Initialize() {
  if (initialized_) {
    qDebug() << "Python解释器已初始化";
    return true;
  }

  // 设置Python路径
  QString app_dir = QCoreApplication::applicationDirPath();
  script_path_ = app_dir + QDir::separator() + "scripts";

  qDebug() << "Python脚本路径:" << script_path_;

  // 初始化Python解释器
  Py_Initialize();

  if (!Py_IsInitialized()) {
    SetLastError("Python解释器初始化失败");
    emit ErrorOccurred(last_error_);
    return false;
  }

  // 添加脚本路径到sys.path
  PyObject* sys_path = PySys_GetObject("path");
  if (sys_path) {
    PyObject* path = PyUnicode_FromString(script_path_.toUtf8().constData());
    PyList_Append(sys_path, path);
    Py_DECREF(path);
  }

  // 获取主模块
  main_module_ = PyImport_AddModule("__main__");
  if (!main_module_) {
    HandlePythonError();
    Py_Finalize();
    return false;
  }

  Py_INCREF(main_module_);
  initialized_ = true;

  qDebug() << "Python解释器初始化成功";
  emit LogMessage("Python解释器初始化成功");

  return true;
}

bool PythonBridge::LoadModule(const QString& module_name,
                             const QString& module_path) {
  if (!initialized_) {
    SetLastError("Python解释器未初始化");
    return false;
  }

  // 检查是否已加载
  if (loaded_modules_.contains(module_name)) {
    qDebug() << "模块已加载:" << module_name;
    return true;
  }

  // 导入模块
  PyObject* module = PyImport_ImportModule(module_name.toUtf8().constData());
  if (!module) {
    HandlePythonError();
    return false;
  }

  loaded_modules_[module_name] = module;

  qDebug() << "模块加载成功:" << module_name;
  emit LogMessage(QString("模块加载成功: %1").arg(module_name));

  return true;
}

QVariant PythonBridge::CallFunction(const QString& module_name,
                                   const QString& function_name,
                                   const QVariantList& args) {
  if (!initialized_) {
    SetLastError("Python解释器未初始化");
    return QVariant();
  }

  // 获取模块
  PyObject* module = loaded_modules_.value(module_name, nullptr);
  if (!module) {
    SetLastError(QString("模块未加载: %1").arg(module_name));
    return QVariant();
  }

  // 获取函数
  PyObject* func = PyObject_GetAttrString(module,
                                         function_name.toUtf8().constData());
  if (!func || !PyCallable_Check(func)) {
    Py_XDECREF(func);
    SetLastError(QString("函数不存在或不可调用: %1.%2")
                .arg(module_name).arg(function_name));
    return QVariant();
  }

  // 构建参数元组
  PyObject* py_args = PyTuple_New(args.size());
  for (int i = 0; i < args.size(); ++i) {
    PyObject* arg = ConvertQVariantToPython(args[i]);
    if (!arg) {
      Py_DECREF(py_args);
      Py_DECREF(func);
      SetLastError("参数转换失败");
      return QVariant();
    }
    PyTuple_SetItem(py_args, i, arg);
  }

  // 调用函数
  PyObject* result = PyObject_CallObject(func, py_args);

  Py_DECREF(py_args);
  Py_DECREF(func);

  if (!result) {
    HandlePythonError();
    return QVariant();
  }

  // 转换返回值
  QVariant ret_value = ConvertPythonToQVariant(result);
  Py_DECREF(result);

  qDebug() << "函数调用成功:" << module_name << "." << function_name;

  return ret_value;
}

PyObject* PythonBridge::ConvertQVariantToPython(const QVariant& value) {
  switch (value.type()) {
    case QVariant::Bool:
      return PyBool_FromLong(value.toBool() ? 1 : 0);

    case QVariant::Int:
    case QVariant::LongLong:
      return PyLong_FromLongLong(value.toLongLong());

    case QVariant::Double:
      return PyFloat_FromDouble(value.toDouble());

    case QVariant::String:
      return PyUnicode_FromString(value.toString().toUtf8().constData());

    case QVariant::StringList: {
      QStringList list = value.toStringList();
      PyObject* py_list = PyList_New(list.size());
      for (int i = 0; i < list.size(); ++i) {
        PyObject* item = PyUnicode_FromString(list[i].toUtf8().constData());
        PyList_SetItem(py_list, i, item);
      }
      return py_list;
    }

    case QVariant::List: {
      QVariantList list = value.toList();
      PyObject* py_list = PyList_New(list.size());
      for (int i = 0; i < list.size(); ++i) {
        PyObject* item = ConvertQVariantToPython(list[i]);
        if (!item) {
          Py_DECREF(py_list);
          return nullptr;
        }
        PyList_SetItem(py_list, i, item);
      }
      return py_list;
    }

    case QVariant::Map: {
      QVariantMap map = value.toMap();
      PyObject* py_dict = PyDict_New();
      for (auto it = map.begin(); it != map.end(); ++it) {
        PyObject* key = PyUnicode_FromString(it.key().toUtf8().constData());
        PyObject* val = ConvertQVariantToPython(it.value());
        if (!key || !val) {
          Py_XDECREF(key);
          Py_XDECREF(val);
          Py_DECREF(py_dict);
          return nullptr;
        }
        PyDict_SetItem(py_dict, key, val);
        Py_DECREF(key);
        Py_DECREF(val);
      }
      return py_dict;
    }

    default:
      Py_RETURN_NONE;
  }
}

QVariant PythonBridge::ConvertPythonToQVariant(PyObject* obj) {
  if (obj == Py_None) {
    return QVariant();
  }

  if (PyBool_Check(obj)) {
    return QVariant(obj == Py_True);
  }

  if (PyLong_Check(obj)) {
    return QVariant(PyLong_AsLongLong(obj));
  }

  if (PyFloat_Check(obj)) {
    return QVariant(PyFloat_AsDouble(obj));
  }

  if (PyUnicode_Check(obj)) {
    const char* str = PyUnicode_AsUTF8(obj);
    return QVariant(QString::fromUtf8(str));
  }

  if (PyList_Check(obj)) {
    QVariantList list;
    Py_ssize_t size = PyList_Size(obj);
    for (Py_ssize_t i = 0; i < size; ++i) {
      PyObject* item = PyList_GetItem(obj, i);
      list.append(ConvertPythonToQVariant(item));
    }
    return QVariant(list);
  }

  if (PyDict_Check(obj)) {
    QVariantMap map;
    PyObject* keys = PyDict_Keys(obj);
    Py_ssize_t size = PyList_Size(keys);
    for (Py_ssize_t i = 0; i < size; ++i) {
      PyObject* key = PyList_GetItem(keys, i);
      PyObject* val = PyDict_GetItem(obj, key);
      QString key_str = QString::fromUtf8(PyUnicode_AsUTF8(key));
      map[key_str] = ConvertPythonToQVariant(val);
    }
    Py_DECREF(keys);
    return QVariant(map);
  }

  return QVariant();
}

void PythonBridge::HandlePythonError() {
  PyObject* type = nullptr;
  PyObject* value = nullptr;
  PyObject* traceback = nullptr;

  PyErr_Fetch(&type, &value, &traceback);

  QString error_msg = "Python错误: ";

  if (value) {
    PyObject* str = PyObject_Str(value);
    if (str) {
      error_msg += QString::fromUtf8(PyUnicode_AsUTF8(str));
      Py_DECREF(str);
    }
  }

  Py_XDECREF(type);
  Py_XDECREF(value);
  Py_XDECREF(traceback);

  SetLastError(error_msg);
  qWarning() << error_msg;
  emit ErrorOccurred(error_msg);
}

void PythonBridge::SetLastError(const QString& error) {
  last_error_ = error;
}

}  // namespace WPSImageTool
