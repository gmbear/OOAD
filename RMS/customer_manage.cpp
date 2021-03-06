#include "customer_manage.h"
#include "ui_customer_manage.h"
#include "receipt_dilog.h"
#include "dialog.h"
#include <QtDebug>

CustomerManage::CustomerManage(QWidget *parent,RMSHandler * rmsHandler) :
    QWidget(parent),
    ui(new Ui::CustomerManage),
    _rmsHandler(rmsHandler)
{
    ui->setupUi(this);
    ui->removeBtn->setVisible(false);
    connect(ui->ensureBtn,SIGNAL(clicked()),this,SLOT(ensureSeat()));
    connect(ui->enterOrderBtn,SIGNAL(clicked()),this,SLOT(addToTable()));
    connect(ui->payBtn,SIGNAL(clicked()),this,SLOT(showTotalAmount()));
    rmsHandler->SetDisconnectedListener(this,SLOT(onDisconnect()));
    rmsHandler->refreshMenu();
    rmsHandler->refreshSeatList();

    _rmsHandler->SetSocketLisener(this,SLOT(onReceiveSocket(QString)));

    //reset menu&seatList
    _mealList = rmsHandler->showMenu();
    _seatList = rmsHandler->showSeatList();
    rmsHandler->refreshCargoList();
    int index = 0;
    for(std::map<int, Meal * >::iterator it = _mealList->begin(); it != _mealList->end();it++){
        int cargoAmount = _rmsHandler->getCargoAmount(it->second->getId());

        ui->menuTable->insertRow(ui->menuTable->rowCount());
        qDebug()<<"test"<<ui->menuTable->rowCount();
        QString name = QString::fromLocal8Bit(it->second->getName().c_str());
        QString price = QString::number(it->second->getPrice());
        QString description = QString::fromLocal8Bit(it->second->getDescription().c_str());
        ui->menuTable->setItem(index,0,new QTableWidgetItem(name));
        ui->menuTable->setItem(index,1,new QTableWidgetItem(price));
        ui->menuTable->setItem(index,2,new QTableWidgetItem(description));
        index++;

        if(cargoAmount > 0)
            ui->mealCombo->addItem(name,it->second->getId());

    }

    refreshSeatName();
    refreshSeat();
    refreshSeatCombo();

    ui->menuTable->horizontalHeader()->show();
    ui->orderTable->horizontalHeader()->show();
    initViewStatus();
}

CustomerManage::~CustomerManage()
{
    delete ui;
}

void CustomerManage::ensureSeat()
{
    if(ui->seatCombo->currentIndex()!= 0){
        ui->seatCombo->setDisabled(true);
        ui->mealCombo->setDisabled(false);
        ui->amountCombo->setDisabled(false);
        ui->enterOrderBtn->setDisabled(false);
        //ui->deleBtn->setDisabled(false);

        int seatId = ui->seatCombo->currentData().toInt();
        _rmsHandler->createOrder(seatId);
    }
}

void CustomerManage::addToTable()
{
    int mealId = ui->mealCombo->currentData().toInt();
    qDebug()<<"mealId = "<<mealId;
    if(mealId !=0){
//        int cargoAmount = _rmsHandler->getCargoAmount(mealId);
        int amount = ui->amountCombo->currentText().toInt();
        if(!_rmsHandler->checkCargoAmount(mealId,amount)){
            QString dlgTitle = "抱歉";
            QString strInfo = "貨物數量不足";
            QMessageBox::warning(this,dlgTitle,strInfo);
        }
        else {
            ui->payBtn->setDisabled(false);

            int index = ui->orderTable->rowCount();
            ui->orderTable->insertRow(ui->orderTable->rowCount());
            QString name = QString::fromLocal8Bit((*_mealList)[mealId]->getName().c_str());
            ui->orderTable->setItem(index,0,new QTableWidgetItem(name));

            int price =(*_mealList)[mealId]->getPrice();
            int subtotal = price * amount;
            _rmsHandler->enterOrderItem(mealId,amount);

            ui->orderTable->setItem(index,1,new QTableWidgetItem(QString::number(amount)));
            ui->orderTable->setItem(index,2,new QTableWidgetItem(QString::number(subtotal)));

            //cargo
            //_rmsHandler->decreaseCargoAmount(mealId,amount);
        }

    }
}

