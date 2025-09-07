#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDateEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QListWidget>
#include <QTabWidget>
#include <QGroupBox>
#include <QMessageBox>
#include <QPdfWriter>
#include <QPainter>
#include <QDir>
#include <QFileDialog>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QPageSize>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QComboBox>
#include <QInputDialog>

class ProjectTracker : public QMainWindow {
private:
    // Input fields
    QLineEdit *clientNameEdit;
    QLineEdit *clientPhoneEdit;
    QDateEdit *dateEdit;
    QDoubleSpinBox *billEdit;
    QDoubleSpinBox *advancedEdit;
    QDoubleSpinBox *dueEdit;
    QSpinBox *percentageSpinBox;
    QCheckBox *autoCalculateCheckBox;
    QTextEdit *pointsEdit;
    QListWidget *pointsList;
    
    // Dashboard elements
    QChartView *chartView;
    
    // Labels for displaying calculations
    QLabel *calculationInfoLabel;
    
    // Projects management
    QTableWidget *projectsTable;
    QComboBox *projectStatusFilter;
    
public:
    ProjectTracker(QWidget *parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupDatabase();
        loadDashboard();
        loadProjects();
    }

private:
    void setupUI() {
        setWindowTitle("ফ্রিল্যান্স প্রজেক্ট ট্র্যাকার");
        resize(1000, 750);
        
        // Main tab widget
        QTabWidget *tabWidget = new QTabWidget;
        setCentralWidget(tabWidget);
        
        // Project Entry Tab
        QWidget *entryTab = new QWidget;
        tabWidget->addTab(entryTab, "প্রজেক্ট এন্ট্রি");
        
        QVBoxLayout *mainLayout = new QVBoxLayout(entryTab);
        
        // Client Information Group
        QGroupBox *clientGroup = new QGroupBox("ক্লায়েন্ট তথ্য");
        QFormLayout *clientLayout = new QFormLayout;
        clientNameEdit = new QLineEdit("ওমর ফারুক");
        clientPhoneEdit = new QLineEdit;
        dateEdit = new QDateEdit(QDate::currentDate());
        dateEdit->setDisplayFormat("dd MMMM yyyy");
        clientLayout->addRow("ক্লায়েন্টের নাম:", clientNameEdit);
        clientLayout->addRow("ফোন:", clientPhoneEdit);
        clientLayout->addRow("তারিখ:", dateEdit);
        clientGroup->setLayout(clientLayout);
        mainLayout->addWidget(clientGroup);
        
        // Financial Details Group
        QGroupBox *financeGroup = new QGroupBox("আর্থিক বিবরণ");
        QVBoxLayout *financeMainLayout = new QVBoxLayout;
        QFormLayout *financeLayout = new QFormLayout;
        
        // Bill amount
        billEdit = new QDoubleSpinBox;
        billEdit->setRange(0, 9999999.99);
        billEdit->setDecimals(2);
        billEdit->setPrefix("৳ ");
        billEdit->setSingleStep(100);
        financeLayout->addRow("মোট বিল:", billEdit);
        
        // Auto-calculation controls
        QHBoxLayout *autoCalcLayout = new QHBoxLayout;
        autoCalculateCheckBox = new QCheckBox("অটো ক্যালকুলেট");
        autoCalculateCheckBox->setChecked(true);
        percentageSpinBox = new QSpinBox;
        percentageSpinBox->setRange(0, 100);
        percentageSpinBox->setValue(60);
        percentageSpinBox->setSuffix("%");
        QPushButton *applyPercentBtn = new QPushButton("% প্রয়োগ");
        QPushButton *customAdvanceBtn = new QPushButton("কাস্টম অ্যাডভান্স");
        autoCalcLayout->addWidget(autoCalculateCheckBox);
        autoCalcLayout->addWidget(new QLabel("অ্যাডভান্স:"));
        autoCalcLayout->addWidget(percentageSpinBox);
        autoCalcLayout->addWidget(applyPercentBtn);
        autoCalcLayout->addWidget(customAdvanceBtn);
        autoCalcLayout->addStretch();
        
        // Advanced amount
        advancedEdit = new QDoubleSpinBox;
        advancedEdit->setRange(0, 9999999.99);
        advancedEdit->setDecimals(2);
        advancedEdit->setPrefix("৳ ");
        advancedEdit->setSingleStep(100);
        financeLayout->addRow("অ্যাডভান্স পেমেন্ট:", advancedEdit);
        
        // Due amount
        dueEdit = new QDoubleSpinBox;
        dueEdit->setRange(0, 9999999.99);
        dueEdit->setDecimals(2);
        dueEdit->setPrefix("৳ ");
        dueEdit->setSingleStep(100);
        dueEdit->setReadOnly(true);
        dueEdit->setStyleSheet("QDoubleSpinBox:read-only { background-color: #f0f0f0; }");
        financeLayout->addRow("বাকি টাকা:", dueEdit);
        
        // Calculation info label
        calculationInfoLabel = new QLabel;
        calculationInfoLabel->setStyleSheet("QLabel { color: #0066cc; padding: 5px; "
                                           "background-color: #e6f2ff; border-radius: 3px; }");
        calculationInfoLabel->setWordWrap(true);
        
        financeMainLayout->addLayout(autoCalcLayout);
        financeMainLayout->addLayout(financeLayout);
        financeMainLayout->addWidget(calculationInfoLabel);
        
        financeGroup->setLayout(financeMainLayout);
        mainLayout->addWidget(financeGroup);
        
        // Quick calculation buttons
        QGroupBox *quickCalcGroup = new QGroupBox("দ্রুত ক্যালকুলেশন");
        QHBoxLayout *quickCalcLayout = new QHBoxLayout;
        QPushButton *calc50Btn = new QPushButton("৫০% অ্যাডভান্স");
        QPushButton *calc60Btn = new QPushButton("৬০% অ্যাডভান্স");
        QPushButton *calc70Btn = new QPushButton("৭০% অ্যাডভান্স");
        QPushButton *calc100Btn = new QPushButton("সম্পূর্ণ পেমেন্ট");
        quickCalcLayout->addWidget(calc50Btn);
        quickCalcLayout->addWidget(calc60Btn);
        quickCalcLayout->addWidget(calc70Btn);
        quickCalcLayout->addWidget(calc100Btn);
        quickCalcGroup->setLayout(quickCalcLayout);
        mainLayout->addWidget(quickCalcGroup);
        
        // Project Points Group
        QGroupBox *pointsGroup = new QGroupBox("প্রজেক্ট পয়েন্টস");
        QVBoxLayout *pointsLayout = new QVBoxLayout;
        pointsEdit = new QTextEdit;
        pointsEdit->setPlaceholderText("প্রজেক্ট পয়েন্ট লিখুন (প্রতি লাইনে একটি)");
        pointsEdit->setMaximumHeight(60);
        QPushButton *addPointBtn = new QPushButton("পয়েন্ট যোগ করুন");
        pointsList = new QListWidget;
        pointsLayout->addWidget(pointsEdit);
        pointsLayout->addWidget(addPointBtn);
        pointsLayout->addWidget(pointsList);
        pointsGroup->setLayout(pointsLayout);
        mainLayout->addWidget(pointsGroup);
        
        // Action Buttons
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        QPushButton *generateBtn = new QPushButton("পিডিএফ তৈরি করুন");
        QPushButton *saveBtn = new QPushButton("প্রজেক্ট সেভ করুন");
        QPushButton *clearBtn = new QPushButton("ফর্ম পরিষ্কার করুন");
        buttonLayout->addWidget(saveBtn);
        buttonLayout->addWidget(generateBtn);
        buttonLayout->addWidget(clearBtn);
        mainLayout->addLayout(buttonLayout);
        
        // Verification Notice
        QLabel *noticeLabel = new QLabel(
            "<b>নোটিশ:</b> পয়েন্টগুলি যাচাই করুন, আমি, নাদের মাহবুব খান, এই পয়েন্টগুলি ছাড়া "
            "অন্য কোন পয়েন্টে কাজ করব না। অর্ডার নিশ্চিত করার পর হঠাৎ সিদ্ধান্ত পরিবর্তন প্রযোজ্য নয়।");
        noticeLabel->setWordWrap(true);
        noticeLabel->setStyleSheet("QLabel { background-color: #fff3cd; padding: 10px; "
                                   "border: 1px solid #ffeaa7; border-radius: 4px; }");
        mainLayout->addWidget(noticeLabel);
        
        // Projects Management Tab
        QWidget *projectsTab = new QWidget;
        tabWidget->addTab(projectsTab, "প্রজেক্ট ম্যানেজমেন্ট");
        
        QVBoxLayout *projectsLayout = new QVBoxLayout(projectsTab);
        
        // Filter controls
        QHBoxLayout *filterLayout = new QHBoxLayout;
        filterLayout->addWidget(new QLabel("স্ট্যাটাস ফিল্টার:"));
        projectStatusFilter = new QComboBox;
        projectStatusFilter->addItems({"সকল", "সম্পূর্ণ পেইড", "আংশিক পেইড", "বাকি আছে"});
        filterLayout->addWidget(projectStatusFilter);
        filterLayout->addWidget(new QPushButton("রিফ্রেশ"));
        filterLayout->addStretch();
        projectsLayout->addLayout(filterLayout);
        
        // Projects table
        projectsTable = new QTableWidget;
        projectsTable->setColumnCount(8);
        projectsTable->setHorizontalHeaderLabels({
            "আইডি", "ক্লায়েন্ট", "ফোন", "তারিখ", "মোট বিল", "পেইড", "বাকি", "অ্যাকশন"
        });
        projectsTable->setAlternatingRowColors(true);
        projectsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        projectsLayout->addWidget(projectsTable);
        
        // Dashboard Tab
        QWidget *dashboardTab = new QWidget;
        tabWidget->addTab(dashboardTab, "ড্যাশবোর্ড");
        
        QVBoxLayout *dashboardLayout = new QVBoxLayout(dashboardTab);
        QLabel *revenueLabel = new QLabel("মাসিক আয়");
        revenueLabel->setAlignment(Qt::AlignCenter);
        revenueLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; margin: 10px; }");
        dashboardLayout->addWidget(revenueLabel);
        
