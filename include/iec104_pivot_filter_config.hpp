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

#ifndef PIVOT_IEC104_CONFIG_H
#define PIVOT_IEC104_CONFIG_H

#include <map>
#include <memory>

using namespace std;

class IEC104PivotDataPoint
{
public:
    IEC104PivotDataPoint(string label, string pivotId, string pivotType, string typeIdString, int ca, int ioa, string altMappingRule);
    ~IEC104PivotDataPoint();

    std::string& getLabel() {return m_label;};
    std::string& getPivotId() {return m_pivotId;};
    std::string& getPivotType() {return m_pivotType;};
    std::string& getTypeId() {return m_typeIdStr;};
    int getCA() {return m_ca;};
    int getIOA() {return m_ioa;};

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
    void importExchangeConfig(const string& exchangeConfig);

    IEC104PivotDataPoint* getExchangeDefinitionsByLabel(std::string label);
    IEC104PivotDataPoint* getExchangeDefinitionsByAddress(std::string address);
    IEC104PivotDataPoint* getExchangeDefinitionsByPivotId(std::string pivotid);

private:
    
    void m_deleteExchangeDefinitions();

    static bool m_check_string(const rapidjson::Value &json, const char *key);
    static bool m_check_array(const rapidjson::Value &json, const char *key);
    static bool m_check_object(const rapidjson::Value &json, const char *key);

    static bool m_retrieve(const rapidjson::Value &json, const char *key, std::string *target);

    bool m_exchangeConfigComplete = false;

    std::map<std::string, std::shared_ptr<IEC104PivotDataPoint>> m_exchangeDefinitionsLabel;
    std::map<std::string, std::shared_ptr<IEC104PivotDataPoint>> m_exchangeDefinitionsAddress;
    std::map<std::string, std::shared_ptr<IEC104PivotDataPoint>> m_exchangeDefinitionsPivotId;

};

#endif /* PIVOT_IEC104_CONFIG_H */