void CustomerManage::showTotalAmount()
{
    _rmsHandler->createBill();
    //qDebug()<<"test6"<<_rmsHandler->getAmount();
    //_rmsHandler->pay(180);
    payment = new Dialog(this, _rmsHandler);
    connect(payment, SIGNAL(accepted()), this, SLOT(paymentSuccess()));
    connect(payment, SIGNAL(rejected()), this, SLOT(cancelPayment()));
    payment->exec();
    //delete d;
}

void  CustomerManage::initViewStatus()
{
    ui->mealCombo->setDisabled(false);
    ui->seatCombo->setDisabled(false);
    ui->amountCombo->setDisabled(true);
    ui->enterOrderBtn->setDisabled(true);
    ui->removeBtn->setDisabled(true);
    ui->payBtn->setDisabled(true);
    ui->mealCombo->setCurrentIndex(0);
    ui->seatCombo->setCurrentIndex(0);
    ui->amountCombo->setCurrentIndex(0);
    ui-> orderTable->setRowCount(0);
}

void CustomerManage::refreshSeatCombo(){
    ui->seatCombo->clear();
    ui->seatCombo->addItem("請選擇座位" ,0);

    for(std::map<int, Seat *>::iterator it = _seatList->begin(); it!=_seatList->end();it++){
        if(!it->second->isUsed()){
            Seat * seat= it->second;
            int seatId = seat->getSeatId();
            QString seatName = QString::fromLocal8Bit(seat->getTableName().c_str());
            ui->seatCombo->addItem(seatName, seatId);
        }
    }
}

void CustomerManage::refreshSeat(){


    if((*_seatList)[1]->isUsed())
        ui->seatOne->setStyleSheet("background-color: rgb(255, 100, 100);");
    else
        ui->seatOne->setStyleSheet("background-color: rgb(255, 255, 255);");

    if((*_seatList)[2]->isUsed())
        ui->seatTwo->setStyleSheet("background-color: rgb(255, 100, 100);");
    else
        ui->seatTwo->setStyleSheet("background-color: rgb(255, 255, 255);");

    if((*_seatList)[3]->isUsed())
        ui->seatThree->setStyleSheet("background-color: rgb(255, 100, 100);");
    else
        ui->seatThree->setStyleSheet("background-color: rgb(255, 255, 255);");

    if((*_seatList)[4]->isUsed())
        ui->seatFour->setStyleSheet("background-color: rgb(255, 100, 100);");
    else
        ui->seatFour->setStyleSheet("background-color: rgb(255, 255, 255);");

    if((*_seatList)[5]->isUsed())
        ui->seatFive->setStyleSheet("background-color: rgb(255, 100, 100);");
    else
        ui->seatFive->setStyleSheet("background-color: rgb(255, 255, 255);");

    if((*_seatList)[6]->isUsed())
        ui->seatSix->setStyleSheet("background-color: rgb(255, 100, 100);");
    else
        ui->seatSix->setStyleSheet("background-color: rgb(255, 255, 255);");

    if((*_seatList)[7]->isUsed())
        ui->seatSeven->setStyleSheet("background-color: rgb(255, 100, 100);");
    else
        ui->seatSeven->setStyleSheet("background-color: rgb(255, 255, 255);");

    if((*_seatList)[8]->isUsed())
        ui->seatEight->setStyleSheet("background-color: rgb(255, 100, 100);");
    else
        ui->seatEight->setStyleSheet("background-color: rgb(255, 255, 255);");

    if((*_seatList)[9]->isUsed())
        ui->seatNine->setStyleSheet("background-color: rgb(255, 100, 100);");
    else
        ui->seatNine->setStyleSheet("background-color: rgb(255, 255, 255);");

    if((*_seatList)[10]->isUsed())
        ui->seatTen->setStyleSheet("background-color: rgb(255, 100, 100);");
    else
        ui->seatTen->setStyleSheet("background-color: rgb(255, 255, 255);");
}

