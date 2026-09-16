#include "WordQuestHeroWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UWordQuestHeroWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!WidgetTree) return;

    HeroImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("HeroImage"));
    WidgetTree->RootWidget = HeroImage;

    HeroTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/WQHeroRight.WQHeroRight"));
    RefreshBrush();
}

void UWordQuestHeroWidget::SetFacingLeft(bool bInFacingLeft)
{
    if (bFacingLeft == bInFacingLeft && HeroTexture) return;

    bFacingLeft = bInFacingLeft;
    HeroTexture = LoadObject<UTexture2D>(nullptr,
        bFacingLeft
            ? TEXT("/Game/UI/WQHeroLeft.WQHeroLeft")
            : TEXT("/Game/UI/WQHeroRight.WQHeroRight"));

    RefreshBrush();
}

void UWordQuestHeroWidget::RefreshBrush()
{
    if (!HeroImage || !HeroTexture) return;

    HeroImage->SetBrushFromTexture(HeroTexture, true);
    HeroImage->SetColorAndOpacity(FLinearColor::White);
}
