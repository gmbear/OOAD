#ifndef TEST_SEAT_DAO_H
#define TEST_SEAT_DAO_H

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "../../../dao/seat_dao.h"
#include <string>
#include <iostream>
#include <QDebug>

TEST(TestSeatDao, getSeatList)
{
    SeatDao * seatDao = new SeatDao("rms.db");
    ASSERT_EQ("rms.db",seatDao->getPath());
    std::map<int,Seat *> *seatList;
    try {
        seatList = seatDao->getSeatList();
    } catch (std::string error) {
        ASSERT_EQ("打開資料庫失敗",error);
    }
    ASSERT_EQ(true,(*seatList)[1]->isUsed());

}

TEST(TestSeatDao, getSeat)
{
    SeatDao * seatDao = new SeatDao("rms.db");
    ASSERT_EQ("rms.db",seatDao->getPath());
    Seat * seat;
    try {
        seat = seatDao->getSeat(4);
    } catch (std::string error) {
        ASSERT_EQ("打開資料庫失敗",error);
    }
    ASSERT_EQ(4,seat->getSeatId());

}

#endif // TEST_SEAT_DAO_H