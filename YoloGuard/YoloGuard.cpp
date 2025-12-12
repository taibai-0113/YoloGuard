#include "YoloGuard.h"
#include <QDebug>
#include <QFormLayout> // 表单布局，专门用于设置页
#include <QGroupBox>

using namespace cv;
using namespace cv::dnn;
using namespace std;

YoloGuard::YoloGuard(QWidget* parent)
    : QMainWindow(parent)
{
    // 1. 初始化界面布局
    initUI();

    // 2. 绑定交互逻辑
    initSignal();

    //历史报警记录
    loadHistory();

    // 3. 加载模型
    string model_path = "E:\\C++\\YoloGuard\\YoloGuard\\best.onnx";
    log("正在加载 AI 模型...");

    try {
        net = readNetFromONNX(model_path);
        log("模型加载成功！引擎就绪。");
    }
    catch (const cv::Exception& e) {
        log("错误：无法加载模型！请检查路径。");
        // 如果模型加载失败，禁用启动按钮，防止崩溃
        if (startBtn) startBtn->setEnabled(false);
    }
}

YoloGuard::~YoloGuard() {
    if (cap.isOpened()) cap.release();
}

//  界面初始化逻辑 s
void YoloGuard::initUI() {
    // 基础窗口设置
    this->setWindowTitle("YoloGuard - 智能驾驶守护系统 v2.0");
    this->resize(1200, 760);
    this->setWindowIcon(QIcon(":/Resources/logo.png"));

    // 创建中心容器
    QWidget* centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    // 主布局：水平排列 (左边侧边栏 | 右边内容区)
    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0); // 去掉边距
    mainLayout->setSpacing(0); // 紧密连接

    // 左侧：侧边栏 (SideBar)
    sideBar = new QWidget(this);
    sideBar->setFixedWidth(200);
    sideBar->setStyleSheet("background-color: #2b2b2b; border-right: 1px solid #444;");

    QVBoxLayout* sideLayout = new QVBoxLayout(sideBar);
    sideLayout->setContentsMargins(0, 40, 0, 20);
    sideLayout->setSpacing(10);

    // 按钮组 (管理互斥状态)
    navGroup = new QButtonGroup(this);
    navGroup->setExclusive(true);

    // Lambda 辅助函数：快速创建侧边栏按钮
    auto createNavBtn = [&](QString text, int id, QString iconPath) {
        QPushButton* btn = new QPushButton(text, sideBar);
        btn->setCheckable(true);
        btn->setMinimumHeight(60);
        btn->setStyleSheet(
            "QPushButton { border: none; color: #aaa; font-size: 16px; text-align: left; padding-left: 30px; }"
            "QPushButton:checked { background-color: #1e1e1e; color: #4CAF50; border-left: 5px solid #4CAF50; font-weight: bold; }"
            "QPushButton:hover { background-color: #333; color: white; }"
        );
        btn->setIcon(QIcon(iconPath));
        navGroup->addButton(btn, id);
        sideLayout->addWidget(btn);
        return btn;
        };

    btnMonitor = createNavBtn("监控中心", 0, ":/Resources/monitor.png");
    btnSettings = createNavBtn("系统设置", 1, ":/Resources/settings.png");
    btnHistory = createNavBtn("历史记录", 2, ":/Resources/history.png");

    btnMonitor->setChecked(true); // 默认选中第一个
    sideLayout->addStretch();     // 底部弹簧占位
    mainLayout->addWidget(sideBar); // 加入主布局

    // 右侧：堆栈内容区 (MainStack)
    mainStack = new QStackedWidget(this);
    mainStack->setStyleSheet("background-color: #1e1e1e;");

    // 页面 1: 监控页
    pageMonitor = new QWidget();
    QVBoxLayout* monitorLayout = new QVBoxLayout(pageMonitor);

    screenLabel = new QLabel("系统就绪，等待启动...", pageMonitor);
    screenLabel->setAlignment(Qt::AlignCenter);
    screenLabel->setStyleSheet("border: 2px solid #444; background-color: #000; color: #666; font-size: 18px;");
    screenLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    screenLabel->setScaledContents(true);

    // 控制面板
    QWidget* controlPanel = new QWidget(pageMonitor);
    controlPanel->setFixedHeight(200);
    QHBoxLayout* controlLayout = new QHBoxLayout(controlPanel);

    QVBoxLayout* btnLayout = new QVBoxLayout();
    startBtn = new QPushButton("启动检测", pageMonitor);
    startBtn->setMinimumHeight(50);
    startBtn->setStyleSheet("QPushButton { background-color: #2E7D32; color: white; border-radius: 5px; font-weight: bold; font-size: 16px; } QPushButton:hover { background-color: #388E3C; }");

    stopBtn = new QPushButton("停止检测", pageMonitor);
    stopBtn->setMinimumHeight(50);
    stopBtn->setStyleSheet("QPushButton { background-color: #C62828; color: white; border-radius: 5px; font-weight: bold; font-size: 16px; } QPushButton:hover { background-color: #D32F2F; }");
    stopBtn->setEnabled(false);

    btnLayout->addWidget(startBtn);
    btnLayout->addWidget(stopBtn);

    logArea = new QTextEdit(pageMonitor);
    logArea->setReadOnly(true);
    logArea->setStyleSheet("border: 1px solid #444; background-color: #111; color: #0f0; font-family: Consolas; font-size: 14px;");

    controlLayout->addLayout(btnLayout, 1);
    controlLayout->addWidget(logArea, 3);

    monitorLayout->addWidget(screenLabel);
    monitorLayout->addWidget(controlPanel);

    // 页面 2: 设置页
    pageSettings = new QWidget();
    QFormLayout* settingsLayout = new QFormLayout(pageSettings);
    settingsLayout->setContentsMargins(50, 50, 50, 50);
    settingsLayout->setSpacing(20);

    QLabel* setHeader = new QLabel("参数调整", pageSettings);
    setHeader->setStyleSheet("color: white; font-size: 22px; font-weight: bold; margin-bottom: 20px;");
    settingsLayout->addRow(setHeader);

    thresSlider = new QSlider(Qt::Horizontal, pageSettings);
    thresSlider->setRange(10, 90); // 0.1 ~ 0.9
    thresSlider->setValue(45);     // 默认 0.45
    thresSlider->setStyleSheet("QSlider::groove:horizontal { height: 8px; background: #333; border-radius: 4px; } QSlider::handle:horizontal { background: #4CAF50; width: 20px; margin: -6px 0; border-radius: 10px; }");

    thresValueLabel = new QLabel("0.45", pageSettings);
    thresValueLabel->setStyleSheet("color: #4CAF50; font-weight: bold; font-size: 16px;");

    QHBoxLayout* sliderRow = new QHBoxLayout();
    sliderRow->addWidget(thresSlider);
    sliderRow->addWidget(thresValueLabel);

    QLabel* label1 = new QLabel("检测灵敏度 (Confidence):", pageSettings);
    label1->setStyleSheet("color: #ccc; font-size: 14px;");
    settingsLayout->addRow(label1, sliderRow);

    // 页面 3: 历史记录页
    pageHistory = new QWidget();
    QVBoxLayout* historyLayout = new QVBoxLayout(pageHistory);
    historyLayout->setContentsMargins(20, 20, 20, 20);

    QLabel* histHeader = new QLabel("报警历史记录", pageHistory);
    histHeader->setStyleSheet("color: white; font-size: 22px; font-weight: bold;");

    historyList = new QListWidget(pageHistory);
    historyList->setStyleSheet("QListWidget { background-color: #111; border: 1px solid #444; color: #ddd; font-size: 14px; } QListWidget::item { padding: 10px; border-bottom: 1px solid #222; }");

    historyLayout->addWidget(histHeader);
    historyLayout->addWidget(historyList);

    // 把所有页面加入堆栈
    mainStack->addWidget(pageMonitor);  // ID 0
    mainStack->addWidget(pageSettings); // ID 1
    mainStack->addWidget(pageHistory);  // ID 2

    mainLayout->addWidget(mainStack);
}

