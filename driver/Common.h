#pragma once

enum class PortMessageType {
	VolumeMessage = 1,
	FileMessage = 2,
};

struct PortStringMessage
{
	ULONG dataLenBytes;
    UCHAR data[1]; // Variable length data follows
};

struct PortSectionMessage
{
    ULONG fileSizeBytes;
    HANDLE sectionHandle;
};

struct PortMessage {
	PortMessageType type;

	union
	{
		PortStringMessage stringMsg;
        PortSectionMessage sectionMsg;
	};
};

struct PortReply {
	NTSTATUS status;
};

struct PortReplyMessage {
	FILTER_REPLY_HEADER header;
	PortReply reply;
};

#define FILTER_PORT_NAME L"\\com.snowkoan.volume_info.port"

constexpr ULONG COMMUNICATION_BUFFER_LEN = 1 << 16; // 64 KB
constexpr ULONG PortReplyMessageSize = sizeof(FILTER_REPLY_HEADER) + sizeof(PortReply);
