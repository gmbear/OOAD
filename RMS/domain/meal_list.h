#ifndef MEAL_LIST_H
#define MEAL_LIST_H

#include "meal.h"
#include <map>

class MealList
{
public:
    MealList();
    void add(int id,Meal *meal);
    int getNumberOfMeals();

private:
    std::map<int,Meal *> *_mapMeal;
};

#endif // MEAL_LIST_H
