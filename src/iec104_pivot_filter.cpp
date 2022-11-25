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

#include "iec104_pivot_filter.hpp"
#include "iec104_pivot_object.hpp"

#include <config_category.h>
#include <logger.h>
#include <plugin_api.h>

#include <time.h>

IEC104PivotFilter::IEC104PivotFilter(const std::string& filterName,
        ConfigCategory* filterConfig,
        OUTPUT_HANDLE *outHandle,
        OUTPUT_STREAM output)
{
    (void)filterName; /* ignore parameter */

    m_outHandle = outHandle;
    m_output = output;

    reconfigure(filterConfig);
}

IEC104PivotFilter::~IEC104PivotFilter()
{

}

static uint64_t
getTimeInMs()
{
    struct timeval now;

    gettimeofday(&now, NULL);

    return ((uint64_t) now.tv_sec * 1000LL) + (now.tv_usec / 1000);
}

static bool
checkTypeMatch(std::string& incomingType, IEC104PivotDataPoint* exchangeConfig)
{
    if (incomingType == "M_SP_NA_1" || incomingType == "M_SP_TB_1") {
        if (exchangeConfig->getTypeId() == "M_SP_NA_1" || exchangeConfig->getTypeId() == "M_SP_TB_1")
            return true;
    }
    else if (incomingType == "M_DP_NA_1" || incomingType == "M_DP_TB_1") {
        if (exchangeConfig->getTypeId() == "M_DP_NA_1" || exchangeConfig->getTypeId() == "M_DP_TB_1")
            return true;
    }
    else if (incomingType == "M_ME_NA_1" || incomingType == "M_ME_TD_1") { /* normalized measured value */
        if (exchangeConfig->getTypeId() == "M_ME_NA_1" || exchangeConfig->getTypeId() == "M_ME_TD_1")
            return true;
    }
    else if (incomingType == "M_ME_NB_1" || incomingType == "M_ME_TE_1") { /* scaled measured value */
        if (exchangeConfig->getTypeId() == "M_ME_NB_1" || exchangeConfig->getTypeId() == "M_ME_TE_1")
            return true;
    }
    else if (incomingType == "M_ME_NC_1" || incomingType == "M_ME_TF_1") { /* short (float) measured value */
        if (exchangeConfig->getTypeId() == "M_ME_NC_1" || exchangeConfig->getTypeId() == "M_ME_TF_1")
            return true;
    }

    return false;
}

