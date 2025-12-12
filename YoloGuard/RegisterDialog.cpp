#include "RegisterDialog.h"
#include <QVBoxLayout> 
#include <QMessageBox> 
#include <QFile>
#include <QTextStream>

RegisterDialog::RegisterDialog(QWidget* parent)
    : QDialog(parent)
{
    this->setWindowTitle("YoloGuard - 注册新用户");
    this->setFixedSize(350, 360); // 稍微高一点点
    this->setWindowIcon(QIcon(":/Resources/register.png"));

    // 1. 初始化控件
    userEdit = new QLineEdit(this);
    userEdit->setPlaceholderText("请输入用户名");

    passEdit = new QLineEdit(this);
    passEdit->setPlaceholderText("请输入密码");
    passEdit->setEchoMode(QLineEdit::Password);

    confirmPassEdit = new QLineEdit(this);
    confirmPassEdit->setPlaceholderText("请确认密码");
    confirmPassEdit->setEchoMode(QLineEdit::Password);
    
    //明文密文切换
    passToggleAction = new QAction(this);
    passToggleAction->setIcon(QIcon(":/Resources/ciphertext.png")); // 默认闭眼
    passEdit->addAction(passToggleAction, QLineEdit::TrailingPosition);
    connect(passToggleAction, &QAction::triggered, this, &RegisterDialog::onTogglePass);

    confirmToggleAction = new QAction(this);
    confirmToggleAction->setIcon(QIcon(":/Resources/ciphertext.png")); 
    confirmPassEdit->addAction(confirmToggleAction, QLineEdit::TrailingPosition);
    connect(confirmToggleAction, &QAction::triggered, this, &RegisterDialog::onToggleConfirm);

    QAction* userIconAction = new QAction(this);
    userIconAction->setIcon(QIcon(":/Resources/user.png"));
    userEdit->addAction(userIconAction, QLineEdit::LeadingPosition);

    QAction* passIconAction = new QAction(this);
    passIconAction->setIcon(QIcon(":/Resources/lock.png"));
    passEdit->addAction(passIconAction, QLineEdit::LeadingPosition);

    QAction* confirmPassIconAction = new QAction(this);
    confirmPassIconAction->setIcon(QIcon(":/Resources/lock.png"));
    confirmPassEdit->addAction(confirmPassIconAction, QLineEdit::LeadingPosition);

    registerBtn = new QPushButton("立即注册", this);
    registerBtn->setCursor(Qt::PointingHandCursor);

    // 2. 布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(40, 40, 40, 40);

    QLabel* title = new QLabel("创建账号", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #4CAF50; margin-bottom: 10px;");
    layout->addWidget(title);

    layout->addWidget(userEdit);
    layout->addWidget(passEdit);
    layout->addWidget(confirmPassEdit);
    layout->addSpacing(10);
    layout->addWidget(registerBtn);

    connect(registerBtn, &QPushButton::clicked, this, &RegisterDialog::onRegister);

    loadStyle();
}

RegisterDialog::~RegisterDialog() {}

void RegisterDialog::loadStyle() {
    this->setStyleSheet(
        "QDialog { background-color: #2b2b2b; color: white; }"
        "QLineEdit { background-color: #3c3c3c; border: 1px solid #555; border-radius: 5px; padding: 8px; color: white; }"
        "QLineEdit:focus { border: 1px solid #4CAF50; }"
        "QPushButton { background-color: #4CAF50; color: white; border-radius: 5px; padding: 10px; font-weight: bold; }"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:pressed { background-color: #3e8e41; }"
    );
}


void RegisterDialog::onTogglePass()
{
    if (passEdit->echoMode() == QLineEdit::Password) {
        passEdit->setEchoMode(QLineEdit::Normal);
        passToggleAction->setIcon(QIcon(":/Resources/plaintext.png")); 
    }
    else {
        passEdit->setEchoMode(QLineEdit::Password);
        passToggleAction->setIcon(QIcon(":/Resources/ciphertext.png")); 
    }
}

void RegisterDialog::onToggleConfirm()
{
    if (confirmPassEdit->echoMode() == QLineEdit::Password) {
        confirmPassEdit->setEchoMode(QLineEdit::Normal);
        confirmToggleAction->setIcon(QIcon(":/Resources/plaintext.png"));
    }
    else {
        confirmPassEdit->setEchoMode(QLineEdit::Password);
        confirmToggleAction->setIcon(QIcon(":/Resources/ciphertext.png"));
    }
}

void RegisterDialog::onRegister() {
    QString user = userEdit->text().trimmed();
    QString pass = passEdit->text().trimmed();
    QString confirm = confirmPassEdit->text().trimmed();

    if (user.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "提示", "用户名或密码不能为空！");
        return;
    }

    if (pass != confirm) {
        QMessageBox::warning(this, "错误", "两次输入的密码不一致！");
        return;
    }

    if (saveUserToFile(user, pass)) {
        QMessageBox::information(this, "成功", "注册成功！请返回登录。");
        this->accept();
    }
    else {
        QMessageBox::critical(this, "错误", "无法写入文件，请检查权限！");
    }
}

bool RegisterDialog::saveUserToFile(QString user, QString pass) {
    QFile file("users.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        out << user << "," << pass << "\n";
        file.close();
        return true;
    }
    return false;
}