#include "iec104_pivot_filter.hpp"


IEC104PivotFilter::IEC104PivotFilter(const std::string& filterName,
        ConfigCategory& filterConfig,
        OUTPUT_HANDLE *outHandle,
        OUTPUT_STREAM output)
{
    m_outHandle = outHandle;
    m_output = output;
}

IEC104PivotFilter::~IEC104PivotFilter()
{

}

void
IEC104PivotFilter::ingest(READINGSET* readingSet)
{
    //TODO apply transformation

    m_output(m_outHandle, readingSet);
}

void 
IEC104PivotFilter::reconfigure(const string& newConfig)
{

}
