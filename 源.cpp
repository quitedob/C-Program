#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <limits>
#include <cstdio>    // for std::rename
#include <map>       // for std::map
#include <cstdlib>   // for system()

// 引入 nlohmann/json 库，请确保 json.hpp 文件与本文件在同一目录
#include "json.hpp"

// 为 nlohmann::json 创建一个更短的别名，方便使用
using json = nlohmann::json;
using namespace std;

// ========== 数据结构定义 (Date 提前) ==========
struct Date {
    int year, month, day;
    Date() : year(0), month(0), day(0) {}
    Date(int y, int m, int d) : year(y), month(m), day(d) {}

    // 简化注释：将日期转换为 "YYYY-MM-DD" 格式的字符串
    string toString() const {
        ostringstream oss;
        oss << year << "-" << setw(2) << setfill('0') << month << "-" << setw(2) << setfill('0') << day;
        return oss.str();
    }

    // 简化注释：获取当前系统日期
    static Date currentDate() {
        time_t now = time(0);
        tm* lt = localtime(&now);
        return Date(1900 + lt->tm_year, 1 + lt->tm_mon, lt->tm_mday);
    }

    // 简化注释：重载小于等于运算符，用于日期比较
    bool operator<=(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day <= other.day;
    }

    // 简化注释：重载大于等于运算符，用于日期比较
    bool operator>=(const Date& other) const {
        if (year != other.year) return year > other.year;
        if (month != other.month) return month > other.month;
        return day >= other.day;
    }

    // 简化注释：为 nlohmann/json 库提供序列化/反序列化支持
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Date, year, month, day)
};


// ========== 辅助工具函数 ==========

// 简化注释：向 log.txt 文件记录一条带时间戳的日志
void logAction(const string& message) {
    ofstream logfile("log.txt", ios::app);
    if (logfile.is_open()) {
        time_t now = time(0);
        char timestamp[30];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        logfile << "[" << timestamp << "] " << message << endl;
    }
}

// 简化注释：清空控制台屏幕
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// 简化注释：安全地读取一行字符串，可以处理空输入
string readString(const string& prompt, bool allowEmpty = false) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        if (!allowEmpty && value.empty()) {
            cout << "输入不能为空，请重新输入。\n";
        }
        else {
            return value;
        }
    }
}

// 简化注释：安全地读取一个指定范围内的整数
int readInt(const string& prompt, int minVal = numeric_limits<int>::min(), int maxVal = numeric_limits<int>::max()) {
    int value;
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        stringstream ss(input);
        if (ss >> value && ss.eof() && value >= minVal && value <= maxVal) {
            return value;
        }
        cout << "输入无效，请输入一个介于 " << minVal << " 和 " << maxVal << " 之间的整数。\n";
    }
}

// 简化注释：安全地读取一个指定范围内的浮点数
double readDouble(const string& prompt, double minVal = numeric_limits<double>::lowest(), double maxVal = numeric_limits<double>::max()) {
    double value;
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        stringstream ss(input);
        if (ss >> value && ss.eof() && value >= minVal && value <= maxVal) {
            return value;
        }
        cout << "输入无效，请输入一个介于 " << minVal << " 和 " << maxVal << " 之间的数字。\n";
    }
}

// 简化注释：检查是否为闰年
bool isLeap(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// 简化注释：安全地读取并验证一个合法的日期
Date readDate(const string& prompt) {
    int year, month, day;
    int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);

        // 尝试解析 "YYYY-MM-DD" 或 "YYYY MM DD" 格式
        char delimiter = ' ';
        if (line.find('-') != string::npos) {
            delimiter = '-';
        }
        for (char& c : line) {
            if (c == delimiter) {
                c = ' ';
            }
        }

        stringstream ss(line);
        if (ss >> year >> month >> day && ss.eof()) {
            if (month >= 1 && month <= 12) {
                if (isLeap(year)) daysInMonth[2] = 29; else daysInMonth[2] = 28;
                if (day >= 1 && day <= daysInMonth[month]) {
                    return Date(year, month, day);
                }
            }
        }
        cout << "无效的日期格式或日期不存在，请输入 '年 月 日' (例如: 2025 7 1): \n";
    }
}


// ========== 其余数据结构定义 ==========

