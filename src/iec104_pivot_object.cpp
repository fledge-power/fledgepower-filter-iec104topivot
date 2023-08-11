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

#include "iec104_pivot_object.hpp"

#include <sys/time.h>

static Datapoint*
createDp(const string& name)
{
    vector<Datapoint*>* datapoints = new vector<Datapoint*>;

    DatapointValue dpv(datapoints, true);

    Datapoint* dp = new Datapoint(name, dpv);

    return dp;
}

template <class T>
static Datapoint*
createDpWithValue(const string& name, const T value)
{
    DatapointValue dpv(value);

    Datapoint* dp = new Datapoint(name, dpv);

    return dp;
}

static Datapoint*
addElement(Datapoint* dp, const string& name)
{
    DatapointValue& dpv = dp->getData();

    std::vector<Datapoint*>* subDatapoints = dpv.getDpVec();

    Datapoint* element = createDp(name);

    if (element) {
       subDatapoints->push_back(element);
    }

    return element;
}

template <class T>
static Datapoint*
addElementWithValue(Datapoint* dp, const string& name, const T value)
{
    DatapointValue& dpv = dp->getData();

    std::vector<Datapoint*>* subDatapoints = dpv.getDpVec();

    Datapoint* element = createDpWithValue(name, value);

    if (element) {
       subDatapoints->push_back(element);
    }

    return element;
}

static Datapoint*
getChild(Datapoint* dp, const string& name)
{
    Datapoint* childDp = nullptr;

    DatapointValue& dpv = dp->getData();

    if (dpv.getType() == DatapointValue::T_DP_DICT) {
        std::vector<Datapoint*>* datapoints = dpv.getDpVec();
    
        for (Datapoint* child : *datapoints) {
            if (child->getName() == name) {
                childDp = child;
                break;
            }
        }
    }

    return childDp;
}

static const string
getValueStr(Datapoint* dp)
{
    DatapointValue& dpv = dp->getData();

    if (dpv.getType() == DatapointValue::T_STRING) {
        return dpv.toStringValue();
    }
    else {
        throw PivotObjectException("datapoint " + dp->getName() + " has mot a string value");
    }
}

static const string
getChildValueStr(Datapoint* dp, const string& name)
{
    Datapoint* childDp = getChild(dp, name);

    if (childDp) {
        return getValueStr(childDp);
    }
    else {
        throw PivotObjectException("No such child: " + name);
    }
}

static long
getValueInt(Datapoint* dp)
{
    DatapointValue& dpv = dp->getData();

    if (dpv.getType() == DatapointValue::T_INTEGER) {
        return dpv.toInt();
    }
    else {
        throw PivotObjectException("datapoint " + dp->getName() + " has not an int value");
    }
}

static int
getChildValueInt(Datapoint* dp, const string& name)
{
    Datapoint* childDp = getChild(dp, name);

    if (childDp) {
        return getValueInt(childDp);
    }
    else {
        throw PivotObjectException("No such child: " + name);
    }
}

static float
getValueFloat(Datapoint* dp)
{
    DatapointValue& dpv = dp->getData();

    if (dpv.getType() == DatapointValue::T_FLOAT) {
        return (float) dpv.toDouble();
    }
    else {
        throw PivotObjectException("datapoint " + dp->getName() + " has not a float value");
    }
}

void
PivotTimestamp::handleTimeQuality(Datapoint* timeQuality)
{
    DatapointValue& dpv = timeQuality->getData();

    if (dpv.getType() == DatapointValue::T_DP_DICT)
    {
        std::vector<Datapoint*>* datapoints = dpv.getDpVec();
    
        for (Datapoint* child : *datapoints)
        {
            if (child->getName() == "clockFailure") {
                if (getValueInt(child) > 0)
                    m_clockFailure = true;
                else
                    m_clockFailure = false;
            }
            else if (child->getName() == "clockNotSynchronized") {
                if (getValueInt(child) > 0)
                    m_clockNotSynchronized = true;
                else
                    m_clockNotSynchronized = false;
            }
            else if (child->getName() == "leapSecondKnown") {
                if (getValueInt(child) > 0)
                    m_leapSecondKnown = true;
                else
                    m_leapSecondKnown = false;
            }
            else if (child->getName() == "timeAccuracy") {
                m_timeAccuracy = getValueInt(child);
            }
        }
    }
}

