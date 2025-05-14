#include "lrcwidget.h"
#include "ui_lrcwidget.h"
#include<QLabel>
#include<QPainter>
#include<QGraphicsBlurEffect>
#include <QRegularExpression>
#include<QFile>
#include<QMap>
#include<QTime>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

lrcwidget::lrcwidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::lrcwidget)
{
    ui->setupUi(this);


    coverPixmap.load(":/images/images/KK.jpg");

    // 创建并初始化 stackedWidget
    stackedWidget = new QStackedWidget(this);
    //stackedWidget->setGeometry(this->rect());


    // 初始化 noLyricsLabel
    noLyricsLabel = new QLabel(stackedWidget);
    noLyricsLabel->setText(
        "    天在将黑未黑时最美\n\n"
        "    因为那时走在回家的路上\n\n"
        "    爱在将爱未爱时最迷人\n\n"
        "    因为心动伴随着美好的幢景\n\n"
        "    幸福往往在将来未来时最幸福"
        );
    noLyricsLabel->setAlignment(Qt::AlignCenter);
    noLyricsLabel->setStyleSheet("color: white; font-size: 20pt; padding: 20px;");
    noLyricsLabel->setWordWrap(true);
    noLyricsLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    // 设置 noLyricsLabel 的字体
    QFont font;
    font.setFamily("Times New Roman");
    font.setPointSize(15);
    font.setBold(true);
    font.setItalic(false);
    noLyricsLabel->setFont(font);


    stackedWidget->setGeometry(320,80,500,400);




    // 将 noLyricsLabel 添加到 stackedWidget
    stackedWidget->addWidget(noLyricsLabel);

    // 将 lrc_list 添加到 stackedWidget
    //stackedWidget->addWidget(ui->lrc_list);
    stackedWidget->setCurrentWidget(noLyricsLabel);


    // 调整按钮层级关系
    ui->btnLrcClose->raise();

    //创建动画对象
    ui->lrc_list->setStyleSheet(
        "QListWidget {"
        "    background: transparent;"      // 设置背景透明
        "    font-family: 'Arial';"     // 设置字体家族
        "    font-size: 15pt;"          // 设置字体大小
        "    border: none;"                 // 取消边框
        "}"
        "QListWidget::item {"
        "    color: white;"                 // 设置文字颜色为白色
        "    text-align: center;"           // 文字居中显示
        "    padding: 10px 0px;"        // 设置上下边距为10px，左右边距为0px
        "}"

        "QListWidget::item:selected {"
        "    color: yellow;"

        "    font-weight: bold;"
        "}"
        );

    ui->lrc_list->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->lrc_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->labCov->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    ui->labCov->setStyleSheet("margin: 0px; padding: 0px; border: none;");


    animation = new QPropertyAnimation(this, "geometry", this);
    animation->setDuration(500);//动画持续时间
    animation->setEasingCurve(QEasingCurve::OutQuad);//使用缓动曲线

    connect(ui->horizontalSlider, &QSlider::sliderMoved, this, &lrcwidget::on_horizontalSlider_sliderMoved);
    connect(ui->horizontalSlider, &QSlider::sliderPressed, this, &lrcwidget::on_horizontalSlider_sliderPressed);
    connect(ui->horizontalSlider, &QSlider::sliderReleased, this, &lrcwidget::on_horizontalSlider_sliderReleased);


}

lrcwidget::~lrcwidget()
{
    delete ui;
}


QSlider* lrcwidget::getSlider() const {
    return ui->horizontalSlider;
}

QPushButton* lrcwidget::getPlayButton() const {
    return ui->btnPlay;
}

QPushButton* lrcwidget::getPrevButton() const {
    return ui->btnPrev;
}

QPushButton* lrcwidget::getNextButton() const {
    return ui->btnNext;
}

QSlider* lrcwidget::getSoundSlider() const {
    return ui->soundSlider;
}

QDoubleSpinBox* lrcwidget::getSpeedSpinBox() const {
    return ui->spinBoxSpeed;
}

QPushButton* lrcwidget::getModeButton() const {
    return ui->btnMode;
}

void lrcwidget::updateLabProcess(const QString &text)
{
    ui->labProcess->setText(text);
}


