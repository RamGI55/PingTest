// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TP_PingComponent.generated.h"


struct FInputActionValue;
class AEnemyCharacter;
class UInputAction;
class UInputComponent;
class UInputMappingContext; 
class APingTestCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyPinged, AEnemyCharacter*, PingedEnemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyPingLost, AEnemyCharacter*, LostEnemy);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PINGTEST_API UTP_PingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTP_PingComponent();

	UFUNCTION()
	void InitComponent();

	virtual void BeginPlay() override;
	/** Called to ping a location */

	UFUNCTION()
	void SetupInputComponent(class UInputComponent* PlayerInputComponent);
	
	
	/** Dynamic ping input handlers */
	UFUNCTION(BlueprintCallable, Category = "Ping")
	void StartDynamicPing();

	UFUNCTION(BlueprintCallable, Category = "Ping")
	void StopDynamicPing(); 

	UFUNCTION(BlueprintCallable, Category = "Ping")
	void UpdateDynamicPing();

	UFUNCTION(BlueprintCallable, Category = "Screen")
	bool GetEnemiesInCrosshair(TArray<AEnemyCharacter*>& OutEnemies) const;

	UFUNCTION(Category = "Ping")
	bool isEnemyNearCrosshair(AEnemyCharacter*& OutEnemy, const FVector2D& CrosshairPos, APlayerController* PC) const; 
	
	UFUNCTION(BlueprintCallable, Category = "Ping")
	void Ping();  // This function will be deprcated in the future, use DyanmicPing instead. 

	//UFUNCTION(Category = "Ping")
	//bool GetEnemyInCrosshair(AEnemyCharacter*& OutEnemy) const;


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> PingAction;

	// Ping Delegations.
	UPROPERTY(BlueprintAssignable, Category = "Ping Events")
	FOnEnemyPinged OnEnemyPinged;

	UPROPERTY(BlueprintAssignable, Category = "Ping Events")
	FOnEnemyPingLost OnEnemyLost; 
	
	FDelegateHandle PingDelegate; 


private:
	UFUNCTION(BlueprintCallable, Category = "LineTrace")
	void LineTraceForPing(); 

	UFUNCTION(Category = "Ping")
	void ProcessPingedEnemy(); 
	
	/** The ping location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ping", meta = (AllowPrivateAccess = "true"))
	FVector PingLocation;

	/** The ping radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ping", meta = (AllowPrivateAccess = "true"))
	float PingRadius;

	UPROPERTY(VisibleAnywhere, Category ="Input")
	TObjectPtr<UInputMappingContext> InputContext;
	
	TObjectPtr<APingTestCharacter> PlayerCharacter;

	/** The hit result from the line trace */ 
	/*UPROPERTY()
	FHitResult HitResult;*/

	UPROPERTY()
	TArray<FHitResult> HitResult; 
	
	UPROPERTY()
	AEnemyCharacter* HitEnemy = nullptr;  // The enemy character that was pinged 

	/** Timer handle for ping duration */
	UPROPERTY()
	FTimerHandle PingTimerHandle;// Timer handle for ping duration

	/** Dynamic Container for pinging enemies */
	UPROPERTY()
	TArray<AEnemyCharacter*> PingedEnemies; // List of pinged enemies

	UPROPERTY()
	TArray<TObjectPtr<AEnemyCharacter>> LostEnemies;

	UPROPERTY(BlueprintReadWrite, Category = "Ping" , meta = (AllowPrivateAccess = "true")) 
	float PingRange = 5000.0f; // Default ping range

	UPROPERTY(BlueprintReadWrite, Category = "Ping" , meta = (AllowPrivateAccess = "true")) 
	float CrosshairTolerancePixels = 50.f;
	
	// Enemy Character to ping
	
};
