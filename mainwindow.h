#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QListWidgetItem>
#include <QListWidget>
#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void addPersonToTable(const QString &name, const QString &dob, const QString &gender, const QString &height, const QString &weight);

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            m_dragging = true;
            m_dragPosition = event->globalPos() - frameGeometry().topLeft();
            event->accept();
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (m_dragging && (event->buttons() & Qt::LeftButton)) {
            move(event->globalPos() - m_dragPosition);
            event->accept();
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            m_dragging = false;
            event->accept();
        }
    }

private slots:
    void on_chat_2_clicked();

    void on_patient_2_clicked();

    void on_add_clicked();

    void on_pdf_clicked();

   // void on_excel_clicked();

    void saveTableToPdf(const QString &filePath);

    void initializeDatabase();

    void addPersonToDatabase(const QString &name, const QString &dob, const QString &gender, const QString &height, const QString &weight);
   // void saveTableToExcel(const QString &filePath);

    void loadPatientsFromDatabase();

    void deletePersonFromDatabase(int row);

    void on_patientSearch_textChanged(const QString &arg1);

    void on_GenderBox_currentTextChanged(const QString &arg1);

    void on_remove_clicked();

    void reNumber();
    void on_searchbutton_clicked();

    void on_newchatbutton_clicked();

    void onNetworkReply(QNetworkReply *reply);

    void loadTopic(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    bool m_dragging = false;
    QPoint m_dragPosition;
    QNetworkAccessManager *networkManager;
    int personCount{};  // Kişi sayısı için sayaç
    const QString apiKey="sk-proj-vwA5Cr5SxGmf2tW4QawtT3BlbkFJKviqYh1KWjVfZPaDVCmk";
    QMap<QListWidgetItem*, QString> topicHistory;
    // QStandardItemModel *model;
    QListWidgetItem *currentItem; // Keep track of the current item
    QMap<QListWidgetItem*, QJsonArray> messageHistory; // Mesaj geçmişi

};
#endif // MAINWINDOW_H