Datapoint*
IEC104PivotFilter::convertDatapointToPivot(Datapoint* sourceDp, IEC104PivotDataPoint* exchangeConfig)
{
    Datapoint* convertedDatapoint = nullptr;

    DatapointValue& dpv = sourceDp->getData();

    if (dpv.getType() == DatapointValue::T_DP_DICT)
    {
        std::vector<Datapoint*>* datapoints = dpv.getDpVec();

        bool hasDoType = false;
        bool hasDoCot = false;

        string doType = "";
        int doCot = 0;

        bool hasDoQualityIv = false;
        bool hasDoQualityBl = false;
        bool hasDoQualityOv = false;
        bool hasDoQualitySb = false;
        bool hasDoQualityNt = false;

        bool doQualityIv = false;
        bool doQualityBl = false;
        bool doQualityOv = false;
        bool doQualitySb = false;
        bool doQualityNt = false;

        bool hasDoTs = false;
        bool hasDoTsIv = false;
        bool hasDoTsSu = false;
        bool hasDoTsSub = false;

        long doTs = 0;
        bool doTsIv = false;
        bool doTsSu = false;
        bool doTsSub = false;

        bool hasDoTest = false;
        bool doTest = false;

        Datapoint* doValue = nullptr;

        for (Datapoint* dp : *datapoints) {
            printf("DP name: %s value: %s\n", dp->getName().c_str(), dp->getData().toString().c_str());

            if ((hasDoType == false) && (dp->getName() == "do_type")) {
                if (dp->getData().getType() == DatapointValue::T_STRING) {
                    doType = dp->getData().toStringValue();

                    if (checkTypeMatch(doType, exchangeConfig)) {
                        hasDoType = true;
                    }
                    else {
                        Logger::getLogger()->warn("Input type does not match configured type for %s", exchangeConfig->getLabel().c_str());
                    }
                }
            }
            else if ((hasDoCot == false) && (dp->getName() == "do_cot")) {
                if (dp->getData().getType() == DatapointValue::T_INTEGER) {
                    doCot = dp->getData().toInt();
                    hasDoCot = true;
                }
            }
            else if ((doValue == nullptr) && (dp->getName() == "do_value"))
            {
                doValue = dp;
            }
            else if ((hasDoQualityIv == false) && (dp->getName() == "do_quality_iv")) {
                if (dp->getData().getType() == DatapointValue::T_INTEGER) {
                    if (dp->getData().toInt() > 0)
                        doQualityIv = true;
                }
               
                hasDoQualityIv = true;
            }
            else if ((hasDoQualityBl == false) && (dp->getName() == "do_quality_bl")) {
                if (dp->getData().getType() == DatapointValue::T_INTEGER) {
                    if (dp->getData().toInt() > 0)
                        doQualityBl = true;
                }
               
                hasDoQualityBl = true;
            }
            else if ((hasDoQualityOv == false) && (dp->getName() == "do_quality_ov")) {
                if (dp->getData().getType() == DatapointValue::T_INTEGER) {
                    if (dp->getData().toInt() > 0)
                        doQualityOv = true;
                }
               
                hasDoQualityOv = true;
            }
            else if ((hasDoQualitySb == false) && (dp->getName() == "do_quality_sb")) {
                if (dp->getData().getType() == DatapointValue::T_INTEGER) {
                    if (dp->getData().toInt() > 0)
                        doQualitySb = true;
                }
               
                hasDoQualitySb = true;
            }
            else if ((hasDoQualityNt == false) && (dp->getName() == "do_quality_nt")) {
                if (dp->getData().getType() == DatapointValue::T_INTEGER) {
                    if (dp->getData().toInt() > 0)
                        doQualityNt = true;
                }
               
                hasDoQualityNt = true;
            }
            else if ((hasDoTs == false) && (dp->getName() == "do_ts")) {
                if (dp->getData().getType() == DatapointValue::T_INTEGER) {
                    doTs = dp->getData().toInt();
                    hasDoTs = true;
                }
            }
            else if ((hasDoTsIv == false) && (dp->getName() == "do_ts_iv")) {
                if (dp->getData().getType() == DatapointValue::T_INTEGER) {
                    if (dp->getData().toInt() > 0)
                        doTsIv = true;
                }
               
                hasDoTsIv = true;
            }
            else if ((hasDoTsSu == false) && (dp->getName() == "do_ts_su")) {
                if (dp->getData().getType() == DatapointValue::T_INTEGER) {
                    if (dp->getData().toInt() > 0)
                        doTsSu = true;
                }
               
                hasDoTsSu = true;
            }
            else if ((hasDoTsSub == false) && (dp->getName() == "do_ts_sub")) {
                if (dp->getData().getType() == DatapointValue::T_INTEGER) {
                    if (dp->getData().toInt() > 0)
                        doTsSub = true;
                }
               
                hasDoTsSub = true;
            }
            else if ((hasDoTest == false) && (dp->getName() == "do_test")) {
                if (dp->getData().getType() == DatapointValue::T_INTEGER) {
                    if (dp->getData().toInt() > 0)
                        doTest = true;
                }
               
                hasDoTest= true;
            }
        }

        //NOTE: when doValue is missing it could be an ACK!

        if (hasDoType && hasDoCot && doValue) {

            if (doType == "M_SP_NA_1" || doType == "M_SP_TB_1") 
            {
                PivotObject pivot("GTIS", "SpsTyp");

                pivot.setIdentifier(exchangeConfig->getPivotId());
                pivot.setCause(doCot);
                
                if (doValue) {
                    bool spsValue = false;

                    if (doValue->getData().getType() == DatapointValue::T_INTEGER) {
                        if (doValue->getData().toInt() > 0) {
                            spsValue = true;
                        }
                    }

                    pivot.setStVal(spsValue);
                }

                pivot.addQuality(doQualityBl, doQualityIv, doQualityNt, doQualityOv, doQualitySb, doTest);

                if (hasDoTs) {
                    pivot.addTimestamp(doTs, doTsIv, doTsSu, doTsSub);
                    pivot.addTmOrg(false);
                }
                else {
                    doTs = (long)getTimeInMs();
                    pivot.addTimestamp(doTs, false, false, true);
                    pivot.addTmOrg(true);
                }
                
                convertedDatapoint = pivot.toDatapoint();
            }
            else if (doType == "M_DP_NA_1" || doType == "M_DP_TB_1") 
            {
                PivotObject pivot("GTIS", "DpsTyp");

                pivot.setIdentifier(exchangeConfig->getPivotId());
                pivot.setCause(doCot);
                
                if (doValue) {
                     
                    if (doValue->getData().getType() == DatapointValue::T_INTEGER) {
                        int dpsValue = doValue->getData().toInt();

                        if (dpsValue == 0) {
                            pivot.setStValStr("intermediate-state");
                        }
                        else if (dpsValue == 1) {
                            pivot.setStValStr("off");
                        }
                        else if (dpsValue == 2) {
                            pivot.setStValStr("on");
                        }
                        else {
                            pivot.setStValStr("bad-state");
                        }
                    }
                }

                pivot.addQuality(doQualityBl, doQualityIv, doQualityNt, doQualityOv, doQualitySb, doTest);

                if (hasDoTs) {
                    pivot.addTimestamp(doTs, doTsIv, doTsSu, doTsSub);
                    pivot.addTmOrg(false);
                }
                else {
                    doTs = (long)getTimeInMs();
                    pivot.addTimestamp(doTs, false, false, true);
                    pivot.addTmOrg(true);
                }

                convertedDatapoint = pivot.toDatapoint();
            }
            else if (doType == "M_ME_NA_1" || doType == "M_ME_TD_1") /* normalized measured value */
            {
                PivotObject pivot("GTIM", "MvTyp");

                pivot.setIdentifier(exchangeConfig->getPivotId());
                pivot.setCause(doCot);

                if (doValue) {
                    if (doValue->getData().getType() == DatapointValue::T_INTEGER) {
                        pivot.setMagI(doValue->getData().toInt());
                    }
                    else if (doValue->getData().getType() == DatapointValue::T_FLOAT) {
                        pivot.setMagF(doValue->getData().toDouble());
                    }
                }

                pivot.addQuality(doQualityBl, doQualityIv, doQualityNt, doQualityOv, doQualitySb, doTest);

                if (hasDoTs) {
                    pivot.addTimestamp(doTs, doTsIv, doTsSu, doTsSub);
                    pivot.addTmOrg(false);
                }
                else {
                    doTs = (long)getTimeInMs();
                    pivot.addTimestamp(doTs, false, false, true);
                    pivot.addTmOrg(true);
                }

                convertedDatapoint = pivot.toDatapoint();
            }
            else if (doType == "M_ME_NB_1" || doType == "M_ME_TE_1") /* scaled measured value */
            {
                PivotObject pivot("GTIM", "MvTyp");

                pivot.setIdentifier(exchangeConfig->getPivotId());
                pivot.setCause(doCot);

                if (doValue) {
                    if (doValue->getData().getType() == DatapointValue::T_INTEGER) {
                        pivot.setMagI(doValue->getData().toInt());
                    }
                    else if (doValue->getData().getType() == DatapointValue::T_FLOAT) {
                        pivot.setMagF(doValue->getData().toDouble());
                    }
                }

                pivot.addQuality(doQualityBl, doQualityIv, doQualityNt, doQualityOv, doQualitySb, doTest);

                if (hasDoTs) {
                    pivot.addTimestamp(doTs, doTsIv, doTsSu, doTsSub);
                    pivot.addTmOrg(false);
                }
                else {
                    doTs = (long)getTimeInMs();
                    pivot.addTimestamp(doTs, false, false, true);
                    pivot.addTmOrg(true);
                }

                convertedDatapoint = pivot.toDatapoint();
            }
            else if (doType == "M_ME_NC_1" || doType == "M_ME_TF_1") /* short (float) measured value */
            {
                PivotObject pivot("GTIM", "MvTyp");

                pivot.setIdentifier(exchangeConfig->getPivotId());
                pivot.setCause(doCot);

                if (doValue) {
                    if (doValue->getData().getType() == DatapointValue::T_INTEGER) {
                        pivot.setMagI(doValue->getData().toInt());
                    }
                    else if (doValue->getData().getType() == DatapointValue::T_FLOAT) {
                        pivot.setMagF(doValue->getData().toDouble());
                    }
                }

                pivot.addQuality(doQualityBl, doQualityIv, doQualityNt, doQualityOv, doQualitySb, doTest);

                if (hasDoTs) {
                    pivot.addTimestamp(doTs, doTsIv, doTsSu, doTsSub);
                    pivot.addTmOrg(false);
                }
                else {
                    doTs = (long)getTimeInMs();
                    pivot.addTimestamp(doTs, false, false, true);
                    pivot.addTmOrg(true);
                }

                convertedDatapoint = pivot.toDatapoint();
            }

        }
        else {
            printf("element is missing\n");
        }
    }
    else {
        printf("datapointvalue has unexpected type: %s(%i)\n", dpv.getTypeStr().c_str(), dpv.getType());
    }

    return convertedDatapoint;
}

