/*
 * FledgePower IEC 104 <-> pivot filter plugin.
 *
 * Copyright (c) 2022, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Michael Zillgith (michael.zillgith at mz-automation.de)
 *
 */

#ifndef _IEC104_PIVOT_OBJECT_H
#define _IEC104_PIVOT_OBJECT_H

#include <plugin_api.h>
#include <datapoint.h>

#include "iec104_pivot_filter_config.hpp"

using namespace std;

class PivotObjectException : public std::exception //NOSONAR
{
 public:
    explicit PivotObjectException(const std::string& context):
        m_context(context) {}

    const std::string& getContext(void) {return m_context;};

 private:
    const std::string m_context;
};

class PivotTimestamp
{
public:
    PivotTimestamp(Datapoint* timestampData);
    PivotTimestamp(long ms);
    ~PivotTimestamp();

    void setTimeInMs(long ms);

    int SecondSinceEpoch();
    int FractionOfSecond();
    uint64_t getTimeInMs();

    bool ClockFailure() {return m_clockFailure;};
    bool LeapSecondKnown() {return m_leapSecondKnown;};
    bool ClockNotSynchronized() {return m_clockNotSynchronized;};
    int TimeAccuracy() {return m_timeAccuracy;};

    static uint64_t GetCurrentTimeInMs();

private:

    void handleTimeQuality(Datapoint* timeQuality);

    uint8_t* m_valueArray;

    int m_secondSinceEpoch;
    int m_fractionOfSecond;

    int m_timeAccuracy;
    bool m_clockFailure;
    bool m_leapSecondKnown;
    bool m_clockNotSynchronized;
};



class PivotObject
{
public:

    typedef enum
	{
		GTIS,
		GTIM,
        GTIC
	} PivotClass;

    typedef enum
    {
        SPS,
        DPS,
        MV,
        SPC,
        DPC,
        INC,
        APC
    } PivotCdc;

    void setIdentifier(const string& identifier);
    void setCause(int cause);
    void setConfirmation(bool value);
    void setTest(bool value);

    Datapoint* toDatapoint() {return m_dp;};

    std::string& getIdentifier() {return m_identifier;};
    std::string& getComingFrom() {return m_comingFrom;};
    int getCause() {return m_cause;};
    bool isConfirmation() {return m_confirmation;};
    bool Test() {return m_test;};

protected:

    Datapoint* getCdc(Datapoint* dp);

    Datapoint* m_dp;
    Datapoint* m_ln;
    Datapoint* m_cdc;
    PivotClass m_pivotClass;
    PivotCdc m_pivotCdc;

    std::string m_comingFrom = "iec104";
    std::string m_identifier;
    int m_cause = 0;
    bool m_confirmation = false;
    bool m_test = false;

    PivotTimestamp* m_timestamp = nullptr;

    bool hasIntVal = true;
    long intVal;
    float floatVal;
};


class PivotDataObject : public PivotObject
{
public:

    typedef enum
    {
        GOOD,
        INVALID,
        RESERVED,
        QUESTIONABLE
    } Validity;

    typedef enum
    {
        PROCESS,
        SUBSTITUTED
    } Source;

    PivotDataObject(Datapoint* pivotData);
    PivotDataObject(const string& pivotLN, const string& valueType);
    ~PivotDataObject();

    void setStVal(bool value);
    void setStValStr(const std::string& value);
    void setMagI(int value);
    void setMagF(float value);

    void addQuality(bool bl, bool iv, bool nt, bool ov, bool sb, bool test);
    void addTimestamp(long ts, bool iv, bool su, bool sub);

    void addTmOrg(bool substituted);
    void addTmValidity(bool invalid);

    Datapoint* toIec104DataObject(IEC104PivotDataPoint* exchangeConfig);

    Validity getValidity() {return m_validity;};
    Source getSource() {return m_source;};

    bool BadReference() {return m_badReference;};
    bool Failure() {return m_failure;};
    bool Inconsistent() {return m_inconsistent;};
    bool OldData() {return m_oldData;};
    bool Oscillatory() {return m_oscillatory;};
    bool OutOfRange() {return m_outOfRange;};
    bool Overflow() {return m_overflow;};
    bool OperatorBlocked() {return m_operatorBlocked;};

    bool IsTimestampSubstituted() {return m_timestampSubstituted;};
    bool IsTimestampInvalid() {return m_timestampInvalid;};

private:

    void handleDetailQuality(Datapoint* detailQuality);
    void handleQuality(Datapoint* q);

    Validity m_validity = Validity::GOOD;
    bool m_badReference = false;
    bool m_failure = false;
    bool m_inconsistent = false;
    bool m_inacurate = false;
    bool m_oldData = false;
    bool m_oscillatory = false;
    bool m_outOfRange = false;
    bool m_overflow = false;
    Source m_source = Source::PROCESS;
    bool m_operatorBlocked = false;

    bool m_timestampSubstituted = false;
    bool m_timestampInvalid = false;
};

class PivotOperationObject : public PivotObject
{
public:

    PivotOperationObject(Datapoint* pivotData);
    PivotOperationObject(const string& pivotLN, const string& valueType);
    ~PivotOperationObject();

    void setBeh(const string& beh);
    void setCtlValBool(bool value);
    void setCtlValStr(const std::string& value);
    void setCtlValI(int value);
    void setCtlValF(float value);
    void addTimestamp(long ts);

    std::vector<Datapoint*> toIec104OperationObject(IEC104PivotDataPoint* exchangeConfig);

    std::string getBeh() {return m_beh;}

private:
    std::string m_beh = "dct-ctl-wes";

};

#endif /* _IEC104_PIVOT_OBJECT_H */