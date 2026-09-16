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
    void SetFacingLeft(bool bInFacingLeft);

private:
    void RefreshBrush();

    UPROPERTY()
    TObjectPtr<UImage> HeroImage;

    UPROPERTY()
    TObjectPtr<UTexture2D> HeroTexture;

    bool bFacingLeft = false;
};