class Product {
private:
    int id; string name; string category; double price; int stock; Date expirationDate;
public:
    Product() : id(0), price(0.0), stock(0) {}
    Product(int i, string n, string c, double p, int s, Date exp) : id(i), name(n), category(c), price(p), stock(s), expirationDate(exp) {}
    int getId() const { return id; }
    string getName() const { return name; }
    string getCategory() const { return category; }
    double getPrice() const { return price; }
    int getStock() const { return stock; }
    Date getExpirationDate() const { return expirationDate; }
    void setPrice(double p) { price = p; }
    void setStock(int s) { stock = s; }
    void addStock(int quantity) { stock += quantity; }
    void reduceStock(int quantity) { if (stock >= quantity) stock -= quantity; }
    void display() const {
        cout << left << setw(6) << id << setw(25) << name << setw(18) << category << setw(12) << fixed << setprecision(2) << price << setw(8) << stock << setw(12) << expirationDate.toString() << endl;
    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Product, id, name, category, price, stock, expirationDate)
};

class Employee {
private:
    int id; string name; string position; double salary; Date hireDate;
public:
    Employee() : id(0), salary(0.0) {}
    Employee(int i, string n, string p, double s, Date d) :id(i), name(n), position(p), salary(s), hireDate(d) {}
    int getId() const { return id; }
    string getName() const { return name; }
    string getPosition() const { return position; }
    double getSalary() const { return salary; }
    Date getHireDate() const { return hireDate; }
    void setPosition(string pos) { position = pos; }
    void setSalary(double s) { salary = s; }
    void display() const {
        cout << left << setw(6) << id << setw(20) << name << setw(20) << position << setw(12) << fixed << setprecision(2) << salary << setw(12) << hireDate.toString() << endl;
    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Employee, id, name, position, salary, hireDate)
};

class Member {
private:
    int id; string name; string phone; int points; Date registrationDate;
public:
    Member() : id(0), points(0) {}
    Member(int i, string n, string ph, int pt, Date d) :id(i), name(n), phone(ph), points(pt), registrationDate(d) {}
    int getId() const { return id; }
    string getName() const { return name; }
    string getPhone() const { return phone; }
    int getPoints() const { return points; }
    void setPhone(string ph) { phone = ph; }
    void addPoints(int p) { points += p; }
    void usePoints(int p) { if (points >= p) { points -= p; } }
    void display() const {
        cout << left << setw(6) << id << setw(20) << name << setw(15) << phone << setw(8) << points << setw(12) << registrationDate.toString() << endl;
    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Member, id, name, phone, points, registrationDate)
};

class SaleItem {
public:
    int productId; int quantity; double priceAtSale;
    SaleItem() : productId(0), quantity(0), priceAtSale(0.0) {}
    SaleItem(int pid, int q, double p) :productId(pid), quantity(q), priceAtSale(p) {}
    int getProductId()const { return productId; }
    int getQuantity()const { return quantity; }
    double getSubtotal() const { return priceAtSale * quantity; }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SaleItem, productId, quantity, priceAtSale)
};

class SaleRecord {
public:
    int id; Date saleDate; int memberId; int employeeId; vector<SaleItem> items; double totalAmount; double discount;
    SaleRecord() :id(0), memberId(0), employeeId(0), totalAmount(0), discount(0) {}
    double getFinalAmount() const { return totalAmount * (1 - discount); }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SaleRecord, id, saleDate, memberId, employeeId, items, totalAmount, discount)
};

// ========== 系统核心类 ==========
class MallManagementSystem {
private:
    vector<Product> products;
    vector<Employee> employees;
    vector<Member> members;
    vector<SaleRecord> sales;
    int nextProductId = 1, nextEmployeeId = 1, nextMemberId = 1, nextSaleId = 1;

    // --- 持久化与备份 ---
    template<typename T>
    void saveData(const string& filename, const vector<T>& data) {
        string tempFilename = filename + ".tmp";
        string backupFilename = filename + ".bak";

        // 写入临时文件
        ofstream file(tempFilename);
        if (!file.is_open()) {
            logAction("严重错误: 无法创建临时文件 " + tempFilename + " 进行写入。");
            return;
        }
        try {
            json j = data;
            file << j.dump(4);
            file.close();

            // 创建备份
            if (ifstream(filename).good()) {
                std::rename(filename.c_str(), backupFilename.c_str());
            }
            // 重命名临时文件为正式文件
            std::rename(tempFilename.c_str(), filename.c_str());
            logAction("数据成功保存到 " + filename);
        }
        catch (const exception& e) {
            logAction("严重错误: 写入JSON到 " + tempFilename + " 时发生异常: " + e.what());
            file.close();
            std::remove(tempFilename.c_str()); // 删除错误的临时文件
        }
    }

