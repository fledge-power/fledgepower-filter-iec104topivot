#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <logger.h>
#include "iec104_pivot_filter_config.hpp"

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

IEC104PivotConfig::IEC104PivotConfig()
{

}

#define PROTOCOL_IEC104 "iec104"
#define JSON_PROT_NAME "name"
#define JSON_PROT_ADDR "address"
#define JSON_PROT_TYPEID "typeid"


void
IEC104PivotConfig::importExchangeConfig(const string& exchangeConfig)
{
    m_exchangeConfigComplete = false;

    deleteExchangeDefinitions();

    Document document;

    if (document.Parse(const_cast<char*>(exchangeConfig.c_str())).HasParseError()) {
        Logger::getLogger()->fatal("Parsing error in data exchange configuration");

        return;
    }

    if (!document.IsObject())
        return;

    if (!document.HasMember("exchanged_data") || !document["exchanged_data"].IsObject()) {
        return;
    }

    const Value& exchangeData = document["exchanged_data"];

    if (!exchangeData.HasMember("datapoints") || !exchangeData["datapoints"].IsArray()) {
        return;
    }

    const Value& datapoints = exchangeData["datapoints"];

    for (const Value& datapoint : datapoints.GetArray()) {

        if (!datapoint.IsObject()) return;

        if (!datapoint.HasMember("label") || !datapoint["label"].IsString()) return;

        string label = datapoint["label"].GetString();

        if (!datapoint.HasMember("pivot_id") || !datapoint["pivot_id"].IsString()) return;

        string pivotId = datapoint["pivot_id"].GetString();

        if (!datapoint.HasMember("pivot_type") || !datapoint["pivot_type"].IsString()) return;

        string pivotType = datapoint["pivot_type"].GetString();

        if (!datapoint.HasMember("protocols") || !datapoint["protocols"].IsArray()) return;

        for (const Value& protocol : datapoint["protocols"].GetArray()) {
            
            if (!protocol.HasMember("name") || !protocol["name"].IsString()) return;
            
            string protocolName = protocol["name"].GetString();

            if (protocolName == PROTOCOL_IEC104) {

                if (!protocol.HasMember(JSON_PROT_ADDR) || !protocol[JSON_PROT_ADDR].IsString()) return;
                if (!protocol.HasMember(JSON_PROT_TYPEID) || !protocol[JSON_PROT_TYPEID].IsString()) return;

                string address = protocol[JSON_PROT_ADDR].GetString();
                string typeIdStr = protocol[JSON_PROT_TYPEID].GetString();

                string alternateMappingRule = "";

                if (!protocol.HasMember("alternate_mapping_rule")) {
                    alternateMappingRule = protocol["alternate_mapping_rule"].GetString();
                }

                printf("  address: %s type: %s\n", address.c_str(), typeIdStr.c_str());

                size_t sepPos = address.find("-");

                if (sepPos != std::string::npos) {
                    std::string caStr = address.substr(0, sepPos);
                    std::string ioaStr = address.substr(sepPos + 1);

                    int ca = std::stoi(caStr);
                    int ioa = std::stoi(ioaStr);

                    printf("    CA: %i IOA: %i\n", ca, ioa);

                    IEC104PivotDataPoint* newDp = new IEC104PivotDataPoint(label, pivotId, pivotType, typeIdStr, ca, ioa, alternateMappingRule);

                    m_exchangeDefinitions[label] = newDp;
                }
            }
        }
    }

    m_exchangeConfigComplete = true;
}

void
IEC104PivotConfig::deleteExchangeDefinitions()
{
    m_exchangeDefinitions.clear();
}
