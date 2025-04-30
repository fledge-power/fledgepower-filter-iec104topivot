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

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include "iec104_pivot_filter_config.hpp"
#include "iec104_pivot_utility.hpp"

using namespace rapidjson;

IEC104PivotDataPoint::IEC104PivotDataPoint(string label, string pivotId, string pivotType, string typeIdString, int ca, int ioa, string altMappingRule)
{
    m_label = label;
    m_pivotId = pivotId;
    m_pivotType = pivotType;
    m_typeIdStr = typeIdString;
    m_ca = ca;
    m_ioa = ioa;
    m_alternateMappingRule = altMappingRule;
}

IEC104PivotDataPoint::~IEC104PivotDataPoint()
{

}

#define PROTOCOL_IEC104 "iec104"
#define JSON_PROT_NAME "name"
#define JSON_PROT_ADDR "address"
#define JSON_PROT_TYPEID "typeid"

IEC104PivotDataPoint*
IEC104PivotConfig::getExchangeDefinitionsByLabel(std::string label) {
    auto it = m_exchangeDefinitionsLabel.find(label);
    if (it != m_exchangeDefinitionsLabel.end()) {
        return it->second.get();
    }
    else {
        return nullptr;
    }
}

IEC104PivotDataPoint*
IEC104PivotConfig::getExchangeDefinitionsByAddress(std::string address) {
    auto it = m_exchangeDefinitionsAddress.find(address);
    if (it != m_exchangeDefinitionsAddress.end()) {
        return it->second.get();
    }
    else {
        return nullptr;
    }
}

IEC104PivotDataPoint*
IEC104PivotConfig::getExchangeDefinitionsByPivotId(std::string pivotid) {
    auto it = m_exchangeDefinitionsPivotId.find(pivotid);
    if (it != m_exchangeDefinitionsPivotId.end()) {
        return it->second.get();
    }
    else {
        return nullptr;
    }
}

void
IEC104PivotConfig::importExchangeConfig(const string& exchangeConfig)
{
    std::string beforeLog = Iec104PivotUtility::PluginName + " - IEC104PivotConfig::importExchangeConfig -"; //LCOV_EXCL_LINE
    m_exchangeConfigComplete = false;

    m_deleteExchangeDefinitions();

    Document document;

    if (document.Parse(const_cast<char*>(exchangeConfig.c_str())).HasParseError()) {
        Iec104PivotUtility::log_fatal("%s Parsing error in exchanged_data json, offset %u: %s", beforeLog.c_str(), //LCOV_EXCL_LINE
                                    static_cast<unsigned>(document.GetErrorOffset()), GetParseError_En(document.GetParseError()));
        return;
    }

    if (!document.IsObject()) return;

    if (!m_check_object(document, "exchanged_data")) return;

    const Value& exchangeData = document["exchanged_data"];

    if (!m_check_array(exchangeData, "datapoints")) return;

    const Value& datapoints = exchangeData["datapoints"];

    for (const Value& datapoint : datapoints.GetArray())
    {
        if (!datapoint.IsObject()) return;

        string label;
        if (!m_retrieve(datapoint, "label", &label)) return;

        string pivotId;
        if (!m_retrieve(datapoint, "pivot_id", &pivotId)) return;

        string pivotType;
        if (!m_retrieve(datapoint, "pivot_type", &pivotType)) return;

        if (!m_check_array(datapoint, "protocols")) return;

        for (const Value& protocol : datapoint["protocols"].GetArray()) {

            if (!protocol.IsObject()) return;
            
            string protocolName;
            if (!m_retrieve(protocol, JSON_PROT_NAME, &protocolName)) return;

            if (protocolName == PROTOCOL_IEC104)
            {
                string address;
                if (!m_retrieve(protocol, JSON_PROT_ADDR, &address)) return;

                string typeIdStr;
                if (!m_retrieve(protocol, JSON_PROT_TYPEID, &typeIdStr)) return;
                
                string alternateMappingRule;
                if (protocol.HasMember("alternate_mapping_rule")) {
                    m_retrieve(protocol, "alternate_mapping_rule", &alternateMappingRule);
                }

                size_t sepPos = address.find("-");

                if (sepPos != std::string::npos) {
                    std::string caStr = address.substr(0, sepPos);
                    std::string ioaStr = address.substr(sepPos + 1);

                    int ca = 0;
                    int ioa = 0;
                    try {
                        ca = std::stoi(caStr);
                        ioa = std::stoi(ioaStr);
                    } catch (const std::invalid_argument &e) {
                        Iec104PivotUtility::log_error("%s Cannot convert ca '%s' or ioa '%s' to integer: %s", //LCOV_EXCL_LINE
                                                    beforeLog.c_str(), caStr.c_str(), ioaStr.c_str(), e.what());
                        return;
                    } catch (const std::out_of_range &e) {
                        Iec104PivotUtility::log_error("%s Cannot convert ca '%s' or ioa '%s' to integer: %s", //LCOV_EXCL_LINE
                                                    beforeLog.c_str(), caStr.c_str(), ioaStr.c_str(), e.what());
                        return;
                    }

                    auto newDp = std::make_shared<IEC104PivotDataPoint>(label, pivotId, pivotType, typeIdStr, ca, ioa, alternateMappingRule);

                    m_exchangeDefinitionsLabel[label] = newDp;
                    m_exchangeDefinitionsAddress[address] = newDp;
                    m_exchangeDefinitionsPivotId[pivotId] = newDp;
                }
            }
        }
    }

    m_exchangeConfigComplete = true;
}

