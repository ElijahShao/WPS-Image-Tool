#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Excel处理模块 - 使用openpyxl直接操作Excel文件
支持图片插入、单元格信息获取等操作
"""

import os
import sys
from pathlib import Path
from typing import Dict, List, Tuple, Optional

try:
    from openpyxl import load_workbook
    from openpyxl.drawing.image import Image as XLImage
    from openpyxl.utils import get_column_letter, column_index_from_string
    from PIL import Image
except ImportError as e:
    print(f"错误: 缺少必要的Python库: {e}", file=sys.stderr)
    print("请安装: pip install openpyxl Pillow", file=sys.stderr)
    sys.exit(1)


class ExcelHandler:
    """Excel文件操作处理器"""

    def __init__(self):
        self.workbook = None
        self.worksheet = None
        self.file_path = None

    def open_file(self, file_path: str) -> bool:
        """打开Excel文件"""
        try:
            if not os.path.exists(file_path):
                print(f"文件不存在: {file_path}", file=sys.stderr)
                return False

            self.file_path = file_path
            self.workbook = load_workbook(file_path)
            self.worksheet = self.workbook.active
            return True
        except Exception as e:
            print(f"打开文件失败: {e}", file=sys.stderr)
            return False

    def close_file(self):
        """关闭文件"""
        if self.workbook:
            self.workbook.close()
            self.workbook = None
            self.worksheet = None

    def save_file(self, output_path: Optional[str] = None) -> bool:
        """保存文件"""
        try:
            if not self.workbook:
                return False

            save_path = output_path or self.file_path
            self.workbook.save(save_path)
            return True
        except Exception as e:
            print(f"保存文件失败: {e}", file=sys.stderr)
            return False

    def parse_cell_address(self, cell_address: str) -> Tuple[int, int]:
        """
        解析单元格地址
        Args:
            cell_address: 单元格地址，如 "A1", "B2", "AA10"
        Returns:
            (row, column) 元组，从1开始
        """
        import re
        match = re.match(r'^([A-Z]+)(\d+)$', cell_address.upper())
        if not match:
            raise ValueError(f"无效的单元格地址: {cell_address}")

        column_letter = match.group(1)
        row_number = int(match.group(2))

        column_number = column_index_from_string(column_letter)

        return row_number, column_number

    def get_cell_info(self, file_path: str, cell_address: str) -> Dict:
        """
        获取单元格信息
        Args:
            file_path: Excel文件路径
            cell_address: 单元格地址
        Returns:
            包含单元格信息的字典
        """
        try:
            if not self.open_file(file_path):
                return {
                    "success": False,
                    "error": "无法打开文件"
                }

            row, col = self.parse_cell_address(cell_address)
            cell = self.worksheet.cell(row, col)

            # 获取列宽和行高（转换为像素）
            column_letter = get_column_letter(col)
            column_width = self.worksheet.column_dimensions[column_letter].width or 8.43
            row_height = self.worksheet.row_dimensions[row].height or 15

            # Excel单位转换为像素（近似）
            # 列宽单位是字符宽度，行高单位是磅
            width_pixels = int(column_width * 7)  # 近似转换
            height_pixels = int(row_height * 1.33)  # 磅转像素

            result = {
                "success": True,
                "address": cell_address,
                "row": row,
                "column": col,
                "width": width_pixels,
                "height": height_pixels,
                "x": 0,  # 暂不计算精确位置
                "y": 0
            }

            self.close_file()
            return result

        except Exception as e:
            return {
                "success": False,
                "error": str(e)
            }

    def insert_image(self, file_path: str, image_path: str,
                    cell_address: str) -> Dict:
        """
        在指定单元格插入图片
        Args:
            file_path: Excel文件路径
            image_path: 图片文件路径
            cell_address: 目标单元格地址
        Returns:
            操作结果字典
        """
        try:
            # 检查图片文件
            if not os.path.exists(image_path):
                return {
                    "success": False,
                    "message": "图片文件不存在",
                    "error": image_path
                }

            # 打开Excel文件
            if not self.open_file(file_path):
                return {
                    "success": False,
                    "message": "无法打开Excel文件",
                    "error": file_path
                }

            # 解析单元格地址
            row, col = self.parse_cell_address(cell_address)

            # 获取图片信息
            with Image.open(image_path) as img:
                img_width, img_height = img.size

            # 创建openpyxl图片对象
            xl_image = XLImage(image_path)

            # 调整图片大小以适应单元格（可选）
            # 这里使用原始大小，也可以根据单元格大小调整
            max_width = 200  # 最大宽度（像素）
            max_height = 150  # 最大高度（像素）

            if img_width > max_width or img_height > max_height:
                scale_w = max_width / img_width
                scale_h = max_height / img_height
                scale = min(scale_w, scale_h)

                xl_image.width = int(img_width * scale)
                xl_image.height = int(img_height * scale)

            # 将图片添加到工作表
            self.worksheet.add_image(xl_image, cell_address)

            # 保存文件
            if not self.save_file():
                return {
                    "success": False,
                    "message": "保存文件失败",
                    "error": "Save operation failed"
                }

            self.close_file()

            return {
                "success": True,
                "message": f"图片已成功插入到 {cell_address}",
                "error": ""
            }

        except Exception as e:
            return {
                "success": False,
                "message": "插入图片失败",
                "error": str(e)
            }

    def insert_images_batch(self, file_path: str, image_paths: List[str],
                          start_cell: str, layout: str = "vertical") -> Dict:
        """
        批量插入图片
        Args:
            file_path: Excel文件路径
            image_paths: 图片路径列表
            start_cell: 起始单元格
            layout: 布局方式 "vertical"(垂直) 或 "horizontal"(水平)
        Returns:
            操作结果字典
        """
        try:
            if not image_paths:
                return {
                    "success": False,
                    "message": "图片列表为空",
                    "inserted_count": 0
                }

            # 打开文件
            if not self.open_file(file_path):
                return {
                    "success": False,
                    "message": "无法打开Excel文件",
                    "inserted_count": 0
                }

            start_row, start_col = self.parse_cell_address(start_cell)
            inserted_count = 0

            # 根据布局方式插入图片
            for i, image_path in enumerate(image_paths):
                if not os.path.exists(image_path):
                    print(f"警告: 图片文件不存在: {image_path}", file=sys.stderr)
                    continue

                if layout == "vertical":
                    # 垂直布局：向下排列
                    current_row = start_row + i
                    current_col = start_col
                elif layout == "horizontal":
                    # 水平布局：向右排列
                    current_row = start_row
                    current_col = start_col + i
                else:
                    raise ValueError(f"不支持的布局方式: {layout}")

                # 生成单元格地址
                cell_address = f"{get_column_letter(current_col)}{current_row}"

                # 插入图片
                try:
                    with Image.open(image_path) as img:
                        img_width, img_height = img.size

                    xl_image = XLImage(image_path)

                    # 调整大小
                    max_width = 200
                    max_height = 150

                    if img_width > max_width or img_height > max_height:
                        scale_w = max_width / img_width
                        scale_h = max_height / img_height
                        scale = min(scale_w, scale_h)

                        xl_image.width = int(img_width * scale)
                        xl_image.height = int(img_height * scale)

                    self.worksheet.add_image(xl_image, cell_address)
                    inserted_count += 1

                except Exception as e:
                    print(f"插入图片失败 {image_path}: {e}", file=sys.stderr)
                    continue

            # 保存文件
            if not self.save_file():
                return {
                    "success": False,
                    "message": f"插入了 {inserted_count} 张图片，但保存失败",
                    "inserted_count": inserted_count
                }

            self.close_file()

            return {
                "success": True,
                "message": f"成功插入 {inserted_count} 张图片",
                "inserted_count": inserted_count
            }

        except Exception as e:
            return {
                "success": False,
                "message": f"批量插入失败: {str(e)}",
                "inserted_count": 0
            }


def is_file_open(file_path: str) -> bool:
    """
    检查文件是否被打开
    在Windows上，尝试以独占模式打开文件来检测
    """
    if not os.path.exists(file_path):
        return False

    try:
        # 尝试以独占读写模式打开
        with open(file_path, 'r+b') as f:
            pass
        return False
    except (IOError, PermissionError):
        return True


# 导出的函数供C++调用

def get_cell_info(file_path: str, cell_address: str) -> Dict:
    """获取单元格信息"""
    handler = ExcelHandler()
    return handler.get_cell_info(file_path, cell_address)


def insert_image(file_path: str, image_path: str, cell_address: str) -> Dict:
    """插入单张图片"""
    handler = ExcelHandler()
    return handler.insert_image(file_path, image_path, cell_address)


def insert_images_batch(file_path: str, image_paths: List[str],
                       start_cell: str, layout: str = "vertical") -> Dict:
    """批量插入图片"""
    handler = ExcelHandler()
    return handler.insert_images_batch(file_path, image_paths, start_cell, layout)


# 测试代码
if __name__ == "__main__":
    print("Excel处理模块测试")

    # 测试解析单元格地址
    handler = ExcelHandler()
    test_addresses = ["A1", "B2", "AA10", "Z100"]

    for addr in test_addresses:
        try:
            row, col = handler.parse_cell_address(addr)
            print(f"{addr} -> 行:{row}, 列:{col}")
        except ValueError as e:
            print(f"{addr} -> 错误: {e}")
