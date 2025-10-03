#include "pch.h"
#include "Driver.h"

#include <ktl.h>
#include <Locker.h>

#include "pooltag.h"
#include "Communication.h"
#include "common.h"

// Contexts
#include "VolumeContext.h"

#include "Process.h"

NTSTATUS MinifilterInstanceSetup(PCFLT_RELATED_OBJECTS FltObjects,
	FLT_INSTANCE_SETUP_FLAGS Flags,
	_In_ DEVICE_TYPE VolumeDeviceType,
	FLT_FILESYSTEM_TYPE VolumeFilesystemType)
{
	KdPrint((DRIVER_PREFIX "InstanceSetup FS: %u\n", VolumeFilesystemType));

	UNREFERENCED_PARAMETER(VolumeDeviceType);

	if (FLT_FSTYPE_RAW != VolumeFilesystemType)
	{
		VolumeContext* Context;
		auto status = VolumeContext::Factory(FltObjects, Flags, VolumeFilesystemType, &Context);
		if (!NT_SUCCESS(status))
		{
			KdPrint((DRIVER_PREFIX "Failed to allocate volume context (0x%08X)\n", status));
			return STATUS_FLT_DO_NOT_ATTACH;
		}

		if (!NT_SUCCESS(status = VolumeContext::SetContext(FltObjects, Context, true)))
		{
			KdPrint((DRIVER_PREFIX "Failed to set volume context (0x%08X)\n", status));
		}

		FltReleaseContext(Context);

		return NT_SUCCESS(status) ? STATUS_SUCCESS : STATUS_FLT_DO_NOT_ATTACH;

	}
	else
	{
		CommunicationPort::Instance()->SendOutputMessage(PortMessageType::VolumeMessage,
			L"\nSkipping volume (unsupported filesystem %u or device type %u)",
			VolumeFilesystemType,
            VolumeDeviceType);
	}

	return STATUS_FLT_DO_NOT_ATTACH;
}

NTSTATUS MinifilterInstanceQueryTeardown(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
	KdPrint((DRIVER_PREFIX "InstanceQueryTeardown\n"));

	return STATUS_SUCCESS;
}

VOID MinifilterInstanceTeardownStart(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_TEARDOWN_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
}

VOID MinifilterInstanceTeardownComplete(PCFLT_RELATED_OBJECTS FltObjects, FLT_INSTANCE_TEARDOWN_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(Flags);
	VolumeContext* volume = {};

	if (NT_SUCCESS(FltGetVolumeContext(g_Filter, FltObjects->Volume, (PFLT_CONTEXT*)&volume)))
	{
        CommunicationPort::Instance()->SendOutputMessage(PortMessageType::VolumeMessage,
            L"\nVolume detached: %wZ", &volume->volume_name_);
        FltReleaseContext(volume);
	}
}

_IRQL_requires_max_(PASSIVE_LEVEL)
FLT_PREOP_CALLBACK_STATUS OnPreCreate(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Outptr_result_maybenull_ PVOID* CompletionContext)
{
	UNREFERENCED_PARAMETER(FltObjects);
	const ULONG desiredAccess = Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess;
	UNREFERENCED_PARAMETER(desiredAccess);
	*CompletionContext = nullptr;
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

NTSTATUS InitMiniFilter(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) 
{
	HANDLE hKey = nullptr;
	HANDLE hSubKey = nullptr;
	NTSTATUS status;
	do 
	{
		//
		// add registry data for proper mini-filter registration
		//
		OBJECT_ATTRIBUTES keyAttr = RTL_CONSTANT_OBJECT_ATTRIBUTES(RegistryPath, OBJ_KERNEL_HANDLE);
		status = ZwOpenKey(&hKey, KEY_WRITE, &keyAttr);
		if (!NT_SUCCESS(status))
		{
			break;
		}

		UNICODE_STRING subKey = RTL_CONSTANT_STRING(L"Instances");
		OBJECT_ATTRIBUTES subKeyAttr;
		InitializeObjectAttributes(&subKeyAttr, &subKey, OBJ_KERNEL_HANDLE, hKey, nullptr);
		status = ZwCreateKey(&hSubKey, KEY_WRITE, &subKeyAttr, 0, nullptr, 0, nullptr);
		if (!NT_SUCCESS(status))
		{
			break;
		}

		//
		// set "DefaultInstance" value
		//
		UNICODE_STRING valueName = RTL_CONSTANT_STRING(L"DefaultInstance");
		WCHAR name[] = L"BackupDefaultInstance";
		status = ZwSetValueKey(hSubKey, &valueName, 0, REG_SZ, name, sizeof(name));
		if (!NT_SUCCESS(status))
		{
			break;
		}

		//
		// create "instance" key under "Instances"
		//
		UNICODE_STRING instKeyName;
		RtlInitUnicodeString(&instKeyName, name);
		HANDLE hInstKey;
		InitializeObjectAttributes(&subKeyAttr, &instKeyName, OBJ_KERNEL_HANDLE, hSubKey, nullptr);
		status = ZwCreateKey(&hInstKey, KEY_WRITE, &subKeyAttr, 0, nullptr, 0, nullptr);
		if (!NT_SUCCESS(status))
		{
			break;
		}

		//
		// write out altitude 
		// TODO: Don't overwrite this if it exists
		// 
		WCHAR altitude[] = L"335342";
		UNICODE_STRING altitudeName = RTL_CONSTANT_STRING(L"Altitude");
		status = ZwSetValueKey(hInstKey, &altitudeName, 0, REG_SZ, altitude, sizeof(altitude));
		if (!NT_SUCCESS(status))
		{
			break;
		}

		//
		// write out flags
		//
		UNICODE_STRING flagsName = RTL_CONSTANT_STRING(L"Flags");
		ULONG flags = 0;
		status = ZwSetValueKey(hInstKey, &flagsName, 0, REG_DWORD, &flags, sizeof(flags));
		if (!NT_SUCCESS(status))
		{
			break;
		}
	
		ZwClose(hInstKey);
        hInstKey = nullptr;

        ZwClose(hSubKey);
        hSubKey = nullptr;

		FLT_OPERATION_REGISTRATION const callbacks[] = {
			{ IRP_MJ_CREATE, 0, OnPreCreate, nullptr },
			{ IRP_MJ_OPERATION_END }
		};

		const FLT_CONTEXT_REGISTRATION context[] = {
			{ FLT_VOLUME_CONTEXT, 0, VolumeContext::Cleanup, sizeof(VolumeContext), VOLUME_POOLTAG },
			{ FLT_CONTEXT_END }
		};

		FLT_REGISTRATION const reg = {
			sizeof(FLT_REGISTRATION),
			FLT_REGISTRATION_VERSION,
			FLTFL_OPERATION_REGISTRATION_SKIP_PAGING_IO, //  Flags
			context,                 //  Context
			callbacks,               //  Operation callbacks
			MinifilterUnload,                   //  MiniFilterUnload
			MinifilterInstanceSetup,            //  InstanceSetup
			MinifilterInstanceQueryTeardown,    //  InstanceQueryTeardown
			MinifilterInstanceTeardownStart,    //  InstanceTeardownStart
			MinifilterInstanceTeardownComplete, //  InstanceTeardownComplete
		};
		status = FltRegisterFilter(DriverObject, &reg, &g_Filter);
	} while (false);

	if (hSubKey)
	{
		if (!NT_SUCCESS(status))
		{
			ZwDeleteKey(hSubKey);
		}
		ZwClose(hSubKey);
		hSubKey = nullptr;
	}

	if (hKey)
	{
		ZwClose(hKey);
		hKey = nullptr;
	}

	return status;
}

NTSTATUS MinifilterUnload(FLT_FILTER_UNLOAD_FLAGS Flags)
{
	UNREFERENCED_PARAMETER(Flags);

	CommunicationPort::Instance()->FinalizeFilterPort();
	FltUnregisterFilter(g_Filter);

	return STATUS_SUCCESS;
}

