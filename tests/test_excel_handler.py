#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试脚本 - 测试Excel处理模块
"""

import sys
import os

# 添加scripts目录到Python路径
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'scripts'))

from excel_handler import ExcelHandler, get_cell_info, insert_image


def test_cell_parsing():
    """测试单元格地址解析"""
    print("=" * 50)
    print("测试单元格地址解析")
    print("=" * 50)

    handler = ExcelHandler()
    test_cases = [
        ("A1", (1, 1)),
        ("B2", (2, 2)),
        ("Z26", (26, 26)),
        ("AA1", (1, 27)),
        ("AB10", (10, 28)),
    ]

    for address, expected in test_cases:
        try:
            result = handler.parse_cell_address(address)
            status = "✓" if result == expected else "✗"
            print(f"{status} {address} -> {result} (期望: {expected})")
        except Exception as e:
            print(f"✗ {address} -> 错误: {e}")

    print()


def test_invalid_addresses():
    """测试无效的单元格地址"""
    print("=" * 50)
    print("测试无效单元格地址")
    print("=" * 50)

    handler = ExcelHandler()
    invalid_cases = ["1A", "a1", "123", "ABC", ""]

    for address in invalid_cases:
        try:
            result = handler.parse_cell_address(address)
            print(f"✗ {address} -> 应该失败但返回了: {result}")
        except ValueError:
            print(f"✓ {address} -> 正确拒绝")

    print()


def test_file_operations():
    """测试文件操作"""
    print("=" * 50)
    print("测试文件操作")
    print("=" * 50)

    # 创建测试Excel文件
    test_file = "test_workbook.xlsx"

    try:
        from openpyxl import Workbook

        # 创建测试文件
        wb = Workbook()
        ws = wb.active
        ws['A1'] = "测试单元格"
        wb.save(test_file)
        print(f"✓ 创建测试文件: {test_file}")

        # 测试打开文件
        handler = ExcelHandler()
        if handler.open_file(test_file):
            print(f"✓ 成功打开文件")
            handler.close_file()
        else:
            print(f"✗ 打开文件失败")

        # 测试获取单元格信息
        info = get_cell_info(test_file, "A1")
        if info.get("success"):
            print(f"✓ 获取单元格信息成功: {info}")
        else:
            print(f"✗ 获取单元格信息失败: {info.get('error')}")

        # 清理测试文件
        os.remove(test_file)
        print(f"✓ 清理测试文件")

    except ImportError:
        print("✗ 缺少openpyxl库，跳过文件操作测试")
    except Exception as e:
        print(f"✗ 测试失败: {e}")
        if os.path.exists(test_file):
            os.remove(test_file)

    print()


def main():
    """运行所有测试"""
    print("\n" + "=" * 50)
    print("Excel处理模块测试套件")
    print("=" * 50 + "\n")

    test_cell_parsing()
    test_invalid_addresses()
    test_file_operations()

    print("=" * 50)
    print("测试完成")
    print("=" * 50)


if __name__ == "__main__":
    main()
