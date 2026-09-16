#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WordQuestHeroWidget.generated.h"

class UCanvasPanel;
class UImage;
class UTexture2D;

UCLASS()
class WORDQUEST_API UWordQuestHeroWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void SetFacingLeft(bool bInFacingLeft);
    void SetMovementState(bool bInMoving, bool bInAir);

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    void RefreshBrushes();
    void ApplyBandBrush(UImage* Image, const FBox2f& UVRegion, const FVector2D& ImageSize);
    void ResetPartTransforms();

    UPROPERTY()
    TObjectPtr<UCanvasPanel> RootPanel;

    UPROPERTY()
    TObjectPtr<UImage> HeadImage;

    UPROPERTY()
    TObjectPtr<UImage> TorsoImage;

    UPROPERTY()
    TObjectPtr<UImage> LegsImage;

    UPROPERTY()
    TObjectPtr<UTexture2D> HeroTexture;

    bool bFacingLeft = false;
    bool bMoving = false;
    bool bAirborne = false;
    float WalkPhase = 0.f;
};
