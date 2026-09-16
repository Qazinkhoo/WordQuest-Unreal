#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WordQuestStageThreeBuilder.generated.h"

UCLASS()
class WORDQUEST_API AWordQuestStageThreeBuilder : public AActor
{
    GENERATED_BODY()

public:
    AWordQuestStageThreeBuilder();
    virtual void BeginPlay() override;

private:
    void SpawnBlock(const FVector& Location, const FVector& Scale, const FRotator& Rotation = FRotator::ZeroRotator);
    void SpawnCrystalCluster(const FVector& Location, float Scale = 1.f);
    void SpawnCaveRib(const FVector& Location, float Scale = 1.f);
};
