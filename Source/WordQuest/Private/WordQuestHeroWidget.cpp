#include "WordQuestHeroWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/SNullWidget.h"

TSharedRef<SWidget> UWordQuestHeroWidget::RebuildWidget()
{
    HeroImage = NewObject<UImage>(this, TEXT("HeroImage"));
    if (!HeroImage)
    {
        return SNullWidget::NullWidget;
    }

    return HeroImage->TakeWidget();
}

void UWordQuestHeroWidget::NativeConstruct()
{
    Super::NativeConstruct();

    HeroTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/WQHeroSheet.WQHeroSheet"));
    RefreshBrush();
}

void UWordQuestHeroWidget::SetFacingLeft(bool bInFacingLeft)
{
    if (bFacingLeft == bInFacingLeft) return;

    bFacingLeft = bInFacingLeft;
    RefreshBrush();
}

void UWordQuestHeroWidget::RefreshBrush()
{
    if (!HeroImage || !HeroTexture) return;

    FSlateBrush Brush;
    Brush.SetResourceObject(HeroTexture);
    Brush.DrawAs = ESlateBrushDrawType::Image;
    Brush.ImageSize = FVector2D(140.f, 220.f);

    // Exact crop regions for the generated 1448x1086 four-view sheet.
    // Second character = left-facing. Fourth character = right-facing.
    const FBox2f UVRegion = bFacingLeft
        ? FBox2f(FVector2f(0.297652f, 0.151013f), FVector2f(0.475138f, 0.863720f))
        : FBox2f(FVector2f(0.791436f, 0.151013f), FVector2f(0.968923f, 0.863720f));

    Brush.SetUVRegion(UVRegion);
    HeroImage->SetBrush(Brush);
    HeroImage->SetColorAndOpacity(FLinearColor::White);
}
