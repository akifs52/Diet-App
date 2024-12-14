#ifndef ADDNEW_H
#define ADDNEW_H

#include "ui_addnew.h"
#include <QDialog>


namespace Ui {
class addnew;
}

class addnew : public QDialog
{
    Q_OBJECT

public:
    explicit addnew(QWidget *parent = nullptr);
    ~addnew();

private slots:

    void on_kaydet_clicked();

private:
    Ui::addnew *ui;
    QString genderChoosing;
    QString name;
    QString dob ;
    QString height ;
    QString weight ;
};

#endif // ADDNEW_H
