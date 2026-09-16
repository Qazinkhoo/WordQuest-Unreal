#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WordQuestStageTwoBuilder.generated.h"

UCLASS()
class WORDQUEST_API AWordQuestStageTwoBuilder : public AActor
{
    GENERATED_BODY()
public:
    AWordQuestStageTwoBuilder();
    virtual void BeginPlay() override;

private:
    void SpawnBlock(const FVector& Location, const FVector& Scale);
    void SpawnFlower(const FVector& Location, float Scale = 1.f);
    void SpawnMeadowArch(const FVector& Location, float Scale = 1.f);
};
