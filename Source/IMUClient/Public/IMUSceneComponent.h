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
#include "Components/SceneComponent.h"
#include "Networking.h"
#include "IMUSubsystem.h"
#include "IMUSceneComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FIMUButtonEvent, int, Index);

UCLASS(ClassGroup = (IMUClient), meta = (BlueprintSpawnableComponent)/*, Blueprintable*/)
class IMUCLIENT_API UIMUSceneComponent : public USceneComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UIMUSubsystem* IMUSystem = nullptr;

    /** Temporary variable for button states changed detection. */
    TArray<bool> PrevButtons;

    /** Temporary variable for lagging camera rotation, for previous rotation. */
    FRotator PreviousDesiredRot = FRotator(0, 0, 0);

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Channel = -1;

	UPROPERTY(BlueprintAssignable)
	FIMUButtonEvent OnButtonPressed;

	UPROPERTY(BlueprintAssignable)
	FIMUButtonEvent OnButtonReleased;

	UPROPERTY(BlueprintReadOnly)
	TArray<bool> Buttons;

	UPROPERTY(BlueprintReadOnly)
	TArray<int> Analogs;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (UIMin = "0", UIMax = "180", ClampMin = "0", ClampMax = "180"))
    float RotationLagSpeed = 0;

public:	
    UIMUSceneComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
