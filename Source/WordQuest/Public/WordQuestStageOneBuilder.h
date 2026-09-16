#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WordQuestStageOneBuilder.generated.h"

UCLASS()
class WORDQUEST_API AWordQuestStageOneBuilder : public AActor
{
    GENERATED_BODY()
public:
    AWordQuestStageOneBuilder();
    virtual void BeginPlay() override;

private:
    void SpawnBlock(const FVector& Location, const FVector& Scale);
    void SpawnTree(const FVector& Location, float Scale = 1.f);
};
