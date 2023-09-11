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
    if (incomingType == "M_ST_NA_1" || incomingType == "M_ST_TB_1") {
        if (exchangeConfig->getTypeId() == "M_ST_NA_1" || exchangeConfig->getTypeId() == "M_ST_TB_1")
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
    else if (incomingType == "C_SE_NA_1" || incomingType == "C_SE_TA_1") {
        if (exchangeConfig->getTypeId() == "C_SE_NA_1" || exchangeConfig->getTypeId() == "C_SE_TA_1")
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
    else if (incomingType == "C_RC_NA_1" || incomingType == "C_RC_TA_1") {
        if (exchangeConfig->getTypeId() == "C_RC_NA_1" || exchangeConfig->getTypeId() == "C_RC_TA_1")
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
        else if (doType == "M_ST_NA_1" || doType == "M_ST_TB_1")
        {
            PivotDataObject pivot("GTIM", "BscTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(doCot);

            if (doValue) {
                if (doValue->getData().getType() == DatapointValue::T_STRING) {
                    int wtrVal;
                    int transInd;
                    std::string str = doValue->getData().toString();
                    std::string cleaned_str = str.substr(2, str.length() - 4);
                    std::size_t commaPos = cleaned_str.find(',');

                    if(commaPos != std::string::npos) {
                        std::string numStr = cleaned_str.substr(0, commaPos);
                        printf("%s",numStr.c_str());
                        std::string boolStr = cleaned_str.substr(commaPos+1);
                        int wtrVal = std::stoi(numStr);
                        bool transInd = (boolStr == "true");

                        pivot.setPosVal(wtrVal, transInd);
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

        else if (doType == "C_SC_NA_1" || doType == "C_SC_TA_1")
        {
            PivotDataObject pivot("GTIC", "SpcTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(doCot);
            if(hasDoTest)pivot.setTest(doTest);
            if(hasDoNegative) pivot.setConfirmation(doNegative);
            if (doValue) {
                bool spsValue = false;

                if (doValue->getData().getType() == DatapointValue::T_INTEGER) {
                    if (doValue->getData().toInt() > 0) {
                        spsValue = true;
                    }
                }

                pivot.setCtlValBool(spsValue);
            }

            appendTimestampDataObject(pivot, hasDoTs, doTs, doTsIv, doTsSu, doTsSub);
            convertedDatapoint = pivot.toDatapoint();
        }
        else if (doType == "C_DC_NA_1" || doType == "C_DC_TA_1")
        {
            PivotDataObject pivot("GTIC", "DpcTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(doCot);
            if(hasDoTest)pivot.setTest(doTest);
            if(hasDoNegative) pivot.setConfirmation(doNegative);

            if (doValue) {

                if (doValue->getData().getType() == DatapointValue::T_INTEGER) {
                    int dpsValue = doValue->getData().toInt();

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

            appendTimestampDataObject(pivot, hasDoTs, doTs, doTsIv, doTsSu, doTsSub);
            convertedDatapoint = pivot.toDatapoint();
        }

        else if (doType == "C_SE_NA_1" || doType == "C_SE_TA_1" || doType == "C_SE_NC_1" || doType == "C_SE_TC_1")
        {
            PivotDataObject pivot("GTIC", "ApcTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(doCot);
            if(hasDoTest)pivot.setTest(doTest);
            if(hasDoNegative) pivot.setConfirmation(doNegative);

            if (doValue) {
                pivot.setCtlValF(doValue->getData().toDouble());
            }

            appendTimestampDataObject(pivot, hasDoTs, doTs, doTsIv, doTsSu, doTsSub);
            convertedDatapoint = pivot.toDatapoint();
        }
        else if (doType == "C_SE_NB_1" || doType == "C_SE_TB_1")
        {
            PivotDataObject pivot("GTIC", "IncTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(doCot);
            if(hasDoTest)pivot.setTest(doTest);
            if(hasDoNegative) pivot.setConfirmation(doNegative);

            if (doValue) {
                pivot.setCtlValI(doValue->getData().toInt());
            }

            appendTimestampDataObject(pivot, hasDoTs, doTs, doTsIv, doTsSu, doTsSub);
            convertedDatapoint = pivot.toDatapoint();
        }
        else if (doType == "C_RC_NA_1" || doType == "C_RC_TA_1")
        {
         PivotDataObject pivot("GTIC", "BscTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(doCot);
            if(hasDoTest)pivot.setTest(doTest);
            if(hasDoNegative) pivot.setConfirmation(doNegative);

            if (doValue) {
                int ctlValue = doValue->getData().toInt();

                    switch(ctlValue){
                        case 0:
                            pivot.setCtlValStr("stop");
                            break;
                        case 1:
                            pivot.setCtlValStr("lower");
                            break;
                        case 2:
                            pivot.setCtlValStr("higher");
                            break;
                        case 3:
                            pivot.setCtlValStr("reserved");
                            break;
                        default:
                            Logger::getLogger()->warn("Invalid step dommand value: %s", exchangeConfig->getPivotId());
                            break;
                    }
            }

            appendTimestampDataObject(pivot, hasDoTs, doTs, doTsIv, doTsSu, doTsSub);
            convertedDatapoint = pivot.toDatapoint();
        }

    }

    return convertedDatapoint;
}

bool
is_int(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

Datapoint*
IEC104PivotFilter::convertOperationObjectToPivot(std::vector<Datapoint*> datapoints)
{
    Datapoint* convertedDatapoint = nullptr;

    bool hasCoType = false;
    bool hasCoCot = false;

    string coType = "";
    int coCot = 0;

    bool hasCoTs = false;

    bool hasSe = false;
    int coSe = 0;

    long coTs = 0;

    bool hasCoTest = false;
    bool coTest = false;

    bool hasComingFrom = false;
    bool comingFromIec104 = true;

    string coIoa = "";
    string coCa = "";

    Datapoint* coValue = nullptr;

    for(Datapoint* dp : datapoints){
        if ((dp->getName() == "co_ioa")) {
            if (dp->getData().getType() == DatapointValue::T_STRING) {
                coIoa =  dp->getData().toStringValue();
            }
        }
        else if ((dp->getName() == "co_ca")) {
            if (dp->getData().getType() == DatapointValue::T_STRING) {
                coCa =  dp->getData().toStringValue();
            }
        }
    }

    IEC104PivotDataPoint* exchangeConfig = m_config.getExchangeDefinitionsByAddress(coCa+"-"+coIoa);

    if(!exchangeConfig){
        Logger::getLogger()->error("CA and IOA not in exchange data");
        return convertedDatapoint;
    }

    for (Datapoint* dp : datapoints)
    {
        if ((hasCoType == false) && (dp->getName() == "co_type")) {
            if (dp->getData().getType() == DatapointValue::T_STRING) {
                coType = dp->getData().toStringValue();

                if (checkTypeMatch(coType, exchangeConfig)) {
                    hasCoType = true;
                }
                else {
                    Logger::getLogger()->warn("Input type does not match configured type for %s-%s ", to_string(exchangeConfig->getCA()).c_str(),to_string(exchangeConfig->getIOA()).c_str());
                }
            }
        }
        else if ((hasCoCot == false) && (dp->getName() == "co_cot")) {
            if (dp->getData().getType() == DatapointValue::T_STRING && is_int(dp->getData().toStringValue())) {
                int value = stoi(dp->getData().toStringValue());

                if(value >= 0 && value <=63){
                    coCot =  value;
                    hasCoCot = true;
                }
                else{
                    Logger::getLogger()->error("COT value out of range for %s-%s", to_string(exchangeConfig->getCA()).c_str(),to_string(exchangeConfig->getIOA()).c_str());
                }
            }
            else {
                Logger::getLogger()->error("COT data type invalid for %s-%s ", to_string(exchangeConfig->getCA()).c_str(),to_string(exchangeConfig->getIOA()).c_str());
                printf("COT data type invalid for %s-%s ", to_string(exchangeConfig->getCA()).c_str(),to_string(exchangeConfig->getIOA()).c_str());
            }
        }
        else if ((hasSe == false) && (dp->getName() == "co_se")) {
            if (dp->getData().getType() == DatapointValue::T_STRING) {
                string coSeStr =  dp->getData().toStringValue();

                if(!coSeStr.empty()){
                    hasSe = true;
                    coSe = stoi(coSeStr);
                }
            }
            else {
                Logger::getLogger()->warn("Select input data type invalid for %s-%s , defaulting to execute", to_string(exchangeConfig->getCA()).c_str(),to_string(exchangeConfig->getIOA()).c_str());
            }
        }

        else if ((coValue == nullptr) && (dp->getName() == "co_value"))
        {
            coValue = dp;
        }

        else if ((hasCoTs == false) && (dp->getName() == "co_ts")) {
            if (dp->getData().getType() == DatapointValue::T_STRING && is_int(dp->getData().toStringValue())){
                hasCoTs = true;
                coTs =  stol(dp->getData().toStringValue());
            }
            else {
                if(dp->getData().toStringValue()!= "")
                    Logger::getLogger()->error("Timestamp input data type invalid for %s-%s ", to_string(exchangeConfig->getCA()).c_str(),to_string(exchangeConfig->getIOA()).c_str());
            }
        }

        else if ((hasCoTest == false) && (dp->getName() == "co_test")) {
            if (dp->getData().getType() == DatapointValue::T_STRING && is_int(dp->getData().toStringValue())){
                int value = stoi(dp->getData().toStringValue());

                if (value == 1){
                    coTest = true;
                    hasCoTest = true;
                }
                else if(value != 0 && value != 1){
                    Logger::getLogger()->warn("Test value out of range for %s-%s , defaulting to false", to_string(exchangeConfig->getCA()).c_str(),to_string(exchangeConfig->getIOA()).c_str());
                }
            }
            else {
                Logger::getLogger()->warn("Test value input data type invalid for %s-%s , defaulting to false", to_string(exchangeConfig->getCA()).c_str(),to_string(exchangeConfig->getIOA()).c_str());
            }
        }

        else if ((hasComingFrom == false) && (dp->getName() == "co_comingfrom")) {
            if (dp->getData().getType() == DatapointValue::T_STRING) {
                string comingFromValue = dp->getData().toStringValue();

                if (comingFromValue != "iec104") {
                    comingFromIec104 = false;
                }
            }
        }
    }

    if(!hasCoTs && (coType[5] == 'T'))
    {
        Logger::getLogger()->error("Command has ASDU type with timestamp, but no timestamp was received -> ignore");
        return convertedDatapoint;
    }

    if (comingFromIec104 == false) {
        Logger::getLogger()->warn("data_object for %s is not from IEC 104 plugin -> ignore", exchangeConfig->getLabel().c_str());
    }

    if (comingFromIec104 && hasCoType && hasCoCot) {

        if (coType == "C_SC_NA_1" || coType == "C_SC_TA_1")
        {
            PivotOperationObject pivot("GTIC", "SpcTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(coCot);
            if(hasSe)pivot.setSelect(coSe);
            if(hasCoTest)pivot.setTest(coTest);

            if (coValue) {
                bool spsValue = false;

                if (coValue->getData().getType() == DatapointValue::T_STRING) {
                    if (stoi(coValue->getData().toStringValue()) > 0) {
                        spsValue = true;
                    }
                }

                pivot.setCtlValBool(spsValue);
            }

            appendTimestampOperationObject(pivot, hasCoTs, coTs);

            convertedDatapoint = pivot.toDatapoint();
        }
        else if (coType == "C_DC_NA_1" || coType == "C_DC_TA_1")
        {
            PivotOperationObject pivot("GTIC", "DpcTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(coCot);
            if(hasSe)pivot.setSelect(coSe);
            if(hasCoTest)pivot.setTest(coTest);

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

            appendTimestampOperationObject(pivot, hasCoTs, coTs);

            convertedDatapoint = pivot.toDatapoint();
        }
        else if (coType == "C_SE_NB_1" || coType == "C_SE_TB_1")
        {
            PivotOperationObject pivot("GTIC", "IncTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(coCot);
            if(hasSe)pivot.setSelect(coSe);
            if(hasCoTest)pivot.setTest(coTest);

            if (coValue) {
                pivot.setCtlValI(stoi(coValue->getData().toStringValue()));
            }

            appendTimestampOperationObject(pivot, hasCoTs, coTs);

            convertedDatapoint = pivot.toDatapoint();
        }
        else if (coType == "C_SE_NA_1" || coType == "C_SE_TA_1" || coType == "C_SE_NC_1" || coType == "C_SE_TC_1")
        {
            PivotOperationObject pivot("GTIC", "ApcTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(coCot);
            if(hasSe)pivot.setSelect(coSe);
            if(hasCoTest)pivot.setTest(coTest);

            if (coValue) {
                pivot.setCtlValF(stof(coValue->getData().toStringValue()));
            }

            appendTimestampOperationObject(pivot, hasCoTs, coTs);

            convertedDatapoint = pivot.toDatapoint();
        }
        else if (coType == "C_RC_NA_1" || coType == "C_RC_TA_1")
        {
            PivotOperationObject pivot("GTIC", "BscTyp");

            pivot.setIdentifier(exchangeConfig->getPivotId());
            pivot.setCause(coCot);
            if(hasSe)pivot.setSelect(coSe);
            if(hasCoTest)pivot.setTest(coTest);

            if (coValue) {
                int ctlValue = stoi(coValue->getData().toStringValue());

                    switch(ctlValue){
                        case 0:
                            pivot.setCtlValStr("stop");
                            break;
                        case 1:
                            pivot.setCtlValStr("lower");
                            break;
                        case 2:
                            pivot.setCtlValStr("higher");
                            break;
                        case 3:
                            pivot.setCtlValStr("reserved");
                            break;
                        default:
                            Logger::getLogger()->warn("Invalid step command value: %s", exchangeConfig->getPivotId());
                            break;
                    }
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
IEC104PivotFilter::convertReadingToIEC104OperationObject(Datapoint* sourceDp)
{
    std::vector<Datapoint*> convertedDatapoints;

    try {
        PivotOperationObject pivotOperationObject(sourceDp);
        IEC104PivotDataPoint* exchangeConfig = m_config.getExchangeDefinitionsByPivotId(pivotOperationObject.getIdentifier());

        if(!exchangeConfig){
            Logger::getLogger()->error("Pivot ID not in exchangedData");
        }
        else{
            convertedDatapoints = pivotOperationObject.toIec104OperationObject(exchangeConfig);
        }
    }
    catch (PivotObjectException& e)
    {
        Logger::getLogger()->error("Failed to convert pivot operation object: %s", e.getContext().c_str());
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


        std::vector<Datapoint*>& datapoints = reading->getReadingData();

        std::vector<Datapoint*> convertedDatapoints;


        // printf("original Reading: (%s)\n", reading->toJSON().c_str());
        Logger::getLogger()->debug("original Reading: (%s)", reading->toJSON().c_str());

        if(assetName == "IEC104Command"){
            Datapoint* convertedOperation = convertOperationObjectToPivot(datapoints);

            if (!convertedOperation) {
                Logger::getLogger()->error("Failed to convert IEC command object");
            }
            else{
                convertedDatapoints.push_back(convertedOperation);
            }

            reading->setAssetName("PivotCommand");
        }

        else if(assetName == "PivotCommand"){
            std::vector<Datapoint*> convertedReadingDatapoints = convertReadingToIEC104OperationObject(datapoints[0]);

            if (convertedReadingDatapoints.empty()) {
                Logger::getLogger()->error("Failed to convert Pivot operation object");
            }

            for(Datapoint* dp : convertedReadingDatapoints)
                convertedDatapoints.push_back(dp);

            reading->setAssetName("IEC104Command");
        }

        else{
            IEC104PivotDataPoint* exchangeConfig = m_config.getExchangeDefinitionsByLabel(assetName);

            if(exchangeConfig){
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
            else{
                Logger::getLogger()->error("Asset (%s) not found", assetName.c_str());
            }
        }

        reading->removeAllDatapoints();

        for (Datapoint* convertedDatapoint : convertedDatapoints) {
            reading->addDatapoint(convertedDatapoint);
        }

        //printf("converted Reading: (%s)\n", reading->toJSON().c_str());
        Logger::getLogger()->debug("converted Reading: (%s)", reading->toJSON().c_str());


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