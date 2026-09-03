// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PeCoPoolDeveloperSettings.generated.h"

/**
 * Local editor preferences for Pest Control object pooling.
 *
 * Stored in EditorPerProjectUserSettings.ini so each developer can use different
 * pooling and debug options without changing shared project configuration.
 */
UCLASS(Config=EditorPerProjectUserSettings, meta=(DisplayName="Object Pooling"))
class PESTCONTROL_API UPeCoPoolDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	virtual FName GetContainerName() const override { return TEXT("Editor"); }
	virtual FName GetCategoryName() const override { return TEXT("Pest Control"); }
	virtual FName GetSectionName() const override { return TEXT("Object Pooling"); }

	/** Draw per-class pool statistics on screen during PIE and game worlds. */
	UPROPERTY(Config, EditAnywhere, Category="Pool", meta=(DisplayName="Debug HUD"))
	bool bEnableDebugHUD = false;

	/** Reuse APeCoEnemyCharacter instances. Boss enemies remain excluded by the spawner. */
	UPROPERTY(Config, EditAnywhere, Category="Pool", meta=(DisplayName="Enemy Pooling"))
	bool bEnableEnemyPooling = false;

	/** Reuse AProjectile instances, including derived fragment projectiles. */
	UPROPERTY(Config, EditAnywhere, Category="Pool", meta=(DisplayName="Projectile Pooling"))
	bool bEnableProjectilePooling = false;
};
