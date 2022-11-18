/*
 * FledgePower IEC 104 to pivot filter pluging
 */
#include <plugin_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string>
#include <logger.h>
#include <plugin_exception.h>
#include <iostream>
#include <config_category.h>
#include <version.h>
#include "iec104_pivot_filter.hpp"


using namespace std;
using namespace rapidjson;

extern "C" {

#define PLUGIN_NAME "iec104_pivot_filter"
#define VERSION "1.0.0"

/**
 * Plugin specific default configuration
 */
static const char *default_config = QUOTE({
			"plugin" : {
				"description" : "IEC 104 to pivot filter plugin",
				"type" : "string",
				"default" : PLUGIN_NAME,
				"readonly" : "true"
			},
            "exchanged_data": {
                    "description" : "exchanged data list",
                    "type" : "string",
                    "displayName" : "Exchanged data list",
                    "order" : "1",
                    "default" : QUOTE({
                                "exchanged_data" : {
                                    "name" : "iec104pivot",
                                    "version" : "1.0",
                                    "datapoints":[
                                        {
                                            "label":"TS1",
                                            "pivot_id":"ID-45-672",
                                            "pivot_type":"SpsTyp",
                                            "protocols":[
                                               {
                                                  "name":"iec104",
                                                  "address":"45-672",
                                                  "typeid":"M_SP_NA_1"
                                               }
                                            ]
                                        },
                                        {
                                            "label":"TS2",
                                            "pivot_id":"ID-45-872",
                                            "pivot_type":"SpsTyp",
                                            "protocols":[
                                               {
                                                  "name":"iec104",
                                                  "address":"45-872",
                                                  "typeid":"M_SP_TB_1"
                                               }
                                            ]
                                        },
                                        {
                                            "label":"TS3",
                                            "pivot_id":"ID-45-890",
                                            "pivot_type":"DpsTyp",
                                            "protocols":[
                                               {
                                                  "name":"iec104",
                                                  "address":"45-890",
                                                  "typeid":"M_DP_TB_1"
                                               }
                                            ]
                                        },
                                        {
                                            "label":"TM1",
                                            "pivot_id":"ID-45-984",
                                            "pivot_type":"MvTyp",
                                            "protocols":[
                                               {
                                                  "name":"iec104",
                                                  "address":"45-984",
                                                  "typeid":"M_ME_NA_1"
                                               }
                                            ]
                                        }
                                    ]
                                }
                            })
            }
		});


/**
 * The C API plugin information structure
 */
static PLUGIN_INFORMATION info = {
	   PLUGIN_NAME,			// Name
	   VERSION,			    // Version
	   0,		            // Flags
	   PLUGIN_TYPE_FILTER,	// Type
	   "1.0.0",			    // Interface version
	   default_config		// Configuration
};

/**
 * Return the information about this plugin
 */
PLUGIN_INFORMATION *plugin_info()
{
	return &info;
}

/**
 * Initialise the plugin with configuration.
 *
 * This function is called to get the plugin handle.
 */
PLUGIN_HANDLE plugin_init(ConfigCategory* config,
                          OUTPUT_HANDLE *outHandle,
                          OUTPUT_STREAM output)
{
    Logger::getLogger()->info("Initializing the plugin");

    IEC104PivotFilter* pivotFilter = new IEC104PivotFilter(PLUGIN_NAME,
                                config, outHandle, output);

	return (PLUGIN_HANDLE)pivotFilter;
}

/**
 * Ingest a set of readings into the plugin for processing
 *
 * @param handle     The plugin handle returned from plugin_init
 * @param readingSet The readings to process
 */
void plugin_ingest(PLUGIN_HANDLE handle,
                   READINGSET *readingSet)
{
    IEC104PivotFilter* pivotFilter = (IEC104PivotFilter*)handle;
    pivotFilter->ingest(readingSet);
}

/**
 * Plugin reconfiguration method
 *
 * @param handle     The plugin handle
 * @param newConfig  The updated configuration
 */
void plugin_reconfigure(PLUGIN_HANDLE handle, const std::string& newConfig)
{
    IEC104PivotFilter* pivotFilter = (IEC104PivotFilter*)handle;

    ConfigCategory config("iec104pivot", newConfig);

    pivotFilter->reconfigure(&config);
}

/**
 * Call the shutdown method in the plugin
 */
void plugin_shutdown(PLUGIN_HANDLE handle)
{
    IEC104PivotFilter* pivotFilter = (IEC104PivotFilter*)handle;
    delete pivotFilter;
}

// End of extern "C"
};
