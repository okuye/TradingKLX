#pragma once

class Order2Go2 IO2GGetMarketResponseReader : public IAddRef
{
 protected:
    IO2GGetMarketResponseReader();
 public:
    /** Gets a number of Market.*/
    virtual int size() = 0;

    /** Get Status.*/
    virtual const char *getStatus(int index) = 0;
    /** Get IntrFlag.*/
    virtual const char *getIntrFlag(int index) = 0;
    /** Get Msg.*/
    virtual const char *getMsg(int index) = 0;
};