//  信号绑定逻辑
void YoloGuard::initSignal() {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &YoloGuard::updateFrame);

    connect(startBtn, &QPushButton::clicked, this, &YoloGuard::onStart);
    connect(stopBtn, &QPushButton::clicked, this, &YoloGuard::onStop);

    // 侧边栏点击事件
    connect(navGroup, &QButtonGroup::idClicked, this, &YoloGuard::onNavClicked);

    // 滑块拖动事件
    connect(thresSlider, &QSlider::valueChanged, this, &YoloGuard::onThresChanged);
}

//  业务槽函数
void YoloGuard::onNavClicked(int id) {
    mainStack->setCurrentIndex(id); // 翻页
}

void YoloGuard::onThresChanged(int value) {
    float floatVal = value / 100.0f;
    thresValueLabel->setText(QString::number(floatVal, 'f', 2));
    this->scoreThreshold = floatVal; // 更新核心阈值
}

void YoloGuard::onStart() {
    log("正在打开摄像头...");
    cap.open(0);
    if (!cap.isOpened()) {
        log("错误：无法连接摄像头！");
        return;
    }
    log("摄像头启动成功。");
    startBtn->setEnabled(false);
    stopBtn->setEnabled(true);
    timer->start(30);
}

void YoloGuard::onStop() {
    timer->stop();
    cap.release();
    screenLabel->clear();
    screenLabel->setText("检测已停止");
    log("系统已停止。");
    startBtn->setEnabled(true);
    stopBtn->setEnabled(false);
    // 恢复正常边框
    screenLabel->setStyleSheet("border: 2px solid #444; background-color: #000; color: #666; font-size: 18px;");
}

