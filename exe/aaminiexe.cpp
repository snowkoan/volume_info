// This file contains the 'main' function. Program execution begins and ends there.
//

#define WIN32_NO_STATUS // Avoid name clash in ntstatus.h
#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <fltUser.h>
#include <stdio.h>
#include <string>
#include "..\driver\Common.h"

#pragma comment(lib, "fltlib")

NTSTATUS HandleMessage(const BYTE* buffer, bool& reply) 
{
	static ULONG blockCount = 0;
	auto msg = (PortMessage*)buffer;
	auto status = STATUS_SUCCESS;
	reply = false;

	switch (msg->type)
	{
	case PortMessageType::VolumeMessage:
	case PortMessageType::FileMessage:
	{
		std::wstring output_string(reinterpret_cast<wchar_t*>(msg->stringMsg.data), msg->stringMsg.dataLenBytes / sizeof(wchar_t));
        bool needsNewline = !output_string.empty() && output_string.back() != L'\n';
        wprintf(L"%s%s", output_string.c_str(), needsNewline ? L"\n" : L"");
		break;
	}
	default:
	{
		printf("Unknown message type");
		break;
	}
	}

	return status;
}

int wmain(const int argc, const wchar_t* argv[]) 
{
	HANDLE hPort;
	auto hr = FilterConnectCommunicationPort(FILTER_PORT_NAME, 0, nullptr, 0, nullptr, &hPort);
	if (FAILED(hr)) 
	{
		wprintf(L"Error connecting to port %ls. (HR=0x%08X)\n", FILTER_PORT_NAME, hr);
		return 1;
	}
    wprintf(L"Connected to port %ls. Waiting for messages.\n", FILTER_PORT_NAME);

	BYTE buffer[COMMUNICATION_BUFFER_LEN];	// 4 KB
	auto message = (FILTER_MESSAGE_HEADER*)buffer;

	for (;;) {
		hr = FilterGetMessage(hPort, message, sizeof(buffer), nullptr);

		if (FAILED(hr)) 
		{
			wprintf(L"Error receiving message (0x%08X)\n", hr);
			break;
		}

		bool reply = false;
		auto status = HandleMessage(buffer + sizeof(FILTER_MESSAGE_HEADER), reply);

		if (reply)
		{
			PortReplyMessage replyMsg = {};		
			replyMsg.header.Status = STATUS_SUCCESS;
			replyMsg.header.MessageId = message->MessageId;
			replyMsg.reply.status = status;

			hr = FilterReplyMessage(hPort,
				reinterpret_cast<PFILTER_REPLY_HEADER>(&replyMsg),
				PortReplyMessageSize);

            if (FAILED(hr))
            {
                wprintf(L"Error replying to message (0x%08X)\n", hr);
				continue; // not fatal
            } 
		}
	}

	CloseHandle(hPort);

	return 0;
}