PivotTimestamp::PivotTimestamp(Datapoint* timestampData)
{
    DatapointValue& dpv = timestampData->getData();

    if (dpv.getType() == DatapointValue::T_DP_DICT)
    {
        std::vector<Datapoint*>* datapoints = dpv.getDpVec();
    
        for (Datapoint* child : *datapoints)
        {
            if (child->getName() == "SecondSinceEpoch") {
                m_secondSinceEpoch = getValueInt(child);
            }
            else if (child->getName() == "FractionOfSecond") {
                m_fractionOfSecond = getValueInt(child);
            }
            else if (child->getName() == "TimeQuality") {
                handleTimeQuality(child);
            }
        }
    }
}

uint64_t
PivotTimestamp::GetCurrentTimeInMs()
{
    struct timeval now;

    gettimeofday(&now, nullptr);

    return ((uint64_t) now.tv_sec * 1000LL) + (now.tv_usec / 1000);
}

Datapoint*
PivotObject::getCdc(Datapoint* dp)
{
    Datapoint* cdcDp = nullptr;

    DatapointValue& dpv = dp->getData();

    if (dpv.getType() == DatapointValue::T_DP_DICT) {
        std::vector<Datapoint*>* datapoints = dpv.getDpVec();
    
        for (Datapoint* child : *datapoints) {
            if (child->getName() == "SpsTyp") {
                cdcDp = child;
                m_pivotCdc = PivotCdc::SPS;
                break;
            }
            else if (child->getName() == "MvTyp") {
                cdcDp = child;
                m_pivotCdc = PivotCdc::MV;
                break;
            }
            else if (child->getName() == "DpsTyp") {
                cdcDp = child;
                m_pivotCdc = PivotCdc::DPS;
                break;
            }
            else if (child->getName() == "SpcTyp") {
                cdcDp = child;
                m_pivotCdc = PivotCdc::SPC;
                break;
            }
            else if (child->getName() == "DpcTyp") {
                cdcDp = child;
                m_pivotCdc = PivotCdc::DPC;
                break;
            }
            else if (child->getName() == "IncTyp") {
                cdcDp = child;
                m_pivotCdc = PivotCdc::INC;
                break;
            }
            else if (child->getName() == "ApcTyp") {
                cdcDp = child;
                m_pivotCdc = PivotCdc::APC;
                break;
            }
        }
    }

    return cdcDp;
}

void
PivotDataObject::handleDetailQuality(Datapoint* detailQuality)
{
    DatapointValue& dpv = detailQuality->getData();

    if (dpv.getType() == DatapointValue::T_DP_DICT) {
        std::vector<Datapoint*>* datapoints = dpv.getDpVec();
    
        for (Datapoint* child : *datapoints)
        {
            if (child->getName() == "badReference") {
                if (getValueInt(child) > 0)
                    m_badReference = true;
                else
                    m_badReference = false;
            }
            else if (child->getName() == "failure") {
                if (getValueInt(child) > 0)
                    m_failure = true;
                else
                    m_failure = false;
            }
            else if (child->getName() == "inconsistent") {
                if (getValueInt(child) > 0)
                    m_inconsistent = true;
                else
                    m_inconsistent = false;
            }
            else if (child->getName() == "inacurate") {
                if (getValueInt(child) > 0)
                    m_inacurate = true;
                else
                    m_inacurate = false;
            }
            else if (child->getName() == "oldData") {
                if (getValueInt(child) > 0)
                    m_oldData = true;
                else
                    m_oldData = false;
            }
            else if (child->getName() == "oscillatory") {
                if (getValueInt(child) > 0)
                    m_oscillatory = true;
                else
                    m_oscillatory = false;
            }
            else if (child->getName() == "outOfRange") {
                if (getValueInt(child) > 0)
                    m_outOfRange = true;
                else
                    m_outOfRange = false;
            }
            else if (child->getName() == "overflow") {
                if (getValueInt(child) > 0)
                    m_overflow = true;
                else
                    m_overflow = false;
            }
        }
    }
}

