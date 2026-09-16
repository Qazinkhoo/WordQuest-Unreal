#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WordQuestStageBackground.generated.h"

class USceneComponent;
class UWidgetComponent;
class UWordQuestStageBackgroundWidget;

UCLASS()
class WORDQUEST_API AWordQuestStageBackground : public AActor
{
    GENERATED_BODY()

public:
    AWordQuestStageBackground();
    virtual void BeginPlay() override;

    void ConfigureBackground(int32 InStageNumber, const FVector& StageOrigin);

private:
    void RefreshBackground();

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<USceneComponent> Root;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UWidgetComponent> BackgroundWidgetComponent;

    UPROPERTY()
    TObjectPtr<UWordQuestStageBackgroundWidget> BackgroundWidget;

    int32 StageNumber = 1;
};