    template<typename T>
    void loadData(const string& filename, vector<T>& data) {
        ifstream file(filename);
        if (!file.good()) {
            logAction("信息: 文件 " + filename + " 不存在，将创建新文件。");
            saveData(filename, data); return;
        }
        try {
            json j; file >> j;
            if (!j.is_null() && j.is_array()) data = j.get<vector<T>>();
            logAction("数据成功从 " + filename + " 加载。");
        }
        catch (const json::parse_error& e) {
            logAction("错误: JSON文件 '" + filename + "' 格式损坏！详情: " + e.what());
            string backupFilename = filename + ".bak";
            if (ifstream(backupFilename).good()) {
                cout << "错误: 主文件 " << filename << " 损坏。检测到备份文件 " << backupFilename << "。\n";
                cout << "是否从备份恢复? (y/n): ";
                char choice; cin >> choice; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (choice == 'y' || choice == 'Y') {
                    std::remove(filename.c_str());
                    std::rename(backupFilename.c_str(), filename.c_str());
                    logAction("已从 " + backupFilename + " 恢复数据。");
                    loadData(filename, data); // 重新加载恢复的文件
                    return;
                }
            }
            cout << "无法从备份恢复。是否要用空数据覆盖此文件? (y/n): ";
            char choice; cin >> choice; cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (choice == 'y' || choice == 'Y') {
                data.clear(); saveData(filename, data);
                logAction("文件 " + filename + " 已被用户选择重建。");
            }
            else {
                logAction("用户选择不重建文件，程序退出。"); exit(1);
            }
        }
    }

    void updateNextIds() {
        if (!products.empty()) nextProductId = max_element(products.begin(), products.end(), [](const Product& a, const Product& b) { return a.getId() < b.getId(); })->getId() + 1;
        if (!employees.empty()) nextEmployeeId = max_element(employees.begin(), employees.end(), [](const Employee& a, const Employee& b) { return a.getId() < b.getId(); })->getId() + 1;
        if (!members.empty()) nextMemberId = max_element(members.begin(), members.end(), [](const Member& a, const Member& b) { return a.getId() < b.getId(); })->getId() + 1;
        if (!sales.empty()) nextSaleId = max_element(sales.begin(), sales.end(), [](const SaleRecord& a, const SaleRecord& b) { return a.id < b.id; })->id + 1;
    }

    // --- 查找工具函数 ---
    Product* findProduct(int id) { for (auto& p : products) if (p.getId() == id) return &p; return nullptr; }
    Employee* findEmployee(int id) { for (auto& e : employees) if (e.getId() == id) return &e; return nullptr; }
    Member* findMember(int id) { for (auto& m : members) if (m.getId() == id) return &m; return nullptr; }
    const Product* findProduct(int id) const { for (const auto& p : products) if (p.getId() == id) return &p; return nullptr; }
    const Employee* findEmployee(int id) const { for (const auto& e : employees) if (e.getId() == id) return &e; return nullptr; }
    const Member* findMember(int id) const { for (const auto& m : members) if (m.getId() == id) return &m; return nullptr; }

    // --- 列表显示函数 ---
    void displayProducts(const vector<Product>& prods) const {
        if (prods.empty()) { cout << "没有找到符合条件的商品记录。\n"; return; }
        cout << "\n--- 商品列表 ---\n";
        cout << left << setw(6) << "ID" << setw(25) << "名称" << setw(18) << "类别" << setw(12) << "价格" << setw(8) << "库存" << setw(12) << "过期日期" << endl;
        cout << string(91, '-') << endl;
        for (const auto& p : prods) p.display();
    }
    void displayEmployees(const vector<Employee>& emps) const {
        if (emps.empty()) { cout << "没有找到符合条件的员工记录。\n"; return; }
        cout << "\n--- 员工列表 ---\n";
        cout << left << setw(6) << "ID" << setw(20) << "姓名" << setw(20) << "职位" << setw(12) << "工资" << setw(12) << "入职日期" << endl;
        cout << string(80, '-') << endl;
        for (const auto& e : emps) e.display();
    }
    void displayMembers(const vector<Member>& mems) const {
        if (mems.empty()) { cout << "没有找到符合条件的会员记录。\n"; return; }
        cout << "\n--- 会员列表 ---\n";
        cout << left << setw(6) << "ID" << setw(20) << "姓名" << setw(15) << "电话" << setw(8) << "积分" << setw(12) << "注册日期" << endl;
        cout << string(71, '-') << endl;
        for (const auto& m : mems) m.display();
    }

