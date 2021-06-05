// engine header
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Engine/EngineTypes.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "CookMapFoldersSetting.generated.h"

/** Singleton wrapper to allow for using the setting structure in SSettingsView */
UCLASS()
class UCookMapFoldersSetting : public UObject
{
	GENERATED_BODY()
public:

	UCookMapFoldersSetting() {}

	FORCEINLINE static UCookMapFoldersSetting* Get()
	{
		static bool bInitialized = false;
		// This is a singleton, use default object
		UCookMapFoldersSetting* DefaultSettings = GetMutableDefault<UCookMapFoldersSetting>();

		if (!bInitialized)
		{
			bInitialized = true;
		}

		return DefaultSettings;
	}

	FORCEINLINE TArray<FDirectoryPath>& GetCookMapFolders(){return CookMapFolders;}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Directory",meta = (RelativeToGameContentDir, LongPackageName))
		TArray<FDirectoryPath> CookMapFolders;
};