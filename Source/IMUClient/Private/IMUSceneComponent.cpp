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


#include "IMUSceneComponent.h"

UIMUSceneComponent::UIMUSceneComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UIMUSceneComponent::BeginPlay()
{
	Super::BeginPlay();

	IMUSystem = GWorld->GetSubsystem<UIMUSubsystem>();
}

void UIMUSceneComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (Channel >= 0) {
        FIMUData* d = &IMUSystem->Data[Channel];
        // update rotation
        if (RotationLagSpeed > 0) {
            FRotator DesiredRot = FRotator(FMath::QInterpTo(FQuat(PreviousDesiredRot), FQuat(d->Rotation), DeltaTime, RotationLagSpeed));
            PreviousDesiredRot = DesiredRot;
            SetRelativeRotation(DesiredRot);
        }
        else {
            SetRelativeRotation(d->Rotation);
        }
        Analogs = d->Analogs;
        Buttons = d->Digitals;
        PrevButtons.SetNum(Buttons.Num());
        for (uint8_t i = 0; i < Buttons.Num(); i++) {
            if (Buttons[i] != PrevButtons[i]) {
                if (Buttons[i]) {
                    OnButtonPressed.Broadcast(i);
                }
                else {
                    OnButtonReleased.Broadcast(i);
                }
            }
        }
        PrevButtons = Buttons;
    }
}