    // --- 收据打印与导出 ---
    void printReceipt(const SaleRecord& sale) const {
        ostringstream receiptContent;
        const Employee* emp = findEmployee(sale.employeeId);
        const Member* mem = findMember(sale.memberId);
        receiptContent << "\n\n================= 销售收据 =================\n";
        receiptContent << "销售单号: " << sale.id << "\n";
        receiptContent << "销售日期: " << sale.saleDate.toString() << "\n";
        receiptContent << "收 银 员: " << (emp ? emp->getName() : "N/A") << " (ID: " << sale.employeeId << ")\n";
        if (mem) receiptContent << "会    员: " << mem->getName() << " (ID: " << sale.memberId << ")\n";
        receiptContent << "----------------------------------------------\n";
        receiptContent << left << setw(20) << "商品名称" << setw(10) << "单价" << setw(8) << "数量" << setw(10) << "小计" << endl;
        for (const auto& item : sale.items) {
            const Product* p = findProduct(item.productId);
            receiptContent << left << setw(20) << (p ? p->getName() : "已删除商品") << setw(10) << fixed << setprecision(2) << item.priceAtSale << setw(8) << item.getQuantity() << setw(10) << fixed << setprecision(2) << item.getSubtotal() << endl;
        }
        receiptContent << "----------------------------------------------\n";
        receiptContent << "商品总额: " << fixed << setprecision(2) << sale.totalAmount << " 元\n";
        if (sale.discount > 0.0) {
            receiptContent << "折扣优惠: " << fixed << setprecision(2) << sale.discount * 100 << " %\n";
            receiptContent << "折扣金额: " << fixed << setprecision(2) << sale.totalAmount * sale.discount << " 元\n";
        }
        receiptContent << "应付金额: " << fixed << setprecision(2) << sale.getFinalAmount() << " 元\n";
        if (mem) {
            int earnedPoints = static_cast<int>(sale.getFinalAmount() / 10);
            receiptContent << "本次积分: " << earnedPoints << "  (当前总积分: " << mem->getPoints() << ")\n";
        }
        receiptContent << "==============================================\n\n";
        cout << receiptContent.str();

        cout << "是否将此收据导出为文本文件? (y/n): ";
        char choice; cin >> choice; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (choice == 'y' || choice == 'Y') {
            string filename = "receipt_" + to_string(sale.id) + ".txt";
            ofstream file(filename);
            file << receiptContent.str();
            cout << "收据已导出到 " << filename << endl;
            logAction("收据 " + to_string(sale.id) + " 已导出。");
        }
    }

    // --- 报表导出 ---
    void exportSaleReport() {
        clearScreen();
        cout << "--- 导出销售报表 ---\n";
        Date startDate = readDate("请输入开始日期 (年 月 日): ");
        Date endDate = readDate("请输入结束日期 (年 月 日): ");

        vector<SaleRecord> reportSales;
        double totalRevenue = 0;
        map<int, int> productSalesCount;

        for (const auto& sale : sales) {
            if (sale.saleDate >= startDate && sale.saleDate <= endDate) {
                reportSales.push_back(sale);
                totalRevenue += sale.getFinalAmount();
                for (const auto& item : sale.items) {
                    productSalesCount[item.getProductId()] += item.getQuantity();
                }
            }
        }

        if (reportSales.empty()) {
            cout << "该日期区间内无销售记录。\n"; return;
        }

        string filename = "sales_report_" + startDate.toString() + "_to_" + endDate.toString() + ".txt";
        ofstream file(filename);
        file << "========== 销售报表 ==========\n";
        file << "日期范围: " << startDate.toString() << " 至 " << endDate.toString() << "\n";
        file << "总销售笔数: " << reportSales.size() << "\n";
        file << "总销售额: " << fixed << setprecision(2) << totalRevenue << " 元\n";

        if (!productSalesCount.empty()) {
            auto bestSeller = max_element(productSalesCount.begin(), productSalesCount.end(),
                [](const pair<int, int>& a, const pair<int, int>& b) { return a.second < b.second; });
            const Product* p = findProduct(bestSeller->first);
            file << "最畅销商品: " << (p ? p->getName() : "ID " + to_string(bestSeller->first)) << " (售出 " << bestSeller->second << " 件)\n";
        }
        file << "==============================\n";
        cout << "报表已成功导出到 " << filename << endl;
        logAction("导出了销售报表: " + filename);
    }

