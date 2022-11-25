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

    template <class T>
    Datapoint* createDpWithValue(string name, const T value);

    Datapoint* convertDatapointToPivot(Datapoint* sourceDp, IEC104PivotDataPoint* exchangeConfig);

    Datapoint* convertDatapointToIEC104(Datapoint* sourceDp, IEC104PivotDataPoint* exchangeConfig);

    OUTPUT_HANDLE* m_outHandle;
    OUTPUT_STREAM m_output;

    IEC104PivotConfig m_config;
};


#endif /* _IEC104_PIVOT_FILTER_H */