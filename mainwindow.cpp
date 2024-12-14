#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addnew.h"
#include <QFileDialog>
#include <QPdfWriter>
#include <QPainter>
#include <QtSql/QSqlDriverPlugin>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    networkManager(new QNetworkAccessManager(this)),
    currentItem(nullptr)
{
    ui->setupUi(this);
    ui->maxside->hide();
    ui->sidemin->hide();
    ui->normalize->hide();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode());
    initializeDatabase();
    loadPatientsFromDatabase();

    // İlk öğeyi oluştur
    on_newchatbutton_clicked();

    // Liste öğelerine tıklama olayı için bağlantı

    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::loadTopic);
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onNetworkReply);
}




MainWindow::~MainWindow()
{

    delete ui;

}



void MainWindow::initializeDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("patients"); // Veritabanı isminizi yazın
    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.setUserName("root");        // Kullanıcı adınızı yazın




    if (!db.open()) {
        qDebug() << "Veritabanı açılamadı:" << db.lastError().text();
        return;
    } else {
        qDebug() << "MySQL veritabanına başarıyla bağlandı!";
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS patients ("
               "id INTEGER PRIMARY KEY AUTO_INCREMENT,"
               "name VARCHAR(255),"
               "dob DATE,"
               "gender VARCHAR(10),"
               "height DOUBLE,"
               "weight DOUBLE)");
}

void MainWindow::addPersonToDatabase(const QString &name, const QString &dob, const QString &gender, const QString &height, const QString &weight)
{
    // Veritabanı bağlantısını al
    QSqlDatabase db = QSqlDatabase::database();

    // Veritabanı açık mı kontrolü
    if (!db.isOpen()) {
        qDebug() << "Veritabanı bağlantısı açık değil.";
        return;
    }

    // Dob'u (tarihi) uygun formata çevir
    QDate date = QDate::fromString(dob, "dd/MM/yyyy"); // Girdiyi dd/MM/yyyy formatından al
    QString formattedDob = date.toString("yyyy-MM-dd"); // MySQL için uygun formata çevir

    QSqlQuery query;

    // Hazırlanan sorgu ile veri ekleme
    query.prepare("INSERT INTO patients (name, dob, gender, height, weight) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(name);
    query.addBindValue(formattedDob); // MySQL formatında tarih
    query.addBindValue(gender);
    query.addBindValue(height);
    query.addBindValue(weight);

    // Sorguyu çalıştırma ve hata kontrolü
    if (!query.exec()) {
        qDebug() << "Veri eklenemedi:" << query.lastError().text();
    } else {
        qDebug() << "Kişi başarıyla eklendi:" << name;
    }
}

void MainWindow::loadPatientsFromDatabase()
{
    // Tabloyu temizleme
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0); // Satır sayısını sıfırla

    // Veritabanı bağlantısını al
    QSqlDatabase db = QSqlDatabase::database();

    // Veritabanı açık mı kontrolü
    if (!db.isOpen()) {
        qDebug() << "Veritabanı bağlantısı açık değil.";
        return;
    }

    // Sorgu ile veritabanından kayıtları alma
    QSqlQuery query("SELECT name, dob, gender, height, weight FROM patients");

    if (!query.exec()) {
        qDebug() << "Sorgu çalıştırılamadı:" << query.lastError().text();
        return;
    }



    // Kayıtları okuma ve tabloya ekleme
    int row = 0;
    while (query.next()) {
        QString name = query.value(0).toString();
        QString dob = query.value(1).toString();
        QString gender = query.value(2).toString();
        QString height = query.value(3).toString();
        QString weight = query.value(4).toString();


        // MySQL formatındaki tarihi "dd/MM/yyyy" formatına dönüştür
        QDate date = QDate::fromString(dob, "yyyy-MM-dd");
        QString formattedDob = date.toString("dd/MM/yyyy");

        qDebug() << "Eklenen Kayıt: " << name << ", " << formattedDob << ", " << gender << ", " << height << ", " << weight;

        // Satır ekleme ve hücreleri doldurma
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(name));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(formattedDob));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(gender));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(height));
        ui->tableWidget->setItem(row, 4, new QTableWidgetItem(weight));

        row++;
    }

    qDebug() << "Tüm hastalar başarıyla yüklendi.";
}