void
PivotDataObject::handleQuality(Datapoint* q)
{
    DatapointValue& dpv = q->getData();

    if (dpv.getType() == DatapointValue::T_DP_DICT) {
        std::vector<Datapoint*>* datapoints = dpv.getDpVec();
    
        for (Datapoint* child : *datapoints) {
            if (child->getName() == "Validity") {
                string validityStr = getValueStr(child);

                if (validityStr != "good") {
                    if (validityStr == "invalid") {
                        m_validity = Validity::INVALID;
                    }
                    else if (validityStr == "questionable") {
                        m_validity = Validity::QUESTIONABLE;
                    }
                    else if (validityStr == "reserved") {
                        m_validity = Validity::RESERVED;
                    }
                    else {
                        throw PivotObjectException("Validity has invalid value " + validityStr);
                    }
                }
            }
            else if (child->getName() == "Source") {
                
                string sourceStr = getValueStr(child);

                if (sourceStr != "process") {
                    if (sourceStr == "substituted") {
                        m_source = Source::SUBSTITUTED;
                    }
                    else {
                        throw PivotObjectException("Source has invalid value " + sourceStr);
                    }
                }
            }
            else if (child->getName() == "DetailQuality") {
                handleDetailQuality(child);
            }
            else if (child->getName() == "operatorBlocked") {
                if (getValueInt(child) > 0)
                    m_operatorBlocked = true;
                else
                    m_operatorBlocked = false;
            }
            else if (child->getName() == "test") {
                if (getValueInt(child) > 0)
                    m_test = true;
                else
                    m_test = false;
            }
        }
    }
}

