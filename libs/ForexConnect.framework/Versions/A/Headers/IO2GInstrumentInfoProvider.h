#pragma once

class Order2Go2 IO2GInstrumentInfoProvider : public IAddRef
{
public:
    virtual int getBaseUnitSize(const char *instrument) = 0;
    virtual O2GInstrumentType getInstrumentType(const char* instrument) = 0;
    virtual O2GTableStatus getInstrumentTableStatus() = 0;
};
