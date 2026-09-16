#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WordQuestEnemyWidget.generated.h"

class UImage;
class UTexture2D;

UCLASS()
class WORDQUEST_API UWordQuestEnemyWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;

    // Retained for compatibility with the existing normal-monster setup.
    void SetMonsterWave(int32 InWaveNumber);

    // Chooses either the normal wave monster or the boss for the current stage.
    void SetEnemyVisual(int32 InWaveNumber, int32 InStageNumber, bool bInBoss);

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    void RefreshMonster();

    UPROPERTY()
    TObjectPtr<UImage> MonsterImage;

    UPROPERTY()
    TObjectPtr<UTexture2D> MonsterTexture;

    int32 WaveNumber = 1;
    int32 StageNumber = 1;
    bool bBoss = false;
};
