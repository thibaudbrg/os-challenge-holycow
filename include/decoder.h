#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "request.h"
#include "messages.h"
#include "hashTable.h"

uint64_t decode(Request const *request);

#ifdef __cplusplus
}
#endif
