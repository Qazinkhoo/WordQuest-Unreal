#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WordQuestSwordPickup.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class WORDQUEST_API AWordQuestSwordPickup : public AActor
{
    GENERATED_BODY()
public:
    AWordQuestSwordPickup();
protected:
    virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
public:
    UPROPERTY(VisibleAnywhere) TObjectPtr<UBoxComponent> Trigger;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Blade;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Guard;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Handle;
};