    void exportStockWarningReport() {
        clearScreen();
        int threshold = readInt("请输入库存预警阈值 (如: 10): ", 0);
        string filename = "stock_warning_report_" + Date::currentDate().toString() + ".txt";
        ofstream file(filename);

        file << "========== 库存预警报表 (" << Date::currentDate().toString() << ") ==========\n";
        file << "预警阈值: 低于 " << threshold << " 件\n";
        file << "--------------------------------------------------\n";
        file << left << setw(6) << "ID" << setw(25) << "名称" << setw(10) << "当前库存" << endl;

        int count = 0;
        for (const auto& product : products) {
            if (product.getStock() < threshold) {
                file << left << setw(6) << product.getId() << setw(25) << product.getName() << setw(10) << product.getStock() << endl;
                count++;
            }
        }
        file << "--------------------------------------------------\n";
        file << "共有 " << count << " 项商品库存过低。\n";
        cout << "库存预警报表已导出到 " << filename << endl;
        logAction("导出了库存预警报表。");
    }


public:
    MallManagementSystem() {
        loadData("products.json", products);
        loadData("employees.json", employees);
        loadData("members.json", members);
        loadData("sales.json", sales);
        updateNextIds();
    }

    // --- 各模块菜单 ---
    void productMenu() {
        vector<Product> currentView = products;
        string choiceStr;
        do {
            clearScreen();
            displayProducts(currentView);
            cout << "\n--- 商品管理菜单 ---\n"
                << "1. 添加商品    2. 修改商品    3. 删除商品\n"
                << "4. 模糊搜索    5. 排序        6. 显示全部\n"
                << "q. 返回主菜单\n";
            choiceStr = readString("请选择: ");

            if (choiceStr == "1") {
                string name = readString("商品名称: ");
                string cat = readString("商品类别: ");
                double price = readDouble("价格: ", 0.0);
                int stock = readInt("库存: ", 0);
                Date exp = readDate("过期日期 (年 月 日): ");
                products.emplace_back(nextProductId++, name, cat, price, stock, exp);
                saveData("products.json", products);
                logAction("添加了商品: " + name);
                currentView = products;
            }
            else if (choiceStr == "2") {
                int id = readInt("输入要修改的商品ID: ");
                Product* p = findProduct(id);
                if (p) {
                    p->setPrice(readDouble("输入新价格 (当前: " + to_string(p->getPrice()) + ", 直接回车不修改): ", 0.0));
                    p->setStock(readInt("输入新库存 (当前: " + to_string(p->getStock()) + ", 直接回车不修改): ", 0));
                    saveData("products.json", products);
                    logAction("修改了商品 ID: " + to_string(id));
                    currentView = products;
                }
                else cout << "未找到该商品。\n";
            }
            else if (choiceStr == "3") {
                int id = readInt("输入要删除的商品ID: ");
                Product* p = findProduct(id);
                if (p) {
                    cout << "确定要删除商品 '" << p->getName() << "' 吗? (y/n): ";
                    char confirm; cin >> confirm; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (confirm == 'y' || confirm == 'Y') {
                        auto it = remove_if(products.begin(), products.end(), [id](const Product& pr) { return pr.getId() == id; });
                        products.erase(it, products.end());
                        saveData("products.json", products);
                        logAction("删除了商品 ID: " + to_string(id));
                        currentView = products;
                    }
                    else cout << "删除已取消。\n";
                }
                else cout << "未找到该商品。\n";
            }
            else if (choiceStr == "4") {
                string term = readString("输入搜索关键词(名称或类别): ");
                currentView.clear();
                for (const auto& p : products) {
                    string nameLower = p.getName(), catLower = p.getCategory(), termLower = term;
                    transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
                    transform(catLower.begin(), catLower.end(), catLower.begin(), ::tolower);
                    transform(termLower.begin(), termLower.end(), termLower.begin(), ::tolower);
                    if (nameLower.find(termLower) != string::npos || catLower.find(termLower) != string::npos) {
                        currentView.push_back(p);
                    }
                }
            }
            else if (choiceStr == "5") {
                int sortChoice = readInt("排序方式 (1.按价格升序 2.按库存升序 3.按过期日期升序): ", 1, 3);
                if (sortChoice == 1) sort(currentView.begin(), currentView.end(), [](const Product& a, const Product& b) { return a.getPrice() < b.getPrice(); });
                if (sortChoice == 2) sort(currentView.begin(), currentView.end(), [](const Product& a, const Product& b) { return a.getStock() < b.getStock(); });
                if (sortChoice == 3) sort(currentView.begin(), currentView.end(), [](const Product& a, const Product& b) { return a.getExpirationDate() <= b.getExpirationDate(); });
            }
            else if (choiceStr == "6") {
                currentView = products;
            }

            if (choiceStr != "q" && choiceStr != "Q") {
                cout << "\n按 Enter键 继续...";
                cin.get();
            }

        } while (choiceStr != "q" && choiceStr != "Q");
    }