PivotDataObject::PivotDataObject(Datapoint* pivotData)
{
    if (pivotData->getName() == "PIVOT")
    {
        m_dp = pivotData;
        m_ln = nullptr;

        Datapoint* childDp = nullptr;

        DatapointValue& dpv = pivotData->getData();

        if (dpv.getType() == DatapointValue::T_DP_DICT) {
            std::vector<Datapoint*>* datapoints = dpv.getDpVec();
    
            for (Datapoint* child : *datapoints) {
                if (child->getName() == "GTIS") {
                    m_pivotClass = PivotClass::GTIS;
                    m_ln = child;
                    break;
                }
                else if (child->getName() == "GTIM") {
                    m_pivotClass = PivotClass::GTIM;
                    m_ln = child;
                    break;
                }
                else if (child->getName() == "GTIC") {
                    m_pivotClass = PivotClass::GTIC;
                    m_ln = child;
                    break;
                }
            }
        }

        if (m_ln == nullptr) {
            throw PivotObjectException("pivot object type not supported");
        }

        m_identifier = getChildValueStr(m_ln, "Identifier");

        m_comingFrom = getChildValueStr(m_ln, "ComingFrom");

        Datapoint* cause = getChild(m_ln, "Cause");

        if (cause) {
            m_cause = getChildValueInt(cause, "stVal");
        }

        Datapoint* confirmation = getChild(m_ln, "Cause");

        if (confirmation) {
            int confirmationVal = getChildValueInt(confirmation, "stVal");

            if (confirmationVal > 0) {
                m_confirmation = true;
            }
        }

        Datapoint* tmOrg = getChild(m_ln, "TmOrg");

        if (tmOrg) {
            string tmOrgValue = getChildValueStr(tmOrg, "stVal");

            if (tmOrgValue == "substituted") {
                m_timestampSubstituted = true;
            }
            else {
                m_timestampSubstituted = false;
            }
        }

        Datapoint* tmValidity  = getChild(m_ln, "TmValidity");

        if (tmValidity) {
            string tmValidityValue = getChildValueStr(tmValidity, "stVal");

            if (tmValidityValue == "invalid") {
                m_timestampInvalid = true;
            }
            else {
                m_timestampInvalid = false;
            }
        }

        Datapoint* cdc = getCdc(m_ln);

        if (cdc) {
            Datapoint* q = getChild(cdc, "q");

            if (q) {
                handleQuality(q);
            }

            Datapoint* t = getChild(cdc, "t");

            if (t) {
                m_timestamp = new PivotTimestamp(t);
            }

            if (m_pivotCdc == PivotCdc::SPS) {
                Datapoint* stVal = getChild(cdc, "stVal");

                if (stVal) {
                    hasIntVal = true;

                    if (getValueInt(stVal) > 0) {
                        intVal = 1;
                    }
                    else {
                        intVal = 0;
                    }
                }
            }
            else if (m_pivotCdc == PivotCdc::DPS){
                Datapoint* stVal = getChild(cdc, "stVal");

                if (stVal) {
                    string stValStr = getValueStr(stVal);

                    if (stValStr == "intermediate-state")
                        intVal = 0;
                    else if (stValStr == "off")
                        intVal = 1;
                    else if (stValStr == "on")
                        intVal = 2;
                    else if (stValStr == "bad-state")
                        intVal = 3;
                }
            }
            else if (m_pivotCdc == PivotCdc::MV) {
                Datapoint* mag = getChild(cdc, "mag");

                if (mag) {
                    Datapoint* mag_f = getChild(mag, "f");

                    if (mag_f) {
                        hasIntVal = false;

                        floatVal = getValueFloat(mag_f);
                    }
                    else {
                        Datapoint* mag_i = getChild(mag, "i");

                        if (mag_i) {
                            hasIntVal = true;

                            intVal = getValueInt(mag_i);
                        }
                    }

                }
            }
        }
        else {
            throw PivotObjectException("CDC element not found or CDC type unknown");
        }
    }
    else {
        throw PivotObjectException("No pivot object");
    }
}

PivotDataObject::~PivotDataObject()
{
    if (m_timestamp) delete m_timestamp;
}

PivotDataObject::PivotDataObject(const string& pivotLN, const string& valueType)
{
    m_dp = createDp("PIVOT");

    m_ln = addElement(m_dp, pivotLN);

    addElementWithValue(m_ln, "ComingFrom", "iec104");

    m_cdc = addElement(m_ln, valueType);
}

PivotOperationObject::~PivotOperationObject()
{
    if (m_timestamp) delete m_timestamp;
}

PivotOperationObject::PivotOperationObject(const string& pivotLN, const string& valueType)
{
    m_dp = createDp("PIVOT");

    m_ln = addElement(m_dp, pivotLN);

    addElementWithValue(m_ln, "ComingFrom", "iec104");

    m_cdc = addElement(m_ln, valueType);

}


