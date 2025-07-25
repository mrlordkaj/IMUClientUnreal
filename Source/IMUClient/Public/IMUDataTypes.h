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

#include "CoreMinimal.h"
#include "IMUDataTypes.generated.h"

USTRUCT(BlueprintType)
struct FIMUData
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    int Channel = -1;

    UPROPERTY(EditAnywhere)
    FText Description;

	UPROPERTY(VisibleInstanceOnly)
	FRotator Rotation = FRotator(0, 0, 0);

    UPROPERTY(VisibleInstanceOnly)
    TArray<bool> Digitals;

    UPROPERTY(VisibleInstanceOnly)
    TArray<int> Analogs;
};

USTRUCT(BlueprintType)
struct FIMUBone
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly)
    FName Socket;

    UPROPERTY(EditDefaultsOnly)
    bool bFlipped = false;

    UPROPERTY(BlueprintReadWrite)
    FRotator Offset = FRotator(0, 0, 0);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FRotator Rotation = FRotator(0, 0, 0);
};

USTRUCT(BlueprintType)
struct FIMUDevice
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    FName Name;

    UPROPERTY(EditAnywhere)
    int Channel = -1;

    /* Pointer to the bound bone. */
    FIMUBone* Bone = NULL;
};

USTRUCT(BlueprintType)
struct FIMUSceneSettings
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (UIMin = "0", UIMax = "180", ClampMin = "0", ClampMax = "180"))
    float RotationLagSpeed = 0;
};
