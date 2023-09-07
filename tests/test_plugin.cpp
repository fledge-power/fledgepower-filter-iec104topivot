#include <gtest/gtest.h>
#include <plugin_api.h>
#include <reading.h>
#include <reading_set.h>
#include <filter.h>
#include <string.h>
#include <string>
#include <chrono>
#include <thread>
#include <rapidjson/document.h>
#include "iec104_pivot_object.hpp"

using namespace std;
using namespace rapidjson;

extern "C" {
	PLUGIN_INFORMATION *plugin_info();

    PLUGIN_HANDLE plugin_init(ConfigCategory* config,
                          OUTPUT_HANDLE *outHandle,
                          OUTPUT_STREAM output);

    void plugin_shutdown(PLUGIN_HANDLE handle);

    void plugin_ingest(PLUGIN_HANDLE handle,
                   READINGSET *readingSet);
};

static string exchanged_data = QUOTE({
        "exchanged_data" : {
            "description" : "exchanged data list",
            "type" : "JSON",
            "displayName" : "Exchanged data list",
            "order" : "1",
            "default":  {
                "exchanged_data" : {
                    "name" : "iec104pivot",
                    "version" : "1.0",
                    "datapoints":[
                        {
                            "label":"TS1",
                            "pivot_id":"ID-45-672",
                            "pivot_type":"SpsTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-672",
                                  "typeid":"M_SP_NA_1"
                               }
                            ]
                        },
                        {
                            "label":"TS2",
                            "pivot_id":"ID-45-872",
                            "pivot_type":"SpsTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-872",
                                  "typeid":"M_SP_TB_1"
                               }
                            ]
                        },
                        {
                            "label":"TS3",
                            "pivot_id":"ID-45-890",
                            "pivot_type":"DpsTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-890",
                                  "typeid":"M_DP_TB_1"
                               }
                            ]
                        },
                        {
                            "label":"TM1",
                            "pivot_id":"ID-45-984",
                            "pivot_type":"MvTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-984",
                                  "typeid":"M_ME_NA_1"
                               }
                            ]
                        },
                        {
                            "label":"TM2",
                            "pivot_id":"ID-45-985",
                            "pivot_type":"MvTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-985",
                                  "typeid":"M_ME_NB_1"
                               }
                            ]
                        },
                        {
                            "label":"TM3",
                            "pivot_id":"ID-45-986",
                            "pivot_type":"MvTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-986",
                                  "typeid":"M_ME_NC_1"
                               }
                            ]
                        },
                        {
                            "label":"IEC104Command",
                            "pivot_id":"ID-45-988",
                            "pivot_type":"SpcTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-988",
                                  "typeid":"C_SC_TA_1"
                               }
                            ]
                        },
                        {
                            "label":"PivotCommand",
                            "pivot_id":"ID-45-998",
                            "pivot_type":"SpcTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-998",
                                  "typeid":"C_SC_TA_1"
                               }
                            ]
                        },
                        {
                            "label":"PivotCommand",
                            "pivot_id":"ID-45-999",
                            "pivot_type":"SpcTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-999",
                                  "typeid":"C_SC_NA_1"
                               }
                            ]
                        },
                        {
                            "label":"PivotCommand",
                            "pivot_id":"ID-45-888",
                            "pivot_type":"DpcTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-888",
                                  "typeid":"C_DC_NA_1"
                               }
                            ]
                        },
                        {
                            "label":"PivotCommand",
                            "pivot_id":"ID-45-889",
                            "pivot_type":"DpcTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-889",
                                  "typeid":"C_DC_TA_1"
                               }
                            ]
                        },
                        {
                            "label":"PivotCommand",
                            "pivot_id":"ID-45-890",
                            "pivot_type":"IncTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-890",
                                  "typeid":"C_SE_NB_1"
                               }
                            ]
                        },
                        {
                            "label":"PivotCommand",
                            "pivot_id":"ID-45-891",
                            "pivot_type":"IncTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-891",
                                  "typeid":"C_SE_TB_1"
                               }
                            ]
                        },
                        {
                            "label":"PivotCommand",
                            "pivot_id":"ID-45-892",
                            "pivot_type":"ApcTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-892",
                                  "typeid":"C_SE_NC_1"
                               }
                            ]
                        },
                        {
                            "label":"PivotCommand",
                            "pivot_id":"ID-45-893",
                            "pivot_type":"ApcTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-893",
                                  "typeid":"C_SE_TC_1"
                               }
                            ]
                        },
                        {
                            "label":"TM250",
                            "pivot_id":"ID-45-920",
                            "pivot_type":"BscTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-920",
                                  "typeid":"M_ST_NA_1"
                               }
                            ]
                        },
                        {
                            "label":"TM255",
                            "pivot_id":"ID-45-921",
                            "pivot_type":"BscTyp",
                            "protocols":[
                               {
                                  "name":"iec104",
                                  "address":"45-921",
                                  "typeid":"M_ST_TB_1"
                               }
                            ]
                        }
                    ]
                }
            }
        }
    });

    static string exchanged_data_1 = QUOTE({
        "exchanged_data" : {
            "description" : "exchanged data list",
            "displayName" : "Exchanged data list",
            "order" : "1",
            "default": {}
        }
    });

    static string exchanged_data_2 = QUOTE({

        "exchanged_data" : {

            "description" : "exchanged data list",

            "displayName" : "Exchanged data list",

            "order" : "1",

            "default":  {

                "exchanged_data" : {

                    "name" : "iec104pivot",

                    "version" : "1.0",

                    "datapoints":1

                }

            }

        }

    });

    static string exchanged_data_commands = QUOTE({
        "exchanged_data" : {
            "description" : "exchanged data list",
            "type" : "JSON",
            "displayName" : "Exchanged data list",
            "order" : "1",
            "default":  
                {
                    "exchanged_data": {
                        "name": "iec104pivot",
                        "version": "1.0",
                        "datapoints": [
                            {
                                "label": "IEC104Command",
                                "pivot_id":"ID-1-100",
                                "pivot_type":"ApcTyp",
                                "protocols": [
                                    {
                                        "name": "iec104",
                                        "address": "1-100",
                                        "typeid": "C_SE_NC_1"
                                    }
                                ]
                            },
                            {
                                "label": "IEC104Command",
                                "pivot_id":"ID-1-101",
                                "pivot_type":"SpcTyp",
                                "protocols": [
                                    {
                                        "name": "iec104",
                                        "address": "1-101",
                                        "typeid": "C_SC_NA_1"
                                    }
                                ]
                            },
                            {
                                "label": "IEC104Command",
                                "pivot_id":"ID-1-102",
                                "pivot_type":"DpcTyp",
                                "protocols": [
                                    {
                                        "name": "iec104",
                                        "address": "1-102",
                                        "typeid": "C_DC_NA_1"
                                    }
                                ]
                            },
                            {
                                "label": "IEC104Command",
                                "pivot_id":"ID-1-104",
                                "pivot_type":"IncTyp",
                                "protocols": [
                                    {
                                        "name": "iec104",
                                        "address": "1-104",
                                        "typeid": "C_SE_NB_1"
                                    }
                                ]
                            },
                            {
                                "label": "IEC104Command",
                                "pivot_id":"ID-1-105",
                                "pivot_type":"ApcTyp",
                                "protocols": [
                                    {
                                        "name": "iec104",
                                        "address": "1-105",
                                        "typeid": "C_SE_TC_1"
                                    }
                                ]
                            },
                            {
                                "label": "IEC104Command",
                                "pivot_id":"ID-1-106",
                                "pivot_type":"SpcTyp",
                                "protocols": [
                                    {
                                        "name": "iec104",
                                        "address": "1-106",
                                        "typeid": "C_SC_TA_1"
                                    }
                                ]
                            },
                            {
                                "label": "IEC104Command",
                                "pivot_id":"ID-1-107",
                                "pivot_type":"DpcTyp",
                                "protocols": [
                                    {
                                        "name": "iec104",
                                        "address": "1-107",
                                        "typeid": "C_DC_TA_1"
                                    }
                                ]
                            },
                            {
                                "label": "IEC104Command",
                                "pivot_id":"ID-1-109",
                                "pivot_type":"IncTyp",
                                "protocols": [
                                    {
                                        "name": "iec104",
                                        "address": "1-109",
                                        "typeid": "C_SE_TB_1"
                                    }
                                ]
                            },
                            {
                                "label": "IEC104Command",
                                "pivot_id":"ID-1-110",
                                "pivot_type":"BscTyp",
                                "protocols": [
                                    {
                                        "name": "iec104",
                                        "address": "1-110",
                                        "typeid": "C_RC_NA_1"
                                    }
                                ]
                            },
                            {
                                "label": "IEC104Command",
                                "pivot_id":"ID-1-111",
                                "pivot_type":"BscTyp",
                                "protocols": [
                                    {
                                        "name": "iec104",
                                        "address": "1-111",
                                        "typeid": "C_RC_TA_1"
                                    }
                                ]
                            }
                        ]
                    }
                }
            
        }
    });


static int outputHandlerCalled = 0;
static Reading* lastReading = nullptr;

template <class T>
static Datapoint* createDatapoint(const std::string& dataname,
                                    const T value)
{
    DatapointValue dp_value = DatapointValue(value);
    return new Datapoint(dataname, dp_value);
}


static std::vector<Datapoint*> createCommandObject(const char* type, const char* ca, const char* ioa, const char* cot,
    const char* negative, const char* se, const char* test, const char* ts, const char* value)
    {
    std::vector<Datapoint*> commandObject;

    Datapoint* type1 = createDatapoint("co_type",(std::string)type);
    commandObject.push_back(type1);

    Datapoint* ca1 = createDatapoint("co_ca",(std::string)ca);
    commandObject.push_back(ca1);

    Datapoint* ioa1 = createDatapoint("co_ioa",(std::string)ioa);
    commandObject.push_back(ioa1);

    Datapoint* cot1 = createDatapoint("co_cot",(std::string)cot);
    commandObject.push_back(cot1);

    Datapoint* negative1 = createDatapoint("co_negative",(std::string)negative);
    commandObject.push_back(negative1);

    Datapoint* se1 = createDatapoint("co_se",(std::string)se);
    commandObject.push_back(se1);

    Datapoint* test1 = createDatapoint("co_test",(std::string)test);
    commandObject.push_back(test1);

    Datapoint* ts1 = createDatapoint("co_ts",(std::string)ts);
    commandObject.push_back(ts1);

    Datapoint* value1 = createDatapoint("co_value",(std::string) value);
    commandObject.push_back(value1);

    return commandObject;
}

template <class T>
static Datapoint* createDataObject(int test, const char* type, int ca, int ioa, int cot,
    const T value, bool iv, bool bl, bool ov, bool sb, bool nt, long msTime, bool isInvalid, bool isSummerTime, bool isSubst)
{
    auto* datapoints = new vector<Datapoint*>;

    datapoints->push_back(createDatapoint("do_type", type));
    datapoints->push_back(createDatapoint("do_ca", (int64_t)ca));
    datapoints->push_back(createDatapoint("do_oa", (int64_t)0));
    datapoints->push_back(createDatapoint("do_cot", (int64_t)cot));
    datapoints->push_back(createDatapoint("do_test", (int64_t)0));
    datapoints->push_back(createDatapoint("do_negative", (int64_t)0));
    datapoints->push_back(createDatapoint("do_ioa", (int64_t)ioa));
    datapoints->push_back(createDatapoint("do_value", value));
    datapoints->push_back(createDatapoint("do_quality_iv", (int64_t)iv));
    datapoints->push_back(createDatapoint("do_quality_bl", (int64_t)bl));
    datapoints->push_back(createDatapoint("do_quality_ov", (int64_t)ov));
    datapoints->push_back(createDatapoint("do_quality_sb", (int64_t)sb));
    datapoints->push_back(createDatapoint("do_quality_nt", (int64_t)nt));

    if (msTime != 0) {
         datapoints->push_back(createDatapoint("do_ts", msTime));
         datapoints->push_back(createDatapoint("do_ts_iv", isInvalid ? 1L : 0L));
         datapoints->push_back(createDatapoint("do_ts_su", isSummerTime ? 1L : 0L));
         datapoints->push_back(createDatapoint("do_ts_sub", isSubst ? 1L : 0L));
    }

    DatapointValue dpv(datapoints, true);

    Datapoint* dp = new Datapoint("data_object", dpv);

    return dp;
}

template <class T>

static Datapoint* createFakeDataObject(int doTest, const char* type, int ca, int ioa, int cot,
    const T value, bool iv, bool bl, bool ov, bool sb, bool nt, long msTime, bool isInvalid, bool isSummerTime, bool isSubst)
{
    auto* datapoints = new vector<Datapoint*>;
    datapoints->push_back(createDatapoint("do_type", type));
    datapoints->push_back(createDatapoint("do_ca", (int64_t)ca));
    datapoints->push_back(createDatapoint("do_oa", (int64_t)0));
    datapoints->push_back(createDatapoint("do_cot", (int64_t)cot));
    datapoints->push_back(createDatapoint("do_test", (int64_t)doTest));
    datapoints->push_back(createDatapoint("do_negative", (int64_t)0));
    datapoints->push_back(createDatapoint("do_ioa", (int64_t)ioa));
    datapoints->push_back(createDatapoint("do_value", value));
    datapoints->push_back(createDatapoint("do_quality_iv", (int64_t)iv));
    datapoints->push_back(createDatapoint("do_quality_bl", (int64_t)bl));
    datapoints->push_back(createDatapoint("do_quality_ov", (int64_t)ov));
    datapoints->push_back(createDatapoint("do_quality_sb", (int64_t)sb));
    datapoints->push_back(createDatapoint("do_quality_nt", (int64_t)nt));
    if (msTime != 0) {
         datapoints->push_back(createDatapoint("do_ts", msTime));
         datapoints->push_back(createDatapoint("do_ts_iv", isInvalid ? 1L : 0L));
         datapoints->push_back(createDatapoint("do_ts_su", isSummerTime ? 1L : 0L));
         datapoints->push_back(createDatapoint("do_ts_sub", isSubst ? 1L : 0L));
    }
    DatapointValue dpv(datapoints, true);
    Datapoint* dp = new Datapoint("fake_data_object", dpv);
    return dp;
}

