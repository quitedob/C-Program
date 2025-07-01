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

// ���� nlohmann/json �⣬��ȷ�� json.hpp �ļ��뱾�ļ���ͬһĿ¼
#include "json.hpp"

// Ϊ nlohmann::json ����һ�����̵ı���������ʹ��
using json = nlohmann::json;
using namespace std;

// ========== ���ݽṹ���� (Date ��ǰ) ==========
struct Date {
    int year, month, day;
    Date() : year(0), month(0), day(0) {}
    Date(int y, int m, int d) : year(y), month(m), day(d) {}

    // ��ע�ͣ�������ת��Ϊ "YYYY-MM-DD" ��ʽ���ַ���
    string toString() const {
        ostringstream oss;
        oss << year << "-" << setw(2) << setfill('0') << month << "-" << setw(2) << setfill('0') << day;
        return oss.str();
    }

    // ��ע�ͣ���ȡ��ǰϵͳ����
    static Date currentDate() {
        time_t now = time(0);
        tm* lt = localtime(&now);
        return Date(1900 + lt->tm_year, 1 + lt->tm_mon, lt->tm_mday);
    }

    // ��ע�ͣ�����С�ڵ�����������������ڱȽ�
    bool operator<=(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day <= other.day;
    }

    // ��ע�ͣ����ش��ڵ�����������������ڱȽ�
    bool operator>=(const Date& other) const {
        if (year != other.year) return year > other.year;
        if (month != other.month) return month > other.month;
        return day >= other.day;
    }

    // ��ע�ͣ�Ϊ nlohmann/json ���ṩ���л�/�����л�֧��
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Date, year, month, day)
};


// ========== �������ߺ��� ==========

// ��ע�ͣ��� log.txt �ļ���¼һ����ʱ�������־
void logAction(const string& message) {
    ofstream logfile("log.txt", ios::app);
    if (logfile.is_open()) {
        time_t now = time(0);
        char timestamp[30];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        logfile << "[" << timestamp << "] " << message << endl;
    }
}

// ��ע�ͣ���տ���̨��Ļ
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ��ע�ͣ���ȫ�ض�ȡһ���ַ��������Դ��������
string readString(const string& prompt, bool allowEmpty = false) {
    string value;
    while (true) {
        cout << prompt;
        getline(cin, value);
        if (!allowEmpty && value.empty()) {
            cout << "���벻��Ϊ�գ����������롣\n";
        }
        else {
            return value;
        }
    }
}

// ��ע�ͣ���ȫ�ض�ȡһ��ָ����Χ�ڵ�����
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
        cout << "������Ч��������һ������ " << minVal << " �� " << maxVal << " ֮���������\n";
    }
}

// ��ע�ͣ���ȫ�ض�ȡһ��ָ����Χ�ڵĸ�����
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
        cout << "������Ч��������һ������ " << minVal << " �� " << maxVal << " ֮������֡�\n";
    }
}

