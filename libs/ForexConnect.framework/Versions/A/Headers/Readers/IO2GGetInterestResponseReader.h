#pragma once

class Order2Go2 IO2GGetInterestResponseReader : public IAddRef
{
 protected:
    IO2GGetInterestResponseReader();
 public:
    /** Gets a number of Interest.*/
    virtual int size() = 0;

    /** Get OfferID.*/
    virtual const char *getOfferID(int index) = 0;
    /** Get Instrument.*/
    virtual const char *getInstrument(int index) = 0;
    /** Get IntrBuy.*/
    virtual double getIntrBuy(int index) = 0;
    /** Get IntrSel.*/
    virtual double getIntrSel(int index) = 0;
    /** Get SortOrder.*/
    virtual int getSortOrder(int index) = 0;
    /** Get IntrMult.*/
    virtual int getIntrMult(int index) = 0;
};