void MainWindow::deletePersonFromDatabase(int row)
{
    QString name = ui->tableWidget->item(row, 0)->text(); // Seçilen satırdaki ismi al

    // Veritabanı bağlantısını al
    QSqlDatabase db = QSqlDatabase::database();

    // Veritabanı açık mı kontrolü
    if (!db.isOpen()) {
        qDebug() << "Veritabanı bağlantısı açık değil.";
        return;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM patients WHERE name = ?");
    query.addBindValue(name);

    if (!query.exec()) {
        qDebug() << "Veri silinemedi:" << query.lastError().text();
    } else {
        qDebug() << "Kişi başarıyla silindi:" << name;
    }
}



void MainWindow::on_chat_2_clicked()
{
    if(ui->stackedWidget->currentIndex()!=0)
    {
        ui->stackedWidget->setCurrentIndex(0);
    }
}


void MainWindow::on_patient_2_clicked()
{
    if(ui->stackedWidget->currentIndex()!=1)
    {
        ui->stackedWidget->setCurrentIndex(1);
    }
}


void MainWindow::on_add_clicked()
{
    addnew *addNewPatient = new addnew(this); //parent olarak mainwindowu tanıtmak önemli this yazarak yapıyoruz
    addNewPatient->show();
}

void MainWindow::addPersonToTable(const QString &name, const QString &dob, const QString &gender, const QString &height, const QString &weight)
{
    personCount++;
    int rowPosition = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowPosition);

    //ui->tableWidget->setItem(rowPosition,0,new QTableWidgetItem(QString::number(personCount)));
    ui->tableWidget->setItem(rowPosition,0,new QTableWidgetItem(name));
    ui->tableWidget->setItem(rowPosition,1,new QTableWidgetItem(dob));
    ui->tableWidget->setItem(rowPosition,2,new QTableWidgetItem(gender));
    ui->tableWidget->setItem(rowPosition,3,new QTableWidgetItem(height));
    ui->tableWidget->setItem(rowPosition,4,new QTableWidgetItem(weight));

    // Veritabanına kaydetme
    addPersonToDatabase(name, dob, gender, height, weight);

}

void MainWindow::on_pdf_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save as PDF"),tr("output"),tr("PDF Files (*.pdf)"));

    if(!filePath.endsWith(".pdf")) //sonu .pdf ile bitmiyorsa kontrol eder ve kendi yazar
    {
        filePath += ".pdf";
    }
    if(!filePath.isEmpty())
    {
        saveTableToPdf(filePath);
    }
}

/*
void MainWindow::on_excel_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this,tr("Save as Excel"),tr("Excel Files (*.xlsx)"));
    if(!filePath.isEmpty())
    {
        saveTableToExcel(filePath);
    }
}
*/
void MainWindow::saveTableToPdf(const QString &filePath)
{
    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setResolution(300); // DPI
    QPainter painter(&pdfWriter);

    int rowCount = ui->tableWidget->rowCount();
    int columnCount = ui->tableWidget->columnCount();

    int rowHeight = 80;  // Satır yüksekliği (piksel cinsinden)
    int columnWidth = 300;  // Sütun genişliği (piksel cinsinden)

    int startX = 50;  // Başlangıç X konumu
    int startY = 50;  // Başlangıç Y konumu

    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            QString text = ui->tableWidget->item(row, col)->text();

            int x = startX + col * columnWidth;
            int y = startY + row * rowHeight;

            // Alternatif satırları renklendirme
            if (row % 2 == 0) {
                painter.fillRect(x, y, columnWidth, rowHeight, QColor(220, 220, 220)); // Açık gri renk
            } else {
                painter.fillRect(x, y, columnWidth, rowHeight, QColor(255, 255, 255)); // Beyaz renk
            }

            // Metni çiz
            painter.drawText(x + 10, y + 40, text); // Metni biraz içeriden başlat (10 piksel) ve biraz aşağıdan (40 piksel)

            // Hücre etrafına dikdörtgen çizgi ekleme
            painter.drawRect(x, y, columnWidth, rowHeight);
        }
    }

    painter.end();
}

/*
void MainWindow::saveTableToExcel(const QString &filePath)
{
    // QXlsx kütüphanesini kullanarak tabloyu Excel dosyasına kaydetme
    QXlsx::Document xlsx;

    int rowCount = ui->tableWidget->rowCount();
    int columnCount = ui->tableWidget->columnCount();

    for (int row = 0; row < rowCount; ++row) {
        for (int col = 0; col < columnCount; ++col) {
            QString text = ui->tableWidget->item(row, col)->text();
            xlsx.write(row + 1, col + 1, text); // Excel satır ve sütunları 1'den başlar
        }
    }

    xlsx.saveAs(filePath);
}
*/

