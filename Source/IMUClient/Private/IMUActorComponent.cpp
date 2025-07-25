// Fill out your copyright notice in the Description page of Project Settings.


#include "IMUActorComponent.h"
#include "Kismet/KismetMathLibrary.h"

UIMUActorComponent::UIMUActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UIMUActorComponent::BeginPlay()
{
	Super::BeginPlay();
    IMUSystem = GWorld->GetSubsystem<UIMUSubsystem>();
}

void UIMUActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    for (const FIMUDevice& dev : Devices) {
        if (dev.Channel >= 0 && dev.Bone != NULL) {
            FIMUData* d = &IMUSystem->Data[dev.Channel];
            FRotator r;
            if (dev.Bone->bFlipped) {
                r.Roll = -d->Rotation.Roll;
                r.Pitch = -d->Rotation.Pitch;
            }
            else {
                r.Roll = d->Rotation.Roll;
                r.Pitch = d->Rotation.Pitch;
            }
            r.Yaw = d->Rotation.Yaw;
            dev.Bone->Rotation = UKismetMathLibrary::ComposeRotators(dev.Bone->Offset, r);
        }
    }
}

void UIMUActorComponent::BindBone(UPARAM(ref) FIMUBone& Target)
{
    for (FIMUDevice& dev : Devices) {
        if (dev.Name == Target.Socket) {
            dev.Bone = &Target;
            return;
        }
    }
}

void UIMUActorComponent::UnbindBone(FName Socket)
{
    for (FIMUDevice& dev : Devices) {
        if (dev.Name == Socket) {
            dev.Bone = NULL;
            return;
        }
    }
}