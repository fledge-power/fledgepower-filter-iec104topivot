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

#include <config_category.h>

#include "iec104_pivot_filter.hpp"
#include "iec104_pivot_object.hpp"
#include "iec104_pivot_utility.hpp"

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

static bool checkValueRange(const std::string& beforeLog, int value, int min, int max, const std::string& type)
{
    if (value < min || value > max) {
        Iec104PivotUtility::log_warn("%s do_value out of range [%d..%d] for %s: %d", beforeLog.c_str(), min, max, type.c_str(), value);
        return false;
    }
    return true;
}

static bool checkValueRange(const std::string& beforeLog, long value, long min, long max, const std::string& type)
{
    if (value < min || value > max) {
        Iec104PivotUtility::log_warn("%s do_value out of range [%ld..%ld] for %s: %ld", beforeLog.c_str(), min, max, type.c_str(), value);
        return false;
    }
    return true;
}

static bool checkValueRange(const std::string& beforeLog, double value, double min, double max, const std::string& type)
{
    if (value < min || value > max) {
        Iec104PivotUtility::log_warn("%s do_value out of range [%x..%x] for %s: %x", beforeLog.c_str(), min, max, type.c_str(), value);
        return false;
    }
    return true;
}

static void
appendTimestampDataObject(PivotDataObject& pivot, bool hasDoTs, long doTs, bool doTsIv, bool doTsSu, bool doTsSub)
{
    if (hasDoTs) {
        pivot.addTimestamp(doTs, doTsIv, doTsSu, doTsSub);
        pivot.addTmOrg(doTsSub);
        pivot.addTmValidity(doTsIv);
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

template <typename T>
void IEC104PivotFilter::readAttribute(std::map<std::string, bool>& attributeFound, Datapoint* dp,
                                          const std::string& targetName, T& out) {
    const auto& name = dp->getName();
    if (name != targetName) {
        return;
    }
    if (attributeFound[name]) {
        return;
    }

    if (dp->getData().getType() == DatapointValue::T_INTEGER) {
        out = static_cast<T>(dp->getData().toInt());
        attributeFound[name] = true;
    }
}

void IEC104PivotFilter::readAttribute(std::map<std::string, bool>& attributeFound, Datapoint* dp,
                                               const std::string& targetName, Datapoint*& out) {
    const auto& name = dp->getName();
    if (name != targetName) {
        return;
    }
    if (attributeFound[name]) {
        return;
    }

    out = dp;
    attributeFound[name] = true;
}

void IEC104PivotFilter::readAttribute(std::map<std::string, bool>& attributeFound, Datapoint* dp,
                                                const std::string& targetName, std::string& out) {
    const auto& name = dp->getName();
    if (name != targetName) {
        return;
    }
    if (attributeFound[name]) {
        return;
    }

    if (dp->getData().getType() == DatapointValue::T_STRING) {
        out = dp->getData().toStringValue();
        attributeFound[name] = true;
    }
}

Datapoint*
IEC104PivotFilter::convertDataObjectToPivot(Datapoint* sourceDp, IEC104PivotDataPoint* exchangeConfig)
{
    std::string beforeLog = Iec104PivotUtility::PluginName + " - IEC104PivotFilter::convertDataObjectToPivot -";
    Datapoint* convertedDatapoint = nullptr;

    DatapointValue& dpv = sourceDp->getData();

    if (dpv.getType() != DatapointValue::T_DP_DICT)
        return nullptr;

    std::vector<Datapoint*>* datapoints = dpv.getDpVec();
    std::map<std::string, bool> attributeFound = {
        {"do_type", false},
        {"do_cot", false},
        {"do_value", false},
        {"do_quality_iv", false},
        {"do_quality_bl", false},
        {"do_quality_ov", false},
        {"do_quality_sb", false},
        {"do_quality_nt", false},
        {"do_ts", false},
        {"do_ts_iv", false},
        {"do_ts_su", false},
        {"do_ts_sub", false},
        {"do_test", false},
        {"do_comingfrom", false},
        {"do_negative", false},
    };

    Iec104DataObject dataObject;

    for (Datapoint* dp : *datapoints)
    {
        readAttribute(attributeFound, dp, "do_type", dataObject.doType);
        readAttribute(attributeFound, dp, "do_cot", dataObject.doCot);
        readAttribute(attributeFound, dp, "do_value", dataObject.doValue);
        readAttribute(attributeFound, dp, "do_quality_iv", dataObject.doQualityIv);
        readAttribute(attributeFound, dp, "do_quality_bl", dataObject.doQualityBl);
        readAttribute(attributeFound, dp, "do_quality_ov", dataObject.doQualityOv);
        readAttribute(attributeFound, dp, "do_quality_sb", dataObject.doQualitySb);
        readAttribute(attributeFound, dp, "do_quality_nt", dataObject.doQualityNt);
        readAttribute(attributeFound, dp, "do_ts", dataObject.doTs);
        readAttribute(attributeFound, dp, "do_ts_iv", dataObject.doTsIv);
        readAttribute(attributeFound, dp, "do_ts_su", dataObject.doTsSu);
        readAttribute(attributeFound, dp, "do_ts_sub", dataObject.doTsSub);
        readAttribute(attributeFound, dp, "do_test", dataObject.doTest);
        readAttribute(attributeFound, dp, "do_comingfrom", dataObject.comingFromValue);
        readAttribute(attributeFound, dp, "do_negative", dataObject.doNegative);
    }

    if (!attributeFound["do_type"]) {
        Iec104PivotUtility::log_error("%s Missing do_type", beforeLog.c_str());
        return nullptr;
    }
    if (!attributeFound["do_cot"]) {
        Iec104PivotUtility::log_error("%s Missing do_cot", beforeLog.c_str());
        return nullptr;
    }
    if (dataObject.comingFromValue != "iec104") {
        Iec104PivotUtility::log_warn("%s data_object for %s is not from IEC 104 plugin -> ignore", beforeLog.c_str(),
                                    exchangeConfig->getLabel().c_str());
        return nullptr;
    }
    if (!checkTypeMatch(dataObject.doType, exchangeConfig)) {
        Iec104PivotUtility::log_warn("%s Input type (%s) does not match configured type (%s) for label %s", beforeLog.c_str(),
                                    dataObject.doType.c_str(), exchangeConfig->getTypeId().c_str(), exchangeConfig->getLabel().c_str());
        return nullptr;
    }

    if(!attributeFound["do_ts"] && hasASDUTimestamp(dataObject.doType)) {
        Iec104PivotUtility::log_warn("%s Data object has ASDU type with timestamp (%s), but no timestamp was received",
                                    beforeLog.c_str(), dataObject.doType.c_str());
    }

    //NOTE: when doValue is missing it could be an ACK!

    if (dataObject.doType == "M_SP_NA_1" || dataObject.doType == "M_SP_TB_1")
    {
        // Message structure checks
        if (!attributeFound["do_value"]) {
            if (!attributeFound["do_negative"]) {
                Iec104PivotUtility::log_warn("%s Missing attribute do_negative in SP ACK", beforeLog.c_str());
            }
        }
        
        // Pivot conversion
        PivotDataObject pivot("GTIS", "SpsTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(dataObject.doCot);

        if (attributeFound["do_value"]) {
            bool spsValue = false;
            if (dataObject.doValue->getData().getType() == DatapointValue::T_INTEGER) {
                int value = static_cast<int>(dataObject.doValue->getData().toInt());
                checkValueRange(beforeLog, value, 0, 1, "SP");
                spsValue = (value > 0);
            }

            pivot.setStVal(spsValue);
        }
        else {
            pivot.setConfirmation(dataObject.doNegative);
        }

        pivot.addQuality(dataObject.doQualityBl, dataObject.doQualityIv, dataObject.doQualityNt,
                        dataObject.doQualityOv, dataObject.doQualitySb, dataObject.doTest);

        appendTimestampDataObject(pivot, attributeFound["do_ts"], dataObject.doTs, dataObject.doTsIv, dataObject.doTsSu, dataObject.doTsSub);

        convertedDatapoint = pivot.toDatapoint();
    }
    else if (dataObject.doType == "M_DP_NA_1" || dataObject.doType == "M_DP_TB_1")
    {
        // Message structure checks
        if (!attributeFound["do_value"]) {
            if (!attributeFound["do_negative"]) {
                Iec104PivotUtility::log_warn("%s Missing attribute do_negative in DP ACK", beforeLog.c_str());
            }
        }
        
        // Pivot conversion
        PivotDataObject pivot("GTIS", "DpsTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(dataObject.doCot);

        if (attributeFound["do_value"]) {

            if (dataObject.doValue->getData().getType() == DatapointValue::T_INTEGER) {
                int dpsValue = static_cast<int>(dataObject.doValue->getData().toInt());
                checkValueRange(beforeLog, dpsValue, 0, 3, "DP");

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
            pivot.setConfirmation(dataObject.doNegative);
        }

        pivot.addQuality(dataObject.doQualityBl, dataObject.doQualityIv, dataObject.doQualityNt,
                        dataObject.doQualityOv, dataObject.doQualitySb, dataObject.doTest);

        appendTimestampDataObject(pivot, attributeFound["do_ts"], dataObject.doTs, dataObject.doTsIv, dataObject.doTsSu, dataObject.doTsSub);

        convertedDatapoint = pivot.toDatapoint();
    }
    else if (dataObject.doType == "M_ME_NA_1" || dataObject.doType == "M_ME_TD_1") /* normalized measured value */
    {
        // Message structure checks
        if (!attributeFound["do_value"]) {
            if (!attributeFound["do_negative"]) {
                Iec104PivotUtility::log_warn("%s Missing attribute do_negative in ME normalized ACK", beforeLog.c_str());
            }
        }
        
        // Pivot conversion
        PivotDataObject pivot("GTIM", "MvTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(dataObject.doCot);

        if (attributeFound["do_value"]) {
            if (dataObject.doValue->getData().getType() == DatapointValue::T_INTEGER) {
                long value = dataObject.doValue->getData().toInt();
                checkValueRange(beforeLog, value, -1L, 1L, "ME normalized");
                pivot.setMagI(static_cast<int>(value));
            }
            else if (dataObject.doValue->getData().getType() == DatapointValue::T_FLOAT) {
                double value = dataObject.doValue->getData().toDouble();
                checkValueRange(beforeLog, value, -1.0, 32767.0/32768.0, "ME normalized");
                pivot.setMagF(static_cast<float>(value));
            }
        }
        else {
            pivot.setConfirmation(dataObject.doNegative);
        }

        pivot.addQuality(dataObject.doQualityBl, dataObject.doQualityIv, dataObject.doQualityNt,
                        dataObject.doQualityOv, dataObject.doQualitySb, dataObject.doTest);

        appendTimestampDataObject(pivot, attributeFound["do_ts"], dataObject.doTs, dataObject.doTsIv, dataObject.doTsSu, dataObject.doTsSub);

        convertedDatapoint = pivot.toDatapoint();
    }
    else if (dataObject.doType == "M_ME_NB_1" || dataObject.doType == "M_ME_TE_1") /* scaled measured value */
    {
        // Message structure checks
        if (!attributeFound["do_value"]) {
            if (!attributeFound["do_negative"]) {
                Iec104PivotUtility::log_warn("%s Missing attribute do_negative in ME scaled ACK", beforeLog.c_str());
            }
        }
        if (dataObject.doType == "M_ME_TE_1") {
            if (!attributeFound["do_ts"]) {
                Iec104PivotUtility::log_warn("%s Missing attribute do_ts in ME scaled with timestamp", beforeLog.c_str());
            }
            if (!attributeFound["do_ts_iv"]) {
                Iec104PivotUtility::log_warn("%s Missing attribute do_ts_iv in ME scaled with timestamp", beforeLog.c_str());
            }
            if (!attributeFound["do_ts_su"]) {
                Iec104PivotUtility::log_warn("%s Missing attribute do_ts_su in ME scaled with timestamp", beforeLog.c_str());
            }
            if (!attributeFound["do_ts_sub"]) {
                Iec104PivotUtility::log_warn("%s Missing attribute do_ts_sub in ME scaled with timestamp", beforeLog.c_str());
            }
        }
        
        // Pivot conversion
        PivotDataObject pivot("GTIM", "MvTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(dataObject.doCot);

        if (attributeFound["do_value"]) {
            if (dataObject.doValue->getData().getType() == DatapointValue::T_INTEGER) {
                long value = dataObject.doValue->getData().toInt();
                checkValueRange(beforeLog, value, -32768L, 32767L, "ME scaled");
                pivot.setMagI(static_cast<int>(value));
            }
            else if (dataObject.doValue->getData().getType() == DatapointValue::T_FLOAT) {
                double value = dataObject.doValue->getData().toDouble();
                checkValueRange(beforeLog, value, -32768.0, 32767.0, "ME scaled");
                pivot.setMagF(static_cast<float>(value));
            }
        }
        else {
            pivot.setConfirmation(dataObject.doNegative);
        }

        pivot.addQuality(dataObject.doQualityBl, dataObject.doQualityIv, dataObject.doQualityNt,
                        dataObject.doQualityOv, dataObject.doQualitySb, dataObject.doTest);

        appendTimestampDataObject(pivot, attributeFound["do_ts"], dataObject.doTs, dataObject.doTsIv, dataObject.doTsSu, dataObject.doTsSub);

        convertedDatapoint = pivot.toDatapoint();
    }
    else if (dataObject.doType == "M_ME_NC_1" || dataObject.doType == "M_ME_TF_1") /* short (float) measured value */
    {
        // Message structure checks
        if (!attributeFound["do_value"]) {
            if (!attributeFound["do_negative"]) {
                Iec104PivotUtility::log_warn("%s Missing attribute do_negative in ME floating ACK", beforeLog.c_str());
            }
        }
        
        // Pivot conversion
        PivotDataObject pivot("GTIM", "MvTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(dataObject.doCot);

        if (attributeFound["do_value"]) {
            if (dataObject.doValue->getData().getType() == DatapointValue::T_INTEGER) {
                long value = dataObject.doValue->getData().toInt();
                float iValue = static_cast<int>(value);
                if (static_cast<long>(iValue) != value) {
                    Iec104PivotUtility::log_warn("%s do_value out of range (int) for ME floating: %f", beforeLog.c_str(), value);
                }
                pivot.setMagI(iValue);
            }
            else if (dataObject.doValue->getData().getType() == DatapointValue::T_FLOAT) {
                double value = dataObject.doValue->getData().toDouble();
                float fValue = static_cast<float>(value);
                if (static_cast<double>(fValue) != value) {
                    Iec104PivotUtility::log_warn("%s do_value out of range (float) for ME floating: %f", beforeLog.c_str(), value);
                }
                pivot.setMagF(fValue);
            }
        }
        else {
            pivot.setConfirmation(dataObject.doNegative);
        }

        pivot.addQuality(dataObject.doQualityBl, dataObject.doQualityIv, dataObject.doQualityNt,
                        dataObject.doQualityOv, dataObject.doQualitySb, dataObject.doTest);

        appendTimestampDataObject(pivot, attributeFound["do_ts"], dataObject.doTs, dataObject.doTsIv, dataObject.doTsSu, dataObject.doTsSub);

        convertedDatapoint = pivot.toDatapoint();
    }
    else if (dataObject.doType == "M_ST_NA_1" || dataObject.doType == "M_ST_TB_1")
    {
        // Message structure checks
        if (!attributeFound["do_value"]) {
            if (!attributeFound["do_negative"]) {
                Iec104PivotUtility::log_warn("%s Missing attribute do_negative in ST ACK", beforeLog.c_str());
            }
        }
        
        // Pivot conversion
        PivotDataObject pivot("GTIM", "BscTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(dataObject.doCot);

        if (attributeFound["do_value"]) {
            if (dataObject.doValue->getData().getType() == DatapointValue::T_STRING) {
                int wtrVal;
                int transInd;
                std::string str = dataObject.doValue->getData().toString();
                std::string cleaned_str = str.substr(2, str.length() - 4);
                std::size_t commaPos = cleaned_str.find(',');

                if(commaPos != std::string::npos) {
                    std::string numStr = cleaned_str.substr(0, commaPos);
                    std::string boolStr = cleaned_str.substr(commaPos+1);
                    int wtrVal = 0;
                    try {
                        wtrVal = std::stoi(numStr);
                    } catch (const std::invalid_argument &e) {
                        Iec104PivotUtility::log_warn("%s Cannot convert value '%s' to integer: %s",
                                                    beforeLog.c_str(), numStr.c_str(), e.what());
                    } catch (const std::out_of_range &e) {
                        Iec104PivotUtility::log_warn("%s Cannot convert value '%s' to integer: %s",
                                                    beforeLog.c_str(), numStr.c_str(), e.what());
                    }
                    checkValueRange(beforeLog, wtrVal, -64, 63, "ST");
                    bool transInd = (boolStr == "true");

                    pivot.setPosVal(wtrVal, transInd);
                }
            }
        }
        else {
            pivot.setConfirmation(dataObject.doNegative);
        }

        pivot.addQuality(dataObject.doQualityBl, dataObject.doQualityIv, dataObject.doQualityNt,
                        dataObject.doQualityOv, dataObject.doQualitySb, dataObject.doTest);

        appendTimestampDataObject(pivot, attributeFound["do_ts"], dataObject.doTs, dataObject.doTsIv, dataObject.doTsSu, dataObject.doTsSub);

        convertedDatapoint = pivot.toDatapoint();
    }

    else if (dataObject.doType == "C_SC_NA_1" || dataObject.doType == "C_SC_TA_1")
    {
        // Pivot conversion
        PivotDataObject pivot("GTIC", "SpcTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(dataObject.doCot);
        if(attributeFound["do_test"])pivot.setTest(dataObject.doTest);
        if(attributeFound["do_negative"]) pivot.setConfirmation(dataObject.doNegative);
        if (attributeFound["do_value"]) {
            bool spsValue = false;
            if (dataObject.doValue->getData().getType() == DatapointValue::T_INTEGER) {
                int value = static_cast<int>(dataObject.doValue->getData().toInt());
                checkValueRange(beforeLog, value, 0, 1, "SC");
                spsValue = (value > 0);
            }

            pivot.setCtlValBool(spsValue);
        }

        appendTimestampDataObject(pivot, attributeFound["do_ts"], dataObject.doTs, dataObject.doTsIv, dataObject.doTsSu, dataObject.doTsSub);
        convertedDatapoint = pivot.toDatapoint();
    }
    else if (dataObject.doType == "C_DC_NA_1" || dataObject.doType == "C_DC_TA_1")
    {
        // Pivot conversion
        PivotDataObject pivot("GTIC", "DpcTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(dataObject.doCot);
        if(attributeFound["do_test"])pivot.setTest(dataObject.doTest);
        if(attributeFound["do_negative"]) pivot.setConfirmation(dataObject.doNegative);

        if (attributeFound["do_value"]) {

            if (dataObject.doValue->getData().getType() == DatapointValue::T_INTEGER) {
                int dpsValue = static_cast<int>(dataObject.doValue->getData().toInt());
                checkValueRange(beforeLog, dpsValue, 0, 3, "DC");

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

        appendTimestampDataObject(pivot, attributeFound["do_ts"], dataObject.doTs, dataObject.doTsIv, dataObject.doTsSu, dataObject.doTsSub);
        convertedDatapoint = pivot.toDatapoint();
    }

    else if (dataObject.doType == "C_SE_NA_1" || dataObject.doType == "C_SE_TA_1" || dataObject.doType == "C_SE_NC_1" || dataObject.doType == "C_SE_TC_1")
    {
        // Pivot conversion
        PivotDataObject pivot("GTIC", "ApcTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(dataObject.doCot);
        if(attributeFound["do_test"])pivot.setTest(dataObject.doTest);
        if(attributeFound["do_negative"]) pivot.setConfirmation(dataObject.doNegative);

        if (attributeFound["do_value"]) {
            double value = dataObject.doValue->getData().toDouble();
            float fValue = static_cast<float>(value);
            if (dataObject.doType == "C_SE_NA_1" || dataObject.doType == "C_SE_TA_1") {
                checkValueRange(beforeLog, value, -1.0, 32767.0/32768.0, "SE normalized");
            }
            else if (dataObject.doType == "C_SE_NC_1" || dataObject.doType == "C_SE_TC_1") {
                if (static_cast<double>(fValue) != value) {
                    Iec104PivotUtility::log_warn("%s do_value out of range (float) for SE floating: %f", beforeLog.c_str(), value);
                }
            }
            pivot.setCtlValF(fValue);
        }

        appendTimestampDataObject(pivot, attributeFound["do_ts"], dataObject.doTs, dataObject.doTsIv, dataObject.doTsSu, dataObject.doTsSub);
        convertedDatapoint = pivot.toDatapoint();
    }
    else if (dataObject.doType == "C_SE_NB_1" || dataObject.doType == "C_SE_TB_1")
    {
        // Pivot conversion
        PivotDataObject pivot("GTIC", "IncTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(dataObject.doCot);
        if(attributeFound["do_test"])pivot.setTest(dataObject.doTest);
        if(attributeFound["do_negative"]) pivot.setConfirmation(dataObject.doNegative);

        if (attributeFound["do_value"]) {
            long value = dataObject.doValue->getData().toInt();
            checkValueRange(beforeLog, value, -64L, 63L, "SE scaled");
            pivot.setCtlValI(static_cast<int>(value));
        }

        appendTimestampDataObject(pivot, attributeFound["do_ts"], dataObject.doTs, dataObject.doTsIv, dataObject.doTsSu, dataObject.doTsSub);
        convertedDatapoint = pivot.toDatapoint();
    }
    else if (dataObject.doType == "C_RC_NA_1" || dataObject.doType == "C_RC_TA_1")
    {
        // Pivot conversion
        PivotDataObject pivot("GTIC", "BscTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(dataObject.doCot);
        if(attributeFound["do_test"])pivot.setTest(dataObject.doTest);
        if(attributeFound["do_negative"]) pivot.setConfirmation(dataObject.doNegative);

        if (attributeFound["do_value"]) {
            int ctlValue = dataObject.doValue->getData().toInt();
            checkValueRange(beforeLog, ctlValue, 0, 3, "RC");
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
                    Iec104PivotUtility::log_warn("%s Invalid step command response value: %s", beforeLog.c_str(),
                                                (exchangeConfig->getPivotId()).c_str());
                    break;
            }
        }

        appendTimestampDataObject(pivot, attributeFound["do_ts"], dataObject.doTs, dataObject.doTsIv, dataObject.doTsSu, dataObject.doTsSub);
        convertedDatapoint = pivot.toDatapoint();
    }
    else {
        Iec104PivotUtility::log_warn("%s Unknown do_type: %s -> ignore", beforeLog.c_str(), dataObject.doType.c_str());
    }

    return convertedDatapoint;
}

Datapoint*
IEC104PivotFilter::convertOperationObjectToPivot(std::vector<Datapoint*> datapoints)
{
    std::string beforeLog = Iec104PivotUtility::PluginName + " - IEC104PivotFilter::convertOperationObjectToPivot -";

    Datapoint* convertedDatapoint = nullptr;
    std::map<std::string, bool> attributeFound = {
        {"co_ioa", false},
        {"co_ca", false},
        {"co_type", false},
        {"co_cot", false},
        {"co_value", false},
        {"co_ts", false},
        {"co_se", false},
        {"co_test", false},
        {"co_comingfrom", false},
    };

    Iec104CommandObject commandObject;

    for(Datapoint* dp : datapoints){
        readAttribute(attributeFound, dp, "co_ioa", commandObject.coIoa);
        readAttribute(attributeFound, dp, "co_ca", commandObject.coCa);
        readAttribute(attributeFound, dp, "co_type", commandObject.coType);
        readAttribute(attributeFound, dp, "co_cot", commandObject.coCotStr);
        readAttribute(attributeFound, dp, "co_value", commandObject.coValue);
        readAttribute(attributeFound, dp, "co_ts", commandObject.coTsStr);
        readAttribute(attributeFound, dp, "co_se", commandObject.coSeStr);
        readAttribute(attributeFound, dp, "co_test", commandObject.coTestStr);
        readAttribute(attributeFound, dp, "co_comingfrom", commandObject.comingFromValue);
    }

    if(!attributeFound["co_ca"]){
        Iec104PivotUtility::log_error("%s Missing co_ca", beforeLog.c_str());
        return nullptr;
    }
    if(!attributeFound["co_ioa"]){
        Iec104PivotUtility::log_error("%s Missing co_ioa", beforeLog.c_str());
        return nullptr;
    }
    
    std::string address(commandObject.coCa + "-" + commandObject.coIoa);
    IEC104PivotDataPoint* exchangeConfig = m_config.getExchangeDefinitionsByAddress(address);

    if(!exchangeConfig){
        Iec104PivotUtility::log_error("%s CA (%s) and IOA (%s) not found in exchange data", beforeLog.c_str(),
                                    commandObject.coCa.c_str(), commandObject.coIoa.c_str());
        return nullptr;
    }

    if (!attributeFound["co_type"]) {
        Iec104PivotUtility::log_error("%s Missing co_type", beforeLog.c_str());
        return nullptr;
    }
    if (!attributeFound["co_cot"]) {
        Iec104PivotUtility::log_error("%s Missing co_cot", beforeLog.c_str());
        return nullptr;
    }
    else {
        try {
            commandObject.coCot = std::stoi(commandObject.coCotStr);
        } catch (const std::invalid_argument &e) {
            Iec104PivotUtility::log_error("%s Cannot convert COT value '%s' to integer: %s",
                                        beforeLog.c_str(), commandObject.coCotStr.c_str(), e.what());
            return nullptr;
        } catch (const std::out_of_range &e) {
            Iec104PivotUtility::log_error("%s Cannot convert COT value '%s' to integer: %s",
                                        beforeLog.c_str(), commandObject.coCotStr.c_str(), e.what());
            return nullptr;
        }
        if (commandObject.coCot < 0 || commandObject.coCot > 63) {
            Iec104PivotUtility::log_error("%s COT value out of range [0..63] for address %s: %d", beforeLog.c_str(), address.c_str(), commandObject.coCot);
            return nullptr;
        }
    }

    if (!checkTypeMatch(commandObject.coType, exchangeConfig)) {
        Iec104PivotUtility::log_warn("%s Input type (%s) does not match configured type (%s) for address %s", beforeLog.c_str(),
                                    commandObject.coType.c_str(), exchangeConfig->getTypeId().c_str(), address.c_str());
        return nullptr;
    }

    if (commandObject.comingFromValue != "iec104") {
        Iec104PivotUtility::log_warn("%s data_object for %s is not from IEC 104 plugin -> ignore", beforeLog.c_str(),
                                    exchangeConfig->getLabel().c_str());
        return nullptr;
    }

    if(attributeFound["co_se"]){
        try {
            commandObject.coSe = std::stoi(commandObject.coSeStr);
        } catch (const std::invalid_argument &e) {
            Iec104PivotUtility::log_warn("%s Cannot convert SE value '%s' to integer: %s",
                                        beforeLog.c_str(), commandObject.coSeStr.c_str(), e.what());
            attributeFound["co_se"] = false;
        } catch (const std::out_of_range &e) {
            Iec104PivotUtility::log_warn("%s Cannot convert SE value '%s' to integer: %s",
                                        beforeLog.c_str(), commandObject.coSeStr.c_str(), e.what());
            attributeFound["co_se"] = false;
        }
    }

    if(attributeFound["co_ts"]){
        try {
            commandObject.coTs = std::stol(commandObject.coTsStr);
        } catch (const std::invalid_argument &e) {
            Iec104PivotUtility::log_warn("%s Cannot convert timestamp value '%s' to integer: %s",
                                        beforeLog.c_str(), commandObject.coTsStr.c_str(), e.what());
            attributeFound["co_ts"] = false;
        } catch (const std::out_of_range &e) {
            Iec104PivotUtility::log_warn("%s Cannot convert timestamp value '%s' to integer: %s",
                                        beforeLog.c_str(), commandObject.coTsStr.c_str(), e.what());
            attributeFound["co_ts"] = false;
        }
    }
    if(!attributeFound["co_ts"] && hasASDUTimestamp(commandObject.coType)) {
        Iec104PivotUtility::log_error("%s Command has ASDU type with timestamp (%s), but no timestamp was received -> ignore",
                                    beforeLog.c_str(), commandObject.coType.c_str());
        return nullptr;
    }

    if(attributeFound["co_test"]){
        commandObject.coTest = (commandObject.coTestStr == "1");
        if ((commandObject.coTestStr != "1") && (commandObject.coTestStr != "0")) {
            Iec104PivotUtility::log_warn("%s Test value out of range [0..1] for address %s: %s", beforeLog.c_str(),
                                        address.c_str(), commandObject.coTestStr.c_str());
        }
    }


    if (commandObject.coType == "C_SC_NA_1" || commandObject.coType == "C_SC_TA_1")
    {
        PivotOperationObject pivot("GTIC", "SpcTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(commandObject.coCot);
        if(attributeFound["co_se"])pivot.setSelect(commandObject.coSe);
        if(attributeFound["co_test"])pivot.setTest(commandObject.coTest);

        if (attributeFound["co_value"]) {
            bool spsValue = false;
            if (commandObject.coValue->getData().getType() == DatapointValue::T_STRING) {
                int value = 0;
                try {
                    value = std::stoi(commandObject.coValue->getData().toStringValue());
                } catch (const std::invalid_argument &e) {
                    Iec104PivotUtility::log_warn("%s Cannot convert SC value '%s' to integer: %s",
                                                beforeLog.c_str(), commandObject.coSeStr.c_str(), e.what());
                } catch (const std::out_of_range &e) {
                    Iec104PivotUtility::log_warn("%s Cannot convert SC value '%s' to integer: %s",
                                                beforeLog.c_str(), commandObject.coSeStr.c_str(), e.what());
                }
                checkValueRange(beforeLog, value, 0, 1, "SC");
                spsValue = (value > 0);
            }

            pivot.setCtlValBool(spsValue);
        }

        appendTimestampOperationObject(pivot, attributeFound["co_ts"], commandObject.coTs);

        convertedDatapoint = pivot.toDatapoint();
    }
    else if (commandObject.coType == "C_DC_NA_1" || commandObject.coType == "C_DC_TA_1")
    {
        PivotOperationObject pivot("GTIC", "DpcTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(commandObject.coCot);
        if(attributeFound["co_se"])pivot.setSelect(commandObject.coSe);
        if(attributeFound["co_test"])pivot.setTest(commandObject.coTest);

        if (attributeFound["co_value"]) {

            if (commandObject.coValue->getData().getType() == DatapointValue::T_STRING) {
                int dpsValue = 3;
                try {
                    dpsValue = std::stoi(commandObject.coValue->getData().toStringValue());
                } catch (const std::invalid_argument &e) {
                    Iec104PivotUtility::log_warn("%s Cannot convert DC value '%s' to integer: %s",
                                                beforeLog.c_str(), commandObject.coSeStr.c_str(), e.what());
                } catch (const std::out_of_range &e) {
                    Iec104PivotUtility::log_warn("%s Cannot convert DC value '%s' to integer: %s",
                                                beforeLog.c_str(), commandObject.coSeStr.c_str(), e.what());
                }
                checkValueRange(beforeLog, dpsValue, 0, 3, "DC");

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

        appendTimestampOperationObject(pivot, attributeFound["co_ts"], commandObject.coTs);

        convertedDatapoint = pivot.toDatapoint();
    }
    else if (commandObject.coType == "C_SE_NB_1" || commandObject.coType == "C_SE_TB_1")
    {
        PivotOperationObject pivot("GTIC", "IncTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(commandObject.coCot);
        if(attributeFound["co_se"])pivot.setSelect(commandObject.coSe);
        if(attributeFound["co_test"])pivot.setTest(commandObject.coTest);

        if (attributeFound["co_value"]) {
            int value = 0;
            try {
                value = std::stoi(commandObject.coValue->getData().toStringValue());
            } catch (const std::invalid_argument &e) {
                Iec104PivotUtility::log_warn("%s Cannot convert SE scaled value '%s' to integer: %s",
                                            beforeLog.c_str(), commandObject.coSeStr.c_str(), e.what());
            } catch (const std::out_of_range &e) {
                Iec104PivotUtility::log_warn("%s Cannot convert SE scaled value '%s' to integer: %s",
                                            beforeLog.c_str(), commandObject.coSeStr.c_str(), e.what());
            }
            checkValueRange(beforeLog, value, -32768, 32767, "SE scaled");
            pivot.setCtlValI(value);
        }

        appendTimestampOperationObject(pivot, attributeFound["co_ts"], commandObject.coTs);

        convertedDatapoint = pivot.toDatapoint();
    }
    else if (commandObject.coType == "C_SE_NA_1" || commandObject.coType == "C_SE_TA_1" || commandObject.coType == "C_SE_NC_1" || commandObject.coType == "C_SE_TC_1")
    {
        PivotOperationObject pivot("GTIC", "ApcTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(commandObject.coCot);
        if(attributeFound["co_se"])pivot.setSelect(commandObject.coSe);
        if(attributeFound["co_test"])pivot.setTest(commandObject.coTest);

        if (attributeFound["co_value"]) {
            double value = 0.0;
            try {
                value = std::stod(commandObject.coValue->getData().toStringValue());
            } catch (const std::invalid_argument &e) {
                Iec104PivotUtility::log_warn("%s Cannot convert SE normalized/floating value '%s' to double: %s",
                                            beforeLog.c_str(), commandObject.coSeStr.c_str(), e.what());
            } catch (const std::out_of_range &e) {
                Iec104PivotUtility::log_warn("%s Cannot convert SE normalized/floating value '%s' to double: %s",
                                            beforeLog.c_str(), commandObject.coSeStr.c_str(), e.what());
            }
            float fValue = static_cast<float>(value);
            if (commandObject.coType == "C_SE_NA_1" || commandObject.coType == "C_SE_TA_1") {
                checkValueRange(beforeLog, value, -1.0, 32767.0/32768.0, "SE normalized");
            }
            else if (commandObject.coType == "C_SE_NC_1" || commandObject.coType == "C_SE_TC_1") {
                if (static_cast<double>(fValue) != value) {
                    Iec104PivotUtility::log_warn("%s do_value out of range (float) for SE floating: %f", beforeLog.c_str(), value);
                }
            }
            pivot.setCtlValF(fValue);
        }

        appendTimestampOperationObject(pivot, attributeFound["co_ts"], commandObject.coTs);

        convertedDatapoint = pivot.toDatapoint();
    }
    else if (commandObject.coType == "C_RC_NA_1" || commandObject.coType == "C_RC_TA_1")
    {
        PivotOperationObject pivot("GTIC", "BscTyp");

        pivot.setIdentifier(exchangeConfig->getPivotId());
        pivot.setCause(commandObject.coCot);
        if(attributeFound["co_se"])pivot.setSelect(commandObject.coSe);
        if(attributeFound["co_test"])pivot.setTest(commandObject.coTest);

        if (attributeFound["co_value"]) {
            int ctlValue = 0;
            try {
                ctlValue = std::stoi(commandObject.coValue->getData().toStringValue());
            } catch (const std::invalid_argument &e) {
                Iec104PivotUtility::log_warn("%s Cannot convert RC value '%s' to integer: %s",
                                            beforeLog.c_str(), commandObject.coSeStr.c_str(), e.what());
            } catch (const std::out_of_range &e) {
                Iec104PivotUtility::log_warn("%s Cannot convert RC value '%s' to integer: %s",
                                            beforeLog.c_str(), commandObject.coSeStr.c_str(), e.what());
            }
            checkValueRange(beforeLog, ctlValue, 0, 3, "RC");
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
                    Iec104PivotUtility::log_warn("%s Invalid step command value: %s", beforeLog.c_str(),
                                                (exchangeConfig->getPivotId()).c_str());
                    break;
            }
        }

        appendTimestampOperationObject(pivot, attributeFound["co_ts"], commandObject.coTs);

        convertedDatapoint = pivot.toDatapoint();
    }

    return convertedDatapoint;
}

Datapoint*
IEC104PivotFilter::convertDatapointToIEC104DataObject(Datapoint* sourceDp, IEC104PivotDataPoint* exchangeConfig)
{
    std::string beforeLog = Iec104PivotUtility::PluginName + " - IEC104PivotFilter::convertDatapointToIEC104DataObject -";
    Datapoint* convertedDatapoint = nullptr;

    try {
        PivotDataObject pivotObject(sourceDp);

        convertedDatapoint = pivotObject.toIec104DataObject(exchangeConfig);
    }
    catch (PivotObjectException& e)
    {
        Iec104PivotUtility::log_error("%s Failed to convert pivot object: %s", beforeLog.c_str(), e.getContext().c_str());
    }

    return convertedDatapoint;
}

std::vector<Datapoint*>
IEC104PivotFilter::convertReadingToIEC104OperationObject(Datapoint* sourceDp)
{
    std::string beforeLog = Iec104PivotUtility::PluginName + " - IEC104PivotFilter::convertReadingToIEC104OperationObject -";
    std::vector<Datapoint*> convertedDatapoints;

    try {
        PivotOperationObject pivotOperationObject(sourceDp);
        const std::string& pivotId = pivotOperationObject.getIdentifier();
        IEC104PivotDataPoint* exchangeConfig = m_config.getExchangeDefinitionsByPivotId(pivotId);

        if(!exchangeConfig){
            Iec104PivotUtility::log_error("%s Pivot ID not in exchangedData: %s", beforeLog.c_str(), pivotId.c_str());
        }
        else{
            convertedDatapoints = pivotOperationObject.toIec104OperationObject(exchangeConfig);
        }
    }
    catch (PivotObjectException& e)
    {
        Iec104PivotUtility::log_error("%s Failed to convert pivot operation object: %s", beforeLog.c_str(), e.getContext().c_str());
    }

    return convertedDatapoints;
}

void
IEC104PivotFilter::ingest(READINGSET* readingSet)
{
    std::string beforeLog = Iec104PivotUtility::PluginName + " - IEC104PivotFilter::ingest -";
    /* apply transformation */
    std::vector<Reading*>* readings = readingSet->getAllReadingsPtr();

    std::vector<Reading*>::iterator readIt = readings->begin();

    while(readIt != readings->end())
    {
        Reading* reading = *readIt;

        std::string assetName = reading->getAssetName();


        std::vector<Datapoint*>& datapoints = reading->getReadingData();

        std::vector<Datapoint*> convertedDatapoints;


        Iec104PivotUtility::log_debug("%s original Reading: (%s)", beforeLog.c_str(), reading->toJSON().c_str());

        if(assetName == "IEC104Command"){
            Datapoint* convertedOperation = convertOperationObjectToPivot(datapoints);

            if (!convertedOperation) {
                Iec104PivotUtility::log_error("%s Failed to convert IEC command object", beforeLog.c_str());
            }
            else{
                convertedDatapoints.push_back(convertedOperation);
            }

            reading->setAssetName("PivotCommand");
        }

        else if(assetName == "PivotCommand"){
            std::vector<Datapoint*> convertedReadingDatapoints = convertReadingToIEC104OperationObject(datapoints[0]);

            if (convertedReadingDatapoints.empty()) {
                Iec104PivotUtility::log_error("%s Failed to convert Pivot operation object", beforeLog.c_str());
            }

            for(Datapoint* dp : convertedReadingDatapoints)
                convertedDatapoints.push_back(dp);

            reading->setAssetName("IEC104Command");
        }

        else{
            IEC104PivotDataPoint* exchangeConfig = m_config.getExchangeDefinitionsByLabel(assetName);

            if(exchangeConfig){
                for (Datapoint* dp : datapoints) {
                    if (dp->getName() == "data_object") {
                        Datapoint* convertedDp = convertDataObjectToPivot(dp, exchangeConfig);

                        if (convertedDp) {
                            convertedDatapoints.push_back(convertedDp);
                        }
                        else {
                          Iec104PivotUtility::log_error("%s Failed to convert object", beforeLog.c_str());
                        }
                    }

                    else if (dp->getName() == "PIVOT") {
                        Datapoint* convertedDp = convertDatapointToIEC104DataObject(dp, exchangeConfig);

                        if (convertedDp) {
                            convertedDatapoints.push_back(convertedDp);
                        }
                    }
                    else {
                        Datapoint* dpCopy = new Datapoint(dp->getName(),dp->getData());
                        convertedDatapoints.push_back(dpCopy);
                    }
                }
            }
            else {
                 for (Datapoint* dp : datapoints) {
                        Datapoint* dpCopy = new Datapoint(dp->getName(),dp->getData());
                        convertedDatapoints.push_back(dpCopy);
                    }
            }

        }

        reading->removeAllDatapoints();

        for (Datapoint* convertedDatapoint : convertedDatapoints) {
            reading->addDatapoint(convertedDatapoint);
        }

        Iec104PivotUtility::log_debug("%s converted Reading: (%s)", beforeLog.c_str(), reading->toJSON().c_str());


        if (reading->getReadingData().size() == 0) {
            readIt = readings->erase(readIt);
        }
        else {
            readIt++;
        }
    }

    if (readings->empty() == false)
    {
        if (m_output) {
            Iec104PivotUtility::log_debug("%s Send %lu converted readings", beforeLog.c_str(), readings->size());

            m_output(m_outHandle, readingSet);
        }
        else {
            Iec104PivotUtility::log_error("%s No function to call, discard %lu converted readings", beforeLog.c_str(), readings->size());
        }
    }
}

void
IEC104PivotFilter::reconfigure(ConfigCategory* config)
{
    std::string beforeLog = Iec104PivotUtility::PluginName + " - IEC104PivotFilter::reconfigure -";
    Iec104PivotUtility::log_debug("%s (re)configure called", beforeLog.c_str());

    if (config)
    {
        if (config->itemExists("exchanged_data")) {
            const std::string exchangedData = config->getValue("exchanged_data");

            m_config.importExchangeConfig(exchangedData);
        }
        else {
            Iec104PivotUtility::log_error("%s Missing exchanged_data configuation", beforeLog.c_str());
        }
    }
    else {
        Iec104PivotUtility::log_error("%s No configuration provided", beforeLog.c_str());
    }
}

bool IEC104PivotFilter::hasASDUTimestamp(const std::string& asduType) {
    return asduType[5] == 'T';
}