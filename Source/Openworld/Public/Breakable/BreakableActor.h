

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class ATreasure;

UCLASS()
class OPENWORLD_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	UFUNCTION()
	void OnGeometryBreak(const FChaosBreakEvent& BreakEvent);

protected:
	virtual void BeginPlay() override;

	void ActorSpawn(FVector Location);

private:
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ATreasure>> TreasureClasses;

	bool bSpawned = false;
};
