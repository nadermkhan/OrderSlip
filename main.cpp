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
    
public:
    ProjectTracker(QWidget *parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupDatabase();
        loadDashboard();
    }

private:
    void setupUI() {
        setWindowTitle("Nader's Project Agreement Slip");
        resize(900, 700);
        
        // Main tab widget
        QTabWidget *tabWidget = new QTabWidget;
        setCentralWidget(tabWidget);
        
        // Project Entry Tab
        QWidget *entryTab = new QWidget;
        tabWidget->addTab(entryTab, "Project Entry");
        
        QVBoxLayout *mainLayout = new QVBoxLayout(entryTab);
        
        // Client Information Group
        QGroupBox *clientGroup = new QGroupBox("Client Information");
        QFormLayout *clientLayout = new QFormLayout;
        clientNameEdit = new QLineEdit("");
        clientPhoneEdit = new QLineEdit;
        dateEdit = new QDateEdit(QDate::currentDate());
        dateEdit->setDisplayFormat("dd MMMM yyyy");
        clientLayout->addRow("Client Name:", clientNameEdit);
        clientLayout->addRow("Phone:", clientPhoneEdit);
        clientLayout->addRow("Date:", dateEdit);
        clientGroup->setLayout(clientLayout);
        mainLayout->addWidget(clientGroup);
        
        // Financial Details Group
        QGroupBox *financeGroup = new QGroupBox("Financial Details");
        QVBoxLayout *financeMainLayout = new QVBoxLayout;
        QFormLayout *financeLayout = new QFormLayout;
        
        // Bill amount
        billEdit = new QDoubleSpinBox;
        billEdit->setRange(0, 999999.99);
        billEdit->setDecimals(2);
        billEdit->setPrefix("$");
        billEdit->setSingleStep(100);
        financeLayout->addRow("Total Bill:", billEdit);
        
        // Auto-calculation controls
        QHBoxLayout *autoCalcLayout = new QHBoxLayout;
        autoCalculateCheckBox = new QCheckBox("Auto Calculate");
        autoCalculateCheckBox->setChecked(true);
        percentageSpinBox = new QSpinBox;
        percentageSpinBox->setRange(0, 100);
        percentageSpinBox->setValue(60);
        percentageSpinBox->setSuffix("%");
        QPushButton *applyPercentBtn = new QPushButton("Apply %");
        autoCalcLayout->addWidget(autoCalculateCheckBox);
        autoCalcLayout->addWidget(new QLabel("Advanced:"));
        autoCalcLayout->addWidget(percentageSpinBox);
        autoCalcLayout->addWidget(applyPercentBtn);
        autoCalcLayout->addStretch();
        
        // Advanced amount
        advancedEdit = new QDoubleSpinBox;
        advancedEdit->setRange(0, 999999.99);
        advancedEdit->setDecimals(2);
        advancedEdit->setPrefix("$");
        advancedEdit->setSingleStep(100);
        financeLayout->addRow("Advanced Payment:", advancedEdit);
        
        // Due amount
        dueEdit = new QDoubleSpinBox;
        dueEdit->setRange(0, 999999.99);
        dueEdit->setDecimals(2);
        dueEdit->setPrefix("$");
        dueEdit->setSingleStep(100);
        dueEdit->setReadOnly(true); // Make it read-only by default
        dueEdit->setStyleSheet("QDoubleSpinBox:read-only { background-color: #f0f0f0; }");
        financeLayout->addRow("Due Amount:", dueEdit);
        
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
        QGroupBox *quickCalcGroup = new QGroupBox("Quick Calculations");
        QHBoxLayout *quickCalcLayout = new QHBoxLayout;
        QPushButton *calc50Btn = new QPushButton("50% Advanced");
        QPushButton *calc60Btn = new QPushButton("60% Advanced");
        QPushButton *calc70Btn = new QPushButton("70% Advanced");
        QPushButton *calc100Btn = new QPushButton("Full Payment");
        quickCalcLayout->addWidget(calc50Btn);
        quickCalcLayout->addWidget(calc60Btn);
        quickCalcLayout->addWidget(calc70Btn);
        quickCalcLayout->addWidget(calc100Btn);
        quickCalcGroup->setLayout(quickCalcLayout);
        mainLayout->addWidget(quickCalcGroup);
        
        // Project Points Group
        QGroupBox *pointsGroup = new QGroupBox("Project Points");
        QVBoxLayout *pointsLayout = new QVBoxLayout;
        pointsEdit = new QTextEdit;
        pointsEdit->setPlaceholderText("Enter project points (one per line)");
        pointsEdit->setMaximumHeight(60);
        QPushButton *addPointBtn = new QPushButton("Add Point");
        pointsList = new QListWidget;
        pointsLayout->addWidget(pointsEdit);
        pointsLayout->addWidget(addPointBtn);
        pointsLayout->addWidget(pointsList);
        pointsGroup->setLayout(pointsLayout);
        mainLayout->addWidget(pointsGroup);
        
        // Action Buttons
        QHBoxLayout *buttonLayout = new QHBoxLayout;
        QPushButton *generateBtn = new QPushButton("Generate PDFs");
        QPushButton *saveBtn = new QPushButton("Save Project");
        QPushButton *clearBtn = new QPushButton("Clear Form");
        buttonLayout->addWidget(saveBtn);
        buttonLayout->addWidget(generateBtn);
        buttonLayout->addWidget(clearBtn);
        mainLayout->addLayout(buttonLayout);
        
        // Verification Notice
        QLabel *noticeLabel = new QLabel(
            "<b>Notice:</b> Verify the points, I, Nader Mahbub Khan, will not work on any points "
            "except these. Sudden change on decision after confirming order is not applicable.");
        noticeLabel->setWordWrap(true);
        noticeLabel->setStyleSheet("QLabel { background-color: #fff3cd; padding: 10px; "
                                   "border: 1px solid #ffeaa7; border-radius: 4px; }");
        mainLayout->addWidget(noticeLabel);
        
        // Dashboard Tab
        QWidget *dashboardTab = new QWidget;
        tabWidget->addTab(dashboardTab, "Dashboard");
        
        QVBoxLayout *dashboardLayout = new QVBoxLayout(dashboardTab);
        QLabel *revenueLabel = new QLabel("Monthly Revenue");
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
        
        // Initialize calculation
        calculateDue();
    }
    
    void setupDatabase() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("projects.db");
        if (!db.open()) {
            QMessageBox::critical(this, "Database Error", db.lastError().text());
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
                   "points TEXT)");
        
        query.exec("CREATE TABLE IF NOT EXISTS revenue ("
                   "month TEXT PRIMARY KEY, "
                   "amount REAL)");
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
                QString info = QString("Advanced: $%1 (%2%) | Due: $%3 (%4%)")
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
            QMessageBox::warning(this, "Warning", "Please enter client name!");
            return;
        }
        
        if (billEdit->value() == 0) {
            QMessageBox::warning(this, "Warning", "Please enter bill amount!");
            return;
        }
        
        QSqlQuery query;
        query.prepare("INSERT INTO projects (client_name, client_phone, date, bill, advanced, due, points) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?)");
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
        
        if (query.exec()) {
            QMessageBox::information(this, "Success", "Project saved successfully!");
            updateRevenue();
            loadDashboard();
        } else {
            QMessageBox::critical(this, "Error", "Failed to save project: " + query.lastError().text());
        }
    }
    
    void generatePDFs() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Output Directory");
        if (dir.isEmpty()) return;
        
        // Generate Order PDF
        QPdfWriter orderWriter(dir + "/order_" + clientNameEdit->text() + ".pdf");
        orderWriter.setPageSize(QPageSize(QPageSize::A4));
        orderWriter.setPageMargins(QMargins(30, 30, 30, 30));
        
        QPainter orderPainter(&orderWriter);
        drawPDFContent(&orderPainter, "Order Confirmation");
        orderPainter.end();
        
        // Generate Client Copy PDF
        QPdfWriter clientWriter(dir + "/client_" + clientNameEdit->text() + ".pdf");
        clientWriter.setPageSize(QPageSize(QPageSize::A4));
        clientWriter.setPageMargins(QMargins(30, 30, 30, 30));
        
        QPainter clientPainter(&clientWriter);
        drawPDFContent(&clientPainter, "Client Copy");
        clientPainter.end();
        
        QMessageBox::information(this, "Success", "PDFs generated successfully!");
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
        painter->drawText(100, yPos, "Client Name: " + clientNameEdit->text());
        yPos += 20;
        painter->drawText(100, yPos, "Phone: " + clientPhoneEdit->text());
        yPos += 20;
        painter->drawText(100, yPos, "Date: " + dateEdit->date().toString("dd MMMM yyyy"));
        yPos += 30;
        
        painter->drawText(100, yPos, "Financial Details:");
        yPos += 20;
        painter->drawText(120, yPos, QString("Bill: $%1").arg(billEdit->value(), 0, 'f', 2));
        yPos += 20;
        painter->drawText(120, yPos, QString("Advanced: $%1").arg(advancedEdit->value(), 0, 'f', 2));
        yPos += 20;
        painter->drawText(120, yPos, QString("Due: $%1").arg(dueEdit->value(), 0, 'f', 2));
        yPos += 30;
        
        painter->drawText(100, yPos, "Project Points:");
        yPos += 20;
        for (int i = 0; i < pointsList->count(); ++i) {
            painter->drawText(120, yPos, QString::number(i+1) + ". " + pointsList->item(i)->text());
            yPos += 20;
        }
        
        yPos += 30;
        QFont noticeFont = painter->font();
        noticeFont.setBold(true);
        painter->setFont(noticeFont);
        painter->drawText(100, yPos, "Notice:");
        noticeFont.setBold(false);
        painter->setFont(noticeFont);
        yPos += 20;
        painter->drawText(100, yPos, "Verify the points, I, Nader Mahbub Khan, will not work on any points");
        yPos += 20;
        painter->drawText(100, yPos, "except these. Sudden change on decision after confirming order is not applicable.");
        
        // Add payment status
        yPos += 30;
        if (advancedEdit->value() >= billEdit->value()) {
            painter->drawText(100, yPos, "Payment Status: FULLY PAID");
        } else if (advancedEdit->value() > 0) {
            double percentage = (advancedEdit->value() / billEdit->value()) * 100;
            painter->drawText(100, yPos, QString("Payment Status: %1% PAID").arg(percentage, 0, 'f', 1));
        } else {
            painter->drawText(100, yPos, "Payment Status: PENDING");
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
        QBarSet *set = new QBarSet("Revenue ($)");
        
        QStringList months;
        while (query.next()) {
            months << query.value(0).toString();
            *set << query.value(1).toDouble();
        }
        
        series->append(set);
        
        QChart *chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Monthly Revenue");
        chart->setAnimationOptions(QChart::SeriesAnimations);
        
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(months);
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);
        
        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("Amount ($)");
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
    
    ProjectTracker window;
    window.show();
    
    return app.exec();
}