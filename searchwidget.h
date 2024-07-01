#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include "qboxlayout.h"
#include <QWidget>
#include <QTableWidget> >
#include <QHeaderView>

class searchwidget : public QWidget
{
    Q_OBJECT
public:
    explicit searchwidget(QWidget *parent = nullptr);
    void displaySearchResults(const QStringList &results);


private:
    QTableWidget *tableWidget;

signals:
    void songDoubleClicked(const QString &songName);

};

#endif // SEARCHWIDGET_H
