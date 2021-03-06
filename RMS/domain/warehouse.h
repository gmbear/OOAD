#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include "./cargo.h"
#include <map>
class Warehouse
{
public:
    Warehouse();
    void buildCargo(Cargo * cargo);
    bool increaseCargoAmount(int id,int amount);
    bool decreaseCargoAmount(int id,int amount);
    int getCargoAmount(int id);
    void buildCargoList(std::map<int,Cargo *> * cargoList);
    std::map<int,Cargo *> * getCargoList();

private:
    int _capacity = 200;
    std::map<int,Cargo *> * _cargoList;
};

#endif // WAREHOUSE_H