// ��ע�ͣ�����Ƿ�Ϊ����
bool isLeap(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// ��ע�ͣ���ȫ�ض�ȡ����֤һ���Ϸ�������
Date readDate(const string& prompt) {
    int year, month, day;
    int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    while (true) {
        cout << prompt;
        string line;
        getline(cin, line);

        // ���Խ��� "YYYY-MM-DD" �� "YYYY MM DD" ��ʽ
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
        cout << "��Ч�����ڸ�ʽ�����ڲ����ڣ������� '�� �� ��' (����: 2025 7 1): \n";
    }
}


// ========== �������ݽṹ���� ==========

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

// ========== ϵͳ������ ==========
class MallManagementSystem {
private:
    vector<Product> products;
    vector<Employee> employees;
    vector<Member> members;
    vector<SaleRecord> sales;
    int nextProductId = 1, nextEmployeeId = 1, nextMemberId = 1, nextSaleId = 1;

    // --- �־û��뱸�� ---
    template<typename T>
    void saveData(const string& filename, const vector<T>& data) {
        string tempFilename = filename + ".tmp";
        string backupFilename = filename + ".bak";

        // д����ʱ�ļ�
        ofstream file(tempFilename);
        if (!file.is_open()) {
            logAction("���ش���: �޷�������ʱ�ļ� " + tempFilename + " ����д�롣");
            return;
        }
        try {
            json j = data;
            file << j.dump(4);
            file.close();

            // ��������
            if (ifstream(filename).good()) {
                std::rename(filename.c_str(), backupFilename.c_str());
            }
            // ��������ʱ�ļ�Ϊ��ʽ�ļ�
            std::rename(tempFilename.c_str(), filename.c_str());
            logAction("���ݳɹ����浽 " + filename);
        }
        catch (const exception& e) {
            logAction("���ش���: д��JSON�� " + tempFilename + " ʱ�����쳣: " + e.what());
            file.close();
            std::remove(tempFilename.c_str()); // ɾ���������ʱ�ļ�
        }
    }

    template<typename T>
    void loadData(const string& filename, vector<T>& data) {
        ifstream file(filename);
        if (!file.good()) {
            logAction("��Ϣ: �ļ� " + filename + " �����ڣ����������ļ���");
            saveData(filename, data); return;
        }
        try {
            json j; file >> j;
            if (!j.is_null() && j.is_array()) data = j.get<vector<T>>();
            logAction("���ݳɹ��� " + filename + " ���ء�");
        }
        catch (const json::parse_error& e) {
            logAction("����: JSON�ļ� '" + filename + "' ��ʽ�𻵣�����: " + e.what());
            string backupFilename = filename + ".bak";
            if (ifstream(backupFilename).good()) {
                cout << "����: ���ļ� " << filename << " �𻵡���⵽�����ļ� " << backupFilename << "��\n";
                cout << "�Ƿ�ӱ��ݻָ�? (y/n): ";
                char choice; cin >> choice; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (choice == 'y' || choice == 'Y') {
                    std::remove(filename.c_str());
                    std::rename(backupFilename.c_str(), filename.c_str());
                    logAction("�Ѵ� " + backupFilename + " �ָ����ݡ�");
                    loadData(filename, data); // ���¼��ػָ����ļ�
                    return;
                }
            }
            cout << "�޷��ӱ��ݻָ����Ƿ�Ҫ�ÿ����ݸ��Ǵ��ļ�? (y/n): ";
            char choice; cin >> choice; cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (choice == 'y' || choice == 'Y') {
                data.clear(); saveData(filename, data);
                logAction("�ļ� " + filename + " �ѱ��û�ѡ���ؽ���");
            }
            else {
                logAction("�û�ѡ���ؽ��ļ��������˳���"); exit(1);
            }
        }
    }

    void updateNextIds() {
        if (!products.empty()) nextProductId = max_element(products.begin(), products.end(), [](const Product& a, const Product& b) { return a.getId() < b.getId(); })->getId() + 1;
        if (!employees.empty()) nextEmployeeId = max_element(employees.begin(), employees.end(), [](const Employee& a, const Employee& b) { return a.getId() < b.getId(); })->getId() + 1;
        if (!members.empty()) nextMemberId = max_element(members.begin(), members.end(), [](const Member& a, const Member& b) { return a.getId() < b.getId(); })->getId() + 1;
        if (!sales.empty()) nextSaleId = max_element(sales.begin(), sales.end(), [](const SaleRecord& a, const SaleRecord& b) { return a.id < b.id; })->id + 1;
    }

    // --- ���ҹ��ߺ��� ---
    Product* findProduct(int id) { for (auto& p : products) if (p.getId() == id) return &p; return nullptr; }
    Employee* findEmployee(int id) { for (auto& e : employees) if (e.getId() == id) return &e; return nullptr; }
    Member* findMember(int id) { for (auto& m : members) if (m.getId() == id) return &m; return nullptr; }
    const Product* findProduct(int id) const { for (const auto& p : products) if (p.getId() == id) return &p; return nullptr; }
    const Employee* findEmployee(int id) const { for (const auto& e : employees) if (e.getId() == id) return &e; return nullptr; }
    const Member* findMember(int id) const { for (const auto& m : members) if (m.getId() == id) return &m; return nullptr; }

    // --- �б���ʾ���� ---
    void displayProducts(const vector<Product>& prods) const {
        if (prods.empty()) { cout << "û���ҵ�������������Ʒ��¼��\n"; return; }
        cout << "\n--- ��Ʒ�б� ---\n";
        cout << left << setw(6) << "ID" << setw(25) << "����" << setw(18) << "���" << setw(12) << "�۸�" << setw(8) << "���" << setw(12) << "��������" << endl;
        cout << string(91, '-') << endl;
        for (const auto& p : prods) p.display();
    }
    void displayEmployees(const vector<Employee>& emps) const {
        if (emps.empty()) { cout << "û���ҵ�����������Ա����¼��\n"; return; }
        cout << "\n--- Ա���б� ---\n";
        cout << left << setw(6) << "ID" << setw(20) << "����" << setw(20) << "ְλ" << setw(12) << "����" << setw(12) << "��ְ����" << endl;
        cout << string(80, '-') << endl;
        for (const auto& e : emps) e.display();
    }
    void displayMembers(const vector<Member>& mems) const {
        if (mems.empty()) { cout << "û���ҵ����������Ļ�Ա��¼��\n"; return; }
        cout << "\n--- ��Ա�б� ---\n";
        cout << left << setw(6) << "ID" << setw(20) << "����" << setw(15) << "�绰" << setw(8) << "����" << setw(12) << "ע������" << endl;
        cout << string(71, '-') << endl;
        for (const auto& m : mems) m.display();
    }

    // --- �վݴ�ӡ�뵼�� ---
    void printReceipt(const SaleRecord& sale) const {
        ostringstream receiptContent;
        const Employee* emp = findEmployee(sale.employeeId);
        const Member* mem = findMember(sale.memberId);
        receiptContent << "\n\n================= �����վ� =================\n";
        receiptContent << "���۵���: " << sale.id << "\n";
        receiptContent << "��������: " << sale.saleDate.toString() << "\n";
        receiptContent << "�� �� Ա: " << (emp ? emp->getName() : "N/A") << " (ID: " << sale.employeeId << ")\n";
        if (mem) receiptContent << "��    Ա: " << mem->getName() << " (ID: " << sale.memberId << ")\n";
        receiptContent << "----------------------------------------------\n";
        receiptContent << left << setw(20) << "��Ʒ����" << setw(10) << "����" << setw(8) << "����" << setw(10) << "С��" << endl;
        for (const auto& item : sale.items) {
            const Product* p = findProduct(item.productId);
            receiptContent << left << setw(20) << (p ? p->getName() : "��ɾ����Ʒ") << setw(10) << fixed << setprecision(2) << item.priceAtSale << setw(8) << item.getQuantity() << setw(10) << fixed << setprecision(2) << item.getSubtotal() << endl;
        }
        receiptContent << "----------------------------------------------\n";
        receiptContent << "��Ʒ�ܶ�: " << fixed << setprecision(2) << sale.totalAmount << " Ԫ\n";
        if (sale.discount > 0.0) {
            receiptContent << "�ۿ��Ż�: " << fixed << setprecision(2) << sale.discount * 100 << " %\n";
            receiptContent << "�ۿ۽��: " << fixed << setprecision(2) << sale.totalAmount * sale.discount << " Ԫ\n";
        }
        receiptContent << "Ӧ�����: " << fixed << setprecision(2) << sale.getFinalAmount() << " Ԫ\n";
        if (mem) {
            int earnedPoints = static_cast<int>(sale.getFinalAmount() / 10);
            receiptContent << "���λ���: " << earnedPoints << "  (��ǰ�ܻ���: " << mem->getPoints() << ")\n";
        }
        receiptContent << "==============================================\n\n";
        cout << receiptContent.str();

        cout << "�Ƿ񽫴��վݵ���Ϊ�ı��ļ�? (y/n): ";
        char choice; cin >> choice; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (choice == 'y' || choice == 'Y') {
            string filename = "receipt_" + to_string(sale.id) + ".txt";
            ofstream file(filename);
            file << receiptContent.str();
            cout << "�վ��ѵ����� " << filename << endl;
            logAction("�վ� " + to_string(sale.id) + " �ѵ�����");
        }
    }

    // --- ������ ---
    void exportSaleReport() {
        clearScreen();
        cout << "--- �������۱��� ---\n";
        Date startDate = readDate("�����뿪ʼ���� (�� �� ��): ");
        Date endDate = readDate("������������� (�� �� ��): ");

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
            cout << "�����������������ۼ�¼��\n"; return;
        }

        string filename = "sales_report_" + startDate.toString() + "_to_" + endDate.toString() + ".txt";
        ofstream file(filename);
        file << "========== ���۱��� ==========\n";
        file << "���ڷ�Χ: " << startDate.toString() << " �� " << endDate.toString() << "\n";
        file << "�����۱���: " << reportSales.size() << "\n";
        file << "�����۶�: " << fixed << setprecision(2) << totalRevenue << " Ԫ\n";

        if (!productSalesCount.empty()) {
            auto bestSeller = max_element(productSalesCount.begin(), productSalesCount.end(),
                [](const pair<int, int>& a, const pair<int, int>& b) { return a.second < b.second; });
            const Product* p = findProduct(bestSeller->first);
            file << "�����Ʒ: " << (p ? p->getName() : "ID " + to_string(bestSeller->first)) << " (�۳� " << bestSeller->second << " ��)\n";
        }
        file << "==============================\n";
        cout << "�����ѳɹ������� " << filename << endl;
        logAction("���������۱���: " + filename);
    }

    void exportStockWarningReport() {
        clearScreen();
        int threshold = readInt("��������Ԥ����ֵ (��: 10): ", 0);
        string filename = "stock_warning_report_" + Date::currentDate().toString() + ".txt";
        ofstream file(filename);

        file << "========== ���Ԥ������ (" << Date::currentDate().toString() << ") ==========\n";
        file << "Ԥ����ֵ: ���� " << threshold << " ��\n";
        file << "--------------------------------------------------\n";
        file << left << setw(6) << "ID" << setw(25) << "����" << setw(10) << "��ǰ���" << endl;

        int count = 0;
        for (const auto& product : products) {
            if (product.getStock() < threshold) {
                file << left << setw(6) << product.getId() << setw(25) << product.getName() << setw(10) << product.getStock() << endl;
                count++;
            }
        }
        file << "--------------------------------------------------\n";
        file << "���� " << count << " ����Ʒ�����͡�\n";
        cout << "���Ԥ�������ѵ����� " << filename << endl;
        logAction("�����˿��Ԥ������");
    }


public:
    MallManagementSystem() {
        loadData("products.json", products);
        loadData("employees.json", employees);
        loadData("members.json", members);
        loadData("sales.json", sales);
        updateNextIds();
    }

    // --- ��ģ��˵� ---
    void productMenu() {
        vector<Product> currentView = products;
        string choiceStr;
        do {
            clearScreen();
            displayProducts(currentView);
            cout << "\n--- ��Ʒ����˵� ---\n"
                << "1. �����Ʒ    2. �޸���Ʒ    3. ɾ����Ʒ\n"
                << "4. ģ������    5. ����        6. ��ʾȫ��\n"
                << "q. �������˵�\n";
            choiceStr = readString("��ѡ��: ");

            if (choiceStr == "1") {
                string name = readString("��Ʒ����: ");
                string cat = readString("��Ʒ���: ");
                double price = readDouble("�۸�: ", 0.0);
                int stock = readInt("���: ", 0);
                Date exp = readDate("�������� (�� �� ��): ");
                products.emplace_back(nextProductId++, name, cat, price, stock, exp);
                saveData("products.json", products);
                logAction("�������Ʒ: " + name);
                currentView = products;
            }
            else if (choiceStr == "2") {
                int id = readInt("����Ҫ�޸ĵ���ƷID: ");
                Product* p = findProduct(id);
                if (p) {
                    p->setPrice(readDouble("�����¼۸� (��ǰ: " + to_string(p->getPrice()) + ", ֱ�ӻس����޸�): ", 0.0));
                    p->setStock(readInt("�����¿�� (��ǰ: " + to_string(p->getStock()) + ", ֱ�ӻس����޸�): ", 0));
                    saveData("products.json", products);
                    logAction("�޸�����Ʒ ID: " + to_string(id));
                    currentView = products;
                }
                else cout << "δ�ҵ�����Ʒ��\n";
            }
            else if (choiceStr == "3") {
                int id = readInt("����Ҫɾ������ƷID: ");
                Product* p = findProduct(id);
                if (p) {
                    cout << "ȷ��Ҫɾ����Ʒ '" << p->getName() << "' ��? (y/n): ";
                    char confirm; cin >> confirm; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (confirm == 'y' || confirm == 'Y') {
                        auto it = remove_if(products.begin(), products.end(), [id](const Product& pr) { return pr.getId() == id; });
                        products.erase(it, products.end());
                        saveData("products.json", products);
                        logAction("ɾ������Ʒ ID: " + to_string(id));
                        currentView = products;
                    }
                    else cout << "ɾ����ȡ����\n";
                }
                else cout << "δ�ҵ�����Ʒ��\n";
            }
            else if (choiceStr == "4") {
                string term = readString("���������ؼ���(���ƻ����): ");
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
                int sortChoice = readInt("����ʽ (1.���۸����� 2.��������� 3.��������������): ", 1, 3);
                if (sortChoice == 1) sort(currentView.begin(), currentView.end(), [](const Product& a, const Product& b) { return a.getPrice() < b.getPrice(); });
                if (sortChoice == 2) sort(currentView.begin(), currentView.end(), [](const Product& a, const Product& b) { return a.getStock() < b.getStock(); });
                if (sortChoice == 3) sort(currentView.begin(), currentView.end(), [](const Product& a, const Product& b) { return a.getExpirationDate() <= b.getExpirationDate(); });
            }
            else if (choiceStr == "6") {
                currentView = products;
            }

            if (choiceStr != "q" && choiceStr != "Q") {
                cout << "\n�� Enter�� ����...";
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
            cout << "\n--- Ա������˵� ---\n"
                << "1. ���Ա��    2. �޸�Ա��    3. ɾ��Ա��\n"
                << "4. ģ������    5. ����ְ�������� 6. ��ʾȫ��\n"
                << "q. �������˵�\n";
            choiceStr = readString("��ѡ��: ");

            if (choiceStr == "1") {
                string name = readString("����: ");
                string pos = readString("ְλ: ");
                double salary = readDouble("����: ", 0.0);
                Date hire = readDate("��ְ���� (�� �� ��): ");
                employees.emplace_back(nextEmployeeId++, name, pos, salary, hire);
                saveData("employees.json", employees);
                logAction("�����Ա��: " + name);
                currentView = employees;
            }
            else if (choiceStr == "2") {
                int id = readInt("����Ҫ�޸ĵ�Ա��ID: ");
                Employee* e = findEmployee(id);
                if (e) {
                    e->setPosition(readString("������ְλ (��ǰ: " + e->getPosition() + "): "));
                    e->setSalary(readDouble("�����¹��� (��ǰ: " + to_string(e->getSalary()) + "): ", 0.0));
                    saveData("employees.json", employees);
                    logAction("�޸���Ա�� ID: " + to_string(id));
                    currentView = employees;
                }
                else cout << "δ�ҵ���Ա����\n";
            }
            else if (choiceStr == "3") {
                int id = readInt("����Ҫɾ����Ա��ID: ");
                Employee* e = findEmployee(id);
                if (e) {
                    cout << "ȷ��Ҫɾ��Ա�� '" << e->getName() << "' ��? (y/n): ";
                    char confirm; cin >> confirm; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (confirm == 'y' || confirm == 'Y') {
                        auto it = remove_if(employees.begin(), employees.end(), [id](const Employee& emp) { return emp.getId() == id; });
                        employees.erase(it, employees.end());
                        saveData("employees.json", employees);
                        logAction("ɾ����Ա�� ID: " + to_string(id));
                        currentView = employees;
                    }
                    else cout << "ɾ����ȡ����\n";
                }
                else cout << "δ�ҵ���Ա����\n";
            }
            else if (choiceStr == "4") {
                string term = readString("���������ؼ���(������ְλ): ");
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
                cout << "\n�� Enter�� ����...";
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
            cout << "\n--- ��Ա����˵� ---\n"
                << "1. ��ӻ�Ա    2. �޸ĵ绰    3. ɾ����Ա\n"
                << "4. ģ������    5. ��ʾȫ��\n"
                << "q. �������˵�\n";
            choiceStr = readString("��ѡ��: ");

            if (choiceStr == "1") {
                string name = readString("����: ");
                string phone = readString("�绰: ");
                members.emplace_back(nextMemberId++, name, phone, 0, Date::currentDate());
                saveData("members.json", members);
                logAction("����˻�Ա: " + name);
                currentView = members;
            }
            else if (choiceStr == "2") {
                int id = readInt("����Ҫ�޸ĵĻ�ԱID: ");
                Member* m = findMember(id);
                if (m) {
                    m->setPhone(readString("�����µ绰���� (��ǰ: " + m->getPhone() + "): "));
                    saveData("members.json", members);
                    logAction("�޸��˻�Ա�绰 ID: " + to_string(id));
                    currentView = members;
                }
                else cout << "δ�ҵ��û�Ա��\n";
            }
            else if (choiceStr == "3") {
                int id = readInt("����Ҫɾ���Ļ�ԱID: ");
                Member* m = findMember(id);
                if (m) {
                    cout << "ȷ��Ҫɾ����Ա '" << m->getName() << "' ��? (y/n): ";
                    char confirm; cin >> confirm; cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (confirm == 'y' || confirm == 'Y') {
                        auto it = remove_if(members.begin(), members.end(), [id](const Member& mem) { return mem.getId() == id; });
                        members.erase(it, members.end());
                        saveData("members.json", members);
                        logAction("ɾ���˻�Ա ID: " + to_string(id));
                        currentView = members;
                    }
                    else cout << "ɾ����ȡ����\n";
                }
                else cout << "δ�ҵ��û�Ա��\n";
            }
            else if (choiceStr == "4") {
                string term = readString("���������ؼ���(������绰): ");
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
                cout << "\n�� Enter�� ����...";
                cin.get();
            }

        } while (choiceStr != "q" && choiceStr != "Q");
    }

    void reportMenu() {
        string choiceStr;
        do {
            clearScreen();
            cout << "\n--- �������� ---\n"
                << "1. �������۱���\n"
                << "2. �������Ԥ������\n"
                << "q. �������˵�\n";
            choiceStr = readString("��ѡ��: ");
            if (choiceStr == "1") exportSaleReport();
            else if (choiceStr == "2") exportStockWarningReport();

            if (choiceStr != "q" && choiceStr != "Q") {
                cout << "\n�� Enter�� ����...";
                // cin.get() �� readXXX �����������Ϊ��һ�£�ʹ�� getline ��ջ�����
                string dummy;
                getline(cin, dummy);
            }
        } while (choiceStr != "q" && choiceStr != "Q");
    }

    void createSale() {
        clearScreen();
        if (products.empty()) { cout << "���󣺱���������Ʒ���ܽ������ۡ�\n"; return; }
        if (employees.empty()) { cout << "���󣺱�������Ա�����ܽ������ۡ�\n"; return; }

        displayEmployees(employees);
        int empId = readInt("�����븺�𱾴����۵�Ա��ID: ");
        Employee* emp = findEmployee(empId);
        if (!emp) { cout << "������Ч��Ա��ID��\n"; return; }

        Member* mem = nullptr;
        cout << "�˿��ǻ�Ա��? (y/n): "; char isMember; cin >> isMember; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (isMember == 'y' || isMember == 'Y') {
            displayMembers(members);
            int memId = readInt("�������ԱID (����0����): ");
            if (memId > 0) {
                mem = findMember(memId);
                if (!mem) cout << "���棺��ԱID��Ч�����ǻ�Ա����\n";
            }
        }

        SaleRecord sale; sale.id = nextSaleId; sale.employeeId = emp->getId(); sale.memberId = (mem ? mem->getId() : 0); sale.saleDate = Date::currentDate(); sale.totalAmount = 0.0; sale.discount = 0.0;

        char addMore = 'y';
        while (addMore == 'y' || addMore == 'Y') {
            clearScreen();
            displayProducts(products);
            cout << "\n--- ��ǰ���ﳵ ---\n";
            double currentTotal = 0;
            for (const auto& item : sale.items) {
                const Product* p = findProduct(item.productId);
                cout << (p ? p->getName() : "δ֪��Ʒ") << " x " << item.quantity << " = " << item.getSubtotal() << endl;
                currentTotal += item.getSubtotal();
            }
            cout << "-------------------\n";
            cout << "��ǰ�ܼ�: " << fixed << setprecision(2) << currentTotal << endl;

            int prodId = readInt("\n������Ҫ���۵���ƷID (����0������): ");
            if (prodId == 0) break;

            Product* prod = findProduct(prodId);
            if (!prod) { cout << "������Ч����ƷID��\n"; continue; }
            if (prod->getStock() == 0) { cout << "����: ��Ʒ '" << prod->getName() << "' ��������\n"; continue; }

            int quantity = readInt("����������: ", 1, prod->getStock());

            prod->reduceStock(quantity);
            sale.items.emplace_back(prod->getId(), quantity, prod->getPrice());
            sale.totalAmount += prod->getPrice() * quantity;

            cout << "�Ƿ���������Ʒ? (y/n): "; cin >> addMore; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        if (sale.items.empty()) { cout << "������ȡ����δ����κ���Ʒ��\n"; return; }

        if (mem && mem->getPoints() >= 100) {
            cout << "��Ա��ǰ����Ϊ " << mem->getPoints() << "���Ƿ�ʹ��100���ֶһ�10%�ۿ�? (y/n): ";
            char usePts; cin >> usePts; cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (usePts == 'y' || usePts == 'Y') {
                sale.discount = 0.1;
                mem->usePoints(100);
                cout << "��ʹ��100���֣�����10%�ۿۣ�\n";
            }
        }
        int earnedPoints = static_cast<int>(sale.getFinalAmount() / 10);
        if (mem) { mem->addPoints(earnedPoints); }

        sales.push_back(sale); nextSaleId++;

        saveData("products.json", products);
        if (mem) saveData("members.json", members);
        saveData("sales.json", sales);
        logAction("�������µ����۶��� ID: " + to_string(sale.id));

        cout << "\n������ɣ����������վ�...\n";
        printReceipt(sale);
    }

    void mainMenu() {
        string choiceStr;
        do {
            clearScreen();
            cout << "\n========= �̳�����ϵͳ���˵� (" << Date::currentDate().toString() << ") =========\n"
                << "1. ��Ʒ����\n"
                << "2. Ա������\n"
                << "3. ��Ա����\n"
                << "4. �������۶���\n"
                << "5. ��������\n"
                << "0. �˳�ϵͳ\n"
                << "========================================================\n";
            choiceStr = readString("����������ѡ��: ");
            if (choiceStr == "1") productMenu();
            else if (choiceStr == "2") employeeMenu();
            else if (choiceStr == "3") memberMenu();
            else if (choiceStr == "4") createSale();
            else if (choiceStr == "5") reportMenu();
            else if (choiceStr == "0") {
                logAction("ϵͳ�����˳���");
                cout << "��лʹ�ã�ϵͳ�����˳���\n";
            }
            else {
                cout << "��Чѡ���밴 Enter�� ������...";
                cin.get();
            }
        } while (choiceStr != "0");
    }
};

int main() {
    setlocale(LC_ALL, ""); // ֧��������ʾ
    logAction("ϵͳ������");
    MallManagementSystem mall;
    mall.mainMenu();
    return 0;
}
