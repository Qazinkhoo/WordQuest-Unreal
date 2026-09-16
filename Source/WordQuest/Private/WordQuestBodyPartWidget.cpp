#include "WordQuestBodyPartWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "Widgets/SNullWidget.h"

TSharedRef<SWidget> UWordQuestBodyPartWidget::RebuildWidget()
{
    BodyImage = NewObject<UImage>(this, TEXT("BodyImage"));
    if (!BodyImage)
    {
        return SNullWidget::NullWidget;
    }

    BodyImage->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
    return BodyImage->TakeWidget();
}

void UWordQuestBodyPartWidget::NativeConstruct()
{
    Super::NativeConstruct();

    AtlasTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/WQHeroPartsAtlas.WQHeroPartsAtlas"));
    if (AtlasTexture)
    {
        AtlasTexture->Filter = TF_Nearest;
        AtlasTexture->UpdateResource();
    }

    RefreshBrush();
    SetFlipped(bFlipped);
}

void UWordQuestBodyPartWidget::SetBodyPart(EWordQuestBodyPart InPart)
{
    BodyPart = InPart;
    RefreshBrush();
}

void UWordQuestBodyPartWidget::SetFlipped(bool bInFlipped)
{
    bFlipped = bInFlipped;
    if (BodyImage)
    {
        BodyImage->SetRenderScale(FVector2D(bFlipped ? -1.f : 1.f, 1.f));
    }
}

void UWordQuestBodyPartWidget::RefreshBrush()
{
    if (!BodyImage || !AtlasTexture) return;

    FBox2f UVRegion;
    FVector2D ImageSize(64.f, 64.f);

    switch (BodyPart)
    {
    case EWordQuestBodyPart::Head:
        UVRegion = FBox2f(FVector2f(0.015625f, 0.020833f), FVector2f(0.300781f, 0.341146f));
        ImageSize = FVector2D(146.f, 123.f);
        break;
    case EWordQuestBodyPart::Torso:
        UVRegion = FBox2f(FVector2f(0.332031f, 0.020833f), FVector2f(0.583984f, 0.364583f));
        ImageSize = FVector2D(129.f, 132.f);
        break;
    case EWordQuestBodyPart::LeftArm:
        UVRegion = FBox2f(FVector2f(0.625000f, 0.020833f), FVector2f(0.757813f, 0.403646f));
        ImageSize = FVector2D(68.f, 147.f);
        break;
    case EWordQuestBodyPart::RightArm:
        UVRegion = FBox2f(FVector2f(0.789063f, 0.020833f), FVector2f(0.968750f, 0.403646f));
        ImageSize = FVector2D(92.f, 147.f);
        break;
    case EWordQuestBodyPart::LeftLeg:
        UVRegion = FBox2f(FVector2f(0.015625f, 0.468750f), FVector2f(0.177734f, 0.799479f));
        ImageSize = FVector2D(83.f, 127.f);
        break;
    case EWordQuestBodyPart::RightLeg:
        UVRegion = FBox2f(FVector2f(0.214844f, 0.468750f), FVector2f(0.394531f, 0.809896f));
        ImageSize = FVector2D(92.f, 131.f);
        break;
    default:
        return;
    }

    FSlateBrush Brush;
    Brush.SetResourceObject(AtlasTexture);
    Brush.DrawAs = ESlateBrushDrawType::Image;
    Brush.ImageSize = ImageSize;
    Brush.SetUVRegion(UVRegion);

    BodyImage->SetBrush(Brush);
    BodyImage->SetColorAndOpacity(FLinearColor::White);
}
