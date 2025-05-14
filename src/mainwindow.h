#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMultimedia>
#include "lrcwidget.h"
#include "searchwidget.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    QMediaPlayer *player;
    lrcwidget *lrcWidget;
    bool loopPay = true;
    QString positionTime;
    QString durationTime;
    bool isDragging;
    QPoint lastMousePosition;

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void updateCoverArtSize();
    QStringList getSavedMusicPaths();
    void loadSavedMusic();
    void do_positionChanged(qint64 position);
    void do_durationChanged(qint64 duration);
    void do_sourceChanged(const QUrl &media);
    void do_playbackStateChanged(QMediaPlayer::PlaybackState newState);
    void do_metaDataChanged();


    void on_btnAdd_clicked();

    void on_btnRemove_clicked();

    void on_btnClear_clicked();

    void on_btnPlay_clicked();

    void on_btnPause_clicked();

    void on_btnStop_clicked();

    void on_btnPrevious_clicked();

    void on_btnNext_clicked();

    void on_doubleSpinBox_valueChanged(double arg1);

    void on_btnLoop_clicked(bool checked);

    void on_btnSound_clicked();

    void on_sliderVolumn_valueChanged(int value);

    void on_sliderPosition_valueChanged(int value);

    void on_listWidget_doubleClicked(const QModelIndex &index);

    void on_btnCover_clicked();

    //网络搜索歌曲
    void on_btnSearch_clicked();
    void onSearchFinished(QNetworkReply *reply);

    void on_pushButton_clicked();


    // 新增槽函数
    void lrcWidget_sliderMoved(int value);
    void lrcWidget_playPauseToggled();
    void lrcWidget_prevClicked();
    void lrcWidget_nextClicked();
    void lrcWidget_volumeChanged(int value);
    void lrcWidget_speedChanged(double value);
    void lrcWidget_modeChanged();

    void lrcWidget_sliderPressed();
    void lrcWidget_sliderReleased();

private:
    Ui::MainWindow *ui;

    searchwidget *searchWidget;
    QNetworkAccessManager *networkManager;
    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};


#endif // MAINWINDOW_H