void lrcwidget::loadLyrics(const QString &filePath)
{
    lyricsMap = parseLyrics(filePath);
    ui->lrc_list->clear();

    if (lyricsMap.isEmpty()) {
        stackedWidget->show();
    } else {
        stackedWidget->hide();

        for (auto it = lyricsMap.begin(); it != lyricsMap.end(); ++it) {
            QListWidgetItem *item = new QListWidgetItem(it.value());
            item->setTextAlignment(Qt::AlignCenter);
            ui->lrc_list->addItem(item);
        }

        // 更新列表高度以适应行
        updateListHeight();
    }
}

QMap<QTime, QString> lrcwidget::parseLyrics(const QString &filePath)
{
    QFile file(filePath);
    //QMap<QTime, QString> lyricsMap;
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        lyricsMap.clear();
        QTextStream in(&file);

        QRegularExpression regex(R"(\[(\d{2}):(\d{2})(?::(\d{2}))?\](.*))");


        while(!in.atEnd()){
            QString line = in.readLine();
            QRegularExpressionMatch match = regex.match(line);
            if(match.hasMatch()){
                int minutes = match.captured(1).toInt();
                int seconds = match.captured(2).toInt();

                int milliseconds = match.captured(3).isEmpty() ? 0 : match.captured(3).toInt() * 10;
                QString text = match.captured(4).trimmed();

                QTime time(0, minutes, seconds, milliseconds);
                lyricsMap[time] = text;
                //qDebug() << "Parsed lyrics:" << time.toString() << text; // Debug output
            }
        }
    }

    file.close();
    return lyricsMap;
}


void lrcwidget::updateLyrics(qint64 position)
{



    int totalSeconds = position / 1000; // 将毫秒转换为秒
    int seconds = totalSeconds % 60; // 计算剩余的秒数
    int minutes = totalSeconds / 60; // 计算分钟数
    int milliseconds = position % 1000; // 计算剩余的毫秒

    QTime currentTime(0, minutes, seconds, milliseconds);
    //QString currentLyric = "No lyric available"; // 默认文本
    QTime bestMatch(0, 0, 0, 0); // 初始化为一个有效的最小时间


    // 查找第一个大于当前时间的迭代器
    auto it = lyricsMap.upperBound(currentTime);

    // 如果不是第一个元素，回退到最后一个不大于当前时间的元素
    if (it != lyricsMap.begin()) {
        --it;
    }

    // 确保找到的时间不晚于当前时间
    while (it != lyricsMap.end() && it.key() > currentTime) {
        if (it == lyricsMap.begin()) {
            break;  // 如果已经是第一个元素，则停止
        }
        --it;
    }

    if (it != lyricsMap.end() && it.key() <= currentTime) {
        int index = std::distance(lyricsMap.begin(), it);
        ui->lrc_list->setCurrentRow(index);
        ui->lrc_list->scrollToItem(ui->lrc_list->item(index), QAbstractItemView::PositionAtCenter);
        // 高亮显示当前行
        ui->lrc_list->item(index)->setSelected(true);
    }

    ui->horizontalSlider->setValue(position);

}


void lrcwidget::on_horizontalSlider_sliderMoved(int position)
{
    emit sliderMoved(position);
}

void lrcwidget::on_horizontalSlider_sliderPressed()
{
    emit sliderPressed();
}

void lrcwidget::on_horizontalSlider_sliderReleased()
{
    emit sliderReleased();
}


void lrcwidget::setCoverImage(const QPixmap &pixmap)
{
//    // 将传入的 QPixmap 设置为 lrclabel_cover 的图片
//    ui->lrclabel_cover->setScaledContents(true);
//    ui->lrclabel_cover->setPixmap(pixmap);
//    coverPixmap = pixmap;
//    QPixmap pixmapC(pixmap);
//    pixmapC = pixmap.scaled(ui->labCov->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    if (!pixmap.isNull()) {
        coverPixmap = pixmap;
    }

    QSize labelSize = ui->labCov->size();
    QPixmap scaledPixmap = coverPixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->labCov->setPixmap(scaledPixmap);

    //ui->labCov->setFixedSize(coverPixmap.size());
    update();//触发重绘事件

}