template <class T>
static Datapoint* createDataObjectWithSource(const char* type, int ca, int ioa, int cot,
    const T value, bool iv, bool bl, bool ov, bool sb, bool nt, long msTime, bool isInvalid, bool isSummerTime, bool isSubst, const char* sourceValue)
{
    auto* datapoints = new vector<Datapoint*>;

    datapoints->push_back(createDatapoint("do_type", type));
    datapoints->push_back(createDatapoint("do_ca", (int64_t)ca));
    datapoints->push_back(createDatapoint("do_oa", (int64_t)0));
    datapoints->push_back(createDatapoint("do_cot", (int64_t)cot));
    datapoints->push_back(createDatapoint("do_test", (int64_t)0));
    datapoints->push_back(createDatapoint("do_negative", (int64_t)0));
    datapoints->push_back(createDatapoint("do_ioa", (int64_t)ioa));
    datapoints->push_back(createDatapoint("do_value", value));
    datapoints->push_back(createDatapoint("do_quality_iv", (int64_t)iv));
    datapoints->push_back(createDatapoint("do_quality_bl", (int64_t)bl));
    datapoints->push_back(createDatapoint("do_quality_ov", (int64_t)ov));
    datapoints->push_back(createDatapoint("do_quality_sb", (int64_t)sb));
    datapoints->push_back(createDatapoint("do_quality_nt", (int64_t)nt));

    if (msTime != 0) {
         datapoints->push_back(createDatapoint("do_ts", msTime));
         datapoints->push_back(createDatapoint("do_ts_iv", isInvalid ? 1L : 0L));
         datapoints->push_back(createDatapoint("do_ts_su", isSummerTime ? 1L : 0L));
         datapoints->push_back(createDatapoint("do_ts_sub", isSubst ? 1L : 0L));
    }

    if (sourceValue) {
        datapoints->push_back(createDatapoint("do_comingfrom", sourceValue));
    }

    DatapointValue dpv(datapoints, true);

    Datapoint* dp = new Datapoint("data_object", dpv);

    return dp;
}

