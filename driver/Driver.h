#pragma once

extern PFLT_FILTER g_Filter;

#define DRIVER_PREFIX "VolumeInfo: "

NTSTATUS InitMiniFilter(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);
NTSTATUS MinifilterUnload(FLT_FILTER_UNLOAD_FLAGS Flags);
