

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EquipInterface.generated.h"

class ACharacters;

UINTERFACE(MinimalAPI)
class UEquipInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OPENWORLD_API IEquipInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	void Equip(ACharacters* Character);
};
