// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/EnemyCharacter.h"
#include "PingTest/Public/Enemy/EnemyCharacter.h"

#include "Components/CapsuleComponent.h"
#include "PingSystem/TP_PingComponent.h"
#include "PingTest/PingTestCharacter.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{

	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = World->GetFirstPlayerController())
		{
			if (APingTestCharacter* PlayerCharacter = Cast<APingTestCharacter>(PC->GetPawn()))
			{
				if (UTP_PingComponent* PingComp = PlayerCharacter->FindComponentByClass<UTP_PingComponent>())
				{
					check(PingComp);
					PingComp->OnEnemyPinged.AddDynamic(this, &AEnemyCharacter::GetNotification);
					PingComp->OnEnemyLost.AddDynamic(this, &AEnemyCharacter::DisableOutline);
					// Bind the delegate to the GetNotification function
				}
			}
		}
	}
}

void AEnemyCharacter::GetNotification(AEnemyCharacter* PingedEnemy)
{
	if (PingedEnemy == this)
	{
		EnableOutline(); 
		// Optionally, you can also log or handle the ping notification here
		UE_LOG(LogTemp, Warning, TEXT("Pinged Enemy: %s"), *GetName());
	}
	
}

void AEnemyCharacter::EnableOutline()
{

	if (GetMesh())
	{
		GetMesh()->SetRenderCustomDepth(true);
		GetMesh()->SetCustomDepthStencilValue(2); // Set a custom depth stencil value for the outline
	}
	
}

void AEnemyCharacter::DisableOutline(AEnemyCharacter* LostEnemy)
{
	if (GetMesh())
	{
		GetMesh()->SetRenderCustomDepth(false);
		GetMesh()->SetCustomDepthStencilValue(0); // Reset the custom depth stencil value
	}
}