PivotOperationObject::PivotOperationObject(Datapoint* pivotData)
{
     if (pivotData->getName() == "PIVOT")
    {
        m_dp = pivotData;
        m_ln = nullptr;

        Datapoint* childDp = nullptr;

        DatapointValue& dpv = pivotData->getData();

        if (dpv.getType() == DatapointValue::T_DP_DICT) {
            std::vector<Datapoint*>* datapoints = dpv.getDpVec();

            for (Datapoint* child : *datapoints) {
                if (child->getName() == "GTIC") {
                    m_pivotClass = PivotClass::GTIC;
                    m_ln = child;
                    break;
                }
            }
        }

        if (m_ln == nullptr) {
            throw PivotObjectException("pivot object type not supported");
        }

        m_identifier = getChildValueStr(m_ln, "Identifier");

        Datapoint* comingFromDp = getChild(m_ln,"ComingFrom");

        if(comingFromDp){
            m_comingFrom = getChildValueStr(m_ln, "ComingFrom");
        }
        Datapoint* cause = getChild(m_ln, "Cause");

        if (cause) {
            m_cause = getChildValueInt(cause, "stVal");
        }

        Datapoint* confirmation = getChild(m_ln, "Confirmation");

        if (confirmation) {
            int confirmationVal = getChildValueInt(confirmation, "stVal");

            if (confirmationVal > 0) {
                m_confirmation = true;
            }
        }

        Datapoint* beh = getChild(m_ln, "Beh");

        if(beh){
            m_beh =  getChildValueStr(beh,"stVal");
        }


        Datapoint* cdc = getCdc(m_ln);

        if (cdc) {
            Datapoint* q = getChild(cdc, "q");

            if(q){
                m_test = getChildValueInt(q,"test");
            }

            Datapoint* t = getChild(cdc, "t");

            if (t) {
                m_timestamp = new PivotTimestamp(t);
            }

            if (m_pivotCdc == PivotCdc::SPC) {
                Datapoint* stVal = getChild(cdc, "ctlVal");

                if (stVal) {

                    if (getValueInt(stVal) > 0) {
                        intVal = 1;
                    }
                    else {
                        intVal = 0;
                    }
                }
            }
            else if (m_pivotCdc == PivotCdc::DPC){
                Datapoint* stVal = getChild(cdc, "ctlVal");

                if (stVal) {
                    string stValStr = getValueStr(stVal);

                    if (stValStr == "intermediate-state")
                        intVal = 0;
                    else if (stValStr == "off")
                        intVal = 1;
                    else if (stValStr == "on")
                        intVal = 2;
                    else if (stValStr == "bad-state")
                        intVal = 3;
                }
            }
            else if (m_pivotCdc == PivotCdc::INC) {
                Datapoint* value = getChild(cdc, "ctlVal");

                if(value){
                    intVal = getValueInt(value);
                }
            }
            else if (m_pivotCdc == PivotCdc::APC) {
                Datapoint* value = getChild(cdc, "ctlVal");

                if(value){
                    floatVal = getValueFloat(value);
                }
            }
        }
        else {
            throw PivotObjectException("CDC element not found or CDC type unknown");
        }
    }
    else {
        throw PivotObjectException("No pivot object");
    }
}

void
PivotObject::setIdentifier(const string& identifier)
{
    addElementWithValue(m_ln, "Identifier", identifier);
}


void
PivotOperationObject::setBeh(const string& beh)
{
    Datapoint* behDp = addElement(m_ln, "Beh");
    addElementWithValue(behDp, "stVal", beh);
}

void
PivotObject::setTest(bool value)
{
    Datapoint* q = addElement(m_cdc, "q");

    addElementWithValue(q, "test", (long)value);
}

void
PivotObject::setCause(int cause)
{
    Datapoint* causeDp = addElement(m_ln, "Cause");

    addElementWithValue(causeDp, "stVal", (long)cause);
}

void
PivotDataObject::setStVal(bool value)
{
    addElementWithValue(m_cdc, "stVal", (long)(value ? 1 : 0));
}

void
PivotDataObject::setStValStr(const std::string& value)
{
    addElementWithValue(m_cdc, "stVal", value);
}

void
PivotOperationObject::setCtlValBool(bool value)
{
    addElementWithValue(m_cdc, "ctlVal", (long)(value ? 1 : 0));
}

void
PivotOperationObject::setCtlValStr(const std::string& value)
{
    addElementWithValue(m_cdc, "ctlVal", value);
}

void
PivotOperationObject::setCtlValI(int value)
{
    addElementWithValue(m_cdc, "ctlVal", (long)value);
}

void
PivotOperationObject::setCtlValF(float value)
{
    addElementWithValue(m_cdc, "ctlVal", (float)value);
}

