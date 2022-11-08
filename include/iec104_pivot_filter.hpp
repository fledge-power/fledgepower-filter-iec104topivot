#ifndef _IEC104_PIVOT_FILTER_H
#define _IEC104_PIVOT_FILTER_H

#include <filter.h>

using namespace std;

class IEC104PivotFilter {

public:
    IEC104PivotFilter(const std::string& filterName,
        ConfigCategory& filterConfig,
        OUTPUT_HANDLE *outHandle,
        OUTPUT_STREAM output);

    ~IEC104PivotFilter();

    void ingest(READINGSET* readingSet);
    void reconfigure(const string& newConfig);

private:

    OUTPUT_HANDLE* m_outHandle;
    OUTPUT_STREAM m_output;
};


#endif /* _IEC104_PIVOT_FILTER_H */