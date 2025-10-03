#pragma once

#include "VolumeContextPrinter.h"
#include "pooltag.h"

class VolumeContext {

public:

	UNICODE_STRING volume_name_ = {};
    PFLT_VOLUME_PROPERTIES volume_properties_ = {};
	PFLT_INSTANCE flt_instance_ = {}; // We only ever attach once. What could go wrong?
    PSTORAGE_DEVICE_DESCRIPTOR storage_descriptor_ = {};
    FLT_INSTANCE_SETUP_FLAGS instance_setup_flags_ = {};
    FLT_FILESYSTEM_TYPE filesystem_type_ = {};

	VolumeContext()
	{
	}

	~VolumeContext()
	{
		if (volume_name_.Buffer)
		{
			ExFreePoolWithTag(volume_name_.Buffer, VOLUME_POOLTAG);
			volume_name_.Buffer = nullptr;
		}

		if (volume_properties_)
		{
			ExFreePoolWithTag(volume_properties_, VOLUME_POOLTAG);
			volume_properties_ = nullptr;
		}

        if (storage_descriptor_)
        {
            ExFreePoolWithTag(storage_descriptor_, VOLUME_POOLTAG);
            storage_descriptor_ = nullptr;
        }
	}

    static NTSTATUS Factory(_In_ PCFLT_RELATED_OBJECTS FltObjects, 
        _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
        _In_ FLT_FILESYSTEM_TYPE VolumeFileSystemType,
        _Out_ VolumeContext** Context)
    {
        *Context = nullptr;

        auto status = FltAllocateContext(FltObjects->Filter,
            FLT_VOLUME_CONTEXT, 
			sizeof(VolumeContext), 
			PagedPool,
            (PFLT_CONTEXT*)Context);

        if (!NT_SUCCESS(status)) {
            return status;
        }

        VolumeContext* vol = *Context;

        // Placement new
        new (vol) VolumeContext();

        vol->instance_setup_flags_ = Flags;
        vol->filesystem_type_ = VolumeFileSystemType;

		// Do this outside of constructor so that we can return status
        if (!NT_SUCCESS(status = vol->GetVolumeName(FltObjects->Volume, (*Context)->volume_name_))) {
            FltReleaseContext(vol);
            *Context = nullptr;
            return status;
        }

        if (!NT_SUCCESS(status = vol->GetVolumeProperties(FltObjects->Volume, &vol->volume_properties_))) {
            FltReleaseContext(vol);
            *Context = nullptr;
            return status;
        }

		vol->flt_instance_ = FltObjects->Instance;

        // Best effort, ignore failure
        GetStorageDeviceDescriptor(FltObjects, &vol->storage_descriptor_);

        VolumeContextPrinter::SendVolumeInfo(vol);          

		return STATUS_SUCCESS;
    }

	static NTSTATUS SetContext(PCFLT_RELATED_OBJECTS FltObjects, VolumeContext* Context, bool Replace)
	{
		VolumeContext* OldContext = nullptr;

		auto status = FltSetVolumeContext(FltObjects->Volume,
			Replace ? FLT_SET_CONTEXT_REPLACE_IF_EXISTS : FLT_SET_CONTEXT_KEEP_IF_EXISTS,
			Context,
			reinterpret_cast<PFLT_CONTEXT*>(&OldContext));

		if (OldContext)
		{
			FltReleaseContext(OldContext);
		}

		return status;
	}

	// This is essentially our destructor, called by Filter Manager when the context is deleted
    static void Cleanup(_In_ PFLT_CONTEXT Context, _In_ FLT_CONTEXT_TYPE ContextType)
    {
        if (ContextType != FLT_VOLUME_CONTEXT)
        {
            ASSERT(FALSE);
            return;
        }

        auto ctx = reinterpret_cast<VolumeContext*>(Context);
		ctx->~VolumeContext();

        return;
    }

	static NTSTATUS GetVolumeContextFromFileObject(PFLT_FILTER Filter, PFILE_OBJECT FileObject, _Out_ VolumeContext** Context)
	{
		*Context = nullptr;
		PFLT_VOLUME Volume = {};
		NTSTATUS status = FltGetVolumeFromFileObject(Filter, FileObject, &Volume);
        if (NT_SUCCESS(status)) {
			status = FltGetVolumeContext(Filter, Volume, reinterpret_cast<PFLT_CONTEXT*>(Context));
			FltObjectDereference(Volume);
        }

		return status;
	}

private:

