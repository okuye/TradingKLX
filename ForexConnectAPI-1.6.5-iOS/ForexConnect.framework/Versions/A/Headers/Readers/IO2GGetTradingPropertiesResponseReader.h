#pragma once

class Order2Go2 IO2GGetTradingPropertiesResponseReader : public IAddRef
{
 protected:
    IO2GGetTradingPropertiesResponseReader();
 public:
    /** Gets a number of TradingProperties.*/
    virtual int size() = 0;

    /** Get OrderPrice.*/
    virtual const char *getOrderPrice(int index) = 0;
    /** Get SeatBelt.*/
    virtual int getSeatBelt(int index) = 0;
    /** Get AutoMrgn.*/
    virtual const char *getAutoMrgn(int index) = 0;
    /** Get CondDistance.*/
    virtual int getCondDistance(int index) = 0;
    /** Get MaxQuantity.*/
    virtual int getMaxQuantity(int index) = 0;
    /** Get PanicFlg.*/
    virtual const char *getPanicFlg(int index) = 0;
    /** Get GoneToPeeFlg.*/
    virtual const char *getGoneToPeeFlg(int index) = 0;
    /** Get ManualPrices.*/
    virtual const char *getManualPrices(int index) = 0;
    /** Get CrossCurrency.*/
    virtual const char *getCrossCurrency(int index) = 0;
    /** Get PanicLevel.*/
    virtual int getPanicLevel(int index) = 0;
    /** Get IntrMult.*/
    virtual const char *getIntrMult(int index) = 0;
    /** Get CondDistanceE.*/
    virtual int getCondDistanceE(int index) = 0;
};

