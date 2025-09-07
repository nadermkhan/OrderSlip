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
#include <QChartView>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QValueAxis>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class ProjectTracker : public QMainWindow {
private:
    // Input fields
    QLineEdit *clientNameEdit;
    QLineEdit *clientPhoneEdit;
    QDateEdit *dateEdit;
    QLineEdit *billEdit;
    QLineEdit *advancedEdit;
    QLineEdit *dueEdit;
    QTextEdit *pointsEdit;
    QListWidget *pointsList;
    
    // Dashboard elements
    QChartView *chartView;
    
public:
    ProjectTracker(QWidget *parent = nullptr) : QMainWindow(parent) {
        setupUI();
        setupDatabase();
        loadDashboard();
    }

private:
    void setupUI() {
        setWindowTitle("Freelance Project Tracker");
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
        clientNameEdit = new QLineEdit("Omor Faruk");
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
        QFormLayout *financeLayout = new QFormLayout;
        billEdit = new QLineEdit;
        advancedEdit = new QLineEdit;
        dueEdit = new QLineEdit;
        financeLayout->addRow("Bill:", billEdit);
        financeLayout->addRow("Advanced:", advancedEdit);
        financeLayout->addRow("Due:", dueEdit);
        financeGroup->setLayout(financeLayout);
        mainLayout->addWidget(financeGroup);
        
        // Project Points Group
        QGroupBox *pointsGroup = new QGroupBox("Project Points");
        QVBoxLayout *pointsLayout = new QVBoxLayout;
        pointsEdit = new QTextEdit;
        pointsEdit->setPlaceholderText("Enter project points (one per line)");
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
    
    void addPoint() {
        QString point = pointsEdit->toPlainText().trimmed();
        if (!point.isEmpty()) {
            pointsList->addItem(point);
            pointsEdit->clear();
        }
    }
    
    void saveProject() {
        QSqlQuery query;
        query.prepare("INSERT INTO projects (client_name, client_phone, date, bill, advanced, due, points) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?)");
        query.addBindValue(clientNameEdit->text());
        query.addBindValue(clientPhoneEdit->text());
        query.addBindValue(dateEdit->date().toString("yyyy-MM-dd"));
        query.addBindValue(billEdit->text().toDouble());
        query.addBindValue(advancedEdit->text().toDouble());
        query.addBindValue(dueEdit->text().toDouble());
        
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
        orderWriter.setPageSize(QPdfWriter::A4);
        orderWriter.setPageMargins(QMargins(30, 30, 30, 30));
        
        QPainter orderPainter(&orderWriter);
        drawPDFContent(&orderPainter, "Order Confirmation");
        orderPainter.end();
        
        // Generate Client Copy PDF
        QPdfWriter clientWriter(dir + "/client_" + clientNameEdit->text() + ".pdf");
        clientWriter.setPageSize(QPdfWriter::A4);
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
        painter->drawText(120, yPos, "Bill: $" + billEdit->text());
        yPos += 20;
        painter->drawText(120, yPos, "Advanced: $" + advancedEdit->text());
        yPos += 20;
        painter->drawText(120, yPos, "Due: $" + dueEdit->text());
        yPos += 30;
        
        painter->drawText(100, yPos, "Project Points:");
        yPos += 20;
        for (int i = 0; i < pointsList->count(); ++i) {
            painter->drawText(120, yPos, QString::number(i+1) + ". " + pointsList->item(i)->text());
            yPos += 20;
        }
        
        yPos += 30;
        painter->drawText(100, yPos, "Notice: Verify the points, I, Nader Mahbub Khan, will not work on any points "
                         "except these. Sudden change on decision after confirming order is not applicable.");
    }
    
    void updateRevenue() {
        QSqlQuery query;
        QString month = dateEdit->date().toString("yyyy-MM");
        double bill = billEdit->text().toDouble();
        
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
        billEdit->clear();
        advancedEdit->clear();
        dueEdit->clear();
        pointsEdit->clear();
        pointsList->clear();
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    ProjectTracker window;
    window.show();
    
    return app.exec();
}
