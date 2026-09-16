#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WordQuestBodyPartWidget.generated.h"

class UImage;
class UTexture2D;

enum class EWordQuestBodyPart : uint8
{
    Head,
    Torso,
    LeftArm,
    RightArm,
    LeftLeg,
    RightLeg
};

UCLASS()
class WORDQUEST_API UWordQuestBodyPartWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    void SetBodyPart(EWordQuestBodyPart InPart);
    void SetFlipped(bool bInFlipped);

protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    void RefreshBrush();

    UPROPERTY()
    TObjectPtr<UImage> BodyImage;

    UPROPERTY()
    TObjectPtr<UTexture2D> AtlasTexture;

    EWordQuestBodyPart BodyPart = EWordQuestBodyPart::Head;
    bool bFlipped = false;
};
