#include "searchwidget.h"
#include <QVBoxLayout>
#include <QLabel>

searchwidget::searchwidget(QWidget *parent)
    : QWidget{parent}
{
    tableWidget = new QTableWidget(0, 3, this);  // 创建一个表格，有3列
    QStringList headers;
    headers << "歌曲名" << "作者"  << "时长";
    tableWidget->setHorizontalHeaderLabels(headers);  // 设置表头
    // 设置表格列宽度自动调整
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);  // 设置边距为0
    layout->setSpacing(0);  // 设置间距为0
    layout->addWidget(tableWidget);
    setLayout(layout);

    connect(tableWidget, &QTableWidget::cellDoubleClicked, this, [this](int row, int column) {
        QTableWidgetItem *item = tableWidget->item(row, 0);  // 获取歌曲名所在的单元格
            if (item) {
                emit songDoubleClicked(item->text());
            }
        });
}

void searchwidget::displaySearchResults(const QStringList &results)
{
    tableWidget->setRowCount(results.size());  // 设置行数
    for (int i = 0; i < results.size(); ++i) {
        QStringList songDetails = results[i].split(",");
        for (int j = 0; j < songDetails.size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(songDetails[j]);
            tableWidget->setItem(i, j, item);  // 将歌曲信息添加到表格中
        }
    }
}
