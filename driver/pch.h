#pragma once

#include <fltKernel.h>
#include <Ntstrsafe.h>
#include <ntddstor.h>

#include <ktl.h>

#include "Common.h"
#include "DynamicImports.h"

template <typename _CountofType, size_t _SizeOfArray>
char (*__countof_helper(UNALIGNED _CountofType(&_Array)[_SizeOfArray]))[_SizeOfArray];
#define _countof(_Array) sizeof(*__countof_helper(_Array))