Datapoint*
IEC104PivotFilter::convertDatapointToIEC104(Datapoint* sourceDp, IEC104PivotDataPoint* exchangeConfig)
{
    Datapoint* convertedDatapoint = nullptr;

    try {
        PivotObject pivotObject(sourceDp);

        convertedDatapoint = pivotObject.toIec104DataObject(exchangeConfig);
    }
    catch (PivotObjectException& e)
    {
        printf("Failed to convert pivot object: %s\n", e.getContext().c_str());
    }

    return convertedDatapoint;
}

void
IEC104PivotFilter::ingest(READINGSET* readingSet)
{
    /* apply transformation */
    std::vector<Reading*>* readings = readingSet->getAllReadingsPtr();

    std::vector<Reading*>::iterator readIt = readings->begin();

    while(readIt != readings->end())
    {
        Reading* reading = *readIt;

        std::string assetName = reading->getAssetName();

        auto exchangeData = m_config.getExchangeDefinitions();

        if (exchangeData.find(assetName) != exchangeData.end()) {
            Logger::getLogger()->info("Found asset (%s) in exchanged data", assetName.c_str());

            IEC104PivotDataPoint* exchangeConfig = exchangeData[assetName];

            std::vector<Datapoint*>& datapoints = reading->getReadingData();

            std::vector<Datapoint*> convertedDatapoints;

            printf("original Reading: (%s)\n", reading->toJSON().c_str());

            for (Datapoint* dp : datapoints) {
                if (dp->getName() == "data_object") {
                    Datapoint* convertedDp = convertDatapointToPivot(dp, exchangeConfig);

                    if (convertedDp) {
                        convertedDatapoints.push_back(convertedDp);
                    }
                    else {
                        printf("Failed to convert object\n");
                    }
                }
                else if (dp->getName() == "PIVOT") {
                    Datapoint* convertedDp = convertDatapointToIEC104(dp, exchangeConfig);

                    printf("Converted data point to IEC 104\n");

                    if (convertedDp) {
                        printf("  added converted data point\n");
                        convertedDatapoints.push_back(convertedDp);
                    }
                }
                else {
                    printf("ERROR: (%s) not a data_object\n", dp->getName().c_str());
                    Logger::getLogger()->error("(%s) not a data_object", dp->getName().c_str());
                }
            }

            reading->removeAllDatapoints();

            for (Datapoint* convertedDatapoint : convertedDatapoints) {
                reading->addDatapoint(convertedDatapoint);
            }

            printf("converted Reading: (%s)\n", reading->toJSON().c_str());
        }
        else {
            printf("Asset (%s) not found in exchanged data\n", assetName.c_str());
            Logger::getLogger()->error("Asset (%s) not found in exchanged data", assetName.c_str());
        }

        if (reading->getReadingData().size() == 0) {
            printf("Reading has no elements\n");
            readIt = readings->erase(readIt);
        }
        else {
            readIt++;
        }
    }

    if (readings->empty() == false) {

        printf("Send %lu converted readings\n", readings->size());

        Logger::getLogger()->info("Send %lu converted readings", readings->size());

        m_output(m_outHandle, readingSet);
    }
}

void 
IEC104PivotFilter::reconfigure(ConfigCategory* config)
{
    Logger::getLogger()->debug("(re)configure called");

    if (config)
    {
        if (config->itemExists("name")) {
            std::string name = config->getValue("name");
            printf("reconfigure: name = %s\n", name.c_str());
        }
        else
            Logger::getLogger()->error("Missing name in configuration");

        if (config->itemExists("exchanged_data")) {
            const std::string exchangedData = config->getValue("exchanged_data");

            m_config.importExchangeConfig(exchangedData);
        }
        else {
            Logger::getLogger()->error("Missing exchanged_data configuation");
        }
    }
    else {
        Logger::getLogger()->error("No configuration provided");
    }
}
