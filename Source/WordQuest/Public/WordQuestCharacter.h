#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WordQuestCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class WORDQUEST_API AWordQuestCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AWordQuestCharacter();
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UCameraComponent> SideCamera;

    UFUNCTION(BlueprintCallable) void CollectSword();

private:
    void MoveRight(float Value);
};
