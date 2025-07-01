```markdown
# 商场管理系统 MallManagementSystem

一个基于 C++、JSON 持久化与纯文本报表导出的控制台应用，用于模拟商场中的商品、员工、会员及销售管理。

## 功能概览

1. **商品管理**  
   - 添加 / 修改 / 删除 商品  
   - 模糊搜索（名称/类别）  
   - 按价格 / 库存 / 过期日期 排序  
2. **员工管理**  
   - 添加 / 修改 / 删除 员工  
   - 模糊搜索（姓名/职位）  
   - 按入职日期 排序  
3. **会员管理**  
   - 添加 / 修改 / 删除 会员  
   - 模糊搜索（姓名/电话）  
4. **销售管理**  
   - 选择商品、指定员工、可选会员购物  
   - 支持积分抵扣、自动计算折后金额、累积积分  
   - 实时更新库存  
   - 生成并导出收据（`receipt_<ID>.txt`）  
5. **报表中心**  
   - 导出指定日期区间的销售报表（`sales_report_<start>_to_<end>.txt`）  
   - 导出库存预警报表（`stock_warning_report_<date>.txt`）  
6. **日志与备份**  
   - 操作写入 `log.txt`  
   - 自动创建 `.tmp`、`.bak` 文件以保证数据安全  

## 依赖

- C++17 或更高  
- [nlohmann/json](https://github.com/nlohmann/json) —— 单文件 `json.hpp`  
- 标准库：`<iostream>`, `<vector>`, `<string>`, `<fstream>`, `<sstream>`, `<ctime>`, `<iomanip>`, `<algorithm>`, `<map>`, `<cstdlib>`, `<cstdio>`

## 文件结构


/MallManagementSystem
│
├─ main.cpp           # 主程序，包含所有类与逻辑
├─ json.hpp           # nlohmann/json 库
├─ products.json      # 商品数据文件（启动时自动创建或加载）
├─ employees.json     # 员工数据文件
├─ members.json       # 会员数据文件
├─ sales.json         # 销售记录文件
├─ log.txt            # 操作日志
├─ \*.bak, \*.tmp       # 自动备份与临时文件
└─ README.md          # 本说明文档

## 快速开始

1. **准备**  
   - 确保 `main.cpp` 与 `json.hpp` 在同一目录。  
   - 使用支持 C++17 的编译器（如 `g++`、`clang++`、Visual Studio 2022）。

2. **编译**  
   ```bash
   g++ -std=c++17 main.cpp -o MallManagementSystem
````

3. **运行**

   ```bash
   ./MallManagementSystem
   ```

4. **首次启动**

   * 程序会自动创建 `*.json` 数据文件和 `log.txt`。
   * 在主菜单中按提示进行操作。

## 使用说明

* **在任意菜单**，输入编号并回车。
* 输入 `q` 或 `0` 可返回上一层或退出系统。
* 日期输入支持 `YYYY MM DD` 或 `YYYY-MM-DD` 格式。

## 注意事项

* 如遇 JSON 文件损坏，程序会尝试从 `.bak` 备份恢复，或询问是否用空数据重建。
* 删除操作会有二次确认，防止误删。
* 日志文件 `log.txt` 会记录所有关键操作及异常。

---

```
