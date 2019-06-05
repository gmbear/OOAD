#include "rmshandler.h"
#include "socket.h"
#include <QApplication>
#include <QDebug>

RMSHandler::RMSHandler(SeatDao * seatDao, MenuDao * menuDao, CargoDao * cargoDao):
    _seatDao(seatDao), _menuDao(menuDao), _cargoDao(cargoDao)
{
    _socket = new Socket();
    _seatList = new SeatList();
    _menu = new Menu();
    _seatOrderList = new SeatOrderList();
    _warehouse = new Warehouse();
}

void RMSHandler::Host(QString& IP, quint16& port){
    _socket->Host(IP,port);
}

bool RMSHandler::Connect(QString IP, quint16 port){
    return _socket->Connect(IP,port);
}

void RMSHandler::SetSocketLisener(const QObject * receiver, const char * member){
    connect(_socket,SIGNAL(received(QString)),receiver,member);
}

void RMSHandler::SetNewConnectionListener(const QObject *receiver, const char *member){
    connect(_socket,SIGNAL(newConnection()),receiver,member);
}

void RMSHandler::SetConnectedListener(const QObject *receiver, const char *member){
    connect(_socket,SIGNAL(socketConnected()),receiver,member);
}

void RMSHandler::SetDisconnectedListener(const QObject *receiver, const char *member){
    connect(_socket,SIGNAL(socketDisconnected()),receiver,member);
}

void RMSHandler::SetConnectFailListener(const QObject *receiver, const char *member){
    connect(_socket,SIGNAL(connectFail()),receiver,member);
}

void RMSHandler::notify(QString output){
    _socket->notify(output);
}

bool RMSHandler::IsConnect(){
    return _socket->getConnection();
}

bool RMSHandler::IsHost(){
    return _socket->getHost();
}

void RMSHandler::refreshSeatList(){
    std::map<int,Seat *> * seats = _seatDao->getSeatList();
    _seatList->refresh(seats);
}

std::map<int,Seat *> * RMSHandler::showSeatList(){
    return  _seatList->getAllSeats();
}

void RMSHandler::refreshMenu(){
    std::map<int,Meal *> * mealList = _menuDao->getMealList();
    _menu->refresh(mealList);
}

void RMSHandler::refreshSeatOrderList(){
    std::map<int,Seat *> * seats = _seatDao->getSeatList();
    _seatOrderList->refresh(seats);
    for(int i=1; i<=10;i++){
        std::vector<OrderPair> * orderPairList = _seatDao->getOrderPair(i);
        for(std::vector<OrderPair>::iterator it = orderPairList->begin(); it!= orderPairList->end(); it++){
            Meal * meal = _menu->getMeal(it->getId());
            _seatOrderList->addOrder(i,meal,(*it).getAmount());
        }
    }
}

std::map<int,Meal *> * RMSHandler::showMenu(){
    return  _menu->getMenu();
}

void RMSHandler::createOrder(int seatId){
    Seat * seat = _seatList->getSeat(seatId);
    _order = new Order(seat);
}

void RMSHandler::cancelOrder(){
    delete _order;
}
void RMSHandler::enterOrderItem(int mealId, int amount){
    Meal * meal = _menu->getMeal(mealId);
    _order->add(meal,amount);
}

void RMSHandler::createBill(){
    _order->createBill();
}

bool RMSHandler::pay(int money){
    return _order->pay(money);
}

int RMSHandler::getAmount(){
    return _order->getAmount();
}

int RMSHandler::getBalance(){
    return _order->getBalance();
}

Receipt * RMSHandler::getReceipt(){
    return _order->getReceipt();
}

void RMSHandler::refreshSeatOrder(int seatId){
    _seatOrderList->clearSeat(seatId);
    std::vector<OrderPair> * orderPairList = _seatDao->getOrderPair(seatId);
    for(std::vector<OrderPair>::iterator it = orderPairList->begin(); it!= orderPairList->end(); it++){
     Meal * meal = _menu->getMeal(it->getId());
     _seatOrderList->addOrder(seatId,meal,(*it).getAmount());
    }
}

void RMSHandler::refreshSeat(int seatId){
    Seat * seat = _seatDao->getSeat(seatId);
    _seatList->refresh(seatId,seat);
}
void RMSHandler::completeOrder(){
    int seatId = _order->getSeatId();
    _seatDao->setSeatUsed(seatId);
    _seatDao->setSeatOrderPair(seatId,_order->getOrderPair());
    Seat * seat = _seatDao->getSeat(seatId);
    _seatList->refresh(seatId,seat);
    delete _order;
}

void RMSHandler::clearSeat(int seatId){
    _seatDao->clearSeat(seatId);
    _seatOrderList->clearSeat(seatId);
}

std::map<int, SeatOrder * > * RMSHandler::showSeatOrderList(){
    return _seatOrderList->getAllSeatOrder();
}

int RMSHandler::getOrderSeatId(){
    return _order->getSeatId();
}

void RMSHandler::closeDB(){
    _seatDao->closeDB();
    _menuDao->closeDB();
    _cargoDao->closeDB();
}

bool RMSHandler::createMeal(QString name, QString description, int price){
    bool result = false;
    result = _menuDao->createMeal(name,description,price);
    std::string stdName = string((const char *)name.toLocal8Bit());
    std::string stdDescription = string((const char *)description.toLocal8Bit());

    if(result)
        _menu->createMeal(stdName,stdDescription,price);
    return result;
}

bool RMSHandler::editMeal(int id, QString name, QString description, int price){
    bool result = false;
    result = _menuDao->editMeal(id,name,description,price);
    std::string stdName = string((const char *)name.toLocal8Bit());
    std::string stdDescription = string((const char *)description.toLocal8Bit());
    if(result)
        _menu->editMeal(id,stdName,stdDescription,price);
    return result;
}

bool RMSHandler::deleteMeal(int id){
    bool result = false;
    result = _menuDao->deleteMeal(id);
    if(result)
        _menu->deleteMeal(id);
    return result;
}

//void RMSHandler::refreshCargo(){
//    std::map<int,Meal *> * mealList = _menuDao->getMealList();
//    _menu->refresh(mealList);

//}

std::map<int,Cargo *> * RMSHandler::getCargoList(){
    std::map<int,Cargo *> * cargoList;
    cargoList = _warehouse->getCargoList();
    return cargoList;
}

void RMSHandler::refreshCargoList(){
    std::map<int,Cargo *> * cargoList;
    cargoList = _cargoDao->getCargoList();
    _warehouse->buildCargoList(cargoList);
}

bool RMSHandler::increaseCargoAmount(int id, int amount){
    if(!_warehouse->increaseCargoAmount(id,amount))
        return false;
    else{
        int cargoAmount = _warehouse->getCargoAmount(id);
        qDebug()<<"cargoAmount = "<<cargoAmount;
        qDebug()<<"amount = "<<amount;
        _cargoDao->refresh(id,cargoAmount);
        return true;
    }
}
