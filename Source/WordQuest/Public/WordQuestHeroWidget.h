#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WordQuestHeroWidget.generated.h"

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
    void RefreshBrush();
    void ResetHeroTransform();

    UPROPERTY()
    TObjectPtr<UImage> HeroImage;

    UPROPERTY()
    TObjectPtr<UTexture2D> HeroTexture;

    bool bFacingLeft = false;
    bool bMoving = false;
    bool bAirborne = false;
    float AnimTime = 0.f;
};
