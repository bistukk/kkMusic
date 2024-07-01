#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_lrcwidget.h""
#include<QDebug>
#include <QFileDialog>
#include "lrcwidget.h"
bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{

    // 处理键盘按键事件
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Delete && watched == ui->listWidget) {
            delete ui->listWidget->takeItem(ui->listWidget->currentRow());
            return true;
        }
    }

    // 处理滑块点击事件
    if (event->type() == QEvent::MouseButtonPress && watched == ui->sliderPosition) {

        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        // 计算鼠标点击位置对应的值

        if (mouseEvent->button() == Qt::LeftButton)	//判断左键
        {

            int clickedValue = ui->sliderPosition->minimum() + ((ui->sliderPosition->maximum() - ui->sliderPosition->minimum()) * mouseEvent->pos().x() / ui->sliderPosition->width());
            qDebug() << "触发了点击事件，位置为:" << clickedValue << Qt::endl;
            ui->sliderPosition->setValue(clickedValue);
            player->setPosition(clickedValue); // 设置播放器位置

            return true;

        }

    } else if (event->type() == QEvent::MouseMove && watched == ui->sliderPosition) {
        qDebug() << "触发了移动事件" << Qt::endl;
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->buttons() & Qt::LeftButton) {
            int draggedValue = ui->sliderPosition->minimum() + ((ui->sliderPosition->maximum() - ui->sliderPosition->minimum()) * mouseEvent->pos().x() / ui->sliderPosition->width());
            ui->sliderPosition->setValue(draggedValue); // 更新滑块的值
            player->setPosition(draggedValue); // 设置播放器位置
            return true;
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap pixmap(":/images/images/KK.jpg");
    ui->label->setPixmap(pixmap.scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    isDragging == false;
    setWindowFlags(Qt::FramelessWindowHint);
    loadSavedMusic();
    ui->listWidget->installEventFilter(this);
    ui->sliderPosition->installEventFilter(this);
    player = new QMediaPlayer(this);
    lrcWidget = new lrcwidget(this);
    //lrcWidget->raise();
    lrcWidget->hide();
    QAudioOutput *audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);

    searchWidget = new searchwidget(this);
    searchWidget->resize(760, 405);  // 设置宽度和高度为500像素
    searchWidget->move(11, 52);  // 将searchWidget移动到(100, 100)的位置
    searchWidget->hide();
    networkManager = new QNetworkAccessManager(this);


    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::do_positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::do_durationChanged);
    connect(player, &QMediaPlayer::sourceChanged, this, &MainWindow::do_sourceChanged);
    connect(player, &QMediaPlayer::playbackStateChanged, this, &MainWindow::do_playbackStateChanged);
    connect(player, &QMediaPlayer::metaDataChanged, this, &MainWindow::do_metaDataChanged);

    //链接歌词界面
    connect(player, &QMediaPlayer::positionChanged, lrcWidget, &lrcwidget::updateLyrics);

    //链接网络搜索歌曲
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onSearchFinished);

    // 链接 lrcwidget 的控件与 MainWindow 的槽函数
    connect(lrcWidget->getSlider(), &QSlider::sliderMoved, this, &MainWindow::lrcWidget_sliderMoved);
    connect(lrcWidget->getPlayButton(), &QPushButton::clicked, this, &MainWindow::lrcWidget_playPauseToggled);
    connect(lrcWidget->getPrevButton(), &QPushButton::clicked, this, &MainWindow::lrcWidget_prevClicked);
    connect(lrcWidget->getNextButton(), &QPushButton::clicked, this, &MainWindow::lrcWidget_nextClicked);
    connect(lrcWidget->getSoundSlider(), &QSlider::valueChanged, this, &MainWindow::lrcWidget_volumeChanged);
    connect(lrcWidget->getSpeedSpinBox(), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::lrcWidget_speedChanged);
    connect(lrcWidget->getModeButton(), &QPushButton::clicked, this, &MainWindow::lrcWidget_modeChanged);

    // 连接 lrcwidget 的控件与 MainWindow 的槽函数
    connect(lrcWidget, &lrcwidget::sliderMoved, this, &MainWindow::lrcWidget_sliderMoved);
    connect(lrcWidget, &lrcwidget::sliderPressed, this, &MainWindow::lrcWidget_sliderPressed);
    connect(lrcWidget, &lrcwidget::sliderReleased, this, &MainWindow::lrcWidget_sliderReleased);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::lrcWidget_sliderMoved(int value)
{
    qDebug() << "Slider moved to" << value;
    player->setPosition(value);
}

