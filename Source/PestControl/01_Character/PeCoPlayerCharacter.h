// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "01_Character/PeCoCharacter.h"
#include "PeCoPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PESTCONTROL_API APeCoPlayerCharacter : public APeCoCharacter
{
	GENERATED_BODY()

public:
	APeCoPlayerCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	virtual void PossessedBy(AController* NewController) override;
	// ~ End of Player Character Interface

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:


private:

	void InitPlayerCharacter();

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

};
