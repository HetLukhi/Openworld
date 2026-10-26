


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"


ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(FName("GeometryCollection"));
	SetRootComponent(GeometryCollection);

	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (GeometryCollection)
	{
		GeometryCollection->bNotifyBreaks = true;
		GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::OnGeometryBreak);
	}
}

void ABreakableActor::ActorSpawn(FVector Location)
{
	bSpawned = true;
	if (GeometryCollection)
	{
		GeometryCollection->bNotifyBreaks = false;
		GeometryCollection->OnChaosBreakEvent.RemoveDynamic(this, &ABreakableActor::OnGeometryBreak);
	}
	

	UWorld* World = GetWorld();
	if (World && TreasureClasses.Num() > 0)
	{
		Location.Z += 75.0f;
		const int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		World->SpawnActor<ATreasure>(TreasureClasses[Selection], Location, FRotator::ZeroRotator, SpawnParams);
	}
	SetLifeSpan(1.5f);
}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (bSpawned) return;

	ActorSpawn(GetActorLocation());
}

void ABreakableActor::OnGeometryBreak(const FChaosBreakEvent& BreakEvent)
{
	if (bSpawned) return;
	ActorSpawn(BreakEvent.Location);
}

