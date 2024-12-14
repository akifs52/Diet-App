#include "addnew.h"
#include "ui_addnew.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include "mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QDate>

addnew::addnew(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addnew)
{
    ui->setupUi(this);
    QRegularExpression justLetter("[A-Za-z ]+");  // Sadece harflere izin veren regex
    QRegularExpression justNumber("[0-9]+"); //sadece sayılara izin veren regex

    QRegularExpressionValidator *validatorLetter = new QRegularExpressionValidator(justLetter,this);
    QRegularExpressionValidator *validatorNumber = new QRegularExpressionValidator(justNumber,this);

    ui->isim->setValidator(validatorLetter);
    ui->boy->setValidator(validatorNumber);
    ui->kilo->setValidator(validatorNumber);

    QDate minimum(1900,01,01);
    QDate today = QDate::currentDate();
    ui->dateEdit->setMaximumDate(today);  //maksimum tarih bugün
    ui->dateEdit->setMinimumDate(minimum); //min tarih

}

addnew::~addnew()
{
    delete ui;
}

void addnew::on_kaydet_clicked()
{

    if(ui->isim->text()=="")
    {
        QMessageBox::warning(this,"İsim Girişi","Lütfen İsim Giriniz");
        return; // İsim Girişi seçilmezse kaydetme işlemini durdur
    }
    if(ui->boy->text()=="")
    {
        QMessageBox::warning(this,"Boy Girişi","Lütfen Boy Giriniz");
        return; // Boy Girişi seçilmezse kaydetme işlemini durdur
    }
    if(ui->kilo->text()=="")
    {
        QMessageBox::warning(this,"Kilo Girişi","Lütfen Kilo Giriniz");
        return; // Kilo Girişi seçilmezse kaydetme işlemini durdur
    }
    if(ui->cinsiyet->currentIndex()==0)
    {
        genderChoosing = "Erkek";
    }
    else if(ui->cinsiyet->currentIndex()==1)
        genderChoosing = "Kadın";
    else
    {
        QMessageBox::warning(this,"Cinsiyet Seçimi","Lütfen Cinsiyet Seçiniz");
        return; // Cinsiyet seçilmezse kaydetme işlemini durdur
    }
    name = ui->isim->text();
    dob = ui->dateEdit->text();
    height = ui->boy->text();
    weight = ui->kilo->text();

    // Bilgileri MainWindow'daki tabloya ekle
    MainWindow *mainWindow = qobject_cast<MainWindow*>(this->parent());

    if(mainWindow)
    {
        mainWindow->addPersonToTable(name,dob,genderChoosing,height,weight);
    }
    else
    {
        qDebug()<<"başarısız";
    }

    this->close();  // Kaydetme işleminden sonra pencereyi kapat

}

