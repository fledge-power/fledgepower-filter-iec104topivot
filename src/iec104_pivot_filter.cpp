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
    else if (incomingType == "C_SC_NA_1" || incomingType == "C_SC_TA_1") { 
        if (exchangeConfig->getTypeId() == "C_SC_NA_1" || exchangeConfig->getTypeId() == "C_SC_TA_1")
            return true;
    }
    else if (incomingType == "C_DC_NA_1" || incomingType == "C_DC_TA_1") {
        if (exchangeConfig->getTypeId() == "C_DC_NA_1" || exchangeConfig->getTypeId() == "C_DC_TA_1")
            return true;
    }
    else if (incomingType == "C_SE_NB_1" || incomingType == "C_SE_TB_1") { 
        if (exchangeConfig->getTypeId() == "C_SE_NB_1" || exchangeConfig->getTypeId() == "C_SE_TB_1")
            return true;
    }
    else if (incomingType == "C_SE_NC_1" || incomingType == "C_SE_TC_1") { 
        if (exchangeConfig->getTypeId() == "C_SE_NC_1" || exchangeConfig->getTypeId() == "C_SE_TC_1")
            return true;
    }

    return false;
}

static void
appendTimestampDataObject(PivotDataObject& pivot, bool hasDoTs, long doTs, bool doTsIv, bool doTsSu, bool doTsSub)
{
    if (hasDoTs) {
        pivot.addTimestamp(doTs, doTsIv, doTsSu, doTsSub);
        pivot.addTmOrg(doTsSub);

        if (doTsIv) {
            pivot.addTmValidity(true);
        }
    }
    else {
        doTs = (long)PivotTimestamp::GetCurrentTimeInMs();
        pivot.addTimestamp(doTs, false, false, true);
        pivot.addTmOrg(true);
    }
}

static void
appendTimestampOperationObject(PivotOperationObject& pivot, bool hasCoTs, long coTs)
{
    if (hasCoTs) {
        pivot.addTimestamp(coTs);
    }
    else {
        coTs = (long)PivotTimestamp::GetCurrentTimeInMs();
        pivot.addTimestamp(coTs);
    }
}

Datapoint*
IEC104PivotFilter::convertDataObjectToPivot(Datapoint* sourceDp, IEC104PivotDataPoint* exchangeConfig)
{
    Datapoint* convertedDatapoint = nullptr;

    DatapointValue& dpv = sourceDp->getData();

    if (dpv.getType() != DatapointValue::T_DP_DICT)
        return nullptr;

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
    bool comingFromIec104 = true;

    bool hasComingFrom = false;

    bool hasDoNegative = false;
    bool doNegative = false;

    Datapoint* doValue = nullptr;

    for (Datapoint* dp : *datapoints)
    {
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
        else if ((hasComingFrom == false) && (dp->getName() == "do_comingfrom")) {
            if (dp->getData().getType() == DatapointValue::T_STRING) {
                string comingFromValue = dp->getData().toStringValue();

                if (comingFromValue != "iec104") {
                    comingFromIec104 = false;
                }
            }
        }
        else if ((hasDoNegative == false) && (dp->getName() == "do_negative")) {
            if ((dp->getData().getType() == DatapointValue::T_INTEGER) && (dp->getData().toInt() > 0)) {
                doNegative = true;
            }

            hasDoNegative = true;
        }
    }

    if (comingFromIec104 == false) {
        Logger::getLogger()->warn("data_object for %s is not from IEC 104 plugin -> ignore", exchangeConfig->getLabel().c_str());
    }

    //NOTE: when doValue is missing it could be an ACK!

    if (comingFromIec104 && hasDoType && hasDoCot) {

        if (doType == "M_SP_NA_1" || doType == "M_SP_TB_1") 
        {
            PivotDataObject pivot("GTIS", "SpsTyp");

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
            else {
                pivot.setConfirmation(doNegative);
            }

            pivot.addQuality(doQualityBl, doQualityIv, doQualityNt, doQualityOv, doQualitySb, doTest);

            appendTimestampDataObject(pivot, hasDoTs, doTs, doTsIv, doTsSu, doTsSub);
            
            convertedDatapoint = pivot.toDatapoint();
        }
        else if (doType == "M_DP_NA_1" || doType == "M_DP_TB_1") 
        {
            PivotDataObject pivot("GTIS", "DpsTyp");

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
            else {
                pivot.setConfirmation(doNegative);
            }

            pivot.addQuality(doQualityBl, doQualityIv, doQualityNt, doQualityOv, doQualitySb, doTest);

            appendTimestampDataObject(pivot, hasDoTs, doTs, doTsIv, doTsSu, doTsSub);

            convertedDatapoint = pivot.toDatapoint();
        }
        else if (doType == "M_ME_NA_1" || doType == "M_ME_TD_1") /* normalized measured value */
        {
            PivotDataObject pivot("GTIM", "MvTyp");

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
            else {
                pivot.setConfirmation(doNegative);
            }

            pivot.addQuality(doQualityBl, doQualityIv, doQualityNt, doQualityOv, doQualitySb, doTest);

            appendTimestampDataObject(pivot, hasDoTs, doTs, doTsIv, doTsSu, doTsSub);

            convertedDatapoint = pivot.toDatapoint();
        }
        else if (doType == "M_ME_NB_1" || doType == "M_ME_TE_1") /* scaled measured value */
        {
            PivotDataObject pivot("GTIM", "MvTyp");

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
            else {
                pivot.setConfirmation(doNegative);
            }

            pivot.addQuality(doQualityBl, doQualityIv, doQualityNt, doQualityOv, doQualitySb, doTest);

            appendTimestampDataObject(pivot, hasDoTs, doTs, doTsIv, doTsSu, doTsSub);

            convertedDatapoint = pivot.toDatapoint();
        }
        else if (doType == "M_ME_NC_1" || doType == "M_ME_TF_1") /* short (float) measured value */
        {
            PivotDataObject pivot("GTIM", "MvTyp");

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
            else {
                pivot.setConfirmation(doNegative);
            }

            pivot.addQuality(doQualityBl, doQualityIv, doQualityNt, doQualityOv, doQualitySb, doTest);

            appendTimestampDataObject(pivot, hasDoTs, doTs, doTsIv, doTsSu, doTsSub);

            convertedDatapoint = pivot.toDatapoint();
        }

    }

    return convertedDatapoint;
}


