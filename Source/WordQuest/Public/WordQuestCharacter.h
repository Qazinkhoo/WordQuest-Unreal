#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WordQuestCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;

UCLASS()
class WORDQUEST_API AWordQuestCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    AWordQuestCharacter();
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TObjectPtr<UCameraComponent> SideCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Voxel Visual") TObjectPtr<UStaticMeshComponent> BlockBody;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Voxel Visual") TObjectPtr<UStaticMeshComponent> BlockHead;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Voxel Visual") TObjectPtr<UStaticMeshComponent> LeftArm;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Voxel Visual") TObjectPtr<UStaticMeshComponent> RightArm;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Voxel Visual") TObjectPtr<UStaticMeshComponent> LeftLeg;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Voxel Visual") TObjectPtr<UStaticMeshComponent> RightLeg;

    UFUNCTION(BlueprintCallable) void CollectSword();
    UFUNCTION(BlueprintCallable) void SetBattleLocked(bool bLocked);

private:
    void MoveRight(float Value);
    bool bBattleLocked = false;
};
