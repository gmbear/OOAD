#ifndef STAFF_MANAGE_H
#define STAFF_MANAGE_H

#include <QWidget>
#include "rmshandler.h"
#include "domain/order_item.h"
#include <QPushButton>
#include <QTextBrowser>

namespace Ui {
class StaffManage;
}

class StaffManage : public QWidget
{
    Q_OBJECT

public:
    explicit StaffManage(QWidget *parent,RMSHandler *rmsHandler);
    ~StaffManage();

private:
    Ui::StaffManage *ui;
    RMSHandler * _rmsHandler;
    std::map<int, SeatOrder * > *_seatOrederList;
    void refreshSeatName();
    void refreshSeat();
    void setSeatContent(QPushButton * status, QTextBrowser * content, int id);
    void clearSeatContent(QPushButton * status, QTextBrowser * content);
    void refreshSeatCombo();
private slots:
    void on_determineButton_clicked();
    void on_seatCombo_currentIndexChanged(int index);
    void onReceiveSocket(QString input);
};

#endif // STAFF_MANAGE_H