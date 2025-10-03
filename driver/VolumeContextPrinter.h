#pragma once

#include "Communication.h"

class VolumeContext;

class VolumeContextPrinter
{
public:

	static void SendVolumeInfo(const VolumeContext* vol);

private:

	// Utility function to convert FLT_FILESYSTEM_TYPE to formatted ANSI string
	static NTSTATUS FormatFilesystemType(_In_ FLT_FILESYSTEM_TYPE FilesystemType, _Out_ UNICODE_STRING* FilesystemString)
	{
		if (!FilesystemString) {
			return STATUS_INVALID_PARAMETER;
		}

		const char* fsTypeString = nullptr;

		// Map FLT_FILESYSTEM_TYPE enum values to string representations
		switch (FilesystemType) {
		case FLT_FSTYPE_UNKNOWN:
			fsTypeString = "Unknown";
			break;
		case FLT_FSTYPE_RAW:
			fsTypeString = "Raw";
			break;
		case FLT_FSTYPE_NTFS:
			fsTypeString = "NTFS";
			break;
		case FLT_FSTYPE_FAT:
			fsTypeString = "FAT";
			break;
		case FLT_FSTYPE_CDFS:
			fsTypeString = "CDFS";
			break;
		case FLT_FSTYPE_UDFS:
			fsTypeString = "UDFS";
			break;
		case FLT_FSTYPE_LANMAN:
			fsTypeString = "LANMAN";
			break;
		case FLT_FSTYPE_WEBDAV:
			fsTypeString = "WebDAV";
			break;
		case FLT_FSTYPE_RDPDR:
			fsTypeString = "RDPDR";
			break;
		case FLT_FSTYPE_NFS:
			fsTypeString = "NFS";
			break;
		case FLT_FSTYPE_MS_NETWARE:
			fsTypeString = "MS_NETWARE";
			break;
		case FLT_FSTYPE_NETWARE:
			fsTypeString = "NETWARE";
			break;
		case FLT_FSTYPE_BSUDF:
			fsTypeString = "BSUDF";
			break;
		case FLT_FSTYPE_MUP:
			fsTypeString = "MUP";
			break;
		case FLT_FSTYPE_RSFX:
			fsTypeString = "RSFX";
			break;
		case FLT_FSTYPE_ROXIO_UDF1:
			fsTypeString = "ROXIO_UDF1";
			break;
		case FLT_FSTYPE_ROXIO_UDF2:
			fsTypeString = "ROXIO_UDF2";
			break;
		case FLT_FSTYPE_ROXIO_UDF3:
			fsTypeString = "ROXIO_UDF3";
			break;
		case FLT_FSTYPE_TACIT:
			fsTypeString = "TACIT";
			break;
		case FLT_FSTYPE_FS_REC:
			fsTypeString = "FS_REC";
			break;
		case FLT_FSTYPE_INCD:
			fsTypeString = "INCD";
			break;
		case FLT_FSTYPE_INCD_FAT:
			fsTypeString = "INCD_FAT";
			break;
		case FLT_FSTYPE_EXFAT:
			fsTypeString = "exFAT";
			break;
		case FLT_FSTYPE_PSFS:
			fsTypeString = "PSFS";
			break;
		case FLT_FSTYPE_GPFS:
			fsTypeString = "GPFS";
			break;
		case FLT_FSTYPE_NPFS:
			fsTypeString = "NPFS";
			break;
		case FLT_FSTYPE_MSFS:
			fsTypeString = "MSFS";
			break;
		case FLT_FSTYPE_CSVFS:
			fsTypeString = "CSVFS";
			break;
		case FLT_FSTYPE_REFS:
			fsTypeString = "ReFS";
			break;
		case FLT_FSTYPE_OPENAFS:
			fsTypeString = "OpenAFS";
			break;
		default:
			fsTypeString = "UNSUPPORTED";
			break;
		}

		// Format the string as "EnumValue (StringRepresentation)" using RtlStringVPrintf
		NTSTATUS status = RtlUnicodeStringPrintf(
			FilesystemString,
			L"FileSystemType: %u (%hs)",
			static_cast<ULONG>(FilesystemType),
			fsTypeString);

		return status;
	}

	// Utility function to convert FLT_VOLUME_PROPERTIES to formatted string including device type and characteristics
	static NTSTATUS FormatDeviceDetails(_In_ PFLT_VOLUME_PROPERTIES VolumeProperties, _Out_ UNICODE_STRING* DeviceTypeString)
	{
		if (!DeviceTypeString || !VolumeProperties) {
			return STATUS_INVALID_PARAMETER;
		}

		DEVICE_TYPE DeviceType = VolumeProperties->DeviceType;
		ULONG DeviceCharacteristics = VolumeProperties->DeviceCharacteristics;

		const char* deviceTypeString = nullptr;

		// Map DEVICE_TYPE values to string representations
		switch (DeviceType) {
		case FILE_DEVICE_BEEP:
			deviceTypeString = "BEEP";
			break;
		case FILE_DEVICE_CD_ROM:
			deviceTypeString = "CD_ROM";
			break;
		case FILE_DEVICE_CD_ROM_FILE_SYSTEM:
			deviceTypeString = "CD_ROM_FILE_SYSTEM";
			break;
		case FILE_DEVICE_CONTROLLER:
			deviceTypeString = "CONTROLLER";
			break;
		case FILE_DEVICE_DATALINK:
			deviceTypeString = "DATALINK";
			break;
		case FILE_DEVICE_DFS:
			deviceTypeString = "DFS";
			break;
		case FILE_DEVICE_DISK:
			deviceTypeString = "DISK";
			break;
		case FILE_DEVICE_DISK_FILE_SYSTEM:
			deviceTypeString = "DISK_FILE_SYSTEM";
			break;
		case FILE_DEVICE_FILE_SYSTEM:
			deviceTypeString = "FILE_SYSTEM";
			break;
		case FILE_DEVICE_INPORT_PORT:
			deviceTypeString = "INPORT_PORT";
			break;
		case FILE_DEVICE_KEYBOARD:
			deviceTypeString = "KEYBOARD";
			break;
		case FILE_DEVICE_MAILSLOT:
			deviceTypeString = "MAILSLOT";
			break;
		case FILE_DEVICE_MIDI_IN:
			deviceTypeString = "MIDI_IN";
			break;
		case FILE_DEVICE_MIDI_OUT:
			deviceTypeString = "MIDI_OUT";
			break;
		case FILE_DEVICE_MOUSE:
			deviceTypeString = "MOUSE";
			break;
		case FILE_DEVICE_MULTI_UNC_PROVIDER:
			deviceTypeString = "MULTI_UNC_PROVIDER";
			break;
		case FILE_DEVICE_NAMED_PIPE:
			deviceTypeString = "NAMED_PIPE";
			break;
		case FILE_DEVICE_NETWORK:
			deviceTypeString = "NETWORK";
			break;
		case FILE_DEVICE_NETWORK_BROWSER:
			deviceTypeString = "NETWORK_BROWSER";
			break;
		case FILE_DEVICE_NETWORK_FILE_SYSTEM:
			deviceTypeString = "NETWORK_FILE_SYSTEM";
			break;
		case FILE_DEVICE_NULL:
			deviceTypeString = "NULL";
			break;
		case FILE_DEVICE_PARALLEL_PORT:
			deviceTypeString = "PARALLEL_PORT";
			break;
		case FILE_DEVICE_PHYSICAL_NETCARD:
			deviceTypeString = "PHYSICAL_NETCARD";
			break;
		case FILE_DEVICE_PRINTER:
			deviceTypeString = "PRINTER";
			break;
		case FILE_DEVICE_SCANNER:
			deviceTypeString = "SCANNER";
			break;
		case FILE_DEVICE_SERIAL_MOUSE_PORT:
			deviceTypeString = "SERIAL_MOUSE_PORT";
			break;
		case FILE_DEVICE_SERIAL_PORT:
			deviceTypeString = "SERIAL_PORT";
			break;
		case FILE_DEVICE_SCREEN:
			deviceTypeString = "SCREEN";
			break;
		case FILE_DEVICE_SOUND:
			deviceTypeString = "SOUND";
			break;
		case FILE_DEVICE_STREAMS:
			deviceTypeString = "STREAMS";
			break;
		case FILE_DEVICE_TAPE:
			deviceTypeString = "TAPE";
			break;
		case FILE_DEVICE_TAPE_FILE_SYSTEM:
			deviceTypeString = "TAPE_FILE_SYSTEM";
			break;
		case FILE_DEVICE_TRANSPORT:
			deviceTypeString = "TRANSPORT";
			break;
		case FILE_DEVICE_UNKNOWN:
			deviceTypeString = "UNKNOWN";
			break;
		case FILE_DEVICE_VIDEO:
			deviceTypeString = "VIDEO";
			break;
		case FILE_DEVICE_VIRTUAL_DISK:
			deviceTypeString = "VIRTUAL_DISK";
			break;
		case FILE_DEVICE_WAVE_IN:
			deviceTypeString = "WAVE_IN";
			break;
		case FILE_DEVICE_WAVE_OUT:
			deviceTypeString = "WAVE_OUT";
			break;
		case FILE_DEVICE_8042_PORT:
			deviceTypeString = "8042_PORT";
			break;
		case FILE_DEVICE_NETWORK_REDIRECTOR:
			deviceTypeString = "NETWORK_REDIRECTOR";
			break;
		case FILE_DEVICE_BATTERY:
			deviceTypeString = "BATTERY";
			break;
		case FILE_DEVICE_BUS_EXTENDER:
			deviceTypeString = "BUS_EXTENDER";
			break;
		case FILE_DEVICE_MODEM:
			deviceTypeString = "MODEM";
			break;
		case FILE_DEVICE_VDM:
			deviceTypeString = "VDM";
			break;
		case FILE_DEVICE_MASS_STORAGE:
			deviceTypeString = "MASS_STORAGE";
			break;
		case FILE_DEVICE_SMB:
			deviceTypeString = "SMB";
			break;
		case FILE_DEVICE_KS:
			deviceTypeString = "KS";
			break;
		case FILE_DEVICE_CHANGER:
			deviceTypeString = "CHANGER";
			break;
		case FILE_DEVICE_SMARTCARD:
			deviceTypeString = "SMARTCARD";
			break;
		case FILE_DEVICE_ACPI:
			deviceTypeString = "ACPI";
			break;
		case FILE_DEVICE_DVD:
			deviceTypeString = "DVD";
			break;
		case FILE_DEVICE_FULLSCREEN_VIDEO:
			deviceTypeString = "FULLSCREEN_VIDEO";
			break;
		case FILE_DEVICE_DFS_FILE_SYSTEM:
			deviceTypeString = "DFS_FILE_SYSTEM";
			break;
		case FILE_DEVICE_DFS_VOLUME:
			deviceTypeString = "DFS_VOLUME";
			break;
		case FILE_DEVICE_SERENUM:
			deviceTypeString = "SERENUM";
			break;
		case FILE_DEVICE_TERMSRV:
			deviceTypeString = "TERMSRV";
			break;
		case FILE_DEVICE_KSEC:
			deviceTypeString = "KSEC";
			break;
		case FILE_DEVICE_FIPS:
			deviceTypeString = "FIPS";
			break;
		case FILE_DEVICE_INFINIBAND:
			deviceTypeString = "INFINIBAND";
			break;
		case FILE_DEVICE_VMBUS:
			deviceTypeString = "VMBUS";
			break;
		case FILE_DEVICE_CRYPT_PROVIDER:
			deviceTypeString = "CRYPT_PROVIDER";
			break;
		case FILE_DEVICE_WPD:
			deviceTypeString = "WPD";
			break;
		case FILE_DEVICE_BLUETOOTH:
			deviceTypeString = "BLUETOOTH";
			break;
		case FILE_DEVICE_MT_COMPOSITE:
			deviceTypeString = "MT_COMPOSITE";
			break;
		case FILE_DEVICE_MT_TRANSPORT:
			deviceTypeString = "MT_TRANSPORT";
			break;
		case FILE_DEVICE_BIOMETRIC:
			deviceTypeString = "BIOMETRIC";
			break;
		case FILE_DEVICE_PMI:
			deviceTypeString = "PMI";
			break;
		case FILE_DEVICE_EHSTOR:
			deviceTypeString = "EHSTOR";
			break;
		case FILE_DEVICE_DEVAPI:
			deviceTypeString = "DEVAPI";
			break;
		case FILE_DEVICE_GPIO:
			deviceTypeString = "GPIO";
			break;
		case FILE_DEVICE_USBEX:
			deviceTypeString = "USBEX";
			break;
		case FILE_DEVICE_CONSOLE:
			deviceTypeString = "CONSOLE";
			break;
		case FILE_DEVICE_NFP:
			deviceTypeString = "NFP";
			break;
		case FILE_DEVICE_SYSENV:
			deviceTypeString = "SYSENV";
			break;
		case FILE_DEVICE_VIRTUAL_BLOCK:
			deviceTypeString = "VIRTUAL_BLOCK";
			break;
		case FILE_DEVICE_POINT_OF_SERVICE:
			deviceTypeString = "POINT_OF_SERVICE";
			break;
		case FILE_DEVICE_STORAGE_REPLICATION:
			deviceTypeString = "STORAGE_REPLICATION";
			break;
		case FILE_DEVICE_TRUST_ENV:
			deviceTypeString = "TRUST_ENV";
			break;
		case FILE_DEVICE_UCM:
			deviceTypeString = "UCM";
			break;
		case FILE_DEVICE_UCMTCPCI:
			deviceTypeString = "UCMTCPCI";
			break;
		case FILE_DEVICE_PERSISTENT_MEMORY:
			deviceTypeString = "PERSISTENT_MEMORY";
			break;
		case FILE_DEVICE_NVDIMM:
			deviceTypeString = "NVDIMM";
			break;
		case FILE_DEVICE_HOLOGRAPHIC:
			deviceTypeString = "HOLOGRAPHIC";
			break;
		case FILE_DEVICE_SDFXHCI:
			deviceTypeString = "SDFXHCI";
			break;
		default:
			deviceTypeString = "UNSUPPORTED";
			break;
		}

		// Build the device characteristics string
		char characteristicsBuffer[512] = "";
		bool hasCharacteristics = false;

		// Check for known DeviceCharacteristics flags (as defined in NTIFS.H)
		if (DeviceCharacteristics & FILE_REMOVABLE_MEDIA) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_REMOVABLE_MEDIA", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_READ_ONLY_DEVICE) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_READ_ONLY_DEVICE", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_FLOPPY_DISKETTE) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_FLOPPY_DISKETTE", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_WRITE_ONCE_MEDIA) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_WRITE_ONCE_MEDIA", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_REMOTE_DEVICE) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_REMOTE_DEVICE", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_DEVICE_IS_MOUNTED) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_DEVICE_IS_MOUNTED", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_VIRTUAL_VOLUME) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_VIRTUAL_VOLUME", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_AUTOGENERATED_DEVICE_NAME) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_AUTOGENERATED_DEVICE_NAME", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_DEVICE_SECURE_OPEN) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_DEVICE_SECURE_OPEN", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_CHARACTERISTIC_PNP_DEVICE) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_CHARACTERISTIC_PNP_DEVICE", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_CHARACTERISTIC_TS_DEVICE) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_CHARACTERISTIC_TS_DEVICE", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_CHARACTERISTIC_WEBDAV_DEVICE) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_CHARACTERISTIC_WEBDAV_DEVICE", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_CHARACTERISTIC_CSV) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_CHARACTERISTIC_CSV", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_DEVICE_ALLOW_APPCONTAINER_TRAVERSAL) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_DEVICE_ALLOW_APPCONTAINER_TRAVERSAL", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_PORTABLE_DEVICE) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_PORTABLE_DEVICE", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_REMOTE_DEVICE_VSMB) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_REMOTE_DEVICE_VSMB", _TRUNCATE);
			hasCharacteristics = true;
		}
		if (DeviceCharacteristics & FILE_DEVICE_REQUIRE_SECURITY_CHECK) {
			if (hasCharacteristics) strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), " | ", _TRUNCATE);
			strncat_s(characteristicsBuffer, _countof(characteristicsBuffer), "FILE_DEVICE_REQUIRE_SECURITY_CHECK", _TRUNCATE);
			hasCharacteristics = true;
		}

		// If no known characteristics, just show "NONE"
		if (!hasCharacteristics) {
			strncpy_s(characteristicsBuffer, _countof(characteristicsBuffer), "NONE", _TRUNCATE);
		}

		// Format the string as "DeviceType: 0x{HexValue} ({StringRepresentation}), Characteristics: 0x{HexValue} ({FlagsString})"
		NTSTATUS status = RtlUnicodeStringPrintf(
			DeviceTypeString,
			L"DeviceType: 0x%08X (%hs)\nCharacteristics: 0x%08X (%hs)",
			DeviceType,
			deviceTypeString,
			DeviceCharacteristics,
			characteristicsBuffer);

		return status;
	}

	// Utility function to convert FLT_INSTANCE_SETUP_FLAGS to formatted string
	static NTSTATUS FormatInstanceSetupFlags(_In_ FLT_INSTANCE_SETUP_FLAGS Flags, _Out_ UNICODE_STRING* FlagsString)
	{
		if (!FlagsString) {
			return STATUS_INVALID_PARAMETER;
		}

		// Build the flags string by checking each bit
		char flagsBuffer[256] = "";
		bool hasFlags = false;

		// Check for known FLT_INSTANCE_SETUP_FLAGS
		if (Flags & FLTFL_INSTANCE_SETUP_AUTOMATIC_ATTACHMENT) {
			if (hasFlags) strncat_s(flagsBuffer, _countof(flagsBuffer), " | ", _TRUNCATE);
			strncat_s(flagsBuffer, _countof(flagsBuffer), "FLTFL_INSTANCE_SETUP_AUTOMATIC_ATTACHMENT", _TRUNCATE);
			hasFlags = true;
		}
		if (Flags & FLTFL_INSTANCE_SETUP_MANUAL_ATTACHMENT) {
			if (hasFlags) strncat_s(flagsBuffer, _countof(flagsBuffer), " | ", _TRUNCATE);
			strncat_s(flagsBuffer, _countof(flagsBuffer), "FLTFL_INSTANCE_SETUP_MANUAL_ATTACHMENT", _TRUNCATE);
			hasFlags = true;
		}
		if (Flags & FLTFL_INSTANCE_SETUP_NEWLY_MOUNTED_VOLUME) {
			if (hasFlags) strncat_s(flagsBuffer, _countof(flagsBuffer), " | ", _TRUNCATE);
			strncat_s(flagsBuffer, _countof(flagsBuffer), "FLTFL_INSTANCE_SETUP_NEWLY_MOUNTED_VOLUME", _TRUNCATE);
			hasFlags = true;
		}

		// If no known flags, just show "NONE"
		if (!hasFlags) {
			strncpy_s(flagsBuffer, _countof(flagsBuffer), "NONE", _TRUNCATE);
		}

		// Format the final string as "0x{HexValue} ({FlagsString})" using %hs for char* to wchar_t* conversion
		NTSTATUS status = RtlUnicodeStringPrintf(
			FlagsString,
			L"0x%08X (%hs)",
			Flags,
			flagsBuffer);

		return status;
	}

	// Utility function to convert STORAGE_BUS_TYPE to formatted string
	static NTSTATUS FormatStorageDescriptor(_In_ PSTORAGE_DEVICE_DESCRIPTOR Descriptor, _Out_ UNICODE_STRING* BusTypeString)
	{
		if (!BusTypeString) {
			return STATUS_INVALID_PARAMETER;
		}

		const char* busTypeString = nullptr;

		// Map STORAGE_BUS_TYPE values to string representations
		switch (Descriptor->BusType) {
		case BusTypeUnknown:
			busTypeString = "Unknown";
			break;
		case BusTypeScsi:
			busTypeString = "SCSI";
			break;
		case BusTypeAtapi:
			busTypeString = "ATAPI";
			break;
		case BusTypeAta:
			busTypeString = "ATA";
			break;
		case BusType1394:
			busTypeString = "1394";
			break;
		case BusTypeSsa:
			busTypeString = "SSA";
			break;
		case BusTypeFibre:
			busTypeString = "Fibre";
			break;
		case BusTypeUsb:
			busTypeString = "USB";
			break;
		case BusTypeRAID:
			busTypeString = "RAID";
			break;
		case BusTypeiScsi:
			busTypeString = "iSCSI";
			break;
		case BusTypeSas:
			busTypeString = "SAS";
			break;
		case BusTypeSata:
			busTypeString = "SATA";
			break;
		case BusTypeSd:
			busTypeString = "SD";
			break;
		case BusTypeMmc:
			busTypeString = "MMC";
			break;
		case BusTypeVirtual:
			busTypeString = "Virtual";
			break;
		case BusTypeFileBackedVirtual:
			busTypeString = "FileBackedVirtual";
			break;
		case BusTypeSpaces:
			busTypeString = "Spaces";
			break;
		case BusTypeNvme:
			busTypeString = "NVMe";
			break;
		case BusTypeSCM:
			busTypeString = "SCM";
			break;
		case BusTypeUfs:
			busTypeString = "UFS";
			break;
		default:
			busTypeString = "UNSUPPORTED";
			break;
		}
		
		// Format the string as "{EnumValue} ({StringRepresentation})"
		NTSTATUS status = RtlUnicodeStringPrintf(
			BusTypeString,
			L"Storage Bus Type : %u (%hs), Removable Media? %hs",
			static_cast<ULONG>(Descriptor->BusType),
			busTypeString,
			Descriptor->RemovableMedia ? "Yes" : "No");

		return status;
	}
};