void lrcwidget::updateListHeight()
{
    if (ui->lrc_list->count() > 0) {
        int rowHeight = ui->lrc_list->sizeHintForRow(0);
        int numRowsToShow = 7;
        ui->lrc_list->setFixedHeight(rowHeight * numRowsToShow);
    }
}

void lrcwidget::showLyric()
{
    QWidget *parent = parentWidget();
    if (parent) {
        qDebug() << "Parent widget is valid.";
        qDebug() << "Parent widget width:" << parent->width();
        qDebug() << "Parent widget height:" << parent->height();
    } else {
        qDebug() << "Parent widget is NULL.";
    }

    disconnect(animation, &QPropertyAnimation::finished, this, &QWidget::hide);
    // 设置动画起始位置和结束位置
    qDebug() << 5.1 << Qt::endl;
    animation->setStartValue(QRect(0, parentWidget()->height(), width(), height()));
    qDebug() << "key" << Qt::endl;
    animation->setEndValue(QRect(0, parentWidget()->height() - height(), width(), height()));
    // 开始动画
    qDebug() << 7 << Qt::endl;
    animation->start();
    // 显示歌词界面
    qDebug() << 8 << Qt::endl;
    show();
    qDebug() << 9 << Qt::endl;
}

void lrcwidget::hideLyric()
{
    QWidget *parent = parentWidget();
    qDebug() << 10 << Qt::endl;
    if (parent) {
        qDebug() << "Parent widget is valid.";
        qDebug() << "Parent widget width:" << parent->width();
        qDebug() << "Parent widget height:" << parent->height();
    } else {
        qDebug() << "Parent widget is NULL.";
    }


    // 设置动画起始位置和结束位置
    qDebug() << 11 << Qt::endl;
    animation->setStartValue(QRect(0, 0, width(), height()));
    qDebug() << 12 << Qt::endl;
    animation->setEndValue(QRect(0, parentWidget()->height(), width(), height()));
    // 连接动画结束信号到隐藏小部件的槽函数
    qDebug() << 13 << Qt::endl;
    disconnect(animation, &QPropertyAnimation::finished, this, &QWidget::hide); // 确保不会重复连接
    connect(animation, &QPropertyAnimation::finished, this, &QWidget::hide);
    // 开始动画
    qDebug() << 14 << Qt::endl;
    animation->start();
    qDebug() << 15 << Qt::endl;
}

void lrcwidget::resetCoverImage()
{
    // 加载默认封面
    coverPixmap.load(":/images/images/KK.jpg");

    ui->labCov->clear();
    update(); // 触发重绘事件
}

void lrcwidget::clearLyrics()
{
    ui->lrc_list->clear();  // 清空歌词列表
    lyricsMap.clear();  // 清空歌词映射


}

void lrcwidget::paintEvent(QPaintEvent *event)
{
    //调用基类的绘制函数
    QWidget::paintEvent(event);

    //绘制封面图片
    if(!coverPixmap.isNull()){
        QPainter painter(this);
        QImage image = coverPixmap.toImage();

        // 应用模糊效果
        QImage blurredImage = applyBlurToImage(image, 10); // 10 是模糊半径




        // 将 QImage 转回 QPixmap 并绘制
        QPixmap blurredPixmap = QPixmap::fromImage(blurredImage);
        painter.drawPixmap(0, 0, width(), height(), blurredPixmap);

        // 添加暗色遮罩层
        QColor overlayColor(0, 0, 0, 127); // 黑色遮罩，127 是透明度（0 完全透明到 255 完全不透明）
        painter.fillRect(this->rect(), overlayColor);
    }
}

QImage lrcwidget::applyBlurToImage(QImage sourceImage, int radius)
{
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(sourceImage));

    QGraphicsBlurEffect blur;
    blur.setBlurRadius(radius);
    item.setGraphicsEffect(&blur);

    scene.addItem(&item);
    QImage result(sourceImage.size(), QImage::Format_ARGB32);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    scene.render(&painter);
    return result;
}

void lrcwidget::on_btnLrcClose_clicked()
{
    hideLyric();
}
