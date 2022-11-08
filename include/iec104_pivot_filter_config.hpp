#ifndef PIVOT_IEC104_CONFIG_H
#define PIVOT_IEC104_CONFIG_H

#include <map>
#include <vector>

using namespace std;

class IEC104PivotDataPoint
{
public:
    IEC104PivotDataPoint(string label, string pivotId, string pivotType, string typeIdString, int ca, int ioa, string altMappingRule);
    ~IEC104PivotDataPoint();

private:
    std::string m_label;
    std::string m_pivotId;
    std::string m_pivotType;

    std::string m_typeIdStr;
    int         m_ca;
    int         m_ioa;
    std::string m_alternateMappingRule;
};

class IEC104PivotConfig
{
public:
    IEC104PivotConfig();
    IEC104PivotConfig(const string& exchangeConfig);
    ~IEC104PivotConfig();

    void importExchangeConfig(const string& exchangeConfig);

    std::map<std::string, IEC104PivotDataPoint*>& getExchangeDefinitions() {return m_exchangeDefinitions;};

private:

    void deleteExchangeDefinitions();
    
    bool m_exchangeConfigComplete;

    /* list of exchange data points -> the label is the key */
    std::map<std::string, IEC104PivotDataPoint*> m_exchangeDefinitions;
};

#endif /* PIVOT_IEC104_CONFIG_H */