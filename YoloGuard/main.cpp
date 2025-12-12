#include "YoloGuard.h"
#include "LoginDialog.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("MyStudio");     
    a.setApplicationName("YoloGuard");      

    // 全局字体美化 (可选)
    QFont font("Microsoft YaHei", 10);
    a.setFont(font);

    LoginDialog login;
    if (login.exec() == QDialog::Accepted) {
        YoloGuard w;
        w.show();
        return a.exec();
    }
    return 0;
}