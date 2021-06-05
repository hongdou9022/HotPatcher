#pragma once

#include "Commandlets/Commandlet.h"
#include "HotCookerExCommandlet.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHotCookerExCommandlet, Log, All);

UCLASS()
class UHotCookerExCommandlet :public UCommandlet
{
	GENERATED_BODY()

public:

	virtual int32 Main(const FString& Params)override;
};