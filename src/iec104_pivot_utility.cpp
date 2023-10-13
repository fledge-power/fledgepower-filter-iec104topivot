/*
 * FledgePower HNZ <-> pivot filter utility functions.
 *
 * Copyright (c) 2022, RTE (https://www.rte-france.com)
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Michael Zillgith (michael.zillgith at mz-automation.de)
 * 
 */

#include <sstream>
#include "iec104_pivot_utility.hpp"

std::string Iec104PivotUtility::join(const std::vector<std::string> &list, const std::string &sep /*= ", "*/)
{
    std::string ret;
    for(const auto &str : list) {
        if(!ret.empty()) {
            ret += sep;
        }
        ret += str;
    }
    return ret;
}
