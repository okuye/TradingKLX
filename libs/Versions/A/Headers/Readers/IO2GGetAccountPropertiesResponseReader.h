#pragma once

class Order2Go2 IO2GGetAccountPropertiesResponseReader : public IAddRef
{
 protected:
    IO2GGetAccountPropertiesResponseReader();
 public:
    /** Gets a number of AccountProperties.*/
    virtual int size() = 0;

    /** Get DealerIntFlg.*/
    virtual const char *getDealerIntFlg(int index) = 0;
    /** Get AutoLimit.*/
    virtual int getAutoLimit(int index) = 0;
    /** Get IntrSign.*/
    virtual const char *getIntrSign(int index) = 0;
    /** Get MrgnReq.*/
    virtual double getMrgnReq(int index) = 0;
    /** Get MrgnReqEntry.*/
    virtual double getMrgnReqEntry(int index) = 0;
    /** Get OrderPriceFlg.*/
    virtual const char *getOrderPriceFlg(int index) = 0;
    /** Get AccountID.*/
    virtual const char *getAccountID(int index) = 0;
    /** Get TradeProfile.*/
    virtual const char *getTradeProfile(int index) = 0;
    /** Get TradeLimit.*/
    virtual int getTradeLimit(int index) = 0;
    /** Get Commission.*/
    virtual double getCommission(int index) = 0;
    /** Get TestFlag.*/
    virtual const char *getTestFlag(int index) = 0;
    /** Get NegBalanceFlg.*/
    virtual const char *getNegBalanceFlg(int index) = 0;
    /** Get Dimension.*/
    virtual const char *getDimension(int index) = 0;
    /** Get MrgnEnabledFlg.*/
    virtual const char *getMrgnEnabledFlg(int index) = 0;
    /** Get MrgnReqAware.*/
    virtual double getMrgnReqAware(int index) = 0;
    /** Get MrgnUnitType.*/
    virtual const char *getMrgnUnitType(int index) = 0;
    /** Get EqtyEnabledFlg.*/
    virtual const char *getEqtyEnabledFlg(int index) = 0;
    /** Get EqtyStop.*/
    virtual double getEqtyStop(int index) = 0;
    /** Get EqtyLimit.*/
    virtual double getEqtyLimit(int index) = 0;
    /** Get aPSName.*/
    virtual const char *getaPSName(int index) = 0;
    /** Get aPosMaintenance.*/
    virtual const char *getaPosMaintenance(int index) = 0;
};

