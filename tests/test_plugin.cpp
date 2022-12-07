#include <gtest/gtest.h>
#include <plugin_api.h>
#include <reading.h>
#include <reading_set.h>
#include <filter.h>
#include <string.h>
#include <string>
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

template <class T>
static Datapoint* createDataObject(const char* type, int ca, int ioa, int cot,
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

    dataobjects.push_back(createDataObject("M_SP_NA_1", 45, 672, 3, (int64_t)1, false, false, false, false, false, 0, false, false, false));
    dataobjects.push_back(createDataObject("M_SP_NA_1", 45, 673, 3, (int64_t)0, false, false, false, false, false, 0, false, false, false));
    dataobjects.push_back(createDataObject("M_SP_NA_1", 45, 947, 3, (int64_t)0, false, false, false, false, false, 0, false, false, false));

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

TEST(PivotIEC104Plugin, M_SP_TB_1)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject("M_SP_TB_1", 45, 872, 3, (int64_t)1, false, false, false, false, false, 1668631513250, true, false, false));

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

    dataobjects.push_back(createDataObject("M_DP_TB_1", 45, 890, 3, (int64_t)2, false, false, false, false, false, 1668631513250, true, false, false));

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

    plugin_shutdown(handle);
}

TEST(PivotIEC104Plugin, M_DP_NA_1)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject("M_DP_NA_1", 45, 890, 3, (int64_t)1, false, false, false, false, false, 0, true, false, false));

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
    ASSERT_EQ("off", getValueStr(stVal));

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

    dataobjects.push_back(createDataObject("M_DP_NA_1", 45, 890, 3, (int64_t)0, false, false, false, false, false, 0, true, false, false));

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
    ASSERT_EQ("intermediate-state", getValueStr(stVal));

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

    dataobjects.push_back(createDataObject("M_DP_NA_1", 45, 890, 3, (int64_t)3, false, false, false, false, false, 0, true, false, false));

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

TEST(PivotIEC104Plugin, TypeNotMatching)
{
    outputHandlerCalled = 0;

    vector<Datapoint*> dataobjects;

    dataobjects.push_back(createDataObject("M_SP_TB_1", 45, 890, 3, (int64_t)1, false, false, false, false, false, 1668631513250, true, false, false));

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

    PivotObject* spsTyp = new PivotObject("GTIS", "SpsTyp");

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

    PivotObject* spsTyp = new PivotObject("GTIS", "SpsTyp");

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

    PivotObject* mvTyp = new PivotObject("GTIM", "MvTyp");

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

    plugin_shutdown(handle);
}