#pragma once
#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QDateTime>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <QApplication>
#include <QStackedWidget> // 堆栈窗口：用于多页面切换
#include <QButtonGroup>   // 按钮组：管理侧边栏按钮互斥
#include <QSlider>        // 滑块：用于调节灵敏度
#include <QListWidget>    // 列表：用于显示历史记录
#include <QFile>

class YoloGuard : public QMainWindow
{
    Q_OBJECT

public:
    YoloGuard(QWidget* parent = nullptr);
    ~YoloGuard();

private slots:
    // --- 核心槽函数 ---
    void updateFrame();          // 定时器触发：执行 YOLO 检测
    void onStart();              // 点击启动
    void onStop();               // 点击停止
    void onNavClicked(int id);   // 侧边栏切换
    void onThresChanged(int value); // 滑块拖动：调节灵敏度

private:
    // --- 界面组件：侧边栏 (Sidebar) ---
    QWidget* sideBar;
    QButtonGroup* navGroup;
    QPushButton* btnMonitor;
    QPushButton* btnSettings;
    QPushButton* btnHistory;

    // --- 界面组件：主内容区 (Main Content) ---
    QStackedWidget* mainStack; // 像电视机一样，负责显示不同频道

    // --- 页面 1：监控中心 ---
    QWidget* pageMonitor;
    QLabel* screenLabel;      // 显示视频画面
    QPushButton* startBtn;
    QPushButton* stopBtn;
    QTextEdit* logArea;       // 实时日志

    // --- 页面 2：系统设置 ---
    QWidget* pageSettings;
    QSlider* thresSlider;     // 灵敏度调节滑块
    QLabel* thresValueLabel;  // 显示当前灵敏度数值

    // --- 页面 3：历史记录 ---
    QWidget* pageHistory;
    QListWidget* historyList; // 专门存放报警记录的列表

    // --- 核心引擎 ---
    QTimer* timer;
    cv::VideoCapture cap;
    cv::dnn::Net net;

    // --- 算法参数 ---
    const float INPUT_W = 640.0f;
    const float INPUT_H = 640.0f;
    float scoreThreshold = 0.45f; // 【关键变量】不再是 const，允许修改
    const float NMS_THRES = 0.45f;

    // --- 辅助函数 ---
    void initUI();     // 负责搭建界面布局
    void initSignal(); // 负责绑定信号与槽
    void log(QString text); // 写普通日志
    void addHistory(QString type); // 写报警历史记录
    void loadHistory();
};