// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TP_PingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PINGTEST_API UTP_PingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTP_PingComponent();

private:
	/** The ping location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ping", meta = (AllowPrivateAccess = "true"))
	FVector PingLocation;

	/** The ping radius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ping", meta = (AllowPrivateAccess = "true"))
	float PingRadius;

	/*UPROPERTY(VisibleAnywhere, Category ="Input")
	TObjectPtr<class UInputMappingContext> InputContext;

	UPROPERTY(VisibleAnywhere, Category = "Input") */
	
	
};