Datapoint*
IEC104PivotFilter::convertOperationObjectToPivot(std::vector<Datapoint*> datapoints, IEC104PivotDataPoint* exchangeConfig)
{
    Datapoint* convertedDatapoint = nullptr;


    bool hasCoType = false;
    bool hasCoCot = false;

    string coType = "";
    int coCot = 0;


    bool hasCoTs = false;

    bool hasSe = false;
    string coSe = "";

    long coTs = 0;

    bool hasCoTest = false;
    bool coTest = false;
    bool comingFromIec104 = true;

    bool hasComingFrom = false;

    bool hasCoNegative = false;
    bool coNegative = false;

    Datapoint* coValue = nullptr;

    for (Datapoint* dp : datapoints)
    {
        if ((hasCoType == false) && (dp->getName() == "co_type")) {
            if (dp->getData().getType() == DatapointValue::T_STRING) {
                coType = dp->getData().toStringValue();

                if (checkTypeMatch(coType, exchangeConfig)) {
                    hasCoType = true;
                }
                else {
                    Logger::getLogger()->warn("Input type coes not match configured type for %s", exchangeConfig->getLabel().c_str());
                }
            }
        }
        else if ((hasCoCot == false) && (dp->getName() == "co_cot")) {
            if (dp->getData().getType() == DatapointValue::T_STRING) {
                coCot =  stoi(dp->getData().toStringValue());
                hasCoCot = true;
            }
        }
        else if ((hasSe == false) && (dp->getName() == "co_se")) {
            if (dp->getData().getType() == DatapointValue::T_STRING) {
                coSe =  dp->getData().toStringValue();
                if(coSe!="dct-ctl-wes")hasSe = true;
            }
        }

        else if ((coValue == nullptr) && (dp->getName() == "co_value"))
        {
            coValue = dp;
        }

        else if ((hasCoTs == false) && (dp->getName() == "co_ts")) {
            if (dp->getData().getType() == DatapointValue::T_STRING) {
                coTs =  stol(dp->getData().toStringValue());
                hasCoTs = true;
            }
        }

        else if ((hasCoTest == false) && (dp->getName() == "co_test")) {
            if (dp->getData().getType() == DatapointValue::T_STRING) {
                int test =  stoi(dp->getData().toStringValue());
                if (test > 0){
                    coTest = true;
                    hasCoTest= true;
                }
            }
        }

        if(!hasCoTs && (coType == "C_SC_TA_1" || coType == "C_DC_TA_1" || coType == "C_SE_TB_1" || coType == "C_SE_TC_1"))
        {
              Logger::getLogger()->error("Command has ASDU type with timestamp, but no timestamp was received -> ignore", exchangeConfig->getLabel().c_str());
              return convertedDatapoint;
        }

        else if ((hasComingFrom == false) && (dp->getName() == "co_comingfrom")) {
            if (dp->getData().getType() == DatapointValue::T_STRING) {
                string comingFromValue = dp->getData().toStringValue();

                if (comingFromValue != "iec104") {
                    comingFromIec104 = false;
                }
            }
        }

        else if ((hasCoNegative == false) && (dp->getName() == "co_negative")) {
            if ((dp->getData().getType() == DatapointValue::T_STRING) && (stoi(dp->getData().toStringValue()) > 0)) {
                coNegative = true;
                hasCoNegative = true;
            }
        }
    }

    if (comingFromIec104 == false) {
        Logger::getLogger()->warn("data_object for %s is not from IEC 104 plugin -> ignore", exchangeConfig->getLabel().c_str());
    }

    //NOTE: when coValue is missing it could be an ACK!
    if (comingFromIec104 && hasCoType && hasCoCot) {

        if (coType == "C_SC_NA_1" || coType == "C_SC_TA_1")
        {
            PivotOperationObject pivot("GTIC", "SpcTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(coCot);
            if(hasSe)pivot.setBeh(coSe);
            if(hasCoTest)pivot.setTest(coTest);
            if(hasCoNegative)pivot.setConfirmation(coNegative);

            if (coValue) {
                bool spsValue = false;

                if (coValue->getData().getType() == DatapointValue::T_STRING) {
                    if (stoi(coValue->getData().toStringValue()) > 0) {
                        spsValue = true;
                    }
                }

                pivot.setCtlValBool(spsValue);
            }
            else {
                pivot.setConfirmation(coNegative);
            }

            appendTimestampOperationObject(pivot, hasCoTs, coTs);

            convertedDatapoint = pivot.toDatapoint();
        }
        else if (coType == "C_DC_NA_1" || coType == "C_DC_TA_1")
        {
            PivotOperationObject pivot("GTIC", "DpcTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(coCot);
            if(hasSe)pivot.setBeh(coSe);
            if(hasCoTest)pivot.setTest(coTest);
            if(hasCoNegative)pivot.setConfirmation(coNegative);

            if (coValue) {
                 
                if (coValue->getData().getType() == DatapointValue::T_STRING) {
                    int dpsValue = stoi(coValue->getData().toStringValue());

                    if (dpsValue == 0) {
                        pivot.setCtlValStr("intermediate-state");
                    }
                    else if (dpsValue == 1) {
                        pivot.setCtlValStr("off");
                    }
                    else if (dpsValue == 2) {
                        pivot.setCtlValStr("on");
                    }
                    else {
                        pivot.setCtlValStr("bad-state");
                    }
                }
            }
            else {
                pivot.setConfirmation(coNegative);
            }

            appendTimestampOperationObject(pivot, hasCoTs, coTs);

            convertedDatapoint = pivot.toDatapoint();
        }
        else if (coType == "C_SE_NB_1" || coType == "C_SE_TB_1")
        {
            PivotOperationObject pivot("GTIC", "IncTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(coCot);
            if(hasSe)pivot.setBeh(coSe);
            if(hasCoTest)pivot.setTest(coTest);
            if(hasCoNegative)pivot.setConfirmation(coNegative);

            if (coValue) {
                pivot.setCtlValI(stoi(coValue->getData().toStringValue()));
            }
            else {
                pivot.setConfirmation(coNegative);
            }

            appendTimestampOperationObject(pivot, hasCoTs, coTs);

            convertedDatapoint = pivot.toDatapoint();
        }
        else if (coType == "C_SE_NC_1" || coType == "C_SE_TC_1")
        {
            PivotOperationObject pivot("GTIC", "ApcTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(coCot);
            if(hasSe)pivot.setBeh(coSe);
            if(hasCoTest)pivot.setTest(coTest);
            if(hasCoNegative)pivot.setConfirmation(coNegative);

            if (coValue) {
                pivot.setCtlValF(stof(coValue->getData().toStringValue()));
            }
            else {
                pivot.setConfirmation(coNegative);
            }

            appendTimestampOperationObject(pivot, hasCoTs, coTs);

            convertedDatapoint = pivot.toDatapoint();
        }

    }

    return convertedDatapoint;
}

Datapoint*
IEC104PivotFilter::convertDatapointToIEC104DataObject(Datapoint* sourceDp, IEC104PivotDataPoint* exchangeConfig)
{
    Datapoint* convertedDatapoint = nullptr;

    try {
        PivotDataObject pivotObject(sourceDp);

        convertedDatapoint = pivotObject.toIec104DataObject(exchangeConfig);
    }
    catch (PivotObjectException& e)
    {
        Logger::getLogger()->error("Failed to convert pivot object: %s", e.getContext().c_str());
    }

    return convertedDatapoint;
}

std::vector<Datapoint*>
IEC104PivotFilter::convertReadingToIEC104OperationObject(Datapoint* sourceDp, IEC104PivotDataPoint* exchangeConfig)
{
    std::vector<Datapoint*> convertedDatapoints;

    try {
        PivotOperationObject pivotOperationObject(sourceDp);
        convertedDatapoints = pivotOperationObject.toIec104OperationObject(exchangeConfig);
    }
    catch (PivotObjectException& e)
    {
        Logger::getLogger()->error("Failed to convert pivot object: %s", e.getContext().c_str());
    }

    return convertedDatapoints;
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

        if (exchangeData.find(assetName) != exchangeData.end())
        {
            IEC104PivotDataPoint* exchangeConfig = exchangeData[assetName];

            std::vector<Datapoint*>& datapoints = reading->getReadingData();

            std::vector<Datapoint*> convertedDatapoints;


            printf("original Reading: (%s)\n", reading->toJSON().c_str());
            //Logger::getLogger()->info("original Reading: (%s)", reading->toJSON().c_str());

            if(assetName == "IEC104Command"){
                Datapoint* convertedOperation = convertOperationObjectToPivot(datapoints,exchangeConfig);

                if (!convertedOperation) {
                    Logger::getLogger()->error("Failed to convert object");
                }
                else{
                    convertedDatapoints.push_back(convertedOperation);
                }

                reading->setAssetName("PivotCommand");
            }

            else if(assetName == "PivotCommand"){
                std::vector<Datapoint*> convertedReadingDatapoints = convertReadingToIEC104OperationObject(datapoints[0],exchangeConfig);

                if (convertedReadingDatapoints.empty()) {
                    Logger::getLogger()->error("Failed to convert object");
                }

                for(Datapoint* dp : convertedReadingDatapoints)
                    convertedDatapoints.push_back(dp);

                reading->setAssetName("IEC104Command");
            }

            else{
                for (Datapoint* dp : datapoints) {
                    printf("DATAPOINT: (%s)\n", dp->toJSONProperty().c_str());
                    if (dp->getName() == "data_object") {
                        Datapoint* convertedDp = convertDataObjectToPivot(dp, exchangeConfig);

                        if (convertedDp) {
                            convertedDatapoints.push_back(convertedDp);
                        }
                        else {
                          Logger::getLogger()->error("Failed to convert object");
                        }
                    }
                    
                    else if (dp->getName() == "PIVOT") {
                        Datapoint* convertedDp = convertDatapointToIEC104DataObject(dp, exchangeConfig);

                        if (convertedDp) {
                            convertedDatapoints.push_back(convertedDp);
                        }
                    }
                    else {
                        Logger::getLogger()->error("(%s) not a data_object", dp->getName().c_str());
                    }
                }
            }
            reading->removeAllDatapoints();

            for (Datapoint* convertedDatapoint : convertedDatapoints) {
                reading->addDatapoint(convertedDatapoint);
            }

            //printf("converted Reading: (%s)\n", reading->toJSON().c_str());
            //Logger::getLogger()->info("converted Reading: (%s)", reading->toJSON().c_str());
        }
        else {
            printf("Asset (%s) not found in exchanged data\n", assetName.c_str());
            Logger::getLogger()->error("Asset (%s) not found in exchanged data", assetName.c_str());
        }

        if (reading->getReadingData().size() == 0) {
            readIt = readings->erase(readIt);
        }
        else {
            readIt++;
        }
    }

    if (readings->empty() == false)
    {
        Logger::getLogger()->debug("Send %lu converted readings", readings->size());

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
