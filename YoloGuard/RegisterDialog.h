#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QAction> 

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    RegisterDialog(QWidget* parent = nullptr);
    ~RegisterDialog();

private:
    QLineEdit* userEdit;
    QLineEdit* passEdit;
    QLineEdit* confirmPassEdit;
    QPushButton* registerBtn;

    QAction* passToggleAction;    
    QAction* confirmToggleAction; 

    bool saveUserToFile(QString user, QString pass);
    void loadStyle();

private slots:
    void onRegister();
    void onTogglePass();    
    void onToggleConfirm(); 
};