void MainWindow::on_patientSearch_textChanged(const QString &arg1) //filtreleme işi
{
    for(int i=0; i<ui->tableWidget->rowCount(); i++)
    {
        QTableWidgetItem *nameItem = ui->tableWidget->item(i,0); //0 isim sütunu olan yer

        if(nameItem)
        {                                                                                                                   /*filtre, tablonun i numaralı satırının 0. sütunundaki (isim sütunu) öğeyi (QTableWidgetItem) temsil eder.
                                                                                                                                Bu öğe, tablodaki hücredeki veriyi temsil eder.
                                                                                                                                if(filtre) Kontrolü                                                                                                              Bu kontrol, filtrenin nullptr olmadığından (yani geçerli bir öğe olup olmadığını) emin olmak için kullanılır.                                                                                                                  */
            // İsim ile arama kutusuna girilen metni karşılaştır
            bool match = nameItem->text().toLower().contains(arg1.toLower());                                                  //toLower() fonksiyonu büyük küçük harfe bakmaksızın doğru sonuç almasını sağlar
            // Eğer eşleşme yoksa, ilgili satırı gizle, eşleşme varsa göster                                                 //contains() alt string ifade var mı kontrol eder yani 2 isimli biri varsa true ya döndürür öbür ismide kontrol eder
            ui->tableWidget->setRowHidden(i, !match);

        }
    }
}


void MainWindow::on_GenderBox_currentTextChanged(const QString &arg1)
{
    for(int i=0; i<ui->tableWidget->rowCount(); i++)
    {
        QTableWidgetItem *genderItem = ui->tableWidget->item(i,2); //2 cinsiyetin olduğu sütun

        if(genderItem)
        {
            QString genderText = genderItem->text();
            // Eğer "Tümü" seçiliyse veya cinsiyet eşleşiyorsa satırı göster, değilse gizle
            bool showRow = (arg1 == "Tümü") || (genderText == arg1);
            ui->tableWidget->setRowHidden(i, !showRow);
        }
    }
}


void MainWindow::on_remove_clicked()
{
    //tıklanan satırı al
    int currentRow = ui->tableWidget->currentRow();

    // Eğer geçerli bir satır seçilmişse
    if(currentRow >= 0)
    {
         //satırı sil
        deletePersonFromDatabase(currentRow);
        ui->tableWidget->removeRow(currentRow);
    }

    reNumber();

}

void MainWindow::reNumber()
{

    for(int i=0; i < ui->tableWidget->rowCount(); i++)
    {
        QTableWidgetItem *reNumberitem = ui->tableWidget->item(i,-1); //0 Numaralandırmanın yapılacağı sütun

        if(reNumberitem)
        {
            reNumberitem->setText(QString::number(i+1)); //satırın ilk sütunundan numaralandırır
        }

    }
}


void MainWindow::on_searchbutton_clicked()
{
    QString question;
    if (ui->textQuestionEdit->toPlainText() != "")
    {
        question = ui->textQuestionEdit->toPlainText();
        qDebug() << question;
        QUrl url("https://api.openai.com/v1/chat/completions");
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setRawHeader("Authorization", QString("Bearer %1").arg(apiKey).toUtf8());

        QJsonObject json;
        json["model"] = "gpt-3.5-turbo";

        // Mevcut mesaj geçmişini al ve yeni soruyu ekle


        QJsonArray messages = messageHistory[currentItem];
        QJsonObject message;
        message["role"] = "user";
        message["content"] = question;
        messages.append(message);
        json["messages"] = messages;

        QJsonDocument doc(json);
        QByteArray data = doc.toJson();

        networkManager->post(request, data);


        ui->textQuestionEdit->clear();

        // Yeni itemin başlığını ayarla
        if (currentItem && currentItem->text().isEmpty()) {
            ui->textQuestionEdit->setPlaceholderText("");
            QString topicTitle = "Konu: " + question;
            if (topicTitle == "Konu: ") {
                topicTitle = "Konu " + QString::number(ui->listWidget->count());
            }
            currentItem->setText(topicTitle);
        }

    }
}