void MainWindow::lrcWidget_playPauseToggled()
{
    qDebug() << "Play/Pause toggled";
    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->pause();
    } else {
        player->play();
    }
}

void MainWindow::lrcWidget_sliderPressed()
{
    player->pause();
}

void MainWindow::lrcWidget_sliderReleased()
{
    player->play();
}

void MainWindow::lrcWidget_prevClicked()
{
    qDebug() << "Previous clicked";
    on_btnPrevious_clicked();
}

void MainWindow::lrcWidget_nextClicked()
{
    qDebug() << "Next clicked";
    on_btnNext_clicked();
}

void MainWindow::lrcWidget_volumeChanged(int value)
{
    qDebug() << "Volume changed to" << value;
    player->audioOutput()->setVolume(value / 100.0);
}

void MainWindow::lrcWidget_speedChanged(double value)
{
    qDebug() << "Speed changed to" << value;
    player->setPlaybackRate(value);
}

void MainWindow::lrcWidget_modeChanged()
{
    qDebug() << "Mode changed";
    on_btnLoop_clicked(!loopPay);
}



void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = true;
        lastMousePosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - lastMousePosition);
        event->accept();
    }
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
        event->accept();
    }
    QMainWindow::mouseReleaseEvent(event);
}

QStringList MainWindow::getSavedMusicPaths()
{
    // 获取应用程序的目录
    QString appDir = QCoreApplication::applicationDirPath();

    // 相对于应用程序目录的音乐文件目录
    QString musicDirectory = appDir + "/sound";

    qDebug() << "Music directory:" << musicDirectory;
    // 使用 QDir 类来获取指定路径下的所有音乐文件
    QDir directory(musicDirectory);
    QStringList musicFiles = directory.entryList(QStringList() << "*.mp3" << "*.wav" << "*.wma" << "*.flac", QDir::Files);

    // 将文件名转换成完整路径
    QStringList musicPaths;
    foreach (const QString& fileName, musicFiles) {
        QString filePath = musicDirectory + "/" + fileName;
        musicPaths.append(filePath);
    }

    return musicPaths;
}


void MainWindow::loadSavedMusic()
{

    QStringList savedMusicPaths = getSavedMusicPaths();

    if(savedMusicPaths.isEmpty())
        return;

    foreach (const auto& item, savedMusicPaths) {
        QFileInfo fileInfo(item);
        QListWidgetItem *aItem = new QListWidgetItem(fileInfo.fileName());
        aItem->setIcon(QIcon(":/images/images/musicFile.png"));
        aItem->setData(Qt::UserRole, QUrl::fromLocalFile(item));
        ui->listWidget->addItem(aItem);
    }
}

void MainWindow::do_positionChanged(qint64 position)
{
    if(ui->sliderPosition->isSliderDown())
        return;
    ui->sliderPosition->setSliderPosition(position);
    int secs = position/1000;
    int mins = secs/60;
    secs %= 60;
    positionTime = QString::asprintf("%d:%02d", mins, secs);
    ui->labRatio->setText(positionTime + "/" + durationTime);


    // 更新歌词界面的进度条
    lrcWidget->getSlider()->setValue(position);

    // 更新 lrcWidget 的 labProcess 的值
    QTime currentTime(0, 0);
    currentTime = currentTime.addMSecs(position);
    QString timeStr = currentTime.toString("mm:ss");

    int duration = player->duration();
    QTime totalTime(0, 0);
    totalTime = totalTime.addMSecs(duration);
    QString totalTimeStr = totalTime.toString("mm:ss");

    lrcWidget->updateLabProcess(timeStr + "/" + totalTimeStr);

}

