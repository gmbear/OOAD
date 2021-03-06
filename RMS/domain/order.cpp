#include "order.h"

Order::Order(Seat *seat): _seat(seat)
{
    _orderItemList = new std::map<int, OrderItem*>();
    _orderComplete = false;
}

int Order::orderCount(){
    int count = 0;
    for(std::map<int ,OrderItem *>::iterator it = _orderItemList->begin();it!=_orderItemList->end();it++ ){
        count +=it->second->getQuantity();
    }
    return count;
}

int Order::getAmount(){
    int amount = 0;
    for(std::map<int ,OrderItem *>::iterator it = _orderItemList->begin();it!=_orderItemList->end();it++ ){
        amount +=it->second->subtotal();
    }
    return amount;
}

void Order::add(Meal *meal, int quantity){
    std::map<int ,OrderItem *>::iterator it = _orderItemList->find(meal->getId());

    if( it != _orderItemList->end()){
        it->second->addQuantity(quantity);
    }
    else {
        (*_orderItemList)[meal->getId()] = new OrderItem(meal,quantity);
    }
}

void Order::createBill(){
    if(_bill != nullptr){
        delete _bill;
        _bill = nullptr;
    }
    _bill = new Bill(getAmount());
}

bool Order::pay(int amount){
    _orderComplete = _bill->pay(amount);
    return _orderComplete;
}

int Order::getBalance(){
    return _bill->getBalance();
}

Receipt * Order::getReceipt(){
    if(_orderComplete){
        Receipt * receipt = new Receipt(_seat->getTableName());
        for(std::map<int, OrderItem*>::iterator it=_orderItemList->begin();it != _orderItemList->end();it++){
            Meal * meal = it->second->getMeal();
            receipt->setItem(meal->getName(),it->second->subtotal());
        }
        return receipt;
    }else {
        return nullptr;
    }

}

int Order::getSeatId(){
    return _seat->getSeatId();
}

std::vector<OrderPair> * Order::getOrderPair(){
    std::vector<OrderPair> * orderPairList = new std::vector<OrderPair>();
    for(std::map<int, OrderItem*>::iterator it=_orderItemList->begin();it != _orderItemList->end();it++){
        Meal * meal = it->second->getMeal();
        OrderPair orderPair(meal->getId(),it->second->getQuantity());
        orderPairList->push_back(orderPair);
    }
    return orderPairList;
}

int Order::getMealAmount(int id){
    int amount = 0;
    for(std::map<int, OrderItem*>::iterator it=_orderItemList->begin();it != _orderItemList->end();it++){
        Meal * meal = it->second->getMeal();
        if(meal->getId() == id){
            amount += it->second->getQuantity();
        }
    }
    return amount;
}