void MainWindow::on_newchatbutton_clicked()
{
    // Eğer mevcut bir öğe varsa, onun metnini kaydet
    if (currentItem) {
        topicHistory[currentItem] = ui->plainAnswerTextEdit->toPlainText();
    }

    QListWidgetItem *item = new QListWidgetItem("", ui->listWidget); // Boş isimle yeni item oluştur
    ui->listWidget->setStyleSheet("QListWidget::item {"
                                  "background-color: rgb(170, 255, 127);"
                                  "border: 2px solid rgb(170, 0, 0);"
                                  "border-radius: 12px;"  // Köşe yarıçapını ayarlama
                                  "padding: 5px;"
                                  "}"
                                  "QListWidget::item:hover {"
                                  "border:2px solid rgb(255, 255, 255);"
                                  "border-radius:12px;"
                                  "padding: 5px;"
                                  "}"
                                  "QListWidget::item:selected {"
                                  "border:2px solid rgb(0, 85, 255);"
                                  "border-radius:12px ;"
                                  "padding: 5px;"
                                  "}");
    topicHistory[item] = "";  // Yeni konuyu boş bir metinle başlat
    messageHistory[item] = QJsonArray(); // Yeni bir mesaj geçmişi başlat


    // Yeni öğeyi seç ve metin alanını temizle
    ui->listWidget->setCurrentItem(item);
    ui->plainAnswerTextEdit->clear();
    currentItem = item;
}


void MainWindow::onNetworkReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject json = doc.object();

        // Yanıtın tamamını ekrana yazdır
        qDebug() << "Response JSON:" << doc.toJson(QJsonDocument::Indented);

        if (json.contains("choices")) {
            QJsonArray choices = json["choices"].toArray();
            if (!choices.isEmpty()) {
                QJsonObject choice = choices[0].toObject();
                // QJsonObject içinde "message" olup olmadığını kontrol edin
                if (choice.contains("message")) {
                    QJsonObject message = choice["message"].toObject();
                    if (message.contains("content")) {
                        QString answer = message["content"].toString();
                        ui->plainAnswerTextEdit->setPlainText(answer);
                        if (currentItem) {
                            topicHistory[currentItem] = answer; // Cevabı mevcut öğeye kaydet

                            // Yanıtı mesaj geçmişine ekle
                            QJsonObject replyMessage;
                            replyMessage["role"] = "assistant";
                            replyMessage["content"] = answer;
                            messageHistory[currentItem].append(replyMessage);
                        }
                    } else {
                        ui->plainAnswerTextEdit->setPlainText("Unexpected response format: 'content' not found in 'message'.");
                    }
                } else {
                    ui->plainAnswerTextEdit->setPlainText("Unexpected response format: 'message' not found in 'choices'.");
                }
            } else {
                ui->plainAnswerTextEdit->setPlainText("No response from the model.");
            }
        } else {
            ui->plainAnswerTextEdit->setPlainText("Unexpected response format: 'choices' not found.");
        }
    } else {
        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject json = doc.object();
        qDebug() << "Error JSON:" << doc.toJson(QJsonDocument::Indented);  // Hata yanıtını ekrana yazdır

        if (json.contains("error")) {
            QJsonObject error = json["error"].toObject();
            QString errorMessage = error["message"].toString();
            QString errorType = error["type"].toString();
            QString errorCode = error["code"].toString();

            QString detailedError = QString("Error: %1\nType: %2\nCode: %3").arg(errorMessage).arg(errorType).arg(errorCode);
            ui->plainAnswerTextEdit->setPlainText(detailedError);
        } else {
            ui->plainAnswerTextEdit->setPlainText("An unknown error occurred.");
        }
    }

    reply->deleteLater();
}

void MainWindow::loadTopic(QListWidgetItem *item)
{
    if (messageHistory.contains(item)) {
        QString fullConversation;

        // Sonra iteme ait mesaj geçmişini al ve soru ve yanıtları birleştirerek göster
        QJsonArray messages = messageHistory[item];
        for (const QJsonValue &value : messages) {
            QJsonObject message = value.toObject();
            QString role = message["role"].toString();
            QString content = message["content"].toString();

            // Format the message with role and content
            if (role == "user") {
                fullConversation += QString("User: %1\n").arg(content);
            } else {
                fullConversation += QString("Assistant: %1\n").arg(content);
            }
        }

        ui->plainAnswerTextEdit->setPlainText(fullConversation);
    }
}
