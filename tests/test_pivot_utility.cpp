#include <gtest/gtest.h>

#include "iec104_pivot_utility.hpp"

TEST(PivotIEC104PluginUtility, Logs)
{
    std::string text("This message is at level %s");
    ASSERT_NO_THROW(Iec104PivotUtility::log_debug(text.c_str(), "debug"));
    ASSERT_NO_THROW(Iec104PivotUtility::log_info(text.c_str(), "info"));
    ASSERT_NO_THROW(Iec104PivotUtility::log_warn(text.c_str(), "warning"));
    ASSERT_NO_THROW(Iec104PivotUtility::log_error(text.c_str(), "error"));
    ASSERT_NO_THROW(Iec104PivotUtility::log_fatal(text.c_str(), "fatal"));
}