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
    
    int SecondSinceEpoch() {return m_secondSinceEpoch;};
    int FractionOfSecond() {return m_fractionOfSecond;};
    
    bool ClockFailure() {return m_clockFailure;};
    bool LeapSecondKnown() {return m_leapSecondKnown;};
    bool ClockNotSynchronized() {return m_clockNotSynchronized;};
    int TimeAccuracy() {return m_timeAccuracy;};

private:

    void handleTimeQuality(Datapoint* timeQuality);

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
        MV
    } PivotCdc;

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

    PivotObject(Datapoint* pivotData);
    PivotObject(const string& pivotLN, const string& valueType);
    ~PivotObject();

    void setIdentifier(const string& identifier);
    void setCause(int cause);

    void setStVal(bool value);
    void setStValStr(const std::string& value);

    void setMagF(float value);
    void setMagI(int value);

    void setConfirmation(bool value);

    void addQuality(bool bl, bool iv, bool nt, bool ov, bool sb, bool test);
    void addTimestamp(long ts, bool iv, bool su, bool sub);

    void addTmOrg(bool substituted);

    Datapoint* toDatapoint() {return m_dp;};

    Datapoint* toIec104DataObject(IEC104PivotDataPoint* exchangeConfig);

    std::string& getIdentifier() {return m_identifier;};
    std::string& getComingFrom() {return m_comingFrom;};
    int getCause() {return m_cause;};
    bool isConfirmation() {return m_isConfirmation;};

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
    bool Test() {return m_test;};

private:

    Datapoint* getCdc(Datapoint* dp);
    void handleDetailQuality(Datapoint* detailQuality);
    void handleQuality(Datapoint* q);

    Datapoint* m_dp;
    Datapoint* m_ln;
    Datapoint* m_cdc;
    PivotClass m_pivotClass;
    PivotCdc m_pivotCdc;

    std::string m_comingFrom;
    std::string m_identifier;
    int m_cause = 0;
    bool m_isConfirmation = false;

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
    bool m_test = false;

    PivotTimestamp* m_timestamp = nullptr;

    bool hasIntVal = true;
    long intVal;
    float floatVal;
};

#endif /* _IEC104_PIVOT_OBJECT_H */