void
IEC104PivotConfig::m_deleteExchangeDefinitions()
{
    m_exchangeDefinitionsLabel.clear();
    m_exchangeDefinitionsAddress.clear();
    m_exchangeDefinitionsPivotId.clear();
}

bool IEC104PivotConfig::m_check_string(const rapidjson::Value& json, const char* key) {
    std::string beforeLog = Iec104PivotUtility::PluginName + " - IEC104PivotConfig::m_check_string -"; //LCOV_EXCL_LINE
    if (!json.HasMember(key) || !json[key].IsString()) {
        Iec104PivotUtility::log_error("%s Error with the field %s, the value does not exist or is not a std::string.", beforeLog.c_str(), key); //LCOV_EXCL_LINE
        return false;
    }
    return true;
}

bool IEC104PivotConfig::m_check_array(const rapidjson::Value& json, const char* key) {
    std::string beforeLog = Iec104PivotUtility::PluginName + " - IEC104PivotConfig::m_check_array -"; //LCOV_EXCL_LINE
    if (!json.HasMember(key) || !json[key].IsArray()) {
        Iec104PivotUtility::log_error("%s The array %s is required but not found.", beforeLog.c_str(), key); //LCOV_EXCL_LINE
        return false;
    }
    return true;
}

bool IEC104PivotConfig::m_check_object(const rapidjson::Value& json, const char* key) {
    std::string beforeLog = Iec104PivotUtility::PluginName + " - IEC104PivotConfig::m_check_object -"; //LCOV_EXCL_LINE
    if (!json.HasMember(key) || !json[key].IsObject()) {
        Iec104PivotUtility::log_error("%s The array %s is required but not found.", beforeLog.c_str(), key); //LCOV_EXCL_LINE
        return false;
    }
    return true;
}

bool IEC104PivotConfig::m_retrieve(const rapidjson::Value& json, const char* key, std::string* target) {
    std::string beforeLog = Iec104PivotUtility::PluginName + " - IEC104PivotConfig::m_retrieve -"; //LCOV_EXCL_LINE
    if (!json.HasMember(key) || !json[key].IsString()) {
        Iec104PivotUtility::log_error("%s Error with the field %s, the value does not exist or is not a std::string.", beforeLog.c_str(), key); //LCOV_EXCL_LINE
        return false;
    }
    *target = json[key].GetString();
    return true;
}