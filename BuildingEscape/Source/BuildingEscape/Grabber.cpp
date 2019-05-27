// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Classes/Components/PrimitiveComponent.h"
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	// UE_LOG(LogTemp, Warning, TEXT("Grabber reporting for duty!"));
	FindPhysicsHandleComponent();

	SetupInputComponent();
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/// Line trace once
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(GetReachLineEnd());
	}

}

void UGrabber::Grab() {
	// UE_LOG(LogTemp, Warning, TEXT("Grab pressed"));
	/// Line Trace and see if we reach any actors with physics body collision channel set
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();

	// TODO attach physics handle
	if (ActorHit) // Only try to grab if an actor was hit.
	{
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab, // turns out to be the mesh
			NAME_None, // Bone to grab. Not needed since we're using static mesh
			HitResult.Location, // location to put grabbed object
			FRotator::ZeroRotator // handles rotation of grabbed actor
		);
	}
	
}

void UGrabber::Release() {
	// UE_LOG(LogTemp, Warning, TEXT("Grab released"));
	// TODO release physics handle
	PhysicsHandle->ReleaseComponent();
}

void UGrabber::FindPhysicsHandleComponent() {
	/// Look for attached Physics Handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No Physics Handle found for %s"), *(GetOwner()->GetName()));
	}
}

void UGrabber::SetupInputComponent() {
	/// Look for attached Input Component
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("No Input Component found for %s"), *(GetOwner()->GetName()));
	}
	else
	{
		/// Bind the input axis
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const {
	/// Line-trace (AKA ray-cast) out to reach distance
	FHitResult LineTraceHitResult;
	// the false decides between simple or complex collision volumes
	// GetOwner() is the AActor we want this method to ignore
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		OUT LineTraceHitResult,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	/*AActor* ActorHit = LineTraceHitResult.GetActor();
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *ActorHit->GetName());
	}
	FString HitActorName = LineTraceHitResult.GetActor()->GetName();*/
	return LineTraceHitResult;
}

FVector UGrabber::GetReachLineStart() const {
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	// Note that GetPlayerViewPoint and GetPlayerViewpoint are different
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation);

	return PlayerViewPointLocation;
}

FVector UGrabber::GetReachLineEnd() const {
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	// Note that GetPlayerViewPoint and GetPlayerViewpoint are different
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation);

	// report x, y, z coordinates of actor with this Grabber component.
	/*UE_LOG(LogTemp, Warning, TEXT("Logcation: %s, Position %s"),
		*PlayerViewPointLocation.ToString(),
		*PlayerViewPointRotation.ToString())*/

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	/*DrawDebugLine(
		GetWorld(), PlayerViewPointLocation, LineTraceEnd, FColor(255, 0, 0),
		false, 0.0f, 0.0f, 5.0f
	);*/

	return LineTraceEnd;
}

