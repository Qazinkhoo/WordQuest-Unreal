#include "WordQuestStageBackgroundWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "Widgets/SNullWidget.h"

TSharedRef<SWidget> UWordQuestStageBackgroundWidget::RebuildWidget()
{
    BackgroundImage = NewObject<UImage>(this, TEXT("BackgroundImage"));
    if (!BackgroundImage)
    {
        return SNullWidget::NullWidget;
    }

    BackgroundImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
    return BackgroundImage->TakeWidget();
}

void UWordQuestStageBackgroundWidget::NativeConstruct()
{
    Super::NativeConstruct();
    RefreshBackground();
}

void UWordQuestStageBackgroundWidget::SetStageNumber(int32 InStageNumber)
{
    StageNumber = FMath::Clamp(InStageNumber, 1, 3);
    RefreshBackground();
}

void UWordQuestStageBackgroundWidget::RefreshBackground()
{
    if (!BackgroundImage) return;

    const TCHAR* TexturePath = TEXT("/Game/UI/StageBackgrounds/Stage01_WhisperingForest.Stage01_WhisperingForest");

    switch (StageNumber)
    {
    case 1:
        TexturePath = TEXT("/Game/UI/StageBackgrounds/Stage01_WhisperingForest.Stage01_WhisperingForest");
        break;
    case 2:
        TexturePath = TEXT("/Game/UI/StageBackgrounds/Stage02_SunnyMeadow.Stage02_SunnyMeadow");
        break;
    case 3:
    default:
        TexturePath = TEXT("/Game/UI/StageBackgrounds/Stage03_CrystalCave.Stage03_CrystalCave");
        break;
    }

    BackgroundTexture = LoadObject<UTexture2D>(nullptr, TexturePath);
    if (!BackgroundTexture)
    {
        BackgroundImage->SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    FSlateBrush Brush;
    Brush.SetResourceObject(BackgroundTexture);
    Brush.DrawAs = ESlateBrushDrawType::Image;
    Brush.ImageSize = FVector2D(1920.f, 1080.f);

    BackgroundImage->SetBrush(Brush);
    BackgroundImage->SetColorAndOpacity(FLinearColor::White);
    BackgroundImage->SetVisibility(ESlateVisibility::Visible);
}