        chartView = new QChartView;
        chartView->setRenderHint(QPainter::Antialiasing);
        dashboardLayout->addWidget(chartView);
        
        // Connect signals
        connect(addPointBtn, &QPushButton::clicked, this, &ProjectTracker::addPoint);
        connect(saveBtn, &QPushButton::clicked, this, &ProjectTracker::saveProject);
        connect(generateBtn, &QPushButton::clicked, this, &ProjectTracker::generatePDFs);
        connect(clearBtn, &QPushButton::clicked, this, &ProjectTracker::clearForm);
        connect(customAdvanceBtn, &QPushButton::clicked, this, &ProjectTracker::setCustomAdvance);
        
        // Financial calculation signals
        connect(billEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
                this, &ProjectTracker::calculateDue);
        connect(advancedEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), 
                this, &ProjectTracker::calculateDue);
        connect(autoCalculateCheckBox, &QCheckBox::toggled, 
                this, &ProjectTracker::toggleAutoCalculation);
        connect(applyPercentBtn, &QPushButton::clicked, 
                this, &ProjectTracker::applyPercentage);
        
        // Quick calculation button signals
        connect(calc50Btn, &QPushButton::clicked, [this]() { calculateAdvancedByPercentage(50); });
        connect(calc60Btn, &QPushButton::clicked, [this]() { calculateAdvancedByPercentage(60); });
        connect(calc70Btn, &QPushButton::clicked, [this]() { calculateAdvancedByPercentage(70); });
        connect(calc100Btn, &QPushButton::clicked, [this]() { calculateAdvancedByPercentage(100); });
        
        // Project management signals
        connect(projectStatusFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &ProjectTracker::loadProjects);
        
        // Initialize calculation
        calculateDue();
    }
    
    void setupDatabase() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("projects.db");
        if (!db.open()) {
            QMessageBox::critical(this, "ডাটাবেস ত্রুটি", db.lastError().text());
            return;
        }
        
        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS projects ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "client_name TEXT, "
                   "client_phone TEXT, "
                   "date TEXT, "
                   "bill REAL, "
                   "advanced REAL, "
                   "due REAL, "
                   "points TEXT, "
                   "status TEXT DEFAULT 'pending', "
                   "last_payment_date TEXT)");
        
        query.exec("CREATE TABLE IF NOT EXISTS revenue ("
                   "month TEXT PRIMARY KEY, "
                   "amount REAL)");
        
        query.exec("CREATE TABLE IF NOT EXISTS payment_history ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "project_id INTEGER, "
                   "amount REAL, "
                   "payment_date TEXT, "
                   "note TEXT, "
                   "FOREIGN KEY(project_id) REFERENCES projects(id))");
    }
    
    void calculateDue() {
        if (autoCalculateCheckBox->isChecked()) {
            double bill = billEdit->value();
            double advanced = advancedEdit->value();
            double due = bill - advanced;
            
            dueEdit->setValue(due);
            
            // Update calculation info
            if (bill > 0) {
                double percentage = (advanced / bill) * 100;
                QString info = QString("অ্যাডভান্স: ৳%1 (%2%) | বাকি: ৳%3 (%4%)")
                    .arg(advanced, 0, 'f', 2)
                    .arg(percentage, 0, 'f', 1)
                    .arg(due, 0, 'f', 2)
                    .arg(100 - percentage, 0, 'f', 1);
                calculationInfoLabel->setText(info);
            } else {
                calculationInfoLabel->clear();
            }
        }
    }
    
    void toggleAutoCalculation(bool checked) {
        dueEdit->setReadOnly(checked);
        if (checked) {
            dueEdit->setStyleSheet("QDoubleSpinBox:read-only { background-color: #f0f0f0; }");
            calculateDue();
        } else {
            dueEdit->setStyleSheet("");
        }
    }
    
    void applyPercentage() {
        calculateAdvancedByPercentage(percentageSpinBox->value());
    }
    
    void calculateAdvancedByPercentage(int percentage) {
        double bill = billEdit->value();
        double advanced = bill * (percentage / 100.0);
        advancedEdit->setValue(advanced);
        calculateDue();
    }
    
    void setCustomAdvance() {
        bool ok;
        double amount = QInputDialog::getDouble(this, "কাস্টম অ্যাডভান্স", 
                                               "অ্যাডভান্স পরিমাণ লিখুন (৳):", 
                                               advancedEdit->value(), 0, billEdit->value(), 2, &ok);
        if (ok) {
            advancedEdit->setValue(amount);
            calculateDue();
        }
    }
    
    void addPoint() {
        QString point = pointsEdit->toPlainText().trimmed();
        if (!point.isEmpty()) {
            pointsList->addItem(point);
            pointsEdit->clear();
        }
    }
    
    void saveProject() {
        // Validate required fields
        if (clientNameEdit->text().isEmpty()) {
            QMessageBox::warning(this, "সতর্কতা", "দয়া করে ক্লায়েন্টের নাম লিখুন!");
            return;
        }
        
        if (billEdit->value() == 0) {
            QMessageBox::warning(this, "সতর্কতা", "দয়া করে বিলের পরিমাণ লিখুন!");
            return;
        }
        
        QString status;
        if (advancedEdit->value() >= billEdit->value()) {
            status = "paid";
        } else if (advancedEdit->value() > 0) {
            status = "partial";
        } else {
            status = "pending";
        }
        
        QSqlQuery query;
        query.prepare("INSERT INTO projects (client_name, client_phone, date, bill, advanced, due, points, status, last_payment_date) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
        query.addBindValue(clientNameEdit->text());
        query.addBindValue(clientPhoneEdit->text());
        query.addBindValue(dateEdit->date().toString("yyyy-MM-dd"));
        query.addBindValue(billEdit->value());
        query.addBindValue(advancedEdit->value());
        query.addBindValue(dueEdit->value());
        
        QStringList points;
        for (int i = 0; i < pointsList->count(); ++i) {
            points.append(pointsList->item(i)->text());
        }
        query.addBindValue(points.join("\n"));
        query.addBindValue(status);
        query.addBindValue(dateEdit->date().toString("yyyy-MM-dd"));
        
        if (query.exec()) {
            QMessageBox::information(this, "সফল", "প্রজেক্ট সফলভাবে সেভ হয়েছে!");
            updateRevenue();
            loadDashboard();
            loadProjects();
        } else {
            QMessageBox::critical(this, "ত্রুটি", "প্রজেক্ট সেভ করতে ব্যর্থ: " + query.lastError().text());
        }
    }
    
    void loadProjects() {
        projectsTable->setRowCount(0);
        
        QString filterCondition = "";
        if (projectStatusFilter && projectStatusFilter->currentIndex() > 0) {
            switch(projectStatusFilter->currentIndex()) {
                case 1: filterCondition = " WHERE status = 'paid'"; break;
                case 2: filterCondition = " WHERE status = 'partial'"; break;
                case 3: filterCondition = " WHERE status = 'pending' OR due > 0"; break;
            }
        }
        
        QSqlQuery query("SELECT id, client_name, client_phone, date, bill, advanced, due FROM projects" + filterCondition + " ORDER BY date DESC");
        
        int row = 0;
        while (query.next()) {
            projectsTable->insertRow(row);
            
            for (int col = 0; col < 7; col++) {
                QTableWidgetItem *item = new QTableWidgetItem(query.value(col).toString());
                if (col >= 4 && col <= 6) {
                    item->setText(QString("৳ %1").arg(query.value(col).toDouble(), 0, 'f', 2));
                }
                projectsTable->setItem(row, col, item);
            }
            
            // Add update payment button
            QPushButton *updateBtn = new QPushButton("পেমেন্ট আপডেট");
            connect(updateBtn, &QPushButton::clicked, [this, id = query.value(0).toInt()]() {
                updatePayment(id);
            });
            projectsTable->setCellWidget(row, 7, updateBtn);
            
            row++;
        }
        
        projectsTable->resizeColumnsToContents();
    }
    
    void updatePayment(int projectId) {
        // Get current project details
        QSqlQuery query;
        query.prepare("SELECT client_name, bill, advanced, due FROM projects WHERE id = ?");
        query.addBindValue(projectId);
        query.exec();
        
        if (query.next()) {
            QString clientName = query.value(0).toString();
            double totalBill = query.value(1).toDouble();
            double currentPaid = query.value(2).toDouble();
            double currentDue = query.value(3).toDouble();
            
            bool ok;
            double newPayment = QInputDialog::getDouble(this, 
                QString("পেমেন্ট আপডেট - %1").arg(clientName),
                QString("বর্তমান বাকি: ৳%1\nনতুন পেমেন্ট পরিমাণ:").arg(currentDue, 0, 'f', 2),
                0, 0, currentDue, 2, &ok);
            
            if (ok && newPayment > 0) {
                double newAdvanced = currentPaid + newPayment;
                double newDue = totalBill - newAdvanced;
                
                QString status;
                if (newAdvanced >= totalBill) {
                    status = "paid";
                } else if (newAdvanced > 0) {
                    status = "partial";
                } else {
                    status = "pending";
                }
                
                // Update project
                query.prepare("UPDATE projects SET advanced = ?, due = ?, status = ?, last_payment_date = ? WHERE id = ?");
                query.addBindValue(newAdvanced);
                query.addBindValue(newDue);
                query.addBindValue(status);
                query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
                query.addBindValue(projectId);
                
                if (query.exec()) {
                    // Add to payment history
                    query.prepare("INSERT INTO payment_history (project_id, amount, payment_date, note) VALUES (?, ?, ?, ?)");
                    query.addBindValue(projectId);
                    query.addBindValue(newPayment);
                    query.addBindValue(QDate::currentDate().toString("yyyy-MM-dd"));
                    query.addBindValue(QString("পেমেন্ট রিসিভড"));
                    query.exec();
                    
                    QMessageBox::information(this, "সফল", 
                        QString("পেমেন্ট আপডেট হয়েছে!\nনতুন পেমেন্ট: ৳%1\nমোট পেইড: ৳%2\nবাকি: ৳%3")
                        .arg(newPayment, 0, 'f', 2)
                        .arg(newAdvanced, 0, 'f', 2)
                        .arg(newDue, 0, 'f', 2));
                    
                    loadProjects();
                    loadDashboard();
                }
            }
        }
    }
    
    void generatePDFs() {
        QString dir = QFileDialog::getExistingDirectory(this, "আউটপুট ডিরেক্টরি নির্বাচন করুন");
        if (dir.isEmpty()) return;
        
        // Generate Order PDF
        QPdfWriter orderWriter(dir + "/order_" + clientNameEdit->text() + ".pdf");
        orderWriter.setPageSize(QPageSize(QPageSize::A4));
        orderWriter.setPageMargins(QMargins(30, 30, 30, 30));
        
        QPainter orderPainter(&orderWriter);
        drawPDFContent(&orderPainter, "অর্ডার কনফার্মেশন");
        orderPainter.end();
        
        // Generate Client Copy PDF
        QPdfWriter clientWriter(dir + "/client_" + clientNameEdit->text() + ".pdf");
        clientWriter.setPageSize(QPageSize(QPageSize::A4));
        clientWriter.setPageMargins(QMargins(30, 30, 30, 30));
        
        QPainter clientPainter(&clientWriter);
        drawPDFContent(&clientPainter, "ক্লায়েন্ট কপি");
        clientPainter.end();
        
        QMessageBox::information(this, "সফল", "পিডিএফ সফলভাবে তৈরি হয়েছে!");
    }
    
    void drawPDFContent(QPainter *painter, const QString &title) {
        QFont titleFont = painter->font();
        titleFont.setPointSize(16);
        titleFont.setBold(true);
        painter->setFont(titleFont);
        painter->drawText(100, 100, title);
        
        QFont normalFont = painter->font();
        normalFont.setPointSize(10);
        normalFont.setBold(false);
        painter->setFont(normalFont);
        
        int yPos = 150;
        painter->drawText(100, yPos, "ক্লায়েন্টের নাম: " + clientNameEdit->text());
        yPos += 20;
        painter->drawText(100, yPos, "ফোন: " + clientPhoneEdit->text());
        yPos += 20;
        painter->drawText(100, yPos, "তারিখ: " + dateEdit->date().toString("dd MMMM yyyy"));
        yPos += 30;
        
        painter->drawText(100, yPos, "আর্থিক বিবরণ:");
        yPos += 20;
        painter->drawText(120, yPos, QString("মোট বিল: ৳%1").arg(billEdit->value(), 0, 'f', 2));
        yPos += 20;
        painter->drawText(120, yPos, QString("অ্যাডভান্স: ৳%1").arg(advancedEdit->value(), 0, 'f', 2));
        yPos += 20;
        painter->drawText(120, yPos, QString("বাকি: ৳%1").arg(dueEdit->value(), 0, 'f', 2));
        yPos += 30;
        
        painter->drawText(100, yPos, "প্রজেক্ট পয়েন্টস:");
        yPos += 20;
        for (int i = 0; i < pointsList->count(); ++i) {
            painter->drawText(120, yPos, QString::number(i+1) + ". " + pointsList->item(i)->text());
            yPos += 20;
        }
        
        yPos += 30;
        QFont noticeFont = painter->font();
        noticeFont.setBold(true);
        painter->setFont(noticeFont);
        painter->drawText(100, yPos, "নোটিশ:");
        noticeFont.setBold(false);
        painter->setFont(noticeFont);
        yPos += 20;
        painter->drawText(100, yPos, "পয়েন্টগুলি যাচাই করুন, আমি, নাদের মাহবুব খান, এই পয়েন্টগুলি");
        yPos += 20;
        painter->drawText(100, yPos, "ছাড়া অন্য কোন পয়েন্টে কাজ করব না। অর্ডার নিশ্চিত করার পর");
        yPos += 20;
        painter->drawText(100, yPos, "হঠাৎ সিদ্ধান্ত পরিবর্তন প্রযোজ্য নয়।");
        
        // Add payment status
        yPos += 30;
        if (advancedEdit->value() >= billEdit->value()) {
            painter->drawText(100, yPos, "পেমেন্ট স্ট্যাটাস: সম্পূর্ণ পেইড");
        } else if (advancedEdit->value() > 0) {
            double percentage = (advancedEdit->value() / billEdit->value()) * 100;
            painter->drawText(100, yPos, QString("পেমেন্ট স্ট্যাটাস: %1% পেইড").arg(percentage, 0, 'f', 1));
        } else {
            painter->drawText(100, yPos, "পেমেন্ট স্ট্যাটাস: পেন্ডিং");
        }
    }
    
    void updateRevenue() {
        QSqlQuery query;
        QString month = dateEdit->date().toString("yyyy-MM");
        double bill = billEdit->value();
        
        // Check if month exists
        query.prepare("SELECT amount FROM revenue WHERE month = ?");
        query.addBindValue(month);
        query.exec();
        
        if (query.next()) {
            // Update existing record
            double currentAmount = query.value(0).toDouble();
            query.prepare("UPDATE revenue SET amount = ? WHERE month = ?");
            query.addBindValue(currentAmount + bill);
            query.addBindValue(month);
        } else {
            // Insert new record
            query.prepare("INSERT INTO revenue (month, amount) VALUES (?, ?)");
            query.addBindValue(month);
            query.addBindValue(bill);
        }
        query.exec();
    }
    
    void loadDashboard() {
        QSqlQuery query("SELECT month, amount FROM revenue ORDER BY month");
        
        QBarSeries *series = new QBarSeries();
        QBarSet *set = new QBarSet("আয় (৳)");
        
        QStringList months;
        while (query.next()) {
            months << query.value(0).toString();
            *set << query.value(1).toDouble();
        }
        
        series->append(set);
        
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("মাসিক আয়");
        chart->setAnimationOptions(QChart::SeriesAnimations);
        
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(months);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        
        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("পরিমাণ (৳)");
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);
        
        chartView->setChart(chart);
    }
    
    void clearForm() {
        clientPhoneEdit->clear();
        billEdit->setValue(0);
        advancedEdit->setValue(0);
        dueEdit->setValue(0);
        pointsEdit->clear();
        pointsList->clear();
        calculationInfoLabel->clear();
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Set application font to support Bangla
    QFont appFont = app.font();
    appFont.setFamily("Noto Sans Bengali");
    app.setFont(appFont);
    
    ProjectTracker window;
    window.show();
    
    return app.exec();
}