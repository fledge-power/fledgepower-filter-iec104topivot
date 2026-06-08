#include <gtest/gtest.h>

#include <datapoint.h>
#include <iec104_pivot_object.hpp>
#include <iec104_pivot_filter_config.hpp>

#include <string>
#include <vector>

// Regression test for the signed-integer-overflow (UB) in PivotTimestamp seconds decoding.
//
// Before the fix, `getTimeInMs()` and `SecondSinceEpoch()` reconstructed the 32-bit seconds with
// `m_valueArray[0] * 0x1000000` (uint8_t promoted to int, multiplied by an int literal). For any
// timestamp whose top byte is >= 128 (SecondSinceEpoch >= 2^31, i.e. 2038-01-19 onward) this is
// signed-integer overflow (undefined behaviour) and yields a wrong/truncated `do_ts`.
//
// This drives the public path PivotDataObject builder -> toIec104DataObject(), which is what the
// n104 pipeline uses, and asserts the emitted `do_ts` round-trips exactly.

namespace {

Datapoint* findChild(Datapoint* dictDp, const std::string& name)
{
    if (!dictDp) return nullptr;
    DatapointValue& v = dictDp->getData();
    if (v.getType() != DatapointValue::T_DP_DICT) return nullptr;
    std::vector<Datapoint*>* vec = v.getDpVec();
    if (!vec) return nullptr;
    for (Datapoint* d : *vec) {
        if (d->getName() == name) return d;
    }
    return nullptr;
}

} // namespace

// SecondSinceEpoch >= 2^31 (3e9 s) must not overflow and must round-trip exactly through do_ts.
TEST(PivotTimestampOverflow, SecondsBeyond2038RoundTripExact)
{
    const long long ms = 3000000000000LL; // 3e9 seconds -> top byte 178 (>= 128)

    PivotDataObject pivot("GTIM", "MvTyp");
    pivot.setIdentifier("PID-1");
    pivot.setCause(3); // spontaneous
    pivot.setMagF(1.0f);
    pivot.addTimestamp(static_cast<long>(ms), false, false, false);

    Datapoint* builtPivot = pivot.toDatapoint();
    ASSERT_NE(builtPivot, nullptr);

    IEC104PivotDataPoint exchangeConfig("LABEL-1", "PID-1", "MvTyp", "M_ME_NC_1", 1, 1, "");
    PivotDataObject parsed(builtPivot);
    Datapoint* dataObject = parsed.toIec104DataObject(&exchangeConfig);
    ASSERT_NE(dataObject, nullptr);

    Datapoint* doTs = findChild(dataObject, "do_ts");
    ASSERT_NE(doTs, nullptr);
    EXPECT_EQ(static_cast<long long>(doTs->getData().toInt()), ms);
}

// Direct check of the decoder for a full 32-bit seconds value (no overflow, no truncation).
TEST(PivotTimestampOverflow, DecoderUnsignedSeconds)
{
    PivotTimestamp ts(3000000000000LL); // ms
    EXPECT_EQ(static_cast<unsigned long>(ts.SecondSinceEpoch()), 3000000000UL);
    EXPECT_EQ(static_cast<unsigned long long>(ts.getTimeInMs()), 3000000000000ULL);
}
