#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <QTextStream>
#include <QAction>
#include <QCheckBox> 
#include <QSettings> 

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    LoginDialog(QWidget* parent = nullptr);
    ~LoginDialog();

private:
    QLineEdit* userEdit;
    QLineEdit* passEdit;
    QPushButton* loginBtn;
    QPushButton* regBtn; 

    QCheckBox* rememberCb;

    QAction* changePass;// 明文密文变换

    bool checkUser(QString user, QString pass);
    void loadStyle();

    void saveConfig();
    void loadConfig();

private slots:
    void onInputVisible();// 明文密文变换函数
    void onLogin();
    void onRegister(); 
};