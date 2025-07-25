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

#pragma once

#include "IMUDataTypes.h"
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Networking.h"
#include "IMUSubsystem.generated.h"

#define DATA_PACKET_BEGIN '#'
#define DATA_PACKET_END   '\r'
#define DATA_OFFSET_BEGIN 0
#define DATA_OFFSET_SIZE  1
#define DATA_OFFSET_TYPE  2
#define DATA_OFFSET_BODY  3

#define CLIENT_OFFSET_ID    0
#define CLIENT_OFFSET_ROT   1
#define CLIENT_OFFSET_EXT   7

#define CMD_PACKET_SIZE     8
#define CMD_ZEROIZE         0x11

/**
 * 
 */
UCLASS()
class IMUCLIENT_API UIMUSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FIMUData> Data;

private:
	FSocket* ListenSocket = nullptr;
	
    /* The address tells us where datagrams came from. */
	TSharedRef<FInternetAddr> SourceAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

    /* The flag indicates whether any datagram received from listening began. */
    bool bSourceBound = false;

    uint8 CommandPacket[CMD_PACKET_SIZE] = { DATA_PACKET_BEGIN };

    void processSubPacket(uint8* data);

    bool sendCommandPacket();

    bool sendCommandCode(uint8 code);
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintCallable)
	bool Listen(const FString& Address = "0.0.0.0", int Port = 1234);

    UFUNCTION(BlueprintCallable)
    bool SendMessage(const FString& Message);

    UFUNCTION(BlueprintCallable)
    bool Zeroize() { return sendCommandCode(CMD_ZEROIZE); }
};
