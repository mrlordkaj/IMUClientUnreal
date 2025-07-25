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

#include "IMUMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UIMUMovementComponent::UIMUMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
    bAutoActivate = true;
}

void UIMUMovementComponent::BeginPlay()
{
	Super::BeginPlay();
    IMUSystem = GWorld->GetSubsystem<UIMUSubsystem>();
    RootComponent = GetOwner()->GetRootComponent();
}

void UIMUMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    // Do nothing during inactive status
    if (Channel < 0 || !IsValid(IMUSystem) || !IsActive()) return;
    
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Update target rotation
    TargetRotation = IMUSystem->Data[Channel].Rotation;
    FRotator desiredRot = TargetRotation;
    if (bEnableRotationLag && RotationLagSpeed > 0) {
        if (bUseLagSubstepping && DeltaTime > LagMaxTimeStep) {
            const FRotator rotStep = (desiredRot - PreviousDesiredRot).GetNormalized() * (1 / DeltaTime);
            FRotator lerpTarget = PreviousDesiredRot;
            float remainingTime = DeltaTime;
            while (remainingTime > UE_KINDA_SMALL_NUMBER) {
                const float lerpAmount = FMath::Min(LagMaxTimeStep, remainingTime);
                lerpTarget += rotStep * lerpAmount;
                remainingTime -= lerpAmount;
                desiredRot = FRotator(FMath::QInterpTo(FQuat(PreviousDesiredRot), FQuat(lerpTarget), lerpAmount, RotationLagSpeed));
                PreviousDesiredRot = desiredRot;
            }
        }
        else {
            desiredRot = FRotator(FMath::QInterpTo(FQuat(PreviousDesiredRot), FQuat(desiredRot), DeltaTime, RotationLagSpeed));
            PreviousDesiredRot = desiredRot;
        }
    }
    
    if (bPilotMode)
    {
        // Notify rotation data as controller's input
        const FVector2D output(-1, 1);
        float p = FMath::GetMappedRangeValueClamped(PitchMapping, output, desiredRot.Pitch);
        float y = FMath::GetMappedRangeValueClamped(YawMapping, output, desiredRot.Yaw);
        float r = FMath::GetMappedRangeValueClamped(RollMapping, output, desiredRot.Roll);
        OnPilotInput.Broadcast(p, y, r);
    }
    else if (RootComponent.IsValid()) {
        // Auto-update the assigned component
        desiredRot = UKismetMathLibrary::ComposeRotators(desiredRot, OffsetRotation);
        RootComponent->SetRelativeRotation(desiredRot);
    }
}
