#pragma once

#include "CoreMinimal.h"
#include "Tool.generated.h"

UENUM(BlueprintType)
enum class ETool : uint8
{
	Manage,
	Build,
	Move,
	Road,
	Decoration,
	Demolish
};
