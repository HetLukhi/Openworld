// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasure.h"
#include "Characters/Characters.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

ATreasure::ATreasure()
{
	TreasureFX = CreateDefaultSubobject<UNiagaraComponent>(FName("TreasureFx"));
	TreasureFX->SetupAttachment(GetRootComponent());
}

void ATreasure::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacters* MainCharacter = Cast<ACharacters>(OtherActor);
	if (MainCharacter)
	{
		if (PickupSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				PickupSound,
				GetActorLocation()
			);
		}
		if (PickupFX)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this,
				PickupFX,
				GetActorLocation()
			);
		}
		Destroy();
	}
}
