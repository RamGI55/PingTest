// Fill out your copyright notice in the Description page of Project Settings.


#include "PingSystem/TP_PingComponent.h"
#include "PingTest/Public/PingSystem/TP_PingComponent.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PingTest/PingTestCharacter.h"
#include "InputAction.h"
#include "InputMappingContext.h" 
#include "InputActionValue.h"
#include "Enemy/EnemyCharacter.h"

// Need the delegate for the bhit - send to the enemy character to enable outline. 

// Sets default values for this component's properties
UTP_PingComponent::UTP_PingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...
	PingAction = LoadObject<UInputAction>(nullptr, TEXT("/Game/FirstPerson/Input/Actions/IA_Ping.IA_Ping"));
	InputContext = LoadObject<UInputMappingContext>(nullptr, TEXT("/Game/FirstPerson/Input/IMC_Pings.IMC_Pings"));
	
}

void UTP_PingComponent::InitComponent()
{
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (InputContext)
			{
				Subsystem->AddMappingContext(InputContext, 1);
			}
		}
	}
	
}

void UTP_PingComponent::BeginPlay()
{
	Super::BeginPlay();
	InitComponent();  // ← This adds the mapping context
}

void UTP_PingComponent::SetupInputComponent(class UInputComponent* PlayerInputComponent)
{

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (PingAction)
		{
			EnhancedInputComponent->BindAction(PingAction, ETriggerEvent::Started, this, &UTP_PingComponent::Ping);
			EnhancedInputComponent->BindAction(PingAction, ETriggerEvent::Canceled, this, &UTP_PingComponent::StopDynamicPing); 
			EnhancedInputComponent->BindAction(PingAction, ETriggerEvent::Completed, this, &UTP_PingComponent::StopDynamicPing); 
			
		}
	} 
}

void UTP_PingComponent::StartDynamicPing()
{
	// Hit -> Send Delegation to ping
	
	
}

void UTP_PingComponent::StopDynamicPing()
{
	if (!GetWorld()) { return; };
	GetWorld()->GetTimerManager().ClearTimer(PingTimerHandle);
	for (AEnemyCharacter* Enemy : PingedEnemies)
	{
		if (Enemy)
		{
			Enemy->SetSpotted(GetOwner(), false); // Reset the enemy spotted state
			OnEnemyLost.Broadcast(HitEnemy);
		}
	}
	PingedEnemies.Empty();
	LostEnemies.Empty();
	
}

void UTP_PingComponent::UpdateDynamicPing()
{
	PingedEnemies.Empty();
	AActor* Owner = GetOwner();
	if (!Owner) { return; }

	GetEnemiesInCrosshair(PingedEnemies);
	// Dynamic Crosshair ping logic can be added here
	for (int32 i = 0; i < PingedEnemies.Num(); i++)
	{
		if (PingedEnemies[i])
		{
			FString EnemyName = FString::Printf(TEXT("SPOTTED: %s"), *PingedEnemies[i]->GetName());
			GEngine->AddOnScreenDebugMessage(10 + i, 0.1f, FColor::Red, EnemyName);
		}
	}
	ProcessPingedEnemy();
}

bool UTP_PingComponent::GetEnemiesInCrosshair(TArray<AEnemyCharacter*>& OutEnemies) const
{
	OutEnemies.Empty();

	APawn* PlayerPawn = Cast<APawn>(GetOwner());
	if (!PlayerPawn) { return false; }

	APlayerController* PlayerController = Cast<APlayerController>(PlayerPawn->GetController());
	if (!PlayerController) { return false; }

	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
	FVector2D ViewportCenter(ViewportSizeX / 2.0f, ViewportSizeY / 2.0f);

	for (TActorIterator<AEnemyCharacter> ActorIter(GetWorld()); ActorIter; ++ActorIter)
	{
		AEnemyCharacter* Enemy = *ActorIter;
		if (!Enemy || !Enemy->IsValidLowLevel()) { continue; }
		
		if (isEnemyNearCrosshair(Enemy, ViewportCenter, PlayerController)) // check the bHit, is bhit true? - has been hit enemy? -> then send it valid enemy. 
		{
			OutEnemies.Add(Enemy);
			UE_LOG(LogTemp, VeryVerbose, TEXT("Enemy %s is within crosshair tolerance."), *Enemy->GetName());
		}
		else
		{
			UE_LOG(LogTemp, VeryVerbose, TEXT("Enemy %s is NOT within crosshair tolerance."), *Enemy->GetName());
		}
	}

	return OutEnemies.Num() > 0; 
}