void CustomerManage::refreshSeatName(){
    std::string stdSeatName = (*_seatList)[1]->getTableName();
    QString seatName = QString::fromLocal8Bit(stdSeatName.c_str());
    ui->seatOne->setText(seatName);
    stdSeatName = (*_seatList)[2]->getTableName();
    seatName = QString::fromLocal8Bit(stdSeatName.c_str());
    ui->seatTwo->setText(seatName);
    stdSeatName = (*_seatList)[3]->getTableName();
    seatName = QString::fromLocal8Bit(stdSeatName.c_str());
    ui->seatThree->setText(seatName);
    stdSeatName = (*_seatList)[4]->getTableName();
    seatName = QString::fromLocal8Bit(stdSeatName.c_str());
    ui->seatFour->setText(seatName);
    stdSeatName = (*_seatList)[5]->getTableName();
    seatName = QString::fromLocal8Bit(stdSeatName.c_str());
    ui->seatFive->setText(seatName);
    stdSeatName = (*_seatList)[6]->getTableName();
    seatName = QString::fromLocal8Bit(stdSeatName.c_str());
    ui->seatSix->setText(seatName);
    stdSeatName = (*_seatList)[7]->getTableName();
    seatName = QString::fromLocal8Bit(stdSeatName.c_str());
    ui->seatSeven->setText(seatName);
    stdSeatName = (*_seatList)[8]->getTableName();
    seatName = QString::fromLocal8Bit(stdSeatName.c_str());
    ui->seatEight->setText(seatName);
    stdSeatName = (*_seatList)[9]->getTableName();
    seatName = QString::fromLocal8Bit(stdSeatName.c_str());
    ui->seatNine->setText(seatName);
    stdSeatName = (*_seatList)[10]->getTableName();
    seatName = QString::fromLocal8Bit(stdSeatName.c_str());
    ui->seatTen->setText(seatName);
}

void CustomerManage::on_mealCombo_currentIndexChanged(int index)
{
    if(index == 0 || index == -1){
        ui->mealDescription->clear();
        ui->mealPrice->clear();
    }else if(index != 0){
        ui->mealDescription->clear();
        ui->mealPrice->clear();
        int mealId = ui->mealCombo->currentData().toInt();
        Meal * meal = (*_mealList)[mealId];
        QString description = QString::fromLocal8Bit(meal->getDescription().c_str());
        QString price = QString::number(meal->getPrice());
        ui->mealDescription->setText(description);
        ui->mealPrice->setText(price);
    }
}

void CustomerManage::paymentSuccess()
{
    qDebug()<<"test"<<_rmsHandler->getBalance();
    payment->hide();
    ReceiptDilog receiptDilog(this, _rmsHandler->getReceipt(),_rmsHandler->getBalance());
    receiptDilog.exec();
    int seatId = _rmsHandler->getOrderSeatId();
    _rmsHandler->completeOrder();
    _rmsHandler->notify(QString::number(seatId));
    initViewStatus();
    refreshSeat();
    refreshSeatCombo();
    refreshMenuCombo();
    delete payment;
}

void CustomerManage::cancelPayment(){
    delete payment;
}

void CustomerManage::onReceiveSocket(QString input){
    int seatId = input.split("\n")[0].toInt();
    qDebug()<<seatId;
    int selectSeatId = ui->seatCombo->currentData().toInt();
    int selectMealId = ui->mealCombo->currentData().toInt();

    if(seatId > 0){
        _rmsHandler->refreshSeat(seatId);
        refreshSeat();
        refreshSeatCombo();

        for(int i = 0; i<ui->seatCombo->count();i++){
            if(selectSeatId == ui->seatCombo->itemData(i).toInt()){
                ui->seatCombo->setCurrentIndex(i);
                break;
            }
        }
    }

    refreshMenuCombo();
    for(int i=0; i<ui->mealCombo->count();i++){
        if(selectMealId == ui->mealCombo->itemData(i).toInt()){
            ui->mealCombo->setCurrentIndex(i);
            break;
        }
    }
}

void CustomerManage::refreshMenuCombo(){
    ui->mealCombo->clear();
    ui->mealCombo->addItem("選擇餐點",0);
    _rmsHandler->refreshCargoList();
    for(std::map<int, Meal * >::iterator it = _mealList->begin(); it != _mealList->end();it++){
        int cargoAmount = _rmsHandler->getCargoAmount(it->second->getId());
        QString name = QString::fromLocal8Bit(it->second->getName().c_str());
        if(cargoAmount > 0)
            ui->mealCombo->addItem(QString(name),it->second->getId());
    }
}

void CustomerManage::onDisconnect(){
    qDebug()<<"staff closed";
    QString dlgTitle = "錯誤";
    QString strInfo = "異常錯誤發生，請通知服務員";
    QMessageBox::warning(this,dlgTitle,strInfo);
    this->close();
}
