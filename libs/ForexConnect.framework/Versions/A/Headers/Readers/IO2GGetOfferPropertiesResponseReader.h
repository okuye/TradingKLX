#pragma once

class Order2Go2 IO2GGetOfferPropertiesResponseReader : public IAddRef
{
 protected:
    IO2GGetOfferPropertiesResponseReader();
 public:
    /** Gets a number of OfferProperties.*/
    virtual int size() = 0;

    /** Get DealerIntFlg.*/
    virtual const char *getDealerIntFlg(int index) = 0;
    /** Get AutoLimit.*/
    virtual int getAutoLimit(int index) = 0;
    /** Get RateVariat.*/
    virtual double getRateVariat(int index) = 0;
    /** Get RfqLifetime.*/
    virtual int getRfqLifetime(int index) = 0;
    /** Get OrdrLifetime.*/
    virtual int getOrdrLifetime(int index) = 0;
    /** Get SellIntr.*/
    virtual double getSellIntr(int index) = 0;
    /** Get BuyIntr.*/
    virtual double getBuyIntr(int index) = 0;
    /** Get Feed.*/
    virtual const char *getFeed(int index) = 0;
    /** Get FeedPrice.*/
    virtual const char *getFeedPrice(int index) = 0;
    /** Get FeedAsk.*/
    virtual double getFeedAsk(int index) = 0;
    /** Get FeedBid.*/
    virtual double getFeedBid(int index) = 0;
    /** Get MrgnRate.*/
    virtual double getMrgnRate(int index) = 0;
};