void MainWindow::do_durationChanged(qint64 duration)
{
    ui->sliderPosition->setMaximum(duration);
    int secs = duration / 1000;
    int mins = secs/60;
    secs %= 60;
    durationTime = QString::asprintf("%d:%02d", mins, secs);
    ui->labRatio->setText(positionTime + "/" + durationTime);

    // 更新歌词界面的进度条最大值
    lrcWidget->getSlider()->setMaximum(duration);
}

void MainWindow::do_sourceChanged(const QUrl &media)
{
    ui->labCurMedia->setText(media.fileName());

    // 重置封面为默认封面
    lrcWidget->resetCoverImage();

    // 清空当前歌词
    lrcWidget->clearLyrics();

    QString musicPath = media.toLocalFile();
    QFileInfo fileInfo(musicPath);
    QString lyricsPath = fileInfo.path() + "/" + fileInfo.completeBaseName() + ".lrc";

    lrcWidget->loadLyrics(lyricsPath);
}

void MainWindow::do_playbackStateChanged(QMediaPlayer::PlaybackState newState)
{
    ui->btnPlay->setEnabled(newState != QMediaPlayer::PlayingState);
    ui->btnPause->setEnabled(newState == QMediaPlayer::PlayingState);
    ui->btnStop->setEnabled(newState == QMediaPlayer::PlayingState);


    if((newState == QMediaPlayer::StoppedState) && loopPay)
    {
        //循环播放: 1、自动下一首。2、从最后一首跳到第一首
        int count = ui->listWidget->count();
        int curRow = ui->listWidget->currentRow();
        ++curRow;
        curRow = curRow >= count ? 0 : curRow;
        ui->listWidget->setCurrentRow(curRow);
        player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
        player->play();
    }
    //如果不是循环播放,播完一首就暂停
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    //qDebug() << "Mainwindow_size_changed" << Qt::endl;
    QMainWindow::resizeEvent(event);
    qDebug() << "Mainwindow_size_changed" << Qt::endl;
    updateCoverArtSize();
}

void MainWindow::do_metaDataChanged()
{

    //元数据发生变化，修改显示的图片

    QMediaMetaData metaData = player->metaData();
    qDebug() << metaData.value(QMediaMetaData::ThumbnailImage) << Qt::endl;
    qDebug() << metaData.value(QMediaMetaData::CoverArtImage) << Qt::endl;
    QVariant metaImg = metaData.value(QMediaMetaData::ThumbnailImage);
    if(metaImg.isValid())
    {
        qDebug() << "封面图片加载成功！" << Qt::endl;
        QImage img = metaImg.value<QImage>();
        QPixmap musicPixmp = QPixmap::fromImage(img);

        ui->labPic->setPixmap(musicPixmp);
        ui->btnCover->setIcon(musicPixmp);
        ui->btnCover->setIconSize(ui->btnCover->size()); // 图像大小与按钮相同
        ui->btnCover->setFlat(true); // 使按钮背景透明
        ui->btnCover->setStyleSheet("border: none;"); // 移除按钮的边框

        updateCoverArtSize();
    } else{
        qDebug() << "未找到封面图片!";
        // 未找到封面图片时，使用默认封面
        QPixmap defaultCover(":/images/images/KK.jpg");
        ui->labPic->setPixmap(defaultCover);
        ui->btnCover->setIcon(defaultCover);
        ui->btnCover->setIconSize(ui->btnCover->size());
        ui->btnCover->setFlat(true);
        ui->btnCover->setStyleSheet("border: none;");

        updateCoverArtSize();
        //ui->labPic->clear();
    }

}



