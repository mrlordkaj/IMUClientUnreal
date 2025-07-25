// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IMUSubsystem.h"
#include "IMUActorComponent.generated.h"


UCLASS( DisplayName = "IMUComponent", ClassGroup = (IMUClient), meta = (BlueprintSpawnableComponent) )
class IMUCLIENT_API UIMUActorComponent : public UActorComponent
{
	GENERATED_BODY()

private:
    UPROPERTY()
    UIMUSubsystem* IMUSystem = nullptr;

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    TArray<FIMUDevice> Devices;

public:
	UIMUActorComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable)
    void BindBone(UPARAM(ref) FIMUBone& Target);

    UFUNCTION(BlueprintCallable)
    void UnbindBone(FName Socket);
};
