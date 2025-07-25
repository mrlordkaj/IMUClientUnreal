/*
 * The MIT License
 *
 * Copyright 2023 Thinh Pham.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "IMUSubsystem.h"

void UIMUSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Data.SetNum(256);
    Listen();
}

void UIMUSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TArray<uint8> recvData;
	uint32 recvSize;
	while (ListenSocket && ListenSocket->HasPendingData(recvSize)) {
		int32 bytesRead = 0;
		recvData.SetNumUninitialized(FMath::Min(recvSize, 65507u));
		bSourceBound = ListenSocket->RecvFrom(recvData.GetData(), recvData.Num(), bytesRead, *SourceAddr);
		//ListenSocket->Recv(recvData.GetData(), recvData.Num(), bytesRead);
        uint16 p = 0;
        while (p < bytesRead) {
            uint8 size = recvData[p++];
            uint8 *data = &recvData[p];
            p += size;
            if (p > bytesRead) {
                // handle corrupted packet
                break;
            }
            processSubPacket(data);
        }
	}
}

void UIMUSubsystem::processSubPacket(uint8* data) {
    const uint8 i = data[CLIENT_OFFSET_ID];
    FIMUData* d = &Data[i];
    // rotation section
    int16 *r = (int16*)&data[CLIENT_OFFSET_ROT];
    d->Rotation = FRotator(-r[0], -r[1], r[2]) * (180 / 32767.f);
    // extra data section
    uint8 p = CLIENT_OFFSET_EXT;
    d->Digitals.SetNum(data[p++]);
    d->Analogs.SetNum(data[p++]);
    // read digital pins
    uint8 numDigitalBytes = d->Digitals.IsEmpty() ? 0 : ((d->Digitals.Num() - 1) / 8) + 1;
    {
        uint8 k = p; // use local pointer
        int8 bit = 7;
        for (uint8 j = 0; j < d->Digitals.Num(); j++) {
            d->Digitals[j] = data[k] & (1 << bit);
            if (--bit < 0) {
                bit = 7;
                k++;
            }
        }
        p += numDigitalBytes;
    }
    // read analog pins
    for (uint8 j = 0; j < d->Analogs.Num(); j++) {
        uint8 l = data[p++];
        uint8 h = data[p++];
        d->Analogs[j] = (h << 8) | l;
    }
}

TStatId UIMUSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UIMUSubsystem, STATGROUP_Tickables);
}

bool UIMUSubsystem::Listen(const FString& Address, int Port)
{
	if (ListenSocket) {
        ListenSocket->Close();
		delete ListenSocket;
		ListenSocket = NULL;
	}
	FIPv4Address addr;
	if (FIPv4Address::Parse(Address, addr)) {
		constexpr int32 bufferSize = 2 * 1024 * 1024;
		ListenSocket = FUdpSocketBuilder(TEXT("IMUListener"))
			.AsNonBlocking()
			.AsReusable()
			.BoundToAddress(addr)
			.BoundToPort(Port)
			.WithReceiveBufferSize(bufferSize)
			.WithSendBufferSize(bufferSize);
	}
    bSourceBound = false;
	return ListenSocket != NULL;
}

bool UIMUSubsystem::SendMessage(const FString& Message)
{
    if (bSourceBound) {
        int32 bytesSent;
        FTCHARToUTF8 utf8(*Message);
        return ListenSocket->SendTo((const uint8*)utf8.Get(), utf8.Length(), bytesSent, *SourceAddr);
    }
    return false;
}

bool UIMUSubsystem::sendCommandPacket()
{
    if (bSourceBound) {
        int32 bytesSent;
        return ListenSocket->SendTo(CommandPacket, CMD_PACKET_SIZE, bytesSent, *SourceAddr);
    }
    return false;
}

bool UIMUSubsystem::sendCommandCode(uint8 code)
{
    CommandPacket[1] = code;
    return sendCommandPacket();
}