static Datapoint*
getDatapoint(Reading* reading, const string& key)
{
    std::vector<Datapoint*>& datapoints = reading->getReadingData();

    for (Datapoint* dp : datapoints) {
        if (dp->getName() == key)
            return dp;
    }

    return nullptr;
}
static std::vector<Datapoint*>
getDatapoints(Reading* reading)
{
    std::vector<Datapoint*>& datapoints = reading->getReadingData();
    return datapoints;
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

bool
is_Int(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

static bool
isValueInt(Datapoint* dp)
{
    DatapointValue& dpv = dp->getData();

    if (dpv.getType() == DatapointValue::T_INTEGER) {
        return true;
    }

    return false;
}

static long
getValueInt(Datapoint* dp)
{
    DatapointValue& dpv = dp->getData();

    if (dpv.getType() == DatapointValue::T_INTEGER) {
        return dpv.toInt();
    }

    return 0;
}

static bool
isValueStr(Datapoint* dp)
{
    DatapointValue& dpv = dp->getData();

    if (dpv.getType() == DatapointValue::T_STRING) {
        return true;
    }

    return false;
}

static const string
getValueStr(Datapoint* dp)
{
    DatapointValue& dpv = dp->getData();

    if (dpv.getType() == DatapointValue::T_STRING) {
        return dpv.toStringValue();
    }

    return "";
}

static bool
isValueFloat(Datapoint* dp)
{
    DatapointValue& dpv = dp->getData();

    if (dpv.getType() == DatapointValue::T_FLOAT) {
        return true;
    }

    return false;
}

static float
getValueFloat(Datapoint* dp)
{
    DatapointValue& dpv = dp->getData();

    if (dpv.getType() == DatapointValue::T_FLOAT) {
        return (float) dpv.toDouble();
    }

    return 0.f;
}

TEST(PivotIEC104Plugin, PluginInfo)
{
	PLUGIN_INFORMATION *info = plugin_info();
	ASSERT_STREQ(info->name, "iec104_pivot_filter");
	ASSERT_EQ(info->type, PLUGIN_TYPE_FILTER);
    ASSERT_STREQ(info->version, "1.0.0");
}

static void testOutputStream(OUTPUT_HANDLE * handle, READINGSET* readingSet)
{
    const std::vector<Reading*> readings = readingSet->getAllReadings();

    for (Reading* reading : readings) {
        printf("output: Reading: %s\n", reading->getAssetName().c_str());

        std::vector<Datapoint*>& datapoints = reading->getReadingData();

        for (Datapoint* dp : datapoints) {
            printf("output:   datapoint: %s -> %s\n", dp->getName().c_str(), dp->getData().toString().c_str());
        }

        if (lastReading != nullptr) {
            delete lastReading;
            lastReading = nullptr;
        }

        lastReading = new Reading(*reading);
    }

    outputHandlerCalled++;
}

TEST(PivotIEC104Plugin, PluginInitShutdown)
{
    ConfigCategory config("iec104pivot", exchanged_data);

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);


    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, PluginInitNoConfig)
{
    PLUGIN_HANDLE handle = plugin_init(NULL, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);


    plugin_shutdown(handle);
}


TEST(PivotIEC104Plugin, Plugin_ingest)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject(1,"M_SP_NA_1", 45, 672, 3, (int64_t)1, false, false, false, false, false, 0, false, false, false));
    dataobjects.push_back(createDataObject(1,"M_SP_NA_1", 45, 673, 3, (int64_t)0, false, false, false, false, false, 0, false, false, false));
    dataobjects.push_back(createDataObject(1,"M_SP_NA_1", 45, 947, 3, (int64_t)0, false, false, false, false, false, 0, false, false, false));

    Reading* reading = new Reading(std::string("TS1"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, OperationPlugin_ingest_1)
{
    outputHandlerCalled = 0;

    vector<vector<Datapoint*>> commandobjects;

    commandobjects.push_back(createCommandObject("C_SC_TA_1", "45", "988", "3", "0", "0", "0", "2421512", "1"));

    Reading* reading = new Reading(std::string("IEC104Command"), commandobjects[0]);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    plugin_ingest(handle, &readingSet);


    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, M_SP_TB_1)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject(1,"M_SP_TB_1", 45, 872, 3, (int64_t)1, false, false, false, false, false, 1668631513250, true, false, false));

    Reading* reading = new Reading(std::string("TS2"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* pivot = getDatapoint(lastReading, "PIVOT");
    ASSERT_NE(nullptr, pivot);
    Datapoint* gtis = getChild(pivot, "GTIS");
    ASSERT_NE(nullptr, gtis);
    Datapoint* spsTyp = getChild(gtis, "SpsTyp");
    ASSERT_NE(nullptr, spsTyp);
    Datapoint* stVal = getChild(spsTyp, "stVal");
    ASSERT_NE(nullptr, stVal);
    ASSERT_TRUE(isValueInt(stVal));
    ASSERT_EQ(1, getValueInt(stVal));

    Datapoint* comingFrom = getChild(gtis, "ComingFrom");
    ASSERT_NE(nullptr, comingFrom);
    ASSERT_TRUE(isValueStr(comingFrom));
    ASSERT_EQ("iec104", getValueStr(comingFrom));

    Datapoint* cause = getChild(gtis, "Cause");
    ASSERT_NE(nullptr, cause);
    Datapoint* causeStVal = getChild(cause, "stVal");
    ASSERT_NE(nullptr, causeStVal);
    ASSERT_TRUE(isValueInt(causeStVal));
    ASSERT_EQ(3, getValueInt(causeStVal));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, M_DP_TB_1)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject(1,"M_DP_TB_1", 45, 890, 3, (int64_t)2, false, false, false, false, false, 1668631513250, true, false, false));

    Reading* reading = new Reading(std::string("TS3"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* pivot = getDatapoint(lastReading, "PIVOT");
    ASSERT_NE(nullptr, pivot);
    Datapoint* gtis = getChild(pivot, "GTIS");
    ASSERT_NE(nullptr, gtis);
    Datapoint* spsTyp = getChild(gtis, "DpsTyp");
    ASSERT_NE(nullptr, spsTyp);
    Datapoint* stVal = getChild(spsTyp, "stVal");
    ASSERT_NE(nullptr, stVal);
    ASSERT_TRUE(isValueStr(stVal));
    ASSERT_EQ("on", getValueStr(stVal));

    Datapoint* comingFrom = getChild(gtis, "ComingFrom");
    ASSERT_NE(nullptr, comingFrom);
    ASSERT_TRUE(isValueStr(comingFrom));
    ASSERT_EQ("iec104", getValueStr(comingFrom));

    Datapoint* cause = getChild(gtis, "Cause");
    ASSERT_NE(nullptr, cause);
    Datapoint* causeStVal = getChild(cause, "stVal");
    ASSERT_NE(nullptr, causeStVal);
    ASSERT_TRUE(isValueInt(causeStVal));
    ASSERT_EQ(3, getValueInt(causeStVal));

    Datapoint* t = getChild(spsTyp, "t");
    ASSERT_NE(nullptr, t);

    Datapoint* t_SecondsSinceEpoch = getChild(t, "SecondSinceEpoch");
    ASSERT_NE(nullptr, t_SecondsSinceEpoch);
    ASSERT_TRUE(isValueInt(t_SecondsSinceEpoch));
    long secondSinceEpochValue = getValueInt(t_SecondsSinceEpoch);

    ASSERT_NEAR(1668631513, secondSinceEpochValue, 1);

    Datapoint* tmOrg = getChild(gtis, "TmOrg");
    ASSERT_NE(nullptr, tmOrg);
    Datapoint* tmOrg_stVal = getChild(tmOrg, "stVal");
    ASSERT_NE(nullptr, tmOrg_stVal);
    ASSERT_TRUE(isValueStr(tmOrg_stVal));
    ASSERT_EQ("genuine", getValueStr(tmOrg_stVal));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, M_DP_TB_1_time_substituted)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject(1,"M_DP_TB_1", 45, 890, 3, (int64_t)2, false, false, false, false, false, 1668631513250, true, false, true));

    Reading* reading = new Reading(std::string("TS3"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* pivot = getDatapoint(lastReading, "PIVOT");
    ASSERT_NE(nullptr, pivot);
    Datapoint* gtis = getChild(pivot, "GTIS");
    ASSERT_NE(nullptr, gtis);
    Datapoint* spsTyp = getChild(gtis, "DpsTyp");
    ASSERT_NE(nullptr, spsTyp);
    Datapoint* stVal = getChild(spsTyp, "stVal");
    ASSERT_NE(nullptr, stVal);
    ASSERT_TRUE(isValueStr(stVal));
    ASSERT_EQ("on", getValueStr(stVal));

    Datapoint* comingFrom = getChild(gtis, "ComingFrom");
    ASSERT_NE(nullptr, comingFrom);
    ASSERT_TRUE(isValueStr(comingFrom));
    ASSERT_EQ("iec104", getValueStr(comingFrom));

    Datapoint* cause = getChild(gtis, "Cause");
    ASSERT_NE(nullptr, cause);
    Datapoint* causeStVal = getChild(cause, "stVal");
    ASSERT_NE(nullptr, causeStVal);
    ASSERT_TRUE(isValueInt(causeStVal));
    ASSERT_EQ(3, getValueInt(causeStVal));

    Datapoint* t = getChild(spsTyp, "t");
    ASSERT_NE(nullptr, t);

    Datapoint* t_SecondsSinceEpoch = getChild(t, "SecondSinceEpoch");
    ASSERT_NE(nullptr, t_SecondsSinceEpoch);
    ASSERT_TRUE(isValueInt(t_SecondsSinceEpoch));
    long secondSinceEpochValue = getValueInt(t_SecondsSinceEpoch);

    ASSERT_NEAR(1668631513, secondSinceEpochValue, 1);

    Datapoint* tmOrg = getChild(gtis, "TmOrg");
    ASSERT_NE(nullptr, tmOrg);
    Datapoint* tmOrg_stVal = getChild(tmOrg, "stVal");
    ASSERT_NE(nullptr, tmOrg_stVal);
    ASSERT_TRUE(isValueStr(tmOrg_stVal));
    ASSERT_EQ("substituted", getValueStr(tmOrg_stVal));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, M_DP_TB_1_time_invalid)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject(1,"M_DP_TB_1", 45, 890, 3, (int64_t)2, false, false, false, false, false, 1668631513250, true, false, false));

    Reading* reading = new Reading(std::string("TS3"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* pivot = getDatapoint(lastReading, "PIVOT");
    ASSERT_NE(nullptr, pivot);
    Datapoint* gtis = getChild(pivot, "GTIS");
    ASSERT_NE(nullptr, gtis);
    Datapoint* spsTyp = getChild(gtis, "DpsTyp");
    ASSERT_NE(nullptr, spsTyp);
    Datapoint* stVal = getChild(spsTyp, "stVal");
    ASSERT_NE(nullptr, stVal);
    ASSERT_TRUE(isValueStr(stVal));
    ASSERT_EQ("on", getValueStr(stVal));

    Datapoint* comingFrom = getChild(gtis, "ComingFrom");
    ASSERT_NE(nullptr, comingFrom);
    ASSERT_TRUE(isValueStr(comingFrom));
    ASSERT_EQ("iec104", getValueStr(comingFrom));

    Datapoint* cause = getChild(gtis, "Cause");
    ASSERT_NE(nullptr, cause);
    Datapoint* causeStVal = getChild(cause, "stVal");
    ASSERT_NE(nullptr, causeStVal);
    ASSERT_TRUE(isValueInt(causeStVal));
    ASSERT_EQ(3, getValueInt(causeStVal));

    Datapoint* t = getChild(spsTyp, "t");
    ASSERT_NE(nullptr, t);

    Datapoint* t_SecondsSinceEpoch = getChild(t, "SecondSinceEpoch");
    ASSERT_NE(nullptr, t_SecondsSinceEpoch);
    ASSERT_TRUE(isValueInt(t_SecondsSinceEpoch));
    long secondSinceEpochValue = getValueInt(t_SecondsSinceEpoch);

    ASSERT_NEAR(1668631513, secondSinceEpochValue, 1);

    Datapoint* tmOrg = getChild(gtis, "TmOrg");
    ASSERT_NE(nullptr, tmOrg);
    Datapoint* tmOrg_stVal = getChild(tmOrg, "stVal");
    ASSERT_NE(nullptr, tmOrg_stVal);
    ASSERT_TRUE(isValueStr(tmOrg_stVal));
    ASSERT_EQ("genuine", getValueStr(tmOrg_stVal));

    Datapoint* tmValidity = getChild(gtis, "TmValidity");
    ASSERT_NE(nullptr, tmValidity);

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, M_DP_NA_1)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject(1,"M_DP_NA_1", 45, 890, 3, (int64_t)1, false, false, false, false, false, 0, true, false, false));

    Reading* reading = new Reading(std::string("TS3"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    uint64_t timeInMs = PivotTimestamp::GetCurrentTimeInMs();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);


    ASSERT_NE(nullptr, lastReading);

    Datapoint* pivot = getDatapoint(lastReading, "PIVOT");
    ASSERT_NE(nullptr, pivot);
    Datapoint* gtis = getChild(pivot, "GTIS");
    ASSERT_NE(nullptr, gtis);
    Datapoint* spsTyp = getChild(gtis, "DpsTyp");
    ASSERT_NE(nullptr, spsTyp);
    Datapoint* stVal = getChild(spsTyp, "stVal");
    ASSERT_NE(nullptr, stVal);
    ASSERT_TRUE(isValueStr(stVal));
    ASSERT_EQ("off", getValueStr(stVal));

    Datapoint* t = getChild(spsTyp, "t");
    ASSERT_NE(nullptr, t);

    Datapoint* t_SecondsSinceEpoch = getChild(t, "SecondSinceEpoch");
    ASSERT_NE(nullptr, t_SecondsSinceEpoch);
    ASSERT_TRUE(isValueInt(t_SecondsSinceEpoch));
    long secondSinceEpochValue = getValueInt(t_SecondsSinceEpoch);

    ASSERT_NEAR(timeInMs / 1000, secondSinceEpochValue, 1);

    Datapoint* tmOrg = getChild(gtis, "TmOrg");
    ASSERT_NE(nullptr, tmOrg);
    Datapoint* tmOrg_stVal = getChild(tmOrg, "stVal");
    ASSERT_NE(nullptr, tmOrg_stVal);
    ASSERT_TRUE(isValueStr(tmOrg_stVal));
    ASSERT_EQ("substituted", getValueStr(tmOrg_stVal));

    Datapoint* comingFrom = getChild(gtis, "ComingFrom");
    ASSERT_NE(nullptr, comingFrom);
    ASSERT_TRUE(isValueStr(comingFrom));
    ASSERT_EQ("iec104", getValueStr(comingFrom));

    Datapoint* cause = getChild(gtis, "Cause");
    ASSERT_NE(nullptr, cause);
    Datapoint* causeStVal = getChild(cause, "stVal");
    ASSERT_NE(nullptr, causeStVal);
    ASSERT_TRUE(isValueInt(causeStVal));
    ASSERT_EQ(3, getValueInt(causeStVal));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, M_DP_NA_1_intermediate)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject(1,"M_DP_NA_1", 45, 890, 3, (int64_t)0, false, false, false, false, false, 0, true, false, false));

    Reading* reading = new Reading(std::string("TS3"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    uint64_t timeInMs = PivotTimestamp::GetCurrentTimeInMs();

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* pivot = getDatapoint(lastReading, "PIVOT");
    ASSERT_NE(nullptr, pivot);
    Datapoint* gtis = getChild(pivot, "GTIS");
    ASSERT_NE(nullptr, gtis);
    Datapoint* spsTyp = getChild(gtis, "DpsTyp");
    ASSERT_NE(nullptr, spsTyp);
    Datapoint* stVal = getChild(spsTyp, "stVal");
    ASSERT_NE(nullptr, stVal);
    ASSERT_TRUE(isValueStr(stVal));
    ASSERT_EQ("intermediate-state", getValueStr(stVal));

    Datapoint* t = getChild(spsTyp, "t");
    ASSERT_NE(nullptr, t);

    Datapoint* t_SecondsSinceEpoch = getChild(t, "SecondSinceEpoch");
    ASSERT_NE(nullptr, t_SecondsSinceEpoch);
    ASSERT_TRUE(isValueInt(t_SecondsSinceEpoch));
    long secondSinceEpochValue = getValueInt(t_SecondsSinceEpoch);

    ASSERT_NEAR(timeInMs / 1000, secondSinceEpochValue, 1);

    Datapoint* tmOrg = getChild(gtis, "TmOrg");
    ASSERT_NE(nullptr, tmOrg);
    Datapoint* tmOrg_stVal = getChild(tmOrg, "stVal");
    ASSERT_NE(nullptr, tmOrg_stVal);
    ASSERT_TRUE(isValueStr(tmOrg_stVal));
    ASSERT_EQ("substituted", getValueStr(tmOrg_stVal));

    Datapoint* comingFrom = getChild(gtis, "ComingFrom");
    ASSERT_NE(nullptr, comingFrom);
    ASSERT_TRUE(isValueStr(comingFrom));
    ASSERT_EQ("iec104", getValueStr(comingFrom));

    Datapoint* cause = getChild(gtis, "Cause");
    ASSERT_NE(nullptr, cause);
    Datapoint* causeStVal = getChild(cause, "stVal");
    ASSERT_NE(nullptr, causeStVal);
    ASSERT_TRUE(isValueInt(causeStVal));
    ASSERT_EQ(3, getValueInt(causeStVal));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, M_DP_NA_1_bad)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject(1,"M_DP_NA_1", 45, 890, 3, (int64_t)3, false, false, false, false, false, 0, true, false, false));

    Reading* reading = new Reading(std::string("TS3"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* pivot = getDatapoint(lastReading, "PIVOT");
    ASSERT_NE(nullptr, pivot);
    Datapoint* gtis = getChild(pivot, "GTIS");
    ASSERT_NE(nullptr, gtis);
    Datapoint* spsTyp = getChild(gtis, "DpsTyp");
    ASSERT_NE(nullptr, spsTyp);
    Datapoint* stVal = getChild(spsTyp, "stVal");
    ASSERT_NE(nullptr, stVal);
    ASSERT_TRUE(isValueStr(stVal));
    ASSERT_EQ("bad-state", getValueStr(stVal));

    Datapoint* comingFrom = getChild(gtis, "ComingFrom");
    ASSERT_NE(nullptr, comingFrom);
    ASSERT_TRUE(isValueStr(comingFrom));
    ASSERT_EQ("iec104", getValueStr(comingFrom));

    Datapoint* cause = getChild(gtis, "Cause");
    ASSERT_NE(nullptr, cause);
    Datapoint* causeStVal = getChild(cause, "stVal");
    ASSERT_NE(nullptr, causeStVal);
    ASSERT_TRUE(isValueInt(causeStVal));
    ASSERT_EQ(3, getValueInt(causeStVal));

    plugin_shutdown(handle);
}

/// @brief Test requirement that non-topical flag is mapped to validity="questionable"
TEST(PivotIEC104Plugin, M_ME_NC_1_QualityNonTopical)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject(1,"M_ME_NC_1", 45, 986, 1, (float)0.5f, false, false, false, false, true, 0, false, false, false));

    Reading* reading = new Reading(std::string("TM3"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* pivot = getDatapoint(lastReading, "PIVOT");
    ASSERT_NE(nullptr, pivot);
    Datapoint* gtis = getChild(pivot, "GTIM");
    ASSERT_NE(nullptr, gtis);
    Datapoint* mvTyp = getChild(gtis, "MvTyp");
    ASSERT_NE(nullptr, mvTyp);
    Datapoint* mag = getChild(mvTyp, "mag");
    ASSERT_NE(nullptr, mag);
    Datapoint* magF = getChild(mag, "f");
    ASSERT_NE(nullptr, magF);

    ASSERT_TRUE(isValueFloat(magF));
    ASSERT_EQ(0.5f, getValueFloat(magF));

    Datapoint* q = getChild(mvTyp, "q");
    ASSERT_NE(nullptr, q);

    Datapoint* qValidiy = getChild(q, "Validity");
    ASSERT_NE(nullptr, qValidiy);
    ASSERT_TRUE(isValueStr(qValidiy));
    ASSERT_EQ("questionable", getValueStr(qValidiy));

    Datapoint* qDetailQuality = getChild(q, "DetailQuality");
    ASSERT_NE(nullptr, qDetailQuality);

    Datapoint* qDetailQualityOldData = getChild(qDetailQuality, "oldData");
    ASSERT_NE(nullptr, qDetailQualityOldData);
    ASSERT_TRUE(isValueInt(qDetailQualityOldData));
    ASSERT_EQ(1, getValueInt(qDetailQualityOldData));

    Datapoint* comingFrom = getChild(gtis, "ComingFrom");
    ASSERT_NE(nullptr, comingFrom);
    ASSERT_TRUE(isValueStr(comingFrom));
    ASSERT_EQ("iec104", getValueStr(comingFrom));

    Datapoint* cause = getChild(gtis, "Cause");
    ASSERT_NE(nullptr, cause);
    Datapoint* causeStVal = getChild(cause, "stVal");
    ASSERT_NE(nullptr, causeStVal);
    ASSERT_TRUE(isValueInt(causeStVal));
    ASSERT_EQ(1, getValueInt(causeStVal));

    plugin_shutdown(handle);
}

/// @brief Test requirement that overflow flag is mapped to validity="questionable"
TEST(PivotIEC104Plugin, M_ME_NC_1_QualityOverflow)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject(1,"M_ME_NC_1", 45, 986, 1, (float)100.5f, false, false, true, false, false, 0, false, false, false));

    Reading* reading = new Reading(std::string("TM3"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* pivot = getDatapoint(lastReading, "PIVOT");
    ASSERT_NE(nullptr, pivot);
    Datapoint* gtis = getChild(pivot, "GTIM");
    ASSERT_NE(nullptr, gtis);
    Datapoint* mvTyp = getChild(gtis, "MvTyp");
    ASSERT_NE(nullptr, mvTyp);
    Datapoint* mag = getChild(mvTyp, "mag");
    ASSERT_NE(nullptr, mag);
    Datapoint* magF = getChild(mag, "f");
    ASSERT_NE(nullptr, magF);

    ASSERT_TRUE(isValueFloat(magF));
    ASSERT_EQ(100.5f, getValueFloat(magF));

    Datapoint* q = getChild(mvTyp, "q");
    ASSERT_NE(nullptr, q);

    Datapoint* qValidiy = getChild(q, "Validity");
    ASSERT_NE(nullptr, qValidiy);
    ASSERT_TRUE(isValueStr(qValidiy));
    ASSERT_EQ("questionable", getValueStr(qValidiy));

    Datapoint* qDetailQuality = getChild(q, "DetailQuality");
    ASSERT_NE(nullptr, qDetailQuality);

    Datapoint* qDetailQualityOldData = getChild(qDetailQuality, "oldData");
    ASSERT_EQ(nullptr, qDetailQualityOldData);

    Datapoint* qDetailQualityOverflow = getChild(qDetailQuality, "overflow");
    ASSERT_NE(nullptr, qDetailQualityOverflow);
    ASSERT_TRUE(isValueInt(qDetailQualityOverflow));
    ASSERT_EQ(1, getValueInt(qDetailQualityOverflow));

    Datapoint* comingFrom = getChild(gtis, "ComingFrom");
    ASSERT_NE(nullptr, comingFrom);
    ASSERT_TRUE(isValueStr(comingFrom));
    ASSERT_EQ("iec104", getValueStr(comingFrom));

    Datapoint* cause = getChild(gtis, "Cause");
    ASSERT_NE(nullptr, cause);
    Datapoint* causeStVal = getChild(cause, "stVal");
    ASSERT_NE(nullptr, causeStVal);
    ASSERT_TRUE(isValueInt(causeStVal));
    ASSERT_EQ(1, getValueInt(causeStVal));

    plugin_shutdown(handle);
}


TEST(PivotIEC104Plugin, TypeNotMatching)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject(1,"M_SP_TB_1", 45, 890, 3, (int64_t)1, false, false, false, false, false, 1668631513250, true, false, false));

    Reading* reading = new Reading(std::string("TS3"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(0, outputHandlerCalled);

    delete reading;

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, ToPivotSourceInvalid)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObjectWithSource("M_DP_TB_1", 45, 890, 3, (int64_t)1, false, false, false, false, false, 1668631513250, true, false, false, "iec103"));

    Reading* reading = new Reading(std::string("TS3"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    // expect the output handler is not called because of the wrong source
    ASSERT_EQ(0, outputHandlerCalled);

    delete reading;
    
    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, ToPivotSourceValid)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObjectWithSource("M_DP_TB_1", 45, 890, 3, (int64_t)1, false, false, false, false, false, 1668631513250, true, false, false, "iec104"));

    Reading* reading = new Reading(std::string("TS3"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, SpsTyp_to_M_SP_NA_1)
{
    outputHandlerCalled = 0;

    PivotDataObject* spsTyp = new PivotDataObject("GTIS", "SpsTyp");

    ASSERT_NE(nullptr, spsTyp);

    spsTyp->setIdentifier("ID-45-672");
    spsTyp->setCause(3); /* COT = spont */
    spsTyp->setStVal(true);
    spsTyp->addQuality(false, false, false, false, false, false);

    Datapoint* dp = spsTyp->toDatapoint();

    delete spsTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("TS1"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* dataobject = getDatapoint(lastReading, "data_object");
    ASSERT_NE(nullptr, dataobject);
    Datapoint* doType = getChild(dataobject, "do_type");
    ASSERT_NE(nullptr, doType);
    ASSERT_TRUE(isValueStr(doType));
    ASSERT_EQ("M_SP_NA_1", getValueStr(doType));

    Datapoint* doCa= getChild(dataobject, "do_ca");
    ASSERT_NE(nullptr, doCa);
    ASSERT_TRUE(isValueInt(doCa));
    ASSERT_EQ(45, getValueInt(doCa));

    Datapoint* doIoa= getChild(dataobject, "do_ioa");
    ASSERT_NE(nullptr, doIoa);
    ASSERT_TRUE(isValueInt(doIoa));
    ASSERT_EQ(672, getValueInt(doIoa));

    Datapoint* doCot= getChild(dataobject, "do_cot");
    ASSERT_NE(nullptr, doCot);
    ASSERT_TRUE(isValueInt(doCot));
    ASSERT_EQ(3, getValueInt(doCot));

    Datapoint* doValue= getChild(dataobject, "do_value");
    ASSERT_NE(nullptr, doValue);
    ASSERT_TRUE(isValueInt(doValue));
    ASSERT_EQ(1, getValueInt(doValue));


    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, SpsTyp_to_M_SP_TB_1)
{
    outputHandlerCalled = 0;

    PivotDataObject* spsTyp = new PivotDataObject("GTIS", "SpsTyp");

    ASSERT_NE(nullptr, spsTyp);

    spsTyp->setIdentifier("ID-45-872");
    spsTyp->setCause(3); /* COT = spont */
    spsTyp->setStVal(false);
    spsTyp->addQuality(true, true, true, false, true, true);
    spsTyp->addTimestamp(1669123796250, false, false, false);

    Datapoint* dp = spsTyp->toDatapoint();

    delete spsTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("TS2"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* dataobject = getDatapoint(lastReading, "data_object");
    ASSERT_NE(nullptr, dataobject);
    Datapoint* doType = getChild(dataobject, "do_type");
    ASSERT_NE(nullptr, doType);
    ASSERT_TRUE(isValueStr(doType));
    ASSERT_EQ("M_SP_TB_1", getValueStr(doType));

    Datapoint* doCa= getChild(dataobject, "do_ca");
    ASSERT_NE(nullptr, doCa);
    ASSERT_TRUE(isValueInt(doCa));
    ASSERT_EQ(45, getValueInt(doCa));

    Datapoint* doIoa= getChild(dataobject, "do_ioa");
    ASSERT_NE(nullptr, doIoa);
    ASSERT_TRUE(isValueInt(doIoa));
    ASSERT_EQ(872, getValueInt(doIoa));

    Datapoint* doCot= getChild(dataobject, "do_cot");
    ASSERT_NE(nullptr, doCot);
    ASSERT_TRUE(isValueInt(doCot));
    ASSERT_EQ(3, getValueInt(doCot));

    Datapoint* doValue= getChild(dataobject, "do_value");
    ASSERT_NE(nullptr, doValue);
    ASSERT_TRUE(isValueInt(doValue));
    ASSERT_EQ(0, getValueInt(doValue));

    Datapoint* qualityFlag;

    qualityFlag = getChild(dataobject, "do_quality_bl");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(1, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_iv");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(1, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_nt");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(1, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_ov");
    ASSERT_EQ(nullptr, qualityFlag);

    qualityFlag = getChild(dataobject, "do_quality_sb");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(1, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_test");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(1, getValueInt(qualityFlag));


    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, MvTyp_to_M_ME_NA_1)
{
    outputHandlerCalled = 0;

    PivotDataObject* mvTyp = new PivotDataObject("GTIM", "MvTyp");

    ASSERT_NE(nullptr, mvTyp);

    mvTyp->setIdentifier("ID-45-984");
    mvTyp->setCause(3); /* COT = spont */
    mvTyp->setMagF(0.1f);
    mvTyp->addQuality(false, false, false, true, false, false);
    mvTyp->addTimestamp(1669123796250, false, false, false);

    Datapoint* dp = mvTyp->toDatapoint();

    delete mvTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("TM1"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* dataobject = getDatapoint(lastReading, "data_object");
    ASSERT_NE(nullptr, dataobject);
    Datapoint* doType = getChild(dataobject, "do_type");
    ASSERT_NE(nullptr, doType);
    ASSERT_TRUE(isValueStr(doType));
    ASSERT_EQ("M_ME_NA_1", getValueStr(doType));

    Datapoint* doCa= getChild(dataobject, "do_ca");
    ASSERT_NE(nullptr, doCa);
    ASSERT_TRUE(isValueInt(doCa));
    ASSERT_EQ(45, getValueInt(doCa));

    Datapoint* doIoa= getChild(dataobject, "do_ioa");
    ASSERT_NE(nullptr, doIoa);
    ASSERT_TRUE(isValueInt(doIoa));
    ASSERT_EQ(984, getValueInt(doIoa));

    Datapoint* doCot= getChild(dataobject, "do_cot");
    ASSERT_NE(nullptr, doCot);
    ASSERT_TRUE(isValueInt(doCot));
    ASSERT_EQ(3, getValueInt(doCot));

    Datapoint* doValue= getChild(dataobject, "do_value");
    ASSERT_NE(nullptr, doValue);
    ASSERT_TRUE(isValueFloat(doValue));
    ASSERT_EQ(0.1f, getValueFloat(doValue));

    Datapoint* qualityFlag;

    qualityFlag = getChild(dataobject, "do_quality_bl");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    //TODO should be iv = 1 or iv = 0 here?
    qualityFlag = getChild(dataobject, "do_quality_iv");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_nt");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_ov");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(1, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_sb");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_test");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    Datapoint* doTs = getChild(dataobject, "do_ts");
    ASSERT_NE(nullptr, doTs);
    ASSERT_TRUE(isValueInt(doTs));
    ASSERT_EQ(1669123796250, getValueInt(doTs));

    Datapoint* doTsSub = getChild(dataobject, "do_ts_sub");
    ASSERT_EQ(nullptr, doTsSub);

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, MvTyp_to_M_ME_NB_1)
{
    outputHandlerCalled = 0;

    PivotDataObject* mvTyp = new PivotDataObject("GTIM", "MvTyp");

    ASSERT_NE(nullptr, mvTyp);

    mvTyp->setIdentifier("ID-45-985");
    mvTyp->setCause(3); /* COT = spont */
    mvTyp->setMagI(1234);
    mvTyp->addQuality(false, false, false, true, false, false);
    mvTyp->addTimestamp(1669123796250, false, false, false);

    Datapoint* dp = mvTyp->toDatapoint();

    delete mvTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("TM2"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* dataobject = getDatapoint(lastReading, "data_object");
    ASSERT_NE(nullptr, dataobject);
    Datapoint* doType = getChild(dataobject, "do_type");
    ASSERT_NE(nullptr, doType);
    ASSERT_TRUE(isValueStr(doType));
    ASSERT_EQ("M_ME_NB_1", getValueStr(doType));

    Datapoint* doCa= getChild(dataobject, "do_ca");
    ASSERT_NE(nullptr, doCa);
    ASSERT_TRUE(isValueInt(doCa));
    ASSERT_EQ(45, getValueInt(doCa));

    Datapoint* doIoa= getChild(dataobject, "do_ioa");
    ASSERT_NE(nullptr, doIoa);
    ASSERT_TRUE(isValueInt(doIoa));
    ASSERT_EQ(985, getValueInt(doIoa));

    Datapoint* doCot= getChild(dataobject, "do_cot");
    ASSERT_NE(nullptr, doCot);
    ASSERT_TRUE(isValueInt(doCot));
    ASSERT_EQ(3, getValueInt(doCot));

    Datapoint* doValue= getChild(dataobject, "do_value");
    ASSERT_NE(nullptr, doValue);
    ASSERT_TRUE(isValueInt(doValue));
    ASSERT_EQ(1234, getValueInt(doValue));

    Datapoint* qualityFlag;

    qualityFlag = getChild(dataobject, "do_quality_bl");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    //TODO should be iv = 1 or iv = 0 here?
    qualityFlag = getChild(dataobject, "do_quality_iv");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_nt");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_ov");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(1, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_sb");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_test");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    Datapoint* doTs = getChild(dataobject, "do_ts");
    ASSERT_NE(nullptr, doTs);
    ASSERT_TRUE(isValueInt(doTs));
    ASSERT_EQ(1669123796250, getValueInt(doTs));

    Datapoint* doTsSub = getChild(dataobject, "do_ts_sub");
    ASSERT_EQ(nullptr, doTsSub);

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, MvTyp_to_M_ME_NC_1)
{
    outputHandlerCalled = 0;

    PivotDataObject* mvTyp = new PivotDataObject("GTIM", "MvTyp");

    ASSERT_NE(nullptr, mvTyp);

    mvTyp->setIdentifier("ID-45-986");
    mvTyp->setCause(3); /* COT = spont */
    mvTyp->setMagF(23.45);
    mvTyp->addQuality(false, false, false, true, false, false);
    mvTyp->addTimestamp(1669123796250, false, false, false);

    Datapoint* dp = mvTyp->toDatapoint();

    delete mvTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("TM3"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* dataobject = getDatapoint(lastReading, "data_object");
    ASSERT_NE(nullptr, dataobject);
    Datapoint* doType = getChild(dataobject, "do_type");
    ASSERT_NE(nullptr, doType);
    ASSERT_TRUE(isValueStr(doType));
    ASSERT_EQ("M_ME_NC_1", getValueStr(doType));

    Datapoint* doCa= getChild(dataobject, "do_ca");
    ASSERT_NE(nullptr, doCa);
    ASSERT_TRUE(isValueInt(doCa));
    ASSERT_EQ(45, getValueInt(doCa));

    Datapoint* doIoa= getChild(dataobject, "do_ioa");
    ASSERT_NE(nullptr, doIoa);
    ASSERT_TRUE(isValueInt(doIoa));
    ASSERT_EQ(986, getValueInt(doIoa));

    Datapoint* doCot= getChild(dataobject, "do_cot");
    ASSERT_NE(nullptr, doCot);
    ASSERT_TRUE(isValueInt(doCot));
    ASSERT_EQ(3, getValueInt(doCot));

    Datapoint* doValue= getChild(dataobject, "do_value");
    ASSERT_NE(nullptr, doValue);
    ASSERT_TRUE(isValueFloat(doValue));
    ASSERT_NEAR(23.45, getValueFloat(doValue), 0.1f);

    Datapoint* qualityFlag;

    qualityFlag = getChild(dataobject, "do_quality_bl");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    //TODO should be iv = 1 or iv = 0 here?
    qualityFlag = getChild(dataobject, "do_quality_iv");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_nt");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_ov");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(1, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_sb");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_test");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(0, getValueInt(qualityFlag));

    Datapoint* doTs = getChild(dataobject, "do_ts");
    ASSERT_NE(nullptr, doTs);
    ASSERT_TRUE(isValueInt(doTs));
    ASSERT_EQ(1669123796250, getValueInt(doTs));

    Datapoint* doTsSub = getChild(dataobject, "do_ts_sub");
    ASSERT_EQ(nullptr, doTsSub);

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, SpsTyp_withoutTimestamp_to_M_SP_TB_1)
{
    outputHandlerCalled = 0;

    PivotDataObject* spsTyp = new PivotDataObject("GTIS", "SpsTyp");

    ASSERT_NE(nullptr, spsTyp);

    spsTyp->setIdentifier("ID-45-872");
    spsTyp->setCause(3); /* COT = spont */
    spsTyp->setStVal(false);
    spsTyp->addQuality(true, true, true, false, true, true);

    Datapoint* dp = spsTyp->toDatapoint();

    delete spsTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("TS2"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* dataobject = getDatapoint(lastReading, "data_object");
    ASSERT_NE(nullptr, dataobject);
    Datapoint* doType = getChild(dataobject, "do_type");
    ASSERT_NE(nullptr, doType);
    ASSERT_TRUE(isValueStr(doType));
    ASSERT_EQ("M_SP_TB_1", getValueStr(doType));

    Datapoint* doCa= getChild(dataobject, "do_ca");
    ASSERT_NE(nullptr, doCa);
    ASSERT_TRUE(isValueInt(doCa));
    ASSERT_EQ(45, getValueInt(doCa));

    Datapoint* doIoa= getChild(dataobject, "do_ioa");
    ASSERT_NE(nullptr, doIoa);
    ASSERT_TRUE(isValueInt(doIoa));
    ASSERT_EQ(872, getValueInt(doIoa));

    Datapoint* doCot= getChild(dataobject, "do_cot");
    ASSERT_NE(nullptr, doCot);
    ASSERT_TRUE(isValueInt(doCot));
    ASSERT_EQ(3, getValueInt(doCot));

    Datapoint* doValue= getChild(dataobject, "do_value");
    ASSERT_NE(nullptr, doValue);
    ASSERT_TRUE(isValueInt(doValue));
    ASSERT_EQ(0, getValueInt(doValue));

    Datapoint* qualityFlag;

    qualityFlag = getChild(dataobject, "do_quality_bl");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(1, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_iv");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(1, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_nt");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(1, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_quality_ov");
    ASSERT_EQ(nullptr, qualityFlag);

    qualityFlag = getChild(dataobject, "do_quality_sb");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(1, getValueInt(qualityFlag));

    qualityFlag = getChild(dataobject, "do_test");
    ASSERT_NE(nullptr, qualityFlag);
    ASSERT_TRUE(isValueInt(qualityFlag));
    ASSERT_EQ(1, getValueInt(qualityFlag));

    uint64_t timeInMs = PivotTimestamp::GetCurrentTimeInMs();

    Datapoint* doTs = getChild(dataobject, "do_ts");
    ASSERT_NE(nullptr, doTs);
    ASSERT_TRUE(isValueInt(doTs));
    long tsValue = getValueInt(doTs);
    ASSERT_NEAR(timeInMs, tsValue, 1000);

    Datapoint* doTsSub = getChild(dataobject, "do_ts_sub");
    ASSERT_NE(nullptr, doTsSub);
    ASSERT_TRUE(isValueInt(doTsSub));
    ASSERT_EQ(1, getValueInt(doTsSub));

    Datapoint* doTsIv = getChild(dataobject, "do_ts_iv");
    ASSERT_EQ(nullptr, doTsIv);

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, SpsTyp_to_M_SP_TB_1_timeInvalid)
{
    outputHandlerCalled = 0;

    PivotDataObject* spsTyp = new PivotDataObject("GTIS", "SpsTyp");

    ASSERT_NE(nullptr, spsTyp);

    spsTyp->setIdentifier("ID-45-872");
    spsTyp->setCause(3); /* COT = spont */
    spsTyp->setStVal(false);
    spsTyp->addQuality(true, true, true, false, true, true);
    spsTyp->addTimestamp(1669123796250, false, false, false);
    spsTyp->addTmValidity(true);

    Datapoint* dp = spsTyp->toDatapoint();

    delete spsTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("TS2"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* dataobject = getDatapoint(lastReading, "data_object");
    ASSERT_NE(nullptr, dataobject);
    Datapoint* doType = getChild(dataobject, "do_type");
    ASSERT_NE(nullptr, doType);
    ASSERT_TRUE(isValueStr(doType));
    ASSERT_EQ("M_SP_TB_1", getValueStr(doType));

    Datapoint* doCa= getChild(dataobject, "do_ca");
    ASSERT_NE(nullptr, doCa);
    ASSERT_TRUE(isValueInt(doCa));
    ASSERT_EQ(45, getValueInt(doCa));

    Datapoint* doIoa= getChild(dataobject, "do_ioa");
    ASSERT_NE(nullptr, doIoa);
    ASSERT_TRUE(isValueInt(doIoa));
    ASSERT_EQ(872, getValueInt(doIoa));

    Datapoint* doTs = getChild(dataobject, "do_ts");
    ASSERT_NE(nullptr, doTs);
    ASSERT_TRUE(isValueInt(doTs));
    ASSERT_EQ(1669123796250, getValueInt(doTs));

    Datapoint* doTsSub = getChild(dataobject, "do_ts_sub");
    ASSERT_EQ(nullptr, doTsSub);

    Datapoint* doTsIv = getChild(dataobject, "do_ts_iv");
    ASSERT_NE(nullptr, doTsIv);
    ASSERT_TRUE(isValueInt(doTsIv));
    ASSERT_EQ(1, getValueInt(doTsIv));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, OperationSpcTyp_to_C_SC_NA_1)
{
    outputHandlerCalled = 0;

    PivotOperationObject* spcTyp = new PivotOperationObject("GTIC", "SpcTyp");

    ASSERT_NE(nullptr, spcTyp);

    spcTyp->setIdentifier("ID-45-998");
    spcTyp->setCause(3);
    spcTyp->setCtlValBool(false);

    Datapoint* dp = spcTyp->toDatapoint();

    delete spcTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("PivotCommand"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, OperationPivotToOperationObjectTStrue) {
    outputHandlerCalled = 0;

    PivotOperationObject* spcTyp = new PivotOperationObject("GTIC","SpcTyp");

    ASSERT_NE(nullptr,spcTyp);

    long timestamp = 1669123796250;

    spcTyp->setIdentifier("ID-45-998");
    spcTyp->setCause(3);
    spcTyp->setCtlValBool(false);
    spcTyp->addTimestamp(timestamp);

    Datapoint* dp = spcTyp->toDatapoint();

    ASSERT_NE(nullptr,dp);

    delete spcTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("PivotCommand"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* co_ts = getDatapoint(lastReading,"co_ts");
    ASSERT_NE(nullptr,co_ts);
    ASSERT_EQ(getValueStr(co_ts),to_string(timestamp));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, OperationMultConversionsWithTimestamp) {
    outputHandlerCalled = 0;

    PivotOperationObject* spcTyp = new PivotOperationObject("GTIC","SpcTyp");

    ASSERT_NE(nullptr,spcTyp);

    long TSfractionOfSecond = 1669123796250;
    long timestamp = 1669123796;


    spcTyp->setIdentifier("ID-45-998");
    spcTyp->setCause(3);
    spcTyp->setCtlValBool(false);
    spcTyp->addTimestamp(TSfractionOfSecond);

    Datapoint* dp = spcTyp->toDatapoint();

    ASSERT_NE(nullptr,dp);

    delete spcTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("PivotCommand"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    for(int i = 0; i < 10; i++) {
        if(i%2 == 0) {


            ASSERT_EQ(i+1, outputHandlerCalled);

            ASSERT_NE(nullptr, lastReading);

            Datapoint* co_type = getDatapoint(lastReading,"co_type");
            ASSERT_NE(nullptr,co_type);
            ASSERT_EQ("C_SC_TA_1",getValueStr(co_type));

            Datapoint* co_ca = getDatapoint(lastReading,"co_ca");
            ASSERT_NE(nullptr,co_ca);
            ASSERT_EQ("45",getValueStr(co_ca));

            Datapoint* co_ioa = getDatapoint(lastReading,"co_ioa");
            ASSERT_NE(nullptr,co_ioa);
            ASSERT_EQ("998",getValueStr(co_ioa));

            Datapoint* co_cot = getDatapoint(lastReading,"co_cot");
            ASSERT_NE(nullptr,co_cot);
            ASSERT_EQ("3",getValueStr(co_cot));

            Datapoint* co_negative = getDatapoint(lastReading,"co_negative");
            ASSERT_NE(nullptr,co_negative);
            ASSERT_EQ("0",getValueStr(co_negative));

            Datapoint* co_se = getDatapoint(lastReading,"co_se");
            ASSERT_NE(nullptr,co_se);
            ASSERT_EQ("0",getValueStr(co_se));

            Datapoint* co_test = getDatapoint(lastReading,"co_test");
            ASSERT_NE(nullptr,co_test);
            ASSERT_EQ("0",getValueStr(co_test));

            Datapoint* co_ts = getDatapoint(lastReading,"co_ts");
            ASSERT_NE(nullptr,co_ts);
            ASSERT_EQ("1669123796250",getValueStr(co_ts));

            Datapoint* co_value = getDatapoint(lastReading,"co_value");
            ASSERT_NE(nullptr,co_value);
            ASSERT_EQ("0",getValueStr(co_value));

            plugin_ingest(handle, &readingSet);

        }
        else {


            ASSERT_EQ(i+1, outputHandlerCalled);

            ASSERT_NE(nullptr, lastReading);

            Datapoint* command = getDatapoint(lastReading,"PIVOT");

            ASSERT_NE(nullptr,command);

            Datapoint* root = getChild(command,"GTIC");
            ASSERT_NE(nullptr,root);

            Datapoint* comingFrom = getChild(root,"ComingFrom");
            ASSERT_NE(nullptr,comingFrom);
            ASSERT_EQ(getValueStr(comingFrom),"iec104");

            Datapoint* spcType = getChild(root,"SpcTyp");
            ASSERT_NE(nullptr,spcType);

            Datapoint* ctlVal = getChild(spcType,"ctlVal");
            ASSERT_NE(nullptr,ctlVal);
            ASSERT_EQ(0,getValueInt(ctlVal));

            Datapoint* t = getChild(spcType,"t");
            ASSERT_NE(nullptr,t);

            Datapoint* ts = getChild(t,"SecondSinceEpoch");
            ASSERT_NE(nullptr,ts);
            ASSERT_EQ(timestamp,getValueInt(ts));
            plugin_ingest(handle, &readingSet);

        }
    }

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin,OperationDefaultValues) {
    outputHandlerCalled = 0;

    PivotOperationObject* spcTyp = new PivotOperationObject("GTIC","SpcTyp");

    ASSERT_NE(nullptr,spcTyp);

    long TSfractionOfSecond = 1669123796250;
    long timestamp = 1669123796;

    spcTyp->setIdentifier("ID-45-999");
    spcTyp->setCause(3);
    // spcTyp->addTimestamp(TSfractionOfSecond);

    Datapoint* dp = spcTyp->toDatapoint();

    ASSERT_NE(nullptr,dp);

    delete spcTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("PivotCommand"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    Datapoint* co_negative = getDatapoint(lastReading,"co_negative");
    ASSERT_NE(nullptr,co_negative);
    ASSERT_EQ(getValueStr(co_negative),to_string(0));

    Datapoint* co_se = getDatapoint(lastReading,"co_se");
    ASSERT_NE(nullptr,co_se);
    ASSERT_EQ(getValueStr(co_se),"0");

    Datapoint* co_test = getDatapoint(lastReading,"co_test");
    ASSERT_NE(nullptr,co_test);
    ASSERT_EQ(getValueStr(co_test),to_string(0));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin,OperationTypeConversionSpcTSfalse) {

    outputHandlerCalled = 0;

    PivotOperationObject* spcTyp = new PivotOperationObject("GTIC","SpcTyp");

    ASSERT_NE(nullptr,spcTyp);

    spcTyp->setIdentifier("ID-45-999");
    spcTyp->setCause(3);

    Datapoint* dp = spcTyp->toDatapoint();

    ASSERT_NE(nullptr,dp);

    delete spcTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("PivotCommand"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    ASSERT_NE(nullptr,co_type);
    ASSERT_EQ(getValueStr(co_type),"C_SC_NA_1");

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin,OperationTypeConversionSpcTStrue) {

    outputHandlerCalled = 0;

    PivotOperationObject* spcTyp = new PivotOperationObject("GTIC","SpcTyp");

    ASSERT_NE(nullptr,spcTyp);

    long TSfractionOfSecond = 1669123796250;
    long timestamp = 1669123796;

    spcTyp->setIdentifier("ID-45-998");
    spcTyp->setCause(3);
    spcTyp->addTimestamp(TSfractionOfSecond);

    Datapoint* dp = spcTyp->toDatapoint();

    ASSERT_NE(nullptr,dp);

    delete spcTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("PivotCommand"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    ASSERT_NE(nullptr,co_type);
    ASSERT_EQ(getValueStr(co_type),"C_SC_TA_1");

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin,OperationTypeConversionDpcTSfalse) {

    outputHandlerCalled = 0;

    PivotOperationObject* spcTyp = new PivotOperationObject("GTIC","DpcTyp");

    ASSERT_NE(nullptr,spcTyp);

    spcTyp->setIdentifier("ID-45-888");
    spcTyp->setCause(3);

    Datapoint* dp = spcTyp->toDatapoint();

    ASSERT_NE(nullptr,dp);

    delete spcTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("PivotCommand"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    ASSERT_NE(nullptr,co_type);
    ASSERT_EQ(getValueStr(co_type),"C_DC_NA_1");

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin,OperationTypeConversionDpcTStrue) {

    outputHandlerCalled = 0;

    PivotOperationObject* spcTyp = new PivotOperationObject("GTIC","DpcTyp");

    ASSERT_NE(nullptr,spcTyp);

    long TSfractionOfSecond = 1669123796250;
    long timestamp = 1669123796;

    spcTyp->setIdentifier("ID-45-889");
    spcTyp->setCause(3);
    spcTyp->addTimestamp(TSfractionOfSecond);

    Datapoint* dp = spcTyp->toDatapoint();

    ASSERT_NE(nullptr,dp);

    delete spcTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("PivotCommand"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    ASSERT_NE(nullptr,co_type);
    ASSERT_EQ(getValueStr(co_type),"C_DC_TA_1");

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin,OperationTypeConversionIncTSfalse) {

    outputHandlerCalled = 0;

    PivotOperationObject* spcTyp = new PivotOperationObject("GTIC","IncTyp");

    ASSERT_NE(nullptr,spcTyp);

    spcTyp->setIdentifier("ID-45-890");
    spcTyp->setCause(3);

    Datapoint* dp = spcTyp->toDatapoint();

    ASSERT_NE(nullptr,dp);

    delete spcTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("PivotCommand"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    ASSERT_NE(nullptr,co_type);
    ASSERT_EQ(getValueStr(co_type),"C_SE_NB_1");

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin,OperationTypeConversionIncTStrue) {

    outputHandlerCalled = 0;

    PivotOperationObject* spcTyp = new PivotOperationObject("GTIC","IncTyp");

    ASSERT_NE(nullptr,spcTyp);

    long TSfractionOfSecond = 1669123796250;
    long timestamp = 1669123796;

    spcTyp->setIdentifier("ID-45-891");
    spcTyp->setCause(3);
    spcTyp->addTimestamp(TSfractionOfSecond);

    Datapoint* dp = spcTyp->toDatapoint();

    ASSERT_NE(nullptr,dp);

    delete spcTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("PivotCommand"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    ASSERT_NE(nullptr,co_type);
    ASSERT_EQ(getValueStr(co_type),"C_SE_TB_1");

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin,OperationTypeConversionApcTSfalse) {

    outputHandlerCalled = 0;

    PivotOperationObject* spcTyp = new PivotOperationObject("GTIC","ApcTyp");

    ASSERT_NE(nullptr,spcTyp);

    spcTyp->setIdentifier("ID-45-892");
    spcTyp->setCause(3);

    Datapoint* dp = spcTyp->toDatapoint();

    ASSERT_NE(nullptr,dp);

    delete spcTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("PivotCommand"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    ASSERT_NE(nullptr,co_type);
    ASSERT_EQ(getValueStr(co_type),"C_SE_NC_1");

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin,OperationTypeConversionApcTStrue) {

    outputHandlerCalled = 0;

    PivotOperationObject* spcTyp = new PivotOperationObject("GTIC","ApcTyp");

    ASSERT_NE(nullptr,spcTyp);

    long TSfractionOfSecond = 1669123796250;
    long timestamp = 1669123796;

    spcTyp->setIdentifier("ID-45-893");
    spcTyp->setCause(3);
    spcTyp->addTimestamp(TSfractionOfSecond);

    Datapoint* dp = spcTyp->toDatapoint();

    ASSERT_NE(nullptr,dp);

    delete spcTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("PivotCommand"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    ASSERT_NE(nullptr,co_type);
    ASSERT_EQ(getValueStr(co_type),"C_SE_TC_1");

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, OperationAutomaticPivotTimestamps) {
    outputHandlerCalled = 0;

    vector<vector<Datapoint*>> commandobjects;

    commandobjects.push_back(createCommandObject("C_SC_NA_1", "45", "999", "3", "0", "0", "0", "", "1"));

    Reading* reading = new Reading(std::string("IEC104Command"), commandobjects[0]);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    Datapoint* pivot = getDatapoint(lastReading,"PIVOT");
    ASSERT_NE(nullptr,pivot);

    Datapoint* gtic = getChild(pivot,"GTIC");
    ASSERT_NE(nullptr,gtic);

    Datapoint* SpcTyp = getChild(gtic,"SpcTyp");
    ASSERT_NE(nullptr,SpcTyp);

    Datapoint* t = getChild(SpcTyp,"t");
    ASSERT_NE(nullptr,t);

    Datapoint* SecondSinceEpoch = getChild(t,"SecondSinceEpoch");
    ASSERT_NE(nullptr,SecondSinceEpoch);
    ASSERT_GT(getValueInt(SecondSinceEpoch),0);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(2, outputHandlerCalled);

    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    ASSERT_NE(nullptr,co_type);
    ASSERT_EQ("C_SC_NA_1",getValueStr(co_type));

    Datapoint* co_ts = getDatapoint(lastReading,"co_ts");
    ASSERT_NE(nullptr,co_ts);
    ASSERT_EQ("",getValueStr(co_ts));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, OperationNoTimestampIEC104Command) {
    outputHandlerCalled = 0;

    vector<vector<Datapoint*>> commandobjects;

    commandobjects.push_back(createCommandObject("C_SC_TA_1", "45", "998", "3", "0", "0", "0", "", "1"));

    Reading* reading = new Reading(std::string("IEC104Command"), commandobjects[0]);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(0, outputHandlerCalled);
    
    delete reading;

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, M_ME_NA_1)
{
    
    outputHandlerCalled = 0;
    vector<Datapoint*> dataobjects;
    dataobjects.push_back(createDataObject(1,"M_ME_NA_1", 45, 984, 3, (float)0, false, false, false, false, false, 0, true, false, false));
    dataobjects.push_back(createDataObject(1,"M_ME_NA_1", 45, 984, 3, (float)0, false, false, false, false, false, 0, true, false, false));
    Reading* reading = new Reading(std::string("TM1"), dataobjects);
    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error
    vector<Reading*> readings;
    readings.push_back(reading);
    ReadingSet readingSet;
    readingSet.append(readings);
    ConfigCategory config("exchanged_data", exchanged_data);
    config.setItemsValueFromDefault();
    uint64_t timeInMs = PivotTimestamp::GetCurrentTimeInMs();
    string configValue = config.getValue("exchanged_data");
    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);
    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);
    ASSERT_NE(nullptr, lastReading);
    Datapoint* pivot = getDatapoint(lastReading, "PIVOT");
    ASSERT_NE(nullptr, pivot);
    Datapoint* gtis = getChild(pivot, "GTIM");
    ASSERT_NE(nullptr, gtis);
    Datapoint* mvTyp = getChild(gtis, "MvTyp");
    ASSERT_NE(nullptr, mvTyp);
    Datapoint* q = getChild(mvTyp, "q");
    ASSERT_NE(nullptr, q);
    Datapoint* qValidiy = getChild(q, "Validity");
    ASSERT_NE(nullptr, qValidiy);
    ASSERT_TRUE(isValueStr(qValidiy));
    ASSERT_EQ("good", getValueStr(qValidiy));
    Datapoint* comingFrom = getChild(gtis, "ComingFrom");
    ASSERT_NE(nullptr, comingFrom);
    ASSERT_TRUE(isValueStr(comingFrom));
    ASSERT_EQ("iec104", getValueStr(comingFrom));
    Datapoint* cause = getChild(gtis, "Cause");
    ASSERT_NE(nullptr, cause);
    Datapoint* causeStVal = getChild(cause, "stVal");
    ASSERT_NE(nullptr, causeStVal);
    ASSERT_TRUE(isValueInt(causeStVal));
    ASSERT_EQ(3, getValueInt(causeStVal));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, M_ME_NB_1)
{
    outputHandlerCalled = 0;
    vector<Datapoint*> dataobjects;
    dataobjects.push_back(createDataObject(1,"M_ME_NB_1", 45, 985, 3, (int64_t)0, false, false, false, false, false, 0, true, false, false));
    dataobjects.push_back(createDataObject(1,"M_ME_NB_1", 45, 985, 3, (float)0, false, false, false, false, false, 0, true, false, false));
    Reading* reading = new Reading(std::string("TM2"), dataobjects);
    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error
    vector<Reading*> readings;
    readings.push_back(reading);
    ReadingSet readingSet;
    readingSet.append(readings);
    ConfigCategory config("exchanged_data", exchanged_data);
    config.setItemsValueFromDefault();
    uint64_t timeInMs = PivotTimestamp::GetCurrentTimeInMs();
    string configValue = config.getValue("exchanged_data");
    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);
    ASSERT_TRUE(handle != nullptr);
    plugin_ingest(handle, &readingSet);
    ASSERT_EQ(1, outputHandlerCalled);
    ASSERT_NE(nullptr, lastReading);
    Datapoint* pivot = getDatapoint(lastReading, "PIVOT");
    ASSERT_NE(nullptr, pivot);
    Datapoint* gtis = getChild(pivot, "GTIM");
    ASSERT_NE(nullptr, gtis);
    Datapoint* mvTyp = getChild(gtis, "MvTyp");
    ASSERT_NE(nullptr, mvTyp);
    Datapoint* q = getChild(mvTyp, "q");
    ASSERT_NE(nullptr, q);
    Datapoint* qValidiy = getChild(q, "Validity");
    ASSERT_NE(nullptr, qValidiy);
    ASSERT_TRUE(isValueStr(qValidiy));
    ASSERT_EQ("good", getValueStr(qValidiy));
    Datapoint* comingFrom = getChild(gtis, "ComingFrom");
    ASSERT_NE(nullptr, comingFrom);
    ASSERT_TRUE(isValueStr(comingFrom));
    ASSERT_EQ("iec104", getValueStr(comingFrom));
    Datapoint* cause = getChild(gtis, "Cause");
    ASSERT_NE(nullptr, cause);
    Datapoint* causeStVal = getChild(cause, "stVal");
    ASSERT_NE(nullptr, causeStVal);
    ASSERT_TRUE(isValueInt(causeStVal));
    ASSERT_EQ(3, getValueInt(causeStVal));
    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, M_ME_NC_1)
{
    outputHandlerCalled = 0;
    vector<Datapoint*> dataobjects;
    dataobjects.push_back(createDataObject(1,"M_ME_NC_1", 45, 986, 1, (float)0.2f, false, false, false, false, true, 0, false, false, false));
    Reading* reading = new Reading(std::string("TM3"), dataobjects);
    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error
    vector<Reading*> readings;
    readings.push_back(reading);
    ReadingSet readingSet;
    readingSet.append(readings);
    ConfigCategory config("exchanged_data", exchanged_data);
    config.setItemsValueFromDefault();
    string configValue = config.getValue("exchanged_data");
    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);
    ASSERT_TRUE(handle != nullptr);
    plugin_ingest(handle, &readingSet);
    ASSERT_EQ(1, outputHandlerCalled);
    ASSERT_NE(nullptr, lastReading);
    Datapoint* pivot = getDatapoint(lastReading, "PIVOT");
    ASSERT_NE(nullptr, pivot);
    Datapoint* gtis = getChild(pivot, "GTIM");
    ASSERT_NE(nullptr, gtis);
    Datapoint* mvTyp = getChild(gtis, "MvTyp");
    ASSERT_NE(nullptr, mvTyp);
    Datapoint* q = getChild(mvTyp, "q");
    ASSERT_NE(nullptr, q);
    Datapoint* mag = getChild(mvTyp, "mag");
    ASSERT_NE(nullptr, mag);
    Datapoint* magF = getChild(mag, "f");
    ASSERT_NE(nullptr, magF);
    ASSERT_TRUE(isValueFloat(magF));
    ASSERT_EQ(0.2f, getValueFloat(magF));
    Datapoint* qValidiy = getChild(q, "Validity");
    ASSERT_NE(nullptr, qValidiy);
    ASSERT_TRUE(isValueStr(qValidiy));
    ASSERT_EQ("questionable", getValueStr(qValidiy));
    Datapoint* comingFrom = getChild(gtis, "ComingFrom");
    ASSERT_NE(nullptr, comingFrom);
    ASSERT_TRUE(isValueStr(comingFrom));
    ASSERT_EQ("iec104", getValueStr(comingFrom));
    Datapoint* cause = getChild(gtis, "Cause");
    ASSERT_NE(nullptr, cause);
    Datapoint* causeStVal = getChild(cause, "stVal");
    ASSERT_NE(nullptr, causeStVal);
    ASSERT_TRUE(isValueInt(causeStVal));
    ASSERT_EQ(1, getValueInt(causeStVal));
    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, SingleCommand) {
    outputHandlerCalled = 0;

    vector<vector<Datapoint*>> commandobjects;

    commandobjects.push_back(createCommandObject("C_SC_NA_1", "1", "101", "3", "0", "0", "0", "", "1"));

    Reading* reading = new Reading(std::string("IEC104Command"), commandobjects[0]);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error
    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data_commands);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* command = getDatapoint(lastReading,"PIVOT");
    
    ASSERT_NE(nullptr,command);
    
    Datapoint* root = getChild(command,"GTIC");
    
    ASSERT_NE(nullptr,root);
    
    Datapoint* comingFrom = getChild(root,"ComingFrom");
    
    ASSERT_NE(nullptr,comingFrom);
    
    ASSERT_EQ(getValueStr(comingFrom),"iec104");
    
    Datapoint* spcType = getChild(root,"SpcTyp");
    
    ASSERT_NE(nullptr,spcType);
    
    Datapoint* ctlVal = getChild(spcType,"ctlVal");
    
    ASSERT_NE(nullptr,ctlVal);
    
    ASSERT_EQ(1,getValueInt(ctlVal));
    
    Datapoint* t = getChild(spcType,"t");
    
    ASSERT_NE(nullptr,t);
    
    plugin_ingest(handle, &readingSet);
    
    ASSERT_EQ(2, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    
    ASSERT_NE(nullptr,co_type);
    
    ASSERT_EQ("C_SC_NA_1",getValueStr(co_type));
    
    Datapoint* co_ca = getDatapoint(lastReading,"co_ca");
    
    ASSERT_NE(nullptr,co_ca);
    
    ASSERT_EQ("1",getValueStr(co_ca));
    
    Datapoint* co_ioa = getDatapoint(lastReading,"co_ioa");
    
    ASSERT_NE(nullptr,co_ioa);
    
    ASSERT_EQ("101",getValueStr(co_ioa));
    
    Datapoint* co_cot = getDatapoint(lastReading,"co_cot");
    
    ASSERT_NE(nullptr,co_cot);
    
    ASSERT_EQ("3",getValueStr(co_cot));
    
    Datapoint* co_negative = getDatapoint(lastReading,"co_negative");
    
    ASSERT_NE(nullptr,co_negative);
    
    ASSERT_EQ("0",getValueStr(co_negative));
    
    Datapoint* co_se = getDatapoint(lastReading,"co_se");
    
    ASSERT_NE(nullptr,co_se);
    
    ASSERT_EQ("0",getValueStr(co_se));
    
    Datapoint* co_test = getDatapoint(lastReading,"co_test");
    
    ASSERT_NE(nullptr,co_test);
    
    ASSERT_EQ("0",getValueStr(co_test));
    
    // Datapoint* co_ts = getDatapoint(lastReading,"co_ts");
    
    // ASSERT_NE(nullptr,co_ts);
    
    // ASSERT_EQ("1669123796250",getValueStr(co_ts));
    
    Datapoint* co_value = getDatapoint(lastReading,"co_value");
    
    ASSERT_NE(nullptr,co_value);
    
    ASSERT_EQ("1",getValueStr(co_value));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, SingleCommandTime) {
    outputHandlerCalled = 0;

    vector<vector<Datapoint*>> commandobjects;

    commandobjects.push_back(createCommandObject("C_SC_TA_1", "1", "106", "3", "0", "0", "0", "100", "1"));

    Reading* reading = new Reading(std::string("IEC104Command"), commandobjects[0]);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error
    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data_commands);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* command = getDatapoint(lastReading,"PIVOT");
    
    ASSERT_NE(nullptr,command);
    
    Datapoint* root = getChild(command,"GTIC");
    
    ASSERT_NE(nullptr,root);
    
    Datapoint* comingFrom = getChild(root,"ComingFrom");
    
    ASSERT_NE(nullptr,comingFrom);
    
    ASSERT_EQ(getValueStr(comingFrom),"iec104");
    
    Datapoint* spcType = getChild(root,"SpcTyp");
    
    ASSERT_NE(nullptr,spcType);
    
    Datapoint* ctlVal = getChild(spcType,"ctlVal");
    
    ASSERT_NE(nullptr,ctlVal);
    
    ASSERT_EQ(1,getValueInt(ctlVal));
    
    Datapoint* t = getChild(spcType,"t");
    
    ASSERT_NE(nullptr,t);
    
    plugin_ingest(handle, &readingSet);
    
    ASSERT_EQ(2, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    
    ASSERT_NE(nullptr,co_type);
    
    ASSERT_EQ("C_SC_TA_1",getValueStr(co_type));
    
    Datapoint* co_ca = getDatapoint(lastReading,"co_ca");
    
    ASSERT_NE(nullptr,co_ca);
    
    ASSERT_EQ("1",getValueStr(co_ca));
    
    Datapoint* co_ioa = getDatapoint(lastReading,"co_ioa");
    
    ASSERT_NE(nullptr,co_ioa);
    
    ASSERT_EQ("106",getValueStr(co_ioa));
    
    Datapoint* co_cot = getDatapoint(lastReading,"co_cot");
    
    ASSERT_NE(nullptr,co_cot);
    
    ASSERT_EQ("3",getValueStr(co_cot));
    
    Datapoint* co_negative = getDatapoint(lastReading,"co_negative");
    
    ASSERT_NE(nullptr,co_negative);
    
    ASSERT_EQ("0",getValueStr(co_negative));
    
    Datapoint* co_se = getDatapoint(lastReading,"co_se");
    
    ASSERT_NE(nullptr,co_se);
    
    ASSERT_EQ("0",getValueStr(co_se));
    
    Datapoint* co_test = getDatapoint(lastReading,"co_test");
    
    ASSERT_NE(nullptr,co_test);
    
    ASSERT_EQ("0",getValueStr(co_test));
    
    Datapoint* co_ts = getDatapoint(lastReading,"co_ts");
    
    ASSERT_NE(nullptr,co_ts);
    
    ASSERT_EQ("100",getValueStr(co_ts));
    
    Datapoint* co_value = getDatapoint(lastReading,"co_value");
    
    ASSERT_NE(nullptr,co_value);
    
    ASSERT_EQ("1",getValueStr(co_value));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, DoubleCommand) {
    outputHandlerCalled = 0;

    vector<vector<Datapoint*>> commandobjects;

    commandobjects.push_back(createCommandObject("C_DC_NA_1", "1", "102", "3", "0", "0", "0", "", "1"));

    Reading* reading = new Reading(std::string("IEC104Command"), commandobjects[0]);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error
    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data_commands);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* command = getDatapoint(lastReading,"PIVOT");
    
    ASSERT_NE(nullptr,command);
    
    Datapoint* root = getChild(command,"GTIC");
    
    ASSERT_NE(nullptr,root);
    
    Datapoint* comingFrom = getChild(root,"ComingFrom");
    
    ASSERT_NE(nullptr,comingFrom);
    
    ASSERT_EQ(getValueStr(comingFrom),"iec104");
    
    Datapoint* spcType = getChild(root,"DpcTyp");
    
    ASSERT_NE(nullptr,spcType);
    
    Datapoint* ctlVal = getChild(spcType,"ctlVal");
    
    ASSERT_NE(nullptr,ctlVal);
    
    ASSERT_EQ("off",getValueStr(ctlVal));
    
    Datapoint* t = getChild(spcType,"t");
    
    ASSERT_NE(nullptr,t);
    
    plugin_ingest(handle, &readingSet);
    
    ASSERT_EQ(2, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    
    ASSERT_NE(nullptr,co_type);
    
    ASSERT_EQ("C_DC_NA_1",getValueStr(co_type));
    
    Datapoint* co_ca = getDatapoint(lastReading,"co_ca");
    
    ASSERT_NE(nullptr,co_ca);
    
    ASSERT_EQ("1",getValueStr(co_ca));
    
    Datapoint* co_ioa = getDatapoint(lastReading,"co_ioa");
    
    ASSERT_NE(nullptr,co_ioa);
    
    ASSERT_EQ("102",getValueStr(co_ioa));
    
    Datapoint* co_cot = getDatapoint(lastReading,"co_cot");
    
    ASSERT_NE(nullptr,co_cot);
    
    ASSERT_EQ("3",getValueStr(co_cot));
    
    Datapoint* co_negative = getDatapoint(lastReading,"co_negative");
    
    ASSERT_NE(nullptr,co_negative);
    
    ASSERT_EQ("0",getValueStr(co_negative));
    
    Datapoint* co_se = getDatapoint(lastReading,"co_se");
    
    ASSERT_NE(nullptr,co_se);
    
    ASSERT_EQ("0",getValueStr(co_se));
    
    Datapoint* co_test = getDatapoint(lastReading,"co_test");
    
    ASSERT_NE(nullptr,co_test);
    
    ASSERT_EQ("0",getValueStr(co_test));
    
    // Datapoint* co_ts = getDatapoint(lastReading,"co_ts");
    
    // ASSERT_NE(nullptr,co_ts);
    
    // ASSERT_EQ("1669123796250",getValueStr(co_ts));
    
    Datapoint* co_value = getDatapoint(lastReading,"co_value");
    
    ASSERT_NE(nullptr,co_value);
    
    ASSERT_EQ("1",getValueStr(co_value));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, DoubleCommandTime) {
    outputHandlerCalled = 0;

    vector<vector<Datapoint*>> commandobjects;

    commandobjects.push_back(createCommandObject("C_DC_TA_1", "1", "107", "3", "0", "0", "0", "100", "1"));

    Reading* reading = new Reading(std::string("IEC104Command"), commandobjects[0]);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error
    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data_commands);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* command = getDatapoint(lastReading,"PIVOT");
    
    ASSERT_NE(nullptr,command);
    
    Datapoint* root = getChild(command,"GTIC");
    
    ASSERT_NE(nullptr,root);
    
    Datapoint* comingFrom = getChild(root,"ComingFrom");
    
    ASSERT_NE(nullptr,comingFrom);
    
    ASSERT_EQ(getValueStr(comingFrom),"iec104");
    
    Datapoint* spcType = getChild(root,"DpcTyp");
    
    ASSERT_NE(nullptr,spcType);
    
    Datapoint* ctlVal = getChild(spcType,"ctlVal");
    
    ASSERT_NE(nullptr,ctlVal);
    
    ASSERT_EQ("off",getValueStr(ctlVal));
    
    Datapoint* t = getChild(spcType,"t");
    
    ASSERT_NE(nullptr,t);
    
    plugin_ingest(handle, &readingSet);
    
    ASSERT_EQ(2, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    
    ASSERT_NE(nullptr,co_type);
    
    ASSERT_EQ("C_DC_TA_1",getValueStr(co_type));
    
    Datapoint* co_ca = getDatapoint(lastReading,"co_ca");
    
    ASSERT_NE(nullptr,co_ca);
    
    ASSERT_EQ("1",getValueStr(co_ca));
    
    Datapoint* co_ioa = getDatapoint(lastReading,"co_ioa");
    
    ASSERT_NE(nullptr,co_ioa);
    
    ASSERT_EQ("107",getValueStr(co_ioa));
    
    Datapoint* co_cot = getDatapoint(lastReading,"co_cot");
    
    ASSERT_NE(nullptr,co_cot);
    
    ASSERT_EQ("3",getValueStr(co_cot));
    
    Datapoint* co_negative = getDatapoint(lastReading,"co_negative");
    
    ASSERT_NE(nullptr,co_negative);
    
    ASSERT_EQ("0",getValueStr(co_negative));
    
    Datapoint* co_se = getDatapoint(lastReading,"co_se");
    
    ASSERT_NE(nullptr,co_se);
    
    ASSERT_EQ("0",getValueStr(co_se));
    
    Datapoint* co_test = getDatapoint(lastReading,"co_test");
    
    ASSERT_NE(nullptr,co_test);
    
    ASSERT_EQ("0",getValueStr(co_test));
    
    Datapoint* co_ts = getDatapoint(lastReading,"co_ts");
    
    ASSERT_NE(nullptr,co_ts);
    
    ASSERT_EQ("100",getValueStr(co_ts));
    
    Datapoint* co_value = getDatapoint(lastReading,"co_value");
    
    ASSERT_NE(nullptr,co_value);
    
    ASSERT_EQ("1",getValueStr(co_value));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, SetPointCommandScaled) {
    outputHandlerCalled = 0;

    vector<vector<Datapoint*>> commandobjects;

    commandobjects.push_back(createCommandObject("C_SE_NB_1", "1", "104", "3", "0", "0", "0", "", "1"));

    Reading* reading = new Reading(std::string("IEC104Command"), commandobjects[0]);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error
    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data_commands);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* command = getDatapoint(lastReading,"PIVOT");
    
    ASSERT_NE(nullptr,command);
    
    Datapoint* root = getChild(command,"GTIC");
    
    ASSERT_NE(nullptr,root);
    
    Datapoint* comingFrom = getChild(root,"ComingFrom");
    
    ASSERT_NE(nullptr,comingFrom);
    
    ASSERT_EQ(getValueStr(comingFrom),"iec104");
    
    Datapoint* spcType = getChild(root,"IncTyp");
    
    ASSERT_NE(nullptr,spcType);
    
    Datapoint* ctlVal = getChild(spcType,"ctlVal");
    
    ASSERT_NE(nullptr,ctlVal);
    
    ASSERT_EQ(1,getValueInt(ctlVal));
    
    Datapoint* t = getChild(spcType,"t");
    
    ASSERT_NE(nullptr,t);
    
    plugin_ingest(handle, &readingSet);
    
    ASSERT_EQ(2, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    
    ASSERT_NE(nullptr,co_type);
    
    ASSERT_EQ("C_SE_NB_1",getValueStr(co_type));
    
    Datapoint* co_ca = getDatapoint(lastReading,"co_ca");
    
    ASSERT_NE(nullptr,co_ca);
    
    ASSERT_EQ("1",getValueStr(co_ca));
    
    Datapoint* co_ioa = getDatapoint(lastReading,"co_ioa");
    
    ASSERT_NE(nullptr,co_ioa);
    
    ASSERT_EQ("104",getValueStr(co_ioa));
    
    Datapoint* co_cot = getDatapoint(lastReading,"co_cot");
    
    ASSERT_NE(nullptr,co_cot);
    
    ASSERT_EQ("3",getValueStr(co_cot));
    
    Datapoint* co_negative = getDatapoint(lastReading,"co_negative");
    
    ASSERT_NE(nullptr,co_negative);
    
    ASSERT_EQ("0",getValueStr(co_negative));
    
    Datapoint* co_se = getDatapoint(lastReading,"co_se");
    
    ASSERT_NE(nullptr,co_se);
    
    ASSERT_EQ("0",getValueStr(co_se));
    
    Datapoint* co_test = getDatapoint(lastReading,"co_test");
    
    ASSERT_NE(nullptr,co_test);
    
    ASSERT_EQ("0",getValueStr(co_test));
    
    // Datapoint* co_ts = getDatapoint(lastReading,"co_ts");
    
    // ASSERT_NE(nullptr,co_ts);
    
    // ASSERT_EQ("1669123796250",getValueStr(co_ts));
    
    Datapoint* co_value = getDatapoint(lastReading,"co_value");
    
    ASSERT_NE(nullptr,co_value);
    
    ASSERT_EQ("1",getValueStr(co_value));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, SetPointCommandScaledTime) {
    outputHandlerCalled = 0;

    vector<vector<Datapoint*>> commandobjects;

    commandobjects.push_back(createCommandObject("C_SE_TB_1", "1", "109", "3", "0", "0", "0", "100", "1"));

    Reading* reading = new Reading(std::string("IEC104Command"), commandobjects[0]);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error
    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data_commands);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* command = getDatapoint(lastReading,"PIVOT");
    
    ASSERT_NE(nullptr,command);
    
    Datapoint* root = getChild(command,"GTIC");
    
    ASSERT_NE(nullptr,root);
    
    Datapoint* comingFrom = getChild(root,"ComingFrom");
    
    ASSERT_NE(nullptr,comingFrom);
    
    ASSERT_EQ(getValueStr(comingFrom),"iec104");
    
    Datapoint* spcType = getChild(root,"IncTyp");
    
    ASSERT_NE(nullptr,spcType);
    
    Datapoint* ctlVal = getChild(spcType,"ctlVal");
    
    ASSERT_NE(nullptr,ctlVal);
    
    ASSERT_EQ(1,getValueInt(ctlVal));
    
    Datapoint* t = getChild(spcType,"t");
    
    ASSERT_NE(nullptr,t);
    
    plugin_ingest(handle, &readingSet);
    
    ASSERT_EQ(2, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    
    ASSERT_NE(nullptr,co_type);
    
    ASSERT_EQ("C_SE_TB_1",getValueStr(co_type));
    
    Datapoint* co_ca = getDatapoint(lastReading,"co_ca");
    
    ASSERT_NE(nullptr,co_ca);
    
    ASSERT_EQ("1",getValueStr(co_ca));
    
    Datapoint* co_ioa = getDatapoint(lastReading,"co_ioa");
    
    ASSERT_NE(nullptr,co_ioa);
    
    ASSERT_EQ("109",getValueStr(co_ioa));
    
    Datapoint* co_cot = getDatapoint(lastReading,"co_cot");
    
    ASSERT_NE(nullptr,co_cot);
    
    ASSERT_EQ("3",getValueStr(co_cot));
    
    Datapoint* co_negative = getDatapoint(lastReading,"co_negative");
    
    ASSERT_NE(nullptr,co_negative);
    
    ASSERT_EQ("0",getValueStr(co_negative));
    
    Datapoint* co_se = getDatapoint(lastReading,"co_se");
    
    ASSERT_NE(nullptr,co_se);
    
    ASSERT_EQ("0",getValueStr(co_se));
    
    Datapoint* co_test = getDatapoint(lastReading,"co_test");
    
    ASSERT_NE(nullptr,co_test);
    
    ASSERT_EQ("0",getValueStr(co_test));
    
    Datapoint* co_ts = getDatapoint(lastReading,"co_ts");
    
    ASSERT_NE(nullptr,co_ts);
    
    ASSERT_EQ("100",getValueStr(co_ts));
    
    Datapoint* co_value = getDatapoint(lastReading,"co_value");
    
    ASSERT_NE(nullptr,co_value);
    
    ASSERT_EQ("1",getValueStr(co_value));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, SetPointCommandShort) {
    outputHandlerCalled = 0;

    vector<vector<Datapoint*>> commandobjects;

    commandobjects.push_back(createCommandObject("C_SE_NC_1", "1", "100", "3", "0", "0", "0", "", "1.5"));

    Reading* reading = new Reading(std::string("IEC104Command"), commandobjects[0]);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error
    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data_commands);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* command = getDatapoint(lastReading,"PIVOT");
    
    ASSERT_NE(nullptr,command);
    
    Datapoint* root = getChild(command,"GTIC");
    
    ASSERT_NE(nullptr,root);
    
    Datapoint* comingFrom = getChild(root,"ComingFrom");
    
    ASSERT_NE(nullptr,comingFrom);
    
    ASSERT_EQ(getValueStr(comingFrom),"iec104");
    
    Datapoint* spcType = getChild(root,"ApcTyp");
    
    ASSERT_NE(nullptr,spcType);
    
    Datapoint* ctlVal = getChild(spcType,"ctlVal");
    
    ASSERT_NE(nullptr,ctlVal);
    
    ASSERT_EQ(1.5f,getValueFloat(ctlVal));
    
    Datapoint* t = getChild(spcType,"t");
    
    ASSERT_NE(nullptr,t);
    
    plugin_ingest(handle, &readingSet);
    
    ASSERT_EQ(2, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    
    ASSERT_NE(nullptr,co_type);
    
    ASSERT_EQ("C_SE_NC_1",getValueStr(co_type));
    
    Datapoint* co_ca = getDatapoint(lastReading,"co_ca");
    
    ASSERT_NE(nullptr,co_ca);
    
    ASSERT_EQ("1",getValueStr(co_ca));
    
    Datapoint* co_ioa = getDatapoint(lastReading,"co_ioa");
    
    ASSERT_NE(nullptr,co_ioa);
    
    ASSERT_EQ("100",getValueStr(co_ioa));
    
    Datapoint* co_cot = getDatapoint(lastReading,"co_cot");
    
    ASSERT_NE(nullptr,co_cot);
    
    ASSERT_EQ("3",getValueStr(co_cot));
    
    Datapoint* co_negative = getDatapoint(lastReading,"co_negative");
    
    ASSERT_NE(nullptr,co_negative);
    
    ASSERT_EQ("0",getValueStr(co_negative));
    
    Datapoint* co_se = getDatapoint(lastReading,"co_se");
    
    ASSERT_NE(nullptr,co_se);
    
    ASSERT_EQ("0",getValueStr(co_se));
    
    Datapoint* co_test = getDatapoint(lastReading,"co_test");
    
    ASSERT_NE(nullptr,co_test);
    
    ASSERT_EQ("0",getValueStr(co_test));
    
    // Datapoint* co_ts = getDatapoint(lastReading,"co_ts");
    
    // ASSERT_NE(nullptr,co_ts);
    
    // ASSERT_EQ("1669123796250",getValueStr(co_ts));
    
    Datapoint* co_value = getDatapoint(lastReading,"co_value");
    
    ASSERT_NE(nullptr,co_value);
    
    ASSERT_EQ(1.5f,stof(getValueStr(co_value)));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, SetPointCommandShortTime) {
    outputHandlerCalled = 0;

    vector<vector<Datapoint*>> commandobjects;

    commandobjects.push_back(createCommandObject("C_SE_TC_1", "1", "105", "3", "0", "0", "0", "100", "1.5"));

    Reading* reading = new Reading(std::string("IEC104Command"), commandobjects[0]);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error
    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data_commands);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* command = getDatapoint(lastReading,"PIVOT");
    
    ASSERT_NE(nullptr,command);
    
    Datapoint* root = getChild(command,"GTIC");
    
    ASSERT_NE(nullptr,root);
    
    Datapoint* comingFrom = getChild(root,"ComingFrom");
    
    ASSERT_NE(nullptr,comingFrom);
    
    ASSERT_EQ(getValueStr(comingFrom),"iec104");
    
    Datapoint* spcType = getChild(root,"ApcTyp");
    
    ASSERT_NE(nullptr,spcType);
    
    Datapoint* ctlVal = getChild(spcType,"ctlVal");
    
    ASSERT_NE(nullptr,ctlVal);
    
    ASSERT_EQ(1.5,getValueFloat(ctlVal));
    
    Datapoint* t = getChild(spcType,"t");
    
    ASSERT_NE(nullptr,t);
    
    plugin_ingest(handle, &readingSet);
    
    ASSERT_EQ(2, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    
    ASSERT_NE(nullptr,co_type);
    
    ASSERT_EQ("C_SE_TC_1",getValueStr(co_type));
    
    Datapoint* co_ca = getDatapoint(lastReading,"co_ca");
    
    ASSERT_NE(nullptr,co_ca);
    
    ASSERT_EQ("1",getValueStr(co_ca));
    
    Datapoint* co_ioa = getDatapoint(lastReading,"co_ioa");
    
    ASSERT_NE(nullptr,co_ioa);
    
    ASSERT_EQ("105",getValueStr(co_ioa));
    
    Datapoint* co_cot = getDatapoint(lastReading,"co_cot");
    
    ASSERT_NE(nullptr,co_cot);
    
    ASSERT_EQ("3",getValueStr(co_cot));
    
    Datapoint* co_negative = getDatapoint(lastReading,"co_negative");
    
    ASSERT_NE(nullptr,co_negative);
    
    ASSERT_EQ("0",getValueStr(co_negative));
    
    Datapoint* co_se = getDatapoint(lastReading,"co_se");
    
    ASSERT_NE(nullptr,co_se);
    
    ASSERT_EQ("0",getValueStr(co_se));
    
    Datapoint* co_test = getDatapoint(lastReading,"co_test");
    
    ASSERT_NE(nullptr,co_test);
    
    ASSERT_EQ("0",getValueStr(co_test));
    
    Datapoint* co_ts = getDatapoint(lastReading,"co_ts");
    
    ASSERT_NE(nullptr,co_ts);
    
    ASSERT_EQ("100",getValueStr(co_ts));
    
    Datapoint* co_value = getDatapoint(lastReading,"co_value");
    
    ASSERT_NE(nullptr,co_value);
    
    ASSERT_EQ(1.5f,stof(getValueStr(co_value)));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, StepCommand) {
    outputHandlerCalled = 0;

    vector<vector<Datapoint*>> commandobjects;

    commandobjects.push_back(createCommandObject("C_RC_NA_1", "1", "110", "3", "0", "0", "0", "", "1"));

    Reading* reading = new Reading(std::string("IEC104Command"), commandobjects[0]);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error
    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data_commands);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* command = getDatapoint(lastReading,"PIVOT");
    
    ASSERT_NE(nullptr,command);
    
    Datapoint* root = getChild(command,"GTIC");
    
    ASSERT_NE(nullptr,root);
    
    Datapoint* comingFrom = getChild(root,"ComingFrom");
    
    ASSERT_NE(nullptr,comingFrom);
    
    ASSERT_EQ(getValueStr(comingFrom),"iec104");
    
    Datapoint* spcType = getChild(root,"BscTyp");
    
    ASSERT_NE(nullptr,spcType);
    
    Datapoint* ctlVal = getChild(spcType,"ctlVal");
    
    ASSERT_NE(nullptr,ctlVal);
    
    ASSERT_EQ("lower",getValueStr(ctlVal));
    
    Datapoint* t = getChild(spcType,"t");
    
    ASSERT_NE(nullptr,t);
    
    plugin_ingest(handle, &readingSet);
    
    ASSERT_EQ(2, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    
    ASSERT_NE(nullptr,co_type);
    
    ASSERT_EQ("C_RC_NA_1",getValueStr(co_type));
    
    Datapoint* co_ca = getDatapoint(lastReading,"co_ca");
    
    ASSERT_NE(nullptr,co_ca);
    
    ASSERT_EQ("1",getValueStr(co_ca));
    
    Datapoint* co_ioa = getDatapoint(lastReading,"co_ioa");
    
    ASSERT_NE(nullptr,co_ioa);
    
    ASSERT_EQ("110",getValueStr(co_ioa));
    
    Datapoint* co_cot = getDatapoint(lastReading,"co_cot");
    
    ASSERT_NE(nullptr,co_cot);
    
    ASSERT_EQ("3",getValueStr(co_cot));
    
    Datapoint* co_negative = getDatapoint(lastReading,"co_negative");
    
    ASSERT_NE(nullptr,co_negative);
    
    ASSERT_EQ("0",getValueStr(co_negative));
    
    Datapoint* co_se = getDatapoint(lastReading,"co_se");
    
    ASSERT_NE(nullptr,co_se);
    
    ASSERT_EQ("0",getValueStr(co_se));
    
    Datapoint* co_test = getDatapoint(lastReading,"co_test");
    
    ASSERT_NE(nullptr,co_test);
    
    ASSERT_EQ("0",getValueStr(co_test));
    
    // Datapoint* co_ts = getDatapoint(lastReading,"co_ts");
    
    // ASSERT_NE(nullptr,co_ts);
    
    // ASSERT_EQ("1669123796250",getValueStr(co_ts));
    
    Datapoint* co_value = getDatapoint(lastReading,"co_value");
    
    ASSERT_NE(nullptr,co_value);
    
    ASSERT_EQ("1",getValueStr(co_value));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, StepCommandTime) {
    outputHandlerCalled = 0;

    vector<vector<Datapoint*>> commandobjects;

    commandobjects.push_back(createCommandObject("C_RC_TA_1", "1", "111", "3", "0", "0", "0", "100", "1"));

    Reading* reading = new Reading(std::string("IEC104Command"), commandobjects[0]);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error
    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data_commands);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* command = getDatapoint(lastReading,"PIVOT");
    
    ASSERT_NE(nullptr,command);
    
    Datapoint* root = getChild(command,"GTIC");
    
    ASSERT_NE(nullptr,root);
    
    Datapoint* comingFrom = getChild(root,"ComingFrom");
    
    ASSERT_NE(nullptr,comingFrom);
    
    ASSERT_EQ(getValueStr(comingFrom),"iec104");
    
    Datapoint* spcType = getChild(root,"BscTyp");
    
    ASSERT_NE(nullptr,spcType);
    
    Datapoint* ctlVal = getChild(spcType,"ctlVal");
    
    ASSERT_NE(nullptr,ctlVal);
    
    ASSERT_EQ("lower",getValueStr(ctlVal));
    
    Datapoint* t = getChild(spcType,"t");
    
    ASSERT_NE(nullptr,t);
    
    plugin_ingest(handle, &readingSet);
    
    ASSERT_EQ(2, outputHandlerCalled);
    
    ASSERT_NE(nullptr, lastReading);
    
    Datapoint* co_type = getDatapoint(lastReading,"co_type");
    
    ASSERT_NE(nullptr,co_type);
    
    ASSERT_EQ("C_RC_TA_1",getValueStr(co_type));
    
    Datapoint* co_ca = getDatapoint(lastReading,"co_ca");
    
    ASSERT_NE(nullptr,co_ca);
    
    ASSERT_EQ("1",getValueStr(co_ca));
    
    Datapoint* co_ioa = getDatapoint(lastReading,"co_ioa");
    
    ASSERT_NE(nullptr,co_ioa);
    
    ASSERT_EQ("111",getValueStr(co_ioa));
    
    Datapoint* co_cot = getDatapoint(lastReading,"co_cot");
    
    ASSERT_NE(nullptr,co_cot);
    
    ASSERT_EQ("3",getValueStr(co_cot));
    
    Datapoint* co_negative = getDatapoint(lastReading,"co_negative");
    
    ASSERT_NE(nullptr,co_negative);
    
    ASSERT_EQ("0",getValueStr(co_negative));
    
    Datapoint* co_se = getDatapoint(lastReading,"co_se");
    
    ASSERT_NE(nullptr,co_se);
    
    ASSERT_EQ("0",getValueStr(co_se));
    
    Datapoint* co_test = getDatapoint(lastReading,"co_test");
    
    ASSERT_NE(nullptr,co_test);
    
    ASSERT_EQ("0",getValueStr(co_test));
    
    Datapoint* co_ts = getDatapoint(lastReading,"co_ts");
    
    ASSERT_NE(nullptr,co_ts);
    
    ASSERT_EQ("100",getValueStr(co_ts));
    
    Datapoint* co_value = getDatapoint(lastReading,"co_value");
    
    ASSERT_NE(nullptr,co_value);
    
    ASSERT_EQ("1",getValueStr(co_value));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, M_ST_NA)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject(1,"M_ST_NA_1", 45, 920, 1, "[1,true]", false, false, true, false, false, 0, false, false, false));

    Reading* reading = new Reading(std::string("TM250"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* pivot = getDatapoint(lastReading, "PIVOT");
    ASSERT_NE(nullptr, pivot);
    Datapoint* gtis = getChild(pivot, "GTIM");
    ASSERT_NE(nullptr, gtis);
    Datapoint* bscTyp = getChild(gtis, "BscTyp");
    ASSERT_NE(nullptr, bscTyp);
    Datapoint* valWtr = getChild(bscTyp, "valWtr");
    ASSERT_NE(nullptr, valWtr);
    Datapoint* transient = getChild(valWtr, "transInd");
    ASSERT_NE(nullptr, transient);
    Datapoint* posVal = getChild(valWtr, "posVal");
    ASSERT_NE(nullptr, transient);

    ASSERT_TRUE(isValueInt(transient));
    ASSERT_EQ(1, getValueInt(transient));

    ASSERT_TRUE(isValueInt(posVal));
    ASSERT_EQ(1, getValueInt(posVal));

    Datapoint* q = getChild(bscTyp, "q");
    ASSERT_NE(nullptr, q);

    Datapoint* qDetailQuality = getChild(q, "DetailQuality");
    ASSERT_NE(nullptr, qDetailQuality);

    Datapoint* qDetailQualityOldData = getChild(qDetailQuality, "oldData");
    ASSERT_EQ(nullptr, qDetailQualityOldData);

    Datapoint* qDetailQualityOverflow = getChild(qDetailQuality, "overflow");
    ASSERT_NE(nullptr, qDetailQualityOverflow);
    ASSERT_TRUE(isValueInt(qDetailQualityOverflow));
    ASSERT_EQ(1, getValueInt(qDetailQualityOverflow));

    Datapoint* comingFrom = getChild(gtis, "ComingFrom");
    ASSERT_NE(nullptr, comingFrom);
    ASSERT_TRUE(isValueStr(comingFrom));
    ASSERT_EQ("iec104", getValueStr(comingFrom));

    Datapoint* cause = getChild(gtis, "Cause");
    ASSERT_NE(nullptr, cause);
    Datapoint* causeStVal = getChild(cause, "stVal");
    ASSERT_NE(nullptr, causeStVal);
    ASSERT_TRUE(isValueInt(causeStVal));
    ASSERT_EQ(1, getValueInt(causeStVal));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, BscTyp_to_M_ST_NA_1)
{
    outputHandlerCalled = 0;

    PivotDataObject* bscTyp = new PivotDataObject("GTIM", "BscTyp");

    ASSERT_NE(nullptr, bscTyp);

    bscTyp->setIdentifier("ID-45-920");
    bscTyp->setCause(3); /* COT = spont */
    bscTyp->setPosVal(1,true);
    bscTyp->addQuality(false, false, false, false, false, false);

    Datapoint* dp = bscTyp->toDatapoint();

    delete bscTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("TM250"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* dataobject = getDatapoint(lastReading, "data_object");
    ASSERT_NE(nullptr, dataobject);
    Datapoint* doType = getChild(dataobject, "do_type");
    ASSERT_NE(nullptr, doType);
    ASSERT_TRUE(isValueStr(doType));
    ASSERT_EQ("M_ST_NA_1", getValueStr(doType));

    Datapoint* doCa= getChild(dataobject, "do_ca");
    ASSERT_NE(nullptr, doCa);
    ASSERT_TRUE(isValueInt(doCa));
    ASSERT_EQ(45, getValueInt(doCa));

    Datapoint* doIoa= getChild(dataobject, "do_ioa");
    ASSERT_NE(nullptr, doIoa);
    ASSERT_TRUE(isValueInt(doIoa));
    ASSERT_EQ(920, getValueInt(doIoa));

    Datapoint* doCot= getChild(dataobject, "do_cot");
    ASSERT_NE(nullptr, doCot);
    ASSERT_TRUE(isValueInt(doCot));
    ASSERT_EQ(3, getValueInt(doCot));

    Datapoint* doValue= getChild(dataobject, "do_value");
    ASSERT_NE(nullptr, doValue);
    ASSERT_TRUE(isValueStr(doValue));
    ASSERT_EQ("[1,true]", getValueStr(doValue));


    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, M_ST_TA)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject(1,"M_ST_TB_1", 45, 921, 1, "[1,true]", false, false, true, false, false, 100, false, false, false));

    Reading* reading = new Reading(std::string("TM255"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* pivot = getDatapoint(lastReading, "PIVOT");
    ASSERT_NE(nullptr, pivot);
    Datapoint* gtis = getChild(pivot, "GTIM");
    ASSERT_NE(nullptr, gtis);
    Datapoint* bscTyp = getChild(gtis, "BscTyp");
    ASSERT_NE(nullptr, bscTyp);
    Datapoint* valWtr = getChild(bscTyp, "valWtr");
    ASSERT_NE(nullptr, valWtr);
    Datapoint* transient = getChild(valWtr, "transInd");
    ASSERT_NE(nullptr, transient);
    Datapoint* posVal = getChild(valWtr, "posVal");
    ASSERT_NE(nullptr, transient);

    ASSERT_TRUE(isValueInt(transient));
    ASSERT_EQ(1, getValueInt(transient));

    ASSERT_TRUE(isValueInt(posVal));
    ASSERT_EQ(1, getValueInt(posVal));

    Datapoint* q = getChild(bscTyp, "q");
    ASSERT_NE(nullptr, q);

    Datapoint* qDetailQuality = getChild(q, "DetailQuality");
    ASSERT_NE(nullptr, qDetailQuality);

    Datapoint* qDetailQualityOldData = getChild(qDetailQuality, "oldData");
    ASSERT_EQ(nullptr, qDetailQualityOldData);

    Datapoint* qDetailQualityOverflow = getChild(qDetailQuality, "overflow");
    ASSERT_NE(nullptr, qDetailQualityOverflow);
    ASSERT_TRUE(isValueInt(qDetailQualityOverflow));
    ASSERT_EQ(1, getValueInt(qDetailQualityOverflow));

    Datapoint* comingFrom = getChild(gtis, "ComingFrom");
    ASSERT_NE(nullptr, comingFrom);
    ASSERT_TRUE(isValueStr(comingFrom));
    ASSERT_EQ("iec104", getValueStr(comingFrom));

    Datapoint* cause = getChild(gtis, "Cause");
    ASSERT_NE(nullptr, cause);
    Datapoint* causeStVal = getChild(cause, "stVal");
    ASSERT_NE(nullptr, causeStVal);
    ASSERT_TRUE(isValueInt(causeStVal));
    ASSERT_EQ(1, getValueInt(causeStVal));

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, BscTyp_to_M_ST_TA_1)
{
    outputHandlerCalled = 0;

    PivotDataObject* bscTyp = new PivotDataObject("GTIM", "BscTyp");

    ASSERT_NE(nullptr, bscTyp);

    bscTyp->setIdentifier("ID-45-920");
    bscTyp->setCause(3); /* COT = spont */
    bscTyp->setPosVal(1,true);
    bscTyp->addQuality(false, false, false, false, false, false);

    Datapoint* dp = bscTyp->toDatapoint();

    delete bscTyp;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(dp);

    Reading* reading = new Reading(std::string("TM255"), dataobjects);

    reading->setId(1); // Required: otherwise there will be a "move depends on unitilized value" error

    vector<Reading*> readings;

    readings.push_back(reading);

    ReadingSet readingSet;

    readingSet.append(readings);

    ConfigCategory config("exchanged_data", exchanged_data);

    config.setItemsValueFromDefault();

    string configValue = config.getValue("exchanged_data");

    PLUGIN_HANDLE handle = plugin_init(&config, NULL, testOutputStream);

    ASSERT_TRUE(handle != nullptr);

    plugin_ingest(handle, &readingSet);

    ASSERT_EQ(1, outputHandlerCalled);

    ASSERT_NE(nullptr, lastReading);

    Datapoint* dataobject = getDatapoint(lastReading, "data_object");
    ASSERT_NE(nullptr, dataobject);
    Datapoint* doType = getChild(dataobject, "do_type");
    ASSERT_NE(nullptr, doType);
    ASSERT_TRUE(isValueStr(doType));
    ASSERT_EQ("M_ST_TB_1", getValueStr(doType));

    Datapoint* doCa= getChild(dataobject, "do_ca");
    ASSERT_NE(nullptr, doCa);
    ASSERT_TRUE(isValueInt(doCa));
    ASSERT_EQ(45, getValueInt(doCa));

    Datapoint* doIoa= getChild(dataobject, "do_ioa");
    ASSERT_NE(nullptr, doIoa);
    ASSERT_TRUE(isValueInt(doIoa));
    ASSERT_EQ(921, getValueInt(doIoa));

    Datapoint* doCot= getChild(dataobject, "do_cot");
    ASSERT_NE(nullptr, doCot);
    ASSERT_TRUE(isValueInt(doCot));
    ASSERT_EQ(3, getValueInt(doCot));

    Datapoint* doValue= getChild(dataobject, "do_value");
    ASSERT_NE(nullptr, doValue);
    ASSERT_TRUE(isValueStr(doValue));
    ASSERT_EQ("[1,true]", getValueStr(doValue));


    plugin_shutdown(handle);
}