    void employeeMenu() {
        vector<Employee> currentView = employees;
        string choiceStr;
        do {
            clearScreen();
            displayEmployees(currentView);
            cout << "\n--- 员工管理菜单 ---\n"
                << "1. 添加员工    2. 修改员工    3. 删除员工\n"
                << "4. 模糊搜索    5. 按入职日期排序 6. 显示全部\n"
                << "q. 返回主菜单\n";
            choiceStr = readString("请选择: ");

            if (choiceStr == "1") {
                string name = readString("姓名: ");
                string pos = readString("职位: ");
                double salary = readDouble("工资: ", 0.0);
                Date hire = readDate("入职日期 (年 月 日): ");
                employees.emplace_back(nextEmployeeId++, name, pos, salary, hire);
                saveData("employees.json", employees);
                logAction("添加了员工: " + name);
                currentView = employees;
            }
            else if (choiceStr == "2") {
                int id = readInt("输入要修改的员工ID: ");
                Employee* e = findEmployee(id);
                if (e) {
                    e->setPosition(readString("输入新职位 (当前: " + e->getPosition() + "): "));
                    e->setSalary(readDouble("输入新工资 (当前: " + to_string(e->getSalary()) + "): ", 0.0));
                    saveData("employees.json", employees);
                    logAction("修改了员工 ID: " + to_string(id));
                    currentView = employees;
                }
                else cout << "未找到该员工。\n";
            }
            else if (choiceStr == "3") {
                int id = readInt("输入要删除的员工ID: ");
                Employee* e = findEmployee(id);
                if (e) {
                    cout << "确定要删除员工 '" << e->getName() << "' 吗? (y/n): ";
                    char confirm; cin >> confirm; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (confirm == 'y' || confirm == 'Y') {
                        auto it = remove_if(employees.begin(), employees.end(), [id](const Employee& emp) { return emp.getId() == id; });
                        employees.erase(it, employees.end());
                        saveData("employees.json", employees);
                        logAction("删除了员工 ID: " + to_string(id));
                        currentView = employees;
                    }
                    else cout << "删除已取消。\n";
                }
                else cout << "未找到该员工。\n";
            }
            else if (choiceStr == "4") {
                string term = readString("输入搜索关键词(姓名或职位): ");
                currentView.clear();
                for (const auto& e : employees) {
                    string nameLower = e.getName(), posLower = e.getPosition(), termLower = term;
                    transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
                    transform(posLower.begin(), posLower.end(), posLower.begin(), ::tolower);
                    transform(termLower.begin(), termLower.end(), termLower.begin(), ::tolower);
                    if (nameLower.find(termLower) != string::npos || posLower.find(termLower) != string::npos) {
                        currentView.push_back(e);
                    }
                }
            }
            else if (choiceStr == "5") {
                sort(currentView.begin(), currentView.end(), [](const Employee& a, const Employee& b) { return a.getHireDate() <= b.getHireDate(); });
            }
            else if (choiceStr == "6") {
                currentView = employees;
            }

            if (choiceStr != "q" && choiceStr != "Q") {
                cout << "\n按 Enter键 继续...";
                cin.get();
            }

        } while (choiceStr != "q" && choiceStr != "Q");
    }