void
PivotDataObject::setMagF(float value)
{
    Datapoint* mag = addElement(m_cdc, "mag");

    addElementWithValue(mag, "f", value);
}

void
PivotDataObject::setMagI(int value)
{
    Datapoint* mag = addElement(m_cdc, "mag");

    addElementWithValue(mag, "i", (long)value);
}

void
PivotObject::setConfirmation(bool value)
{
    Datapoint* confirmation = addElement(m_ln, "Confirmation");

    if (confirmation) {
        addElementWithValue(confirmation, "stVal", (long)(value ? 1 : 0));
    }
}

void
PivotDataObject::addQuality(bool bl, bool iv, bool nt, bool ov, bool sb, bool test)
{
    Datapoint* q = addElement(m_cdc, "q");

    if (nt || ov) {
        Datapoint* detailQuality = addElement(q, "DetailQuality");

        if (nt)
            addElementWithValue(detailQuality, "oldData", (long)1);

        if (ov)
            addElementWithValue(detailQuality, "overflow", (long)1);
    }
    
    if (sb) {
        addElementWithValue(q, "Source", "substituted");
    }
    else {
        addElementWithValue(q, "Source", "process");
    }

    if (bl) {
        addElementWithValue(q, "operatorBlocked", (long)1);
    }

    if (test) {
        addElementWithValue(q, "test", (long)1);
    }

    if (iv) {
        addElementWithValue(q, "Validity", "invalid");
    }
    else if (ov || nt) {
        addElementWithValue(q, "Validity", "questionable");
    }
    else {
        addElementWithValue(q, "Validity", "good");
    }
}

void
PivotDataObject::addTmOrg(bool substituted)
{
    Datapoint* tmOrg = addElement(m_ln, "TmOrg");

    if (substituted)
        addElementWithValue(tmOrg, "stVal", "substituted");
    else
        addElementWithValue(tmOrg, "stVal", "genuine");
}

void
PivotDataObject::addTmValidity(bool invalid)
{
    Datapoint* tmValidity = addElement(m_ln, "TmValidity");

    if (invalid)
        addElementWithValue(tmValidity, "stVal", "invalid");
    else
        addElementWithValue(tmValidity, "stVal", "good");
}

void
PivotDataObject::addTimestamp(long ts, bool iv, bool su, bool sub)
{
    Datapoint* t = addElement(m_cdc, "t");

    long remainder = (ts % 1000LL);
    long fractionOfSecond = (remainder) * 16777 + ((remainder * 216) / 1000);

    addElementWithValue(t, "SecondSinceEpoch", ts/1000);
    addElementWithValue(t, "FractionOfSecond", fractionOfSecond);

    Datapoint* timeQuality = addElement(t, "TimeQuality");

    addElementWithValue(timeQuality, "clockFailure", (long)(iv ? 1 : 0));
    addElementWithValue(timeQuality, "leapSecondKnown", (long)1);
    addElementWithValue(timeQuality, "timeAccuracy", (long)10);
}

void
PivotOperationObject::addTimestamp(long ts)
{
    Datapoint* t = addElement(m_cdc, "t");

    long remainder = (ts % 1000LL);
    long fractionOfSecond = (remainder) * 16777 + ((remainder * 216) / 1000);

    addElementWithValue(t, "SecondSinceEpoch", ts/1000);
    addElementWithValue(t, "FractionOfSecond", fractionOfSecond);
}

Datapoint*
PivotDataObject::toIec104DataObject(IEC104PivotDataPoint* exchangeConfig)
{
    Datapoint* dataObject = createDp("data_object");

    if (dataObject) {
        addElementWithValue(dataObject, "do_type", exchangeConfig->getTypeId());
        addElementWithValue(dataObject, "do_ca", (long)(exchangeConfig->getCA()));
        addElementWithValue(dataObject, "do_ioa", (long)(exchangeConfig->getIOA()));
        addElementWithValue(dataObject, "do_cot", (long)getCause());

        addElementWithValue(dataObject, "do_test", (long)(Test() ? 1 : 0));

        addElementWithValue(dataObject, "do_quality_iv", (long)(getValidity() == Validity::INVALID ? 1 : 0));

        addElementWithValue(dataObject, "do_quality_bl", (long)(OperatorBlocked() ? 1 : 0));

        if (getSource() == Source::SUBSTITUTED) {
            addElementWithValue(dataObject, "do_quality_sb", (long)1);
        }
        else {
            addElementWithValue(dataObject, "do_quality_sb", (long)0);
        }

        addElementWithValue(dataObject, "do_quality_nt", (long)(OldData() ? 1 : 0));

        if (hasIntVal)
            addElementWithValue(dataObject, "do_value", intVal);
        else
            addElementWithValue(dataObject, "do_value", (double)floatVal);

        if (m_pivotClass == PivotClass::GTIM) {
            addElementWithValue(dataObject, "do_quality_ov", (long)(Overflow() ? 1 : 0));
        }

        if (m_timestamp) {
            long msPart = ((long)(m_timestamp->FractionOfSecond()) * 1000L) / 16777216L;

            addElementWithValue(dataObject, "do_ts", ((long)(m_timestamp->SecondSinceEpoch()) * 1000L) + msPart);

            bool timeInvalid = m_timestamp->ClockFailure() || m_timestamp->ClockNotSynchronized();

            if (timeInvalid || IsTimestampInvalid()) {
                addElementWithValue(dataObject, "do_ts_iv", (long)1);
            }

            //addElementWithValue(dataObject, "do_ts_su", (long)0);
            
            if (IsTimestampSubstituted()) {
                addElementWithValue(dataObject, "do_ts_sub", (long)1);
            }
        }
        else {
            addElementWithValue(dataObject, "do_ts", (long)PivotTimestamp::GetCurrentTimeInMs());
            addElementWithValue(dataObject, "do_ts_sub", (long)1);
        }
    }

    return dataObject;
}

std::vector<Datapoint*>
PivotOperationObject::toIec104OperationObject(IEC104PivotDataPoint* exchangeConfig)
{
    std::vector<Datapoint*> commandObject;

    Datapoint* type = createDpWithValue("co_type",(std::string)exchangeConfig->getTypeId());
    commandObject.push_back(type);

    Datapoint* ca = createDpWithValue("co_ca",(std::string)to_string(exchangeConfig->getCA()));
    commandObject.push_back(ca);

    Datapoint* ioa = createDpWithValue("co_ioa",(std::string)to_string(exchangeConfig->getIOA()));
    commandObject.push_back(ioa);

    Datapoint* cot = createDpWithValue("co_cot",(std::string)to_string(getCause()));
    commandObject.push_back(cot);

    Datapoint* negative = createDpWithValue("co_negative",(std::string)(to_string(isConfirmation()))); 
    commandObject.push_back(negative);

    Datapoint* se = createDpWithValue("co_se",(std::string)getBeh());
    commandObject.push_back(se);

    Datapoint* test = createDpWithValue("co_test",(std::string)(to_string(Test())));
    commandObject.push_back(test);

    long time;

    if (m_timestamp) {
            long msPart = ((long)(m_timestamp->FractionOfSecond()) * 1000LL) / 16777216LL;

            time = (long) ((m_timestamp->SecondSinceEpoch()) * 1000L + msPart);
    }

    else{
        time = (long)PivotTimestamp::GetCurrentTimeInMs();
    }

    Datapoint* ts = createDpWithValue("co_ts",(std::string) to_string(time));
    commandObject.push_back(ts);

    Datapoint* value;

    if(hasIntVal)
        value = createDpWithValue("co_value",(std::string) to_string(intVal));
    else
        value = createDpWithValue("co_value",(std::string) to_string(floatVal));

    commandObject.push_back(value);

    return commandObject;
}