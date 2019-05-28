// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Engine/TriggerVolume.h" // NOTE! Need to include TriggerVolume.h before...
#include "OpenDoor.generated.h" // NOTE! *generated.h must be last


// This UE MACRO makes it possible to hook a value to a Blueprint
// It makes a class hookable by BluePrints called what is passed as an argument
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOpenRequest);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void OpenDoor();

	void CloseDoor();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// This creates a feature that a Blueprint can hook onto
	// Since the class FOnOpenRequest was created using the macro above...
	// We now instantiate an instance of that class... now look at OpenDoor in .cpp
	UPROPERTY(BlueprintAssignable)
	FOnOpenRequest OnOpenRequest;

private:
	AActor* Owner = GetOwner();

	UPROPERTY(EditAnywhere)
	float OpenAngle = 90.0f;

	UPROPERTY(EditAnywhere)
	ATriggerVolume* PressurePlate = nullptr; // nullptr > undefined

	UPROPERTY(EditAnywhere)
	float DoorCloseDelay = 1.0f;

	float LastDoorOpenTime;

	// Some older code left as an example
	// AActor* ActorThatOpens; // APawn inherits from AActor

	// Returns total mass in kg
	float GetTotalWeightOfActorsOnPlate() const;

};