    void memberMenu() {
        vector<Member> currentView = members;
        string choiceStr;
        do {
            clearScreen();
            displayMembers(currentView);
            cout << "\n--- 会员管理菜单 ---\n"
                << "1. 添加会员    2. 修改电话    3. 删除会员\n"
                << "4. 模糊搜索    5. 显示全部\n"
                << "q. 返回主菜单\n";
            choiceStr = readString("请选择: ");

            if (choiceStr == "1") {
                string name = readString("姓名: ");
                string phone = readString("电话: ");
                members.emplace_back(nextMemberId++, name, phone, 0, Date::currentDate());
                saveData("members.json", members);
                logAction("添加了会员: " + name);
                currentView = members;
            }
            else if (choiceStr == "2") {
                int id = readInt("输入要修改的会员ID: ");
                Member* m = findMember(id);
                if (m) {
                    m->setPhone(readString("输入新电话号码 (当前: " + m->getPhone() + "): "));
                    saveData("members.json", members);
                    logAction("修改了会员电话 ID: " + to_string(id));
                    currentView = members;
                }
                else cout << "未找到该会员。\n";
            }
            else if (choiceStr == "3") {
                int id = readInt("输入要删除的会员ID: ");
                Member* m = findMember(id);
                if (m) {
                    cout << "确定要删除会员 '" << m->getName() << "' 吗? (y/n): ";
                    char confirm; cin >> confirm; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (confirm == 'y' || confirm == 'Y') {
                        auto it = remove_if(members.begin(), members.end(), [id](const Member& mem) { return mem.getId() == id; });
                        members.erase(it, members.end());
                        saveData("members.json", members);
                        logAction("删除了会员 ID: " + to_string(id));
                        currentView = members;
                    }
                    else cout << "删除已取消。\n";
                }
                else cout << "未找到该会员。\n";
            }
            else if (choiceStr == "4") {
                string term = readString("输入搜索关键词(姓名或电话): ");
                currentView.clear();
                for (const auto& m : members) {
                    string nameLower = m.getName(), phoneLower = m.getPhone(), termLower = term;
                    transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
                    transform(phoneLower.begin(), phoneLower.end(), phoneLower.begin(), ::tolower);
                    transform(termLower.begin(), termLower.end(), termLower.begin(), ::tolower);
                    if (nameLower.find(termLower) != string::npos || phoneLower.find(termLower) != string::npos) {
                        currentView.push_back(m);
                    }
                }
            }
            else if (choiceStr == "5") {
                currentView = members;
            }

            if (choiceStr != "q" && choiceStr != "Q") {
                cout << "\n按 Enter键 继续...";
                cin.get();
            }

        } while (choiceStr != "q" && choiceStr != "Q");
    }

    void reportMenu() {
        string choiceStr;
        do {
            clearScreen();
            cout << "\n--- 报表中心 ---\n"
                << "1. 导出销售报表\n"
                << "2. 导出库存预警报表\n"
                << "q. 返回主菜单\n";
            choiceStr = readString("请选择: ");
            if (choiceStr == "1") exportSaleReport();
            else if (choiceStr == "2") exportStockWarningReport();

            if (choiceStr != "q" && choiceStr != "Q") {
                cout << "\n按 Enter键 继续...";
                // cin.get() 在 readXXX 函数后可能行为不一致，使用 getline 清空缓冲区
                string dummy;
                getline(cin, dummy);
            }
        } while (choiceStr != "q" && choiceStr != "Q");
    }

