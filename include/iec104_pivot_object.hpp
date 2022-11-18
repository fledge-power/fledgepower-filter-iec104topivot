#ifndef _IEC104_PIVOT_OBJECT_H
#define _IEC104_PIVOT_OBJECT_H

#include <plugin_api.h>
#include <datapoint.h>

using namespace std;

class PivotObject {

public:
    PivotObject(const string& pivotType, const string& pivotLN, const string& valueType);
    ~PivotObject();

    void setIdentifier(const string& identifier);
    void setCause(int cause);

    void setStVal(bool value);
    void setStValStr(const std::string& value);

    void setMagF(float value);
    void setMagI(int value);

    void addQuality(bool bl, bool iv, bool nt, bool ov, bool sb, bool test);
    void addTimestamp(long ts, bool iv, bool su, bool sub);

    Datapoint* toDatapoint() {return m_dp;};

private:

    Datapoint* m_dp;
    Datapoint* m_ln;
    Datapoint* m_cdc;
};

#endif /* _IEC104_PIVOT_OBJECT_H */