bool UTP_PingComponent::isEnemyNearCrosshair(AEnemyCharacter*& OutEnemy, const FVector2D& CrosshairPos,
	APlayerController* PC) const
{
	if (!OutEnemy || !PC) { return false; }
	FVector EnemyWorldLocation = OutEnemy->GetActorLocation();

	APawn* PlayerPawn = PC->GetPawn();
	if (PlayerPawn)
	{
		float DistanceToPlayer = FVector::Dist(PlayerPawn->GetActorLocation(), EnemyWorldLocation);
		if (DistanceToPlayer > PingRange)
		{
			return false;
		}
	}
	
	FVector2D EnemyScreenPos; 
	bool bIsOnScreen = PC->ProjectWorldLocationToScreen(EnemyWorldLocation, EnemyScreenPos, true); 
	
	if (!bIsOnScreen) { return false; }

	float ScreenDistance = FVector2D::Distance(EnemyScreenPos, CrosshairPos);
	UE_LOG(LogTemp, VeryVerbose, TEXT("OPTION 2: Enemy %s at screen pos: (%.1f, %.1f), Distance from crosshair: %.1f px"), 
		   *OutEnemy->GetName(), EnemyScreenPos.X, EnemyScreenPos.Y, ScreenDistance);
	if (ScreenDistance > CrosshairTolerancePixels)
	{
		return false; 
	}

	return true; 
}

void UTP_PingComponent::Ping()
{
	if (!GetWorld()) { return; }; 
	GetWorld()->GetTimerManager().SetTimer(PingTimerHandle,
		this,
		&UTP_PingComponent::UpdateDynamicPing,
		.5f,
		true,
		0.1f); 
}

// Using the linetracing to pint the enemy characters. 

void UTP_PingComponent::LineTraceForPing()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	check(PlayerController);
	if (PlayerController)
	{
		// use the Capsule ping the actor. 
		FVector CameraLocation;
		FRotator CameraRotation;
		//float PingRange = 5000.0f; 
		PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector TraceStart = CameraLocation;
		FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * PingRange);

		//FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(GetOwner()); // Ignore the player

		bool bHit = GetWorld()->LineTraceMultiByChannel(
			HitResult,
			TraceStart,
			TraceEnd,
			ECC_WorldDynamic,  // Use Visibility channel for the line trace
			QueryParams
		);
		
		if (bHit)
		{
			for (const FHitResult& Hit : HitResult)
			{
				if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Hit.GetActor()))
				{
					PingedEnemies.AddUnique(Enemy);
				}
			}
		}
	}
 ProcessPingedEnemy(); 
	
}

void UTP_PingComponent::ProcessPingedEnemy()
{
	AActor* Spotter = GetOwner();
	if (!Spotter) { return; }
	
	for (AEnemyCharacter* CurrentEnemy : PingedEnemies)
	{
		if (CurrentEnemy && !LostEnemies.Contains(CurrentEnemy))
		{
			CurrentEnemy->SetSpotted(Spotter, true);
			// Process the pinged enemy character
			OnEnemyPinged.Broadcast(CurrentEnemy);
			// Optionally, you can add more logic here to handle the pinged enemy
		}
	} 
	for (AEnemyCharacter* CurrentEnemy : LostEnemies)
	{
		if (CurrentEnemy && !PingedEnemies.Contains(CurrentEnemy))
		{
			CurrentEnemy->SetSpotted(Spotter, false);
			OnEnemyLost.Broadcast(CurrentEnemy);
			// Optionally, you can add more logic here to handle the lost enemy
		}
	}
	LostEnemies = PingedEnemies; 
}


