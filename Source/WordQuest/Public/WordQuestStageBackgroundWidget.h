#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WordQuestStageBackgroundWidget.generated.h"

class UCanvasPanel;
class UImage;
class UTexture2D;

UCLASS()
class WORDQUEST_API UWordQuestStageBackgroundWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    void SetStageNumber(int32 InStageNumber);

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    void RefreshBackground();

    UPROPERTY()
    TObjectPtr<UCanvasPanel> TileCanvas;

    UPROPERTY()
    TArray<TObjectPtr<UImage>> BackgroundTiles;

    UPROPERTY()
    TObjectPtr<UTexture2D> BackgroundTexture;

    int32 StageNumber = 1;
};
