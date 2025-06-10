// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TP_PingComponent.generated.h"


class UInputAction;
class UInputComponent;
class UInputMappingContext; 
class APingTestCharacter;

//DECLARE_MULTICAST_DELEGATE_OneParam(FPingedDelegate, const FHitResult&, bHit);

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

	UFUNCTION(BlueprintCallable, Category="Ping")
	void PingWithValue(const FInputActionValue& Value); 
	
	UFUNCTION (BlueprintCallable, Category = "Ping")
	void Ping();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> PingAction;
	
	FDelegateHandle PingDelegate; 


private:
	UFUNCTION(BlueprintCallable, Category = "LineTrace")
	void LineTraceForPing(); 
	
	/** The ping location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ping", meta = (AllowPrivateAccess = "true"))
	FVector PingLocation;

	/** The ping radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ping", meta = (AllowPrivateAccess = "true"))
	float PingRadius;

	UPROPERTY(VisibleAnywhere, Category ="Input")
	TObjectPtr<UInputMappingContext> InputContext;
	
	TObjectPtr<APingTestCharacter> PlayerCharacter;

	// Enemy Character to ping 
};
