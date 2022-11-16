#include <gtest/gtest.h>
#include <plugin_api.h>
#include <reading.h>
#include <reading_set.h>
#include <filter.h>
#include <string.h>
#include <string>
#include <rapidjson/document.h>

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
            "type" : "json",
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

TEST(PivotIEC104Plugin, PluginInfo)
{
	PLUGIN_INFORMATION *info = plugin_info();
	ASSERT_STREQ(info->name, "iec104_to_pivot");
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


TEST(PivotIEC104Pluging, Plugin_ingest)
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

TEST(PivotIEC104Pluging, M_SP_TB_1)
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

    plugin_shutdown(handle);
}