#include "WordQuestHeroWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"

void UWordQuestHeroWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!WidgetTree) return;

    HeroImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("HeroImage"));
    WidgetTree->RootWidget = HeroImage;

    // Rename the imported generated sprite sheet to WQHeroSheet in Content/UI.
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

    // Exact crop regions for the generated 1448x1086 four-view sheet.
    // Second character = left-facing. Fourth character = right-facing.
    const FBox2f UVRegion = bFacingLeft
        ? FBox2f(FVector2f(0.297652f, 0.151013f), FVector2f(0.475138f, 0.863720f))
        : FBox2f(FVector2f(0.791436f, 0.151013f), FVector2f(0.968923f, 0.863720f));

    Brush.SetUVRegion(TOptional<FBox2f>(UVRegion));
    HeroImage->SetBrush(Brush);
    HeroImage->SetColorAndOpacity(FLinearColor::White);
}