//  核心检测逻辑 (YOLO Inference)
void YoloGuard::updateFrame() {
    Mat frame;
    cap >> frame;
    if (frame.empty()) {
        timer->stop();
        return;
    }

    // 1. 预处理
    Mat blob;
    blobFromImage(frame, blob, 1.0 / 255.0, Size(INPUT_W, INPUT_H), Scalar(0, 0, 0), true, false);
    net.setInput(blob);

    // 2. 推理
    vector<Mat> outputs;
    try {
        net.forward(outputs, net.getUnconnectedOutLayersNames());
    }
    catch (...) { return; }

    if (!outputs.empty()) {
        Mat output = outputs[0];
        int dimensions = output.size[2];
        int num_classes = dimensions - 5;

        if (num_classes > 0) {
            float* data = (float*)output.data;
            int rows = output.size[1];

            vector<int> class_ids;
            vector<float> confidences;
            vector<Rect> boxes;

            float x_factor = frame.cols / INPUT_W;
            float y_factor = frame.rows / INPUT_H;

            for (int i = 0; i < rows; ++i) {
                float confidence = data[4];
                // 使用动态变量 scoreThreshold
                if (confidence >= scoreThreshold) {
                    float* classes_scores = data + 5;
                    Mat scores(1, num_classes, CV_32FC1, classes_scores);
                    Point class_id_point;
                    double max_class_score;
                    minMaxLoc(scores, 0, &max_class_score, 0, &class_id_point);
                    if (max_class_score > scoreThreshold) {
                        float x = data[0]; float y = data[1]; float w = data[2]; float h = data[3];
                        int left = int((x - 0.5 * w) * x_factor);
                        int top = int((y - 0.5 * h) * y_factor);
                        int width = int(w * x_factor);
                        int height = int(h * y_factor);

                        class_ids.push_back(class_id_point.x);
                        confidences.push_back((float)max_class_score);
                        boxes.push_back(Rect(left, top, width, height));
                    }
                }
                data += dimensions;
            }

            // NMS 过滤
            vector<int> nms_result;
            NMSBoxes(boxes, confidences, scoreThreshold, NMS_THRES, nms_result);

            // 自定义类别表
            vector<string> class_names = { "normal", "Closed Eye", "Yawn", "Phone", "left", "right" };

            // 疲劳标志位
            bool isFatigue = false;
            string fatigueType = "";

            // 统一循环：画图 + 逻辑判断
            for (int idx : nms_result) {
                Rect box = boxes[idx];
                int id = class_ids[idx];
                string class_name = (id >= 0 && id < class_names.size()) ? class_names[id] : "Unknown";

                Scalar color = Scalar(0, 255, 0); // 默认绿

                // 检测疲劳
                if (id == 1) { // 闭眼
                    color = Scalar(0, 0, 255); // 红
                    isFatigue = true;
                    fatigueType = "闭眼";
                }
                else if (id == 2) { // 哈欠
                    color = Scalar(0, 255, 255); // 黄
                    isFatigue = true;
                    fatigueType = "打哈欠";
                }

                rectangle(frame, box, color, 2);
                string label = format("%s %.2f", class_name.c_str(), confidences[idx]);

                int baseLine;
                Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.6, 1, &baseLine);
                rectangle(frame, Point(box.x, box.y - labelSize.height), Point(box.x + labelSize.width, box.y + baseLine), color, FILLED);
                putText(frame, label, Point(box.x, box.y), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 0), 1);
            }

            // 报警
            if (isFatigue) {
                // 1. 声音
                QApplication::beep();
                // 2. 视觉 (边框变红)
                screenLabel->setStyleSheet("border: 4px solid red; background-color: #000;");
                // 3. 实时日志
                log("<font color='red'><b>[警告] 检测到: " + QString::fromStdString(fatigueType) + "</b></font>");
                // 4. 【新】写入历史记录页
                addHistory(QString::fromStdString(fatigueType));
            }
            else {
                // 恢复正常边框
                screenLabel->setStyleSheet("border: 2px solid #444; background-color: #000;");
            }
        }
    }

    // 显示
    cvtColor(frame, frame, COLOR_BGR2RGB);
    QImage qimg((const uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    screenLabel->setPixmap(QPixmap::fromImage(qimg));
}

void YoloGuard::log(QString text) {
    QString time = QDateTime::currentDateTime().toString("[hh:mm:ss] ");
    logArea->append(time + text);
}

void YoloGuard::addHistory(QString type) {
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString msg = QString("[%1] 警告：驾驶员出现 %2 行为").arg(time).arg(type);

    // 1. 更新 UI 
    QListWidgetItem* item = new QListWidgetItem(msg);
    item->setForeground(Qt::red);
    historyList->insertItem(0, item); // 插到最前面

    // UI列表只保留最近 50 条 (防止内存爆炸)
    if (historyList->count() > 50) {
        delete historyList->takeItem(50);
    }

    // 2. 保存到文件
    QFile file("history.txt");
    // QIODevice::Append 表示“追加”，不会覆盖旧内容
    if (file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        // 必须设置 UTF-8，否则中文会乱码
        out.setEncoding(QStringConverter::Utf8);
        out << msg << "\n"; // 写入并换行
        file.close();
    }
}

void YoloGuard::loadHistory() {
    QFile file("history.txt");
    // ReadOnly 只读模式
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        in.setEncoding(QStringConverter::Utf8); // 必须用 UTF-8 读

        // 一行一行读，直到读完
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.isEmpty()) {
                // 创建列表项
                QListWidgetItem* item = new QListWidgetItem(line);
                item->setForeground(Qt::darkRed); 
                historyList->insertItem(0, item);
            }
        }
        file.close();
    }
}