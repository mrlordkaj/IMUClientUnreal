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

#include "IMUSubsystem.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IMUMovementComponent.generated.h"

UCLASS(ClassGroup = (IMUClient), meta = (BlueprintSpawnableComponent))
class IMUCLIENT_API UIMUMovementComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPilotInputDelegate, float, Pitch, float, Yaw, float, Roll);

private:
    UPROPERTY()
    UIMUSubsystem* IMUSystem = nullptr;

    TWeakObjectPtr<USceneComponent> RootComponent = nullptr;

    /* Temporary variable for lagging camera rotation, for previous rotation. */
    FRotator PreviousDesiredRot = FRotator(0, 0, 0);

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    int Channel = -1;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool bEnableRotationLag = true;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (editcondition = "bEnableRotationLag", UIMin = "0.0", UIMax = "1000.0", ClampMin = "0.0", ClampMax = "1000.0"))
    float RotationLagSpeed = 10;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay)
    bool bUseLagSubstepping = true;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, AdvancedDisplay, meta = (editcondition = "bUseLagSubstepping", ClampMin = "0.005", ClampMax = "0.5", UIMin = "0.005", UIMax = "0.5"))
    float LagMaxTimeStep = 1.f / 60.f;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool bPilotMode = false;

    UPROPERTY(BlueprintReadWrite)
    FRotator OffsetRotation;

    UPROPERTY(BlueprintReadOnly)
    FRotator TargetRotation;

private:
	UPROPERTY(EditAnywhere, meta = (ClampMin = "-180.0", ClampMax = "180.0", UIMin = "-180.0", UIMax = "180.0"))
	FVector2D PitchMapping = FVector2D(-90, 90);

	UPROPERTY(EditAnywhere, meta = (ClampMin = "-180.0", ClampMax = "180.0", UIMin = "-180.0", UIMax = "180.0"))
	FVector2D YawMapping = FVector2D(-90, 90);

	UPROPERTY(EditAnywhere, meta = (ClampMin = "-180.0", ClampMax = "180.0", UIMin = "-180.0", UIMax = "180.0"))
	FVector2D RollMapping = FVector2D(-90, 90);
	
	UPROPERTY(BlueprintAssignable)
	FPilotInputDelegate OnPilotInput;

public:
	UIMUMovementComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
