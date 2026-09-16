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
    void SetMonsterWave(int32 InWaveNumber);

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    void RefreshMonster();

    UPROPERTY()
    TObjectPtr<UImage> MonsterImage;

    UPROPERTY()
    TObjectPtr<UTexture2D> MonsterTexture;

    int32 WaveNumber = 1;
};