	static NTSTATUS GetVolumeName(_In_ PFLT_VOLUME Volume, _Out_ UNICODE_STRING& VolumeName)
	{
		auto status = STATUS_SUCCESS;
		VolumeName = { 0 };

		// Get the volume name
		do
		{
			// Step 1: First call to FltGetVolumeName to determine the required buffer size.
			// We expect STATUS_BUFFER_TOO_SMALL, and returnedLength will be updated.
			ULONG returnedLength = 0;
			status = FltGetVolumeName(
				Volume,
				NULL, // No buffer provided
				&returnedLength
			);

			// We expect STATUS_BUFFER_TOO_SMALL here. If it's another error, get out.
			if (status != STATUS_BUFFER_TOO_SMALL) {
				break;
			}

			VolumeName.Buffer = (wchar_t*)ExAllocatePool2(POOL_FLAG_NON_PAGED, returnedLength, VOLUME_POOLTAG);
			if (!VolumeName.Buffer) {
				break;
			}
			VolumeName.MaximumLength = static_cast<USHORT>(returnedLength);

			// Step 2. Get the name
			status = FltGetVolumeName(Volume,
				&VolumeName,
				nullptr);

			if (!NT_SUCCESS(status)) {
				break;
			}

		} while (false);

		if (!NT_SUCCESS(status)) {
			if (VolumeName.Buffer) {
				ExFreePoolWithTag(VolumeName.Buffer, VOLUME_POOLTAG);
				VolumeName.Buffer = nullptr;
			}
		}

		return status;
	}

	static NTSTATUS GetVolumeProperties(_In_ PFLT_VOLUME Volume, _Out_ PFLT_VOLUME_PROPERTIES* Properties)
	{
		*Properties = nullptr;

        ULONG returnedLength = 0;
        auto status = FltGetVolumeProperties(Volume, 
			nullptr,
			0,
			&returnedLength);

        if (status != STATUS_BUFFER_TOO_SMALL) {
            return status;
        }

		*Properties = (PFLT_VOLUME_PROPERTIES)ExAllocatePool2(POOL_FLAG_NON_PAGED, returnedLength, VOLUME_POOLTAG);

		if (nullptr == *Properties)
		{
			return STATUS_INSUFFICIENT_RESOURCES;
		}

        status = FltGetVolumeProperties(Volume,
            *Properties,
            returnedLength,
            &returnedLength);       

		if (!NT_SUCCESS(status))
		{
            CommunicationPort::Instance()->SendOutputMessage(PortMessageType::VolumeMessage, L"FltGetVolumeProperties failed (0x%08X)\n", status);
			ExFreePoolWithTag(*Properties, VOLUME_POOLTAG);
			*Properties = nullptr;
		}

		return status;
	}

    static NTSTATUS GetStorageDeviceDescriptor(_In_ PCFLT_RELATED_OBJECTS FltObjects,
        _Out_ PSTORAGE_DEVICE_DESCRIPTOR* DeviceDescriptor)
    {
        NTSTATUS status = {};
        *DeviceDescriptor = {};

        STORAGE_PROPERTY_QUERY storage_query = { 0 };
        storage_query.PropertyId = StorageDeviceProperty;
        storage_query.QueryType = PropertyStandardQuery;

        PFILE_OBJECT volume_fo = {};
        HANDLE volume_handle = {};

        do {
            // This will return STATUS_INVALID_PARAMETER for network volumes
            status = FltOpenVolume(FltObjects->Instance, &volume_handle, &volume_fo);
            if (!NT_SUCCESS(status)) {
                return status;
            }

            STORAGE_DESCRIPTOR_HEADER header = { 0 };
            ULONG returned_length = 0;

            // This is a weird IOCTL - it returns STATUS_SUCCESS even if the buffer is too small.
            // That's fine for us, we don't care about the variable length part.
            status = FltDeviceIoControlFile(
                FltObjects->Instance,
                volume_fo,
                IOCTL_STORAGE_QUERY_PROPERTY,
                &storage_query,
                sizeof(STORAGE_PROPERTY_QUERY),
                &header,
                sizeof(header),
                &returned_length
            );

            if (!NT_SUCCESS(status))
            {
                CommunicationPort::Instance()->SendOutputMessage(PortMessageType::VolumeMessage, L"IOCTL_STORAGE_QUERY_PROPERTY (1) failed (0x%08X)\n", status);
                break;
            }

            *DeviceDescriptor = (PSTORAGE_DEVICE_DESCRIPTOR)ExAllocatePool2(POOL_FLAG_NON_PAGED, header.Size, VOLUME_POOLTAG);
            if (!*DeviceDescriptor) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                break;
            }

            status = FltDeviceIoControlFile(
                FltObjects->Instance,
                volume_fo,
                IOCTL_STORAGE_QUERY_PROPERTY,
                &storage_query,
                sizeof(STORAGE_PROPERTY_QUERY),
                *DeviceDescriptor,
                header.Size,
                &returned_length);

            if (!NT_SUCCESS(status))
            {
                CommunicationPort::Instance()->SendOutputMessage(PortMessageType::VolumeMessage, L"IOCTL_STORAGE_QUERY_PROPERTY (2) failed (0x%08X)\n", status);
                break;
            }

        } while (false);

        if (!NT_SUCCESS(status))
        {
            if (*DeviceDescriptor) {
                ExFreePoolWithTag(*DeviceDescriptor, VOLUME_POOLTAG);
                *DeviceDescriptor = nullptr;
            }
        }

        if (volume_fo) {
            ObDereferenceObject(volume_fo);
            volume_fo = nullptr;
        }

        if (volume_handle) {
            FltClose(volume_handle);
            volume_handle = nullptr;
        }

        return status;
    }
};