void MainWindow:: updateCoverArtSize()
{
    // 获取当前窗口大小
    QSize newSize = ui->scrollArea->size();

    // 获取加载的封面图片
    QPixmap pixmap = ui->labPic->pixmap(Qt::ReturnByValue);
    if (!pixmap.isNull())
    {
        // 根据窗口大小调整封面图片大小
        QSize scaledSize = newSize - QSize(30, 30); // 适当减去一些边距
        QPixmap scaledPixmap = pixmap.scaled(scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->labPic->setPixmap(scaledPixmap);
    }
}

void MainWindow::on_btnAdd_clicked()
{
    QString curPath = QDir::homePath();
    QString dlgTitle = "选择音频文件";
    QString filter = "音频文件(*.mp3 *.wav *.wma);;所有文件(*.*)";

    QStringList fileList = QFileDialog::getOpenFileNames(this, dlgTitle, curPath, filter);
    if(fileList.isEmpty())
        return;

    foreach (const auto& item, fileList) {
        QFileInfo fileInfo(item);
        QListWidgetItem *aItem = new QListWidgetItem(fileInfo.fileName());
        aItem->setIcon(QIcon(":/images/images/musicFile.png"));
        aItem->setData(Qt::UserRole, QUrl::fromLocalFile(item));
        ui->listWidget->addItem(aItem);
    }

    //如果现在没有正在播放，就开始播放第一个文件
    if(player->playbackState() != QMediaPlayer::PlayingState){
        ui->listWidget->setCurrentRow(0);
        QUrl source = ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>();
        player->setSource(source);
        player->play();
    }
}


void MainWindow::on_btnRemove_clicked()
{
    //只是从Widget移除
    int index = ui->listWidget->currentRow();
    if(index < 0) return;

    delete ui->listWidget->takeItem(index);
    if(ui->listWidget->count() <= 0)
        loopPay = false;

}


void MainWindow::on_btnClear_clicked()
{
    loopPay = false;
    ui->listWidget->clear();
    player->stop();

}


void MainWindow::on_btnPlay_clicked()
{
    if(player->playbackState() == QMediaPlayer::PausedState)
    {
        player->play();
        return;
    }
    if(ui->listWidget->count() <= 0)
        return;
    if(ui->listWidget->currentRow() < 0)
        ui->listWidget->setCurrentRow(0);
    player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());

    player->play();
}


void MainWindow::on_btnPause_clicked()
{
    player->pause();

}


void MainWindow::on_btnStop_clicked()
{
    loopPay = false;
    player->stop();
}


void MainWindow::on_btnPrevious_clicked()
{
    int curRow = ui->listWidget->currentRow();
    --curRow;
    curRow = curRow < 0 ? ui->listWidget->count()-1 : curRow;
    ui->listWidget->setCurrentRow(curRow);
    loopPay = false;
    player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
    player->play();
    loopPay = ui->btnLoop->isChecked();
}


void MainWindow::on_btnNext_clicked()
{
    int count = ui->listWidget->count();
    int curRow = ui->listWidget->currentRow();
    ++curRow;
    curRow = curRow >= count ? 0 : curRow;
    ui->listWidget->setCurrentRow(curRow);
    loopPay = false;
    player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
    player->play();
    loopPay = ui->btnLoop->isChecked();
}


void MainWindow::on_doubleSpinBox_valueChanged(double arg1)
{
    //player->setPlaybackRate(arg1);
    // 先暂停播放
    if (player->playbackState() == QMediaPlayer::PlayingState)
        player->pause();

    // 设置播放速率
    player->setPlaybackRate(arg1);

    // 如果之前是播放状态，恢复播放
    if (player->playbackState() == QMediaPlayer::PausedState)
        player->play();
}


void MainWindow::on_btnLoop_clicked(bool checked)
{
    loopPay = checked;
}


void MainWindow::on_btnSound_clicked()
{
    bool mute = player->audioOutput()->isMuted();
    player->audioOutput()->setMuted(!mute);
    if(mute)
        ui->btnSound->setIcon(QIcon(":/images/images/volumn.bmp"));
    else
        ui->btnSound->setIcon(QIcon(":/images/images/mute.bmp"));

}


void MainWindow::on_sliderVolumn_valueChanged(int value)
{
    player->audioOutput()->setVolume(value/100.0);
}


void MainWindow::on_sliderPosition_valueChanged(int value)
{
    player->setPosition(value);
}


void MainWindow::on_listWidget_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    loopPay = false;
    player->setSource(ui->listWidget->currentItem()->data(Qt::UserRole).value<QUrl>());
    player->play();
    loopPay = true;
}


