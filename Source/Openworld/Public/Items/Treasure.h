#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

class USoundBase;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class OPENWORLD_API ATreasure : public AItem
{
	GENERATED_BODY()

public:
	ATreasure();

protected:
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* PickupSound;

	UPROPERTY(VisibleAnywhere, Category = "VFX")
	UNiagaraComponent* TreasureFX;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* PickupFX;

	UPROPERTY(EditDefaultsOnly, Category = "Treasure Properties")
	int32 Soul;
};
