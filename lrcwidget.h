#ifndef LRCWIDGET_H
#define LRCWIDGET_H

#include "qlabel.h"
#include "qpushbutton.h"
#include "qslider.h"
#include "qspinbox.h"
#include <QPropertyAnimation>
#include <QWidget>
#include <QStackedWidget>

namespace Ui {
class lrcwidget;
}

class lrcwidget : public QWidget
{
    Q_OBJECT

public:
    explicit lrcwidget(QWidget *parent = nullptr);
    ~lrcwidget();

    // 添加 getter 方法
    QSlider* getSlider() const;
    QPushButton* getPlayButton() const;
    QPushButton* getPrevButton() const;
    QPushButton* getNextButton() const;
    QSlider* getSoundSlider() const;
    QDoubleSpinBox* getSpeedSpinBox() const;
    QPushButton* getModeButton() const;



    void loadLyrics(const QString& filePath);


    QMap<QTime, QString> parseLyrics(const QString& filePath);

    void setCoverImage(const QPixmap &pixmap);
    void updateListHeight();

    void showLyric();//显示歌词
    void hideLyric();//隐藏歌词
    void resetCoverImage();//更新封面
    void clearLyrics();  // 添加一个清空歌词的方法

    void updateLabProcess(const QString &text);


signals:
    void sliderMoved(int position);
    void sliderPressed();
    void sliderReleased();

private slots:
    void on_horizontalSlider_sliderMoved(int position);
    void on_horizontalSlider_sliderPressed();
    void on_horizontalSlider_sliderReleased();

protected:
    void paintEvent(QPaintEvent *event) override;
    QImage applyBlurToImage(QImage sourceImage, int radius);

public slots:
    void updateLyrics(qint64 position);

private slots:
    void on_pushButton_clicked();


private:
    Ui::lrcwidget *ui;
    QPixmap coverPixmap;//保存封面图像
    QPropertyAnimation *animation;//动画对象
    QMap<QTime, QString> lyricsMap;//歌词时间映射

    QLabel *noLyricsLabel; // 用于显示没有歌词的提示
    QStackedWidget *stackedWidget; // 用于管理多个窗口部件
};

#endif // LRCWIDGET_H
