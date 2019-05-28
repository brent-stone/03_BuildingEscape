// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Engine/Classes/Components/PrimitiveComponent.h"




// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	// ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
	
}

void UOpenDoor::OpenDoor()
{
	// Old code which was used prior to hooking a Blueprint. Insta-Opens door to 90 degrees.
	//Owner->SetActorRotation(FRotator(0.0f, OpenAngle, 0.0f));

	// This is necessary to hook in a Blueprint. Events must be broadcast.
	OnOpenRequest.Broadcast();
}

void UOpenDoor::CloseDoor()
{
	Owner->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Poll the Trigger Volume
	// Some older code left as an example
	// if (PressurePlate->IsOverlappingActor(ActorThatOpens))
	if (GetTotalWeightOfActorsOnPlate() > 30.0f) // TODO lower limit into a parameter
	{
		OpenDoor();
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}

	if (GetWorld()->GetTimeSeconds() - LastDoorOpenTime >= DoorCloseDelay)
	{
		CloseDoor();
	}
	
}

float UOpenDoor::GetTotalWeightOfActorsOnPlate() const
{
	float TotalWeight = 0.0f;
	// Find all the overlapping actors
	TArray<AActor*> OverlappingActors; // GetOverlappingAcotrs also works with TSet
	PressurePlate->GetOverlappingActors(OUT OverlappingActors); 

	// Iterate through them to find sum mass
	for (const auto* Actor : OverlappingActors) // iterate thorugh OverlappingActors
	{
		FString ThisName = Actor->GetName();
		TotalWeight += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		UE_LOG(LogTemp, Warning, TEXT("%s now on pressure plate"), *Actor->GetName());
	}

	return TotalWeight;
}
