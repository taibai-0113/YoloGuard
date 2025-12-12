#include "LoginDialog.h"
#include "RegisterDialog.h" // 必须引用注册窗口头文件
#include <QVBoxLayout> 
#include <QMessageBox> 

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
{
    this->setWindowTitle("YoloGuard - 安全登录");
    this->setFixedSize(350, 350);
    this->setWindowIcon(QIcon(":/Resources/login.png"));

    // 初始化控件
    userEdit = new QLineEdit(this);
    userEdit->setPlaceholderText("用户名");

    passEdit = new QLineEdit(this);
    passEdit->setPlaceholderText("密码");
    passEdit->setEchoMode(QLineEdit::Password);

    // 明文密文切换
    changePass = new QAction(this);
    changePass->setIcon(QIcon(":/Resources/ciphertext.png"));
    passEdit->addAction(changePass, QLineEdit::TrailingPosition);
    connect(changePass, &QAction::triggered, this, &LoginDialog::onInputVisible);

    // 给用户名框加图标
    QAction* userIconAction = new QAction(this);
    userIconAction->setIcon(QIcon(":/Resources/user.png")); 
    userEdit->addAction(userIconAction, QLineEdit::LeadingPosition); 

    // 给密码框加图标
    QAction* passIconAction = new QAction(this);
    passIconAction->setIcon(QIcon(":/Resources/lock.png")); 
    passEdit->addAction(passIconAction, QLineEdit::LeadingPosition);

    // 记住密码复选框
    rememberCb = new QCheckBox("记住密码", this);
    rememberCb->setCursor(Qt::PointingHandCursor);

    loginBtn = new QPushButton("登 录", this);
    loginBtn->setCursor(Qt::PointingHandCursor);

    regBtn = new QPushButton("注册账号", this);
    regBtn->setCursor(Qt::PointingHandCursor);
    // 设置为透明背景样式，作为辅助按钮
    regBtn->setStyleSheet("background: transparent; color: #888; border: none; text-decoration: underline;");

    // 布局
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(40, 40, 40, 20);

    QLabel* title = new QLabel("Welcome Back", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 24px; font-weight: bold; color: #4CAF50; margin-bottom: 20px;");

    layout->addWidget(title);
    layout->addWidget(userEdit);
    layout->addWidget(passEdit);
    layout->addWidget(rememberCb);
    layout->addWidget(loginBtn);
    layout->addWidget(regBtn); // 放入注册按钮
    layout->addStretch();

    // 绑定
    connect(loginBtn, &QPushButton::clicked, this, &LoginDialog::onLogin);
    // 【关键】绑定注册按钮
    connect(regBtn, &QPushButton::clicked, this, &LoginDialog::onRegister);

    loadStyle();
    loadConfig();
}

LoginDialog::~LoginDialog() {}

void LoginDialog::loadStyle() {
    // 这里只设置除了注册按钮以外的样式，因为注册按钮单独设置了
    this->setStyleSheet(
        "QDialog { background-color: #2b2b2b; color: white; }"
        "QLineEdit { background-color: #3c3c3c; border: 1px solid #555; border-radius: 5px; padding: 8px; color: white; }"
        "QLineEdit:focus { border: 1px solid #4CAF50; }"
        "QPushButton#loginBtn { background-color: #4CAF50; color: white; border-radius: 5px; padding: 10px; font-weight: bold; }"
        "QCheckBox {color: white;}"
    );
    // 给登录按钮专门设个ID，防止样式冲突
    loginBtn->setObjectName("loginBtn");
}


void LoginDialog::onInputVisible()
{
    if (passEdit->echoMode() == QLineEdit::Password) {
        passEdit->setEchoMode(QLineEdit::Normal);
        changePass->setIcon(QIcon(":/Resources/plaintext.png"));
    }
    else {
        passEdit->setEchoMode(QLineEdit::Password);
        changePass->setIcon(QIcon(":/Resources/ciphertext.png"));
    }
}


void LoginDialog::onRegister() {
    // 在这里弹出注册窗口
    RegisterDialog dlg(this);
    dlg.exec();
}

void LoginDialog::onLogin() {
    QString user = userEdit->text().trimmed();
    QString pass = passEdit->text().trimmed();

    // 特权账号 or 文件验证
    if ((user == "admin" && pass == "123456") || checkUser(user, pass)) {
        saveConfig();
        this->accept();
    }
    else {
        QMessageBox::warning(this, "错误", "用户名或密码错误！");
    }
}

bool LoginDialog::checkUser(QString user, QString pass) {
    QFile file("users.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(",");
            if (parts.size() >= 2) {
                if (parts[0] == user && parts[1] == pass) return true;
            }
        }
        file.close();
    }
    return false;
}
void LoginDialog::saveConfig() {
    // 创建设置对象 (它会自动去找 main.cpp 里设定的 AppName)
    QSettings settings;

    // 判断复选框是否被勾选
    if (rememberCb->isChecked()) {
        settings.setValue("username", userEdit->text());
        settings.setValue("password", passEdit->text());
        settings.setValue("remember", true);
    }
    else {
        settings.remove("password"); 
        settings.setValue("remember", false);
    }
}

void LoginDialog::loadConfig() {
    QSettings settings;

    // 判断上次是否勾选了记住密码
    // toBool() 把存的值转成 true/false
    bool isRemember = settings.value("remember").toBool();

    if (isRemember) {
        userEdit->setText(settings.value("username").toString());
        passEdit->setText(settings.value("password").toString());
        rememberCb->setChecked(true);
    }
}