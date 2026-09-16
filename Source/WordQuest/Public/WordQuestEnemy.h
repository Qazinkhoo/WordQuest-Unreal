#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WordQuestEnemy.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class WORDQUEST_API AWordQuestEnemy : public AActor
{
    GENERATED_BODY()
public:
    AWordQuestEnemy();
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Word Quest") bool bBoss = false;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Word Quest") int32 MaxHP = 1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Word Quest") int32 CurrentHP = 1;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Word Quest") int32 WaveNumber = 1;

    UFUNCTION(BlueprintCallable) void ConfigureEnemy(int32 InWave, bool bInBoss);
    UFUNCTION(BlueprintCallable) bool ReceiveWordDamage(int32 DamageAmount);
    UFUNCTION(BlueprintCallable) void PlayHitPulse();

    UPROPERTY(VisibleAnywhere) TObjectPtr<UBoxComponent> Trigger;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Visual;
};