    void createSale() {
        clearScreen();
        if (products.empty()) { cout << "错误：必须先有商品才能进行销售。\n"; return; }
        if (employees.empty()) { cout << "错误：必须先有员工才能进行销售。\n"; return; }

        displayEmployees(employees);
        int empId = readInt("请输入负责本次销售的员工ID: ");
        Employee* emp = findEmployee(empId);
        if (!emp) { cout << "错误：无效的员工ID。\n"; return; }

        Member* mem = nullptr;
        cout << "顾客是会员吗? (y/n): "; char isMember; cin >> isMember; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (isMember == 'y' || isMember == 'Y') {
            displayMembers(members);
            int memId = readInt("请输入会员ID (输入0跳过): ");
            if (memId > 0) {
                mem = findMember(memId);
                if (!mem) cout << "警告：会员ID无效，按非会员处理。\n";
            }
        }

        SaleRecord sale; sale.id = nextSaleId; sale.employeeId = emp->getId(); sale.memberId = (mem ? mem->getId() : 0); sale.saleDate = Date::currentDate(); sale.totalAmount = 0.0; sale.discount = 0.0;

        char addMore = 'y';
        while (addMore == 'y' || addMore == 'Y') {
            clearScreen();
            displayProducts(products);
            cout << "\n--- 当前购物车 ---\n";
            double currentTotal = 0;
            for (const auto& item : sale.items) {
                const Product* p = findProduct(item.productId);
                cout << (p ? p->getName() : "未知商品") << " x " << item.quantity << " = " << item.getSubtotal() << endl;
                currentTotal += item.getSubtotal();
            }
            cout << "-------------------\n";
            cout << "当前总计: " << fixed << setprecision(2) << currentTotal << endl;

            int prodId = readInt("\n请输入要销售的商品ID (输入0完成添加): ");
            if (prodId == 0) break;

            Product* prod = findProduct(prodId);
            if (!prod) { cout << "错误：无效的商品ID。\n"; continue; }
            if (prod->getStock() == 0) { cout << "错误: 商品 '" << prod->getName() << "' 已售罄。\n"; continue; }

            int quantity = readInt("请输入数量: ", 1, prod->getStock());

            prod->reduceStock(quantity);
            sale.items.emplace_back(prod->getId(), quantity, prod->getPrice());
            sale.totalAmount += prod->getPrice() * quantity;

            cout << "是否继续添加商品? (y/n): "; cin >> addMore; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        if (sale.items.empty()) { cout << "销售已取消，未添加任何商品。\n"; return; }

        if (mem && mem->getPoints() >= 100) {
            cout << "会员当前积分为 " << mem->getPoints() << "。是否使用100积分兑换10%折扣? (y/n): ";
            char usePts; cin >> usePts; cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (usePts == 'y' || usePts == 'Y') {
                sale.discount = 0.1;
                mem->usePoints(100);
                cout << "已使用100积分，享受10%折扣！\n";
            }
        }
        int earnedPoints = static_cast<int>(sale.getFinalAmount() / 10);
        if (mem) { mem->addPoints(earnedPoints); }

        sales.push_back(sale); nextSaleId++;

        saveData("products.json", products);
        if (mem) saveData("members.json", members);
        saveData("sales.json", sales);
        logAction("创建了新的销售订单 ID: " + to_string(sale.id));

        cout << "\n销售完成！正在生成收据...\n";
        printReceipt(sale);
    }

    void mainMenu() {
        string choiceStr;
        do {
            clearScreen();
            cout << "\n========= 商场管理系统主菜单 (" << Date::currentDate().toString() << ") =========\n"
                << "1. 商品管理\n"
                << "2. 员工管理\n"
                << "3. 会员管理\n"
                << "4. 创建销售订单\n"
                << "5. 报表中心\n"
                << "0. 退出系统\n"
                << "========================================================\n";
            choiceStr = readString("请输入您的选择: ");
            if (choiceStr == "1") productMenu();
            else if (choiceStr == "2") employeeMenu();
            else if (choiceStr == "3") memberMenu();
            else if (choiceStr == "4") createSale();
            else if (choiceStr == "5") reportMenu();
            else if (choiceStr == "0") {
                logAction("系统正常退出。");
                cout << "感谢使用，系统正在退出。\n";
            }
            else {
                cout << "无效选择，请按 Enter键 后重试...";
                cin.get();
            }
        } while (choiceStr != "0");
    }
};

int main() {
    setlocale(LC_ALL, ""); // 支持中文显示
    logAction("系统启动。");
    MallManagementSystem mall;
    mall.mainMenu();
    return 0;
}