void MainWindow::on_btnCover_clicked()
{
    qDebug() << 1 << Qt::endl;
    //lrcwidget *lrc_widget = new lrcwidget(this);

    qDebug() << 2 << Qt::endl;
    lrcWidget->setAttribute(Qt::WA_DeleteOnClose);
    qDebug() << 3 << Qt::endl;
    lrcWidget->setWindowTitle("歌词窗口");
    QMediaMetaData metaData = player->metaData();
    QVariant metaImg = metaData.value(QMediaMetaData::ThumbnailImage);
    if(metaImg.isValid())
    {
        qDebug() << "封面图片2加载成功！" << Qt::endl;
        QImage img = metaImg.value<QImage>();
        QPixmap musicPixmp = QPixmap::fromImage(img);
        lrcWidget->setCoverImage(musicPixmp);

    }
    qDebug() << 4 << Qt::endl;
    lrcWidget->setGeometry(0, height(), width(), height());
    qDebug() << 5 << Qt::endl;
    lrcWidget->show();
    lrcWidget->showLyric();
    qDebug() << 6 << Qt::endl;
}


void MainWindow::on_btnSearch_clicked()
{
    qDebug() << "s1" << Qt::endl;
    QUrl url("http://mobilecdn.kugou.com/api/v3/search/song?format=json&keyword=" + ui->editSerch->text() + "&page=1&pagesize=20&showtype=1");
    qDebug() << "s2" << Qt::endl;

    QNetworkRequest request(url);
    qDebug() << "s3" << Qt::endl;
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0 ...");
    qDebug() << "s4" << Qt::endl;
    request.setRawHeader("Referer", "http://www.kuwo.cn/");
    qDebug() << "s5" << Qt::endl;
    networkManager->get(request);
    qDebug() << "s6" << Qt::endl;


//    QNetworkRequest request(url);
//    qDebug() << "s3" << Qt::endl;
//    networkManager->get(request);
//    qDebug() << "s4" << Qt::endl;
}

void MainWindow::onSearchFinished(QNetworkReply *reply)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << "HTTP Status Code:" << statusCode;
    if(reply->error() == QNetworkReply::NoError)
    {
        qDebug() << "Network success:";
        QByteArray response_data = reply->readAll();
        qDebug() << "Response data:" << response_data;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response_data);
        if (!jsonDoc.isObject()) {
            qDebug() << "Error: JSON document is not an object.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("data") || !jsonObj["data"].isObject()) {
            qDebug() << "Error: JSON 'data' is not an object or not exist.";
            return;
        }
        QJsonObject dataObj = jsonObj["data"].toObject();
        if (!dataObj.contains("info") || !dataObj["info"].isArray()) {
            qDebug() << "Error: JSON 'info' is not an array or not exist.";
            return;
        }
        QJsonArray results = dataObj["info"].toArray();

        QStringList songInfoList;
        for(const QJsonValue &value : results)
        {
            if (!value.isObject()) {
                qDebug() << "Error: JSON element is not an object.";
                continue;
            }
            QJsonObject songObj = value.toObject();
            QString songInfo;
            if (songObj.contains("songname") && songObj["songname"].isString()) {
                songInfo +=  songObj["songname"].toString() + ",";
            }
            if (songObj.contains("singername") && songObj["singername"].isString()) {
                songInfo += songObj["singername"].toString() + ",";
            }
            if (songObj.contains("duration") && songObj["duration"].isDouble()) {
                int duration = songObj["duration"].toInt();
                songInfo +=  QString::number(duration / 60) + "分" + QString::number(duration % 60) + "秒";
            }
            songInfoList.append(songInfo);
        }
        if (searchWidget) {
            searchWidget->displaySearchResults(songInfoList);
            searchWidget->show();
        } else {
            qDebug() << "Error: searchWidget is null.";
        }
    } else {
        qDebug() << "Network error:" << reply->errorString();
    }
    reply->deleteLater();

}




void MainWindow::on_pushButton_clicked()
{
    searchWidget->hide();
}

