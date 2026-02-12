#ifndef NETWORKING_RESEARCH_T_INDEX_H
#define NETWORKING_RESEARCH_T_INDEX_H

#include "test_runner.h"

#ifdef __cplusplus
extern "C" {
#endif

void registerAllTests(TestRunner* runner);
void registerTBitsTests(TestRunner* runner);
void registerTLoggerTests(TestRunner* runner);

#ifdef __cplusplus
}
#endif

#endif
