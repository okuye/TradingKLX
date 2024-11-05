#pragma once

class Order2Go2 IO2GGetUserPropertiesResponseReader : public IAddRef
{
 protected:
    IO2GGetUserPropertiesResponseReader();
 public:
    /** Gets a number of UserProperties.*/
    virtual int size() = 0;

    /** Get ConnectFlag.*/
    virtual const char *getConnectFlag(int index) = 0;
    /** Get FirstName.*/
    virtual const char *getFirstName(int index) = 0;
    /** Get MiddleName.*/
    virtual const char *getMiddleName(int index) = 0;
    /** Get LastName.*/
    virtual const char *getLastName(int index) = 0;
    /** Get TradeProfile.*/
    virtual const char *getTradeProfile(int index) = 0;
    /** Get SSN.*/
    virtual const char *getSSN(int index) = 0;
    /** Get Gender.*/
    virtual const char *getGender(int index) = 0;
    /** Get MaritalStatus.*/
    virtual const char *getMaritalStatus(int index) = 0;
    /** Get JointFirstName.*/
    virtual const char *getJointFirstName(int index) = 0;
    /** Get JointMiddleName.*/
    virtual const char *getJointMiddleName(int index) = 0;
    /** Get JointLastName.*/
    virtual const char *getJointLastName(int index) = 0;
    /** Get DOB.*/
    virtual double getDOB(int index) = 0;
    /** Get Addr.*/
    virtual const char *getAddr(int index) = 0;
    /** Get Addr2.*/
    virtual const char *getAddr2(int index) = 0;
    /** Get City.*/
    virtual const char *getCity(int index) = 0;
    /** Get Country.*/
    virtual const char *getCountry(int index) = 0;
    /** Get Misc.*/
    virtual const char *getMisc(int index) = 0;
    /** Get Email.*/
    virtual const char *getEmail(int index) = 0;
    /** Get Fax.*/
    virtual const char *getFax(int index) = 0;
    /** Get State.*/
    virtual const char *getState(int index) = 0;
    /** Get Phone.*/
    virtual const char *getPhone(int index) = 0;
    /** Get Phone2.*/
    virtual const char *getPhone2(int index) = 0;
    /** Get Zip.*/
    virtual const char *getZip(int index) = 0;
    /** Get URL.*/
    virtual const char *getURL(int index) = 0;
    /** Get BizAddr.*/
    virtual const char *getBizAddr(int index) = 0;
    /** Get BizAddr2.*/
    virtual const char *getBizAddr2(int index) = 0;
    /** Get BizCity.*/
    virtual const char *getBizCity(int index) = 0;
    /** Get BizCompany.*/
    virtual const char *getBizCompany(int index) = 0;
    /** Get BizCountry.*/
    virtual const char *getBizCountry(int index) = 0;
    /** Get BizDesc.*/
    virtual const char *getBizDesc(int index) = 0;
    /** Get BizDepartment.*/
    virtual const char *getBizDepartment(int index) = 0;
    /** Get BizEmail.*/
    virtual const char *getBizEmail(int index) = 0;
    /** Get BizFax.*/
    virtual const char *getBizFax(int index) = 0;
    /** Get BizTitle.*/
    virtual const char *getBizTitle(int index) = 0;
    /** Get BizState.*/
    virtual const char *getBizState(int index) = 0;
    /** Get BizPhone.*/
    virtual const char *getBizPhone(int index) = 0;
    /** Get BizPhone2.*/
    virtual const char *getBizPhone2(int index) = 0;
    /** Get BizZip.*/
    virtual const char *getBizZip(int index) = 0;
    /** Get BizURL.*/
    virtual const char *getBizURL(int index) = 0;
    /** Get SecQuestion.*/
    virtual const char *getSecQuestion(int index) = 0;
    /** Get SecAnswer.*/
    virtual const char *getSecAnswer(int index) = 0;
};

