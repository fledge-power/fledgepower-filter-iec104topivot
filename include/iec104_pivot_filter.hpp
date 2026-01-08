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

#ifndef _IEC104_PIVOT_FILTER_H
#define _IEC104_PIVOT_FILTER_H

#include <filter.h>
#include "iec104_pivot_filter_config.hpp"

using namespace std;

class IEC104PivotFilter {

public:
    /*
     * Struct used to store fields of a data object during processing
    */
    struct Iec104DataObject {
        std::string doType = "";
        int doCot = 0;
        bool doQualityIv = false;
        bool doQualityBl = false;
        bool doQualityOv = false;
        bool doQualitySb = false;
        bool doQualityNt = false;
        long doTs = 0;
        bool doTsIv = false;
        bool doTsSu = false;
        bool doTsSub = false;
        bool doTest = false;
        std::string comingFromValue = "iec104";
        bool doNegative = false;
        Datapoint* doValue = nullptr;
    };
    /*
     * Struct used to store fields of a command object during processing
    */
    struct Iec104CommandObject {
        std::string coType = "";
        int coIoa = 0;
        int coCa = 0;
        int coCot = 0;
        int coSe = 0;
        long coTs = 0;
        bool coTest = false;
        std::string comingFromValue = "iec104";
        Datapoint* coValue = nullptr;
    };

    IEC104PivotFilter(const std::string& filterName,
        ConfigCategory* filterConfig,
        OUTPUT_HANDLE *outHandle,
        OUTPUT_STREAM output);

    ~IEC104PivotFilter();

    void ingest(READINGSET* readingSet);
    void reconfigure(ConfigCategory* config);

private:

    Datapoint* addElement(Datapoint* dp, string elementPath);

    void addQuality(Datapoint* dp, bool bl, bool iv, bool nt, bool ov, bool sb, bool test);

    void addTimestamp(Datapoint* dp, long timestamp, bool iv, bool su, bool sub);

    template <class T>
    Datapoint* addElementWithValue(Datapoint* dp, string elementPath, const T value);

    Datapoint* createDp(string name);

    template <typename T>
    void static readAttribute(std::map<std::string, bool>& attributeFound, Datapoint* dp,
                              const std::string& targetName, T& out);
    void static readAttribute(std::map<std::string, bool>& attributeFound, Datapoint* dp,
                              const std::string& targetName, Datapoint*& out);
    void static readAttribute(std::map<std::string, bool>& attributeFound, Datapoint* dp,
                              const std::string& targetName, std::string& out);

    Datapoint* convertDataObjectToPivot(Datapoint* sourceDp, IEC104PivotDataPoint* exchangeConfig);

    Datapoint* convertOperationObjectToPivot(std::vector<Datapoint*> sourceDp);

    Datapoint* convertDatapointToIEC104DataObject(Datapoint* sourceDp);

    std::vector<Datapoint*> convertReadingToIEC104OperationObject(Datapoint* datapoints);

    bool hasASDUTimestamp(const std::string& asduType);

    OUTPUT_HANDLE* m_outHandle = nullptr;
    OUTPUT_STREAM m_output = nullptr;

    IEC104PivotConfig m_config;
};


#endif /* _IEC104_PIVOT_FILTER_H */