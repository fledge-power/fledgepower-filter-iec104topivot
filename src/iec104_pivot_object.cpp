#include "iec104_pivot_object.hpp"


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

PivotObject::PivotObject(const string& pivotType, const string& pivotLN, const string& valueType)
{
    m_dp = createDp(pivotType);

    m_ln = addElement(m_dp, pivotLN);

    addElementWithValue(m_ln, "ComingFrom", "IEC104");

    m_cdc = addElement(m_ln, valueType);
}

PivotObject::~PivotObject()
{
}

void
PivotObject::setIdentifier(const string& identifier)
{
    addElementWithValue(m_ln, "Identifier", identifier);
}

void
PivotObject::setCause(int cause)
{
    Datapoint* causeDp = addElement(m_ln, "Cause");

    addElementWithValue(causeDp, "stVal", (long)cause);
}

void
PivotObject::setStVal(bool value)
{
    addElementWithValue(m_cdc, "stVal", (long)(value ? 1 : 0));
}

void
PivotObject::setMagF(float value)
{
    Datapoint* mag = addElement(m_cdc, "mag");

    addElementWithValue(mag, "f", value);
}

void
PivotObject::setMagI(int value)
{
    Datapoint* mag = addElement(m_cdc, "mag");

    addElementWithValue(mag, "i", (long)value);
}

void
PivotObject::addQuality(bool bl, bool iv, bool nt, bool ov, bool sb, bool test)
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
    else {
        addElementWithValue(q, "Validity", "Good");
    }
}

void
PivotObject::addTimestamp(long ts, bool iv, bool su, bool sub)
{
    Datapoint* t = addElement(m_cdc, "t");

    float fractionOfSecond = (float)(ts % 1000)/1000.f;

    addElementWithValue(t, "SecondSinceEpoch", ts/1000);
    addElementWithValue(t, "FractionOfSecond", fractionOfSecond);

    Datapoint* timeQuality = addElement(t, "TimeQuality");

    addElementWithValue(timeQuality, "clockFailure", (long)(iv ? 1 : 0));
    addElementWithValue(timeQuality, "leapSecondKnown", (long)1);
    addElementWithValue(timeQuality, "timeAccuracy", (long)10);
}