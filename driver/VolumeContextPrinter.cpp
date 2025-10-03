#include "pch.h"

#include "VolumeContextPrinter.h"
#include "VolumeContext.h"

void VolumeContextPrinter::SendVolumeInfo(const VolumeContext* vol)
{	
	constexpr ULONG buffer_size = 1024;
	UNICODE_STRING tmp = {};
    tmp.Buffer = (wchar_t*)ExAllocatePool2(POOL_FLAG_NON_PAGED, 1024, VOLUME_POOLTAG);

	if (tmp.Buffer)
	{
        tmp.MaximumLength = buffer_size;
		tmp.Length = 0;
        FormatFilesystemType(vol->filesystem_type_, &tmp);

		CommunicationPort::Instance()->SendOutputMessage(PortMessageType::VolumeMessage,
			L"\n--- New Volume Info: %wZ ---\nFileSystemDriverName: %wZ (DeviceName: %wZ)\n%wZ",
			&vol->volume_name_,
			&vol->volume_properties_->FileSystemDriverName,
			&vol->volume_properties_->FileSystemDeviceName,
			&tmp);

		tmp.Length = 0;
        FormatDeviceDetails(vol->volume_properties_, &tmp);
		CommunicationPort::Instance()->SendOutputMessage(PortMessageType::VolumeMessage,L"%wZ", &tmp);

		tmp.Length = 0;
		if (vol->storage_descriptor_)
		{
            FormatStorageDescriptor(vol->storage_descriptor_, &tmp);
			CommunicationPort::Instance()->SendOutputMessage(PortMessageType::VolumeMessage, L"%wZ", &tmp);
		}

		ExFreePool(tmp.Buffer);
		tmp.Buffer = nullptr;
	}

}