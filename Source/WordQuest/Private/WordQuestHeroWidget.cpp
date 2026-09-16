#include "WordQuestHeroWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "Widgets/SNullWidget.h"

TSharedRef<SWidget> UWordQuestHeroWidget::RebuildWidget()
{
    HeroImage = NewObject<UImage>(this, TEXT("HeroImage"));
    if (!HeroImage)
    {
        return SNullWidget::NullWidget;
    }

    HeroImage->SetRenderTransformPivot(FVector2D(0.5f, 1.0f));
    return HeroImage->TakeWidget();
}

void UWordQuestHeroWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Use the original four-view hero sheet. The right- and left-facing crops
    // from this sheet are visually coherent and avoid the mismatched joints
    // that occurred with the generated separate-body-part atlas.
    HeroTexture = LoadObject<UTexture2D>(nullptr, TEXT("/Game/UI/WQHeroSheet.WQHeroSheet"));
    if (HeroTexture)
    {
        HeroTexture->Filter = TF_Nearest;
        HeroTexture->UpdateResource();
    }

    RefreshBrush();
    ResetHeroTransform();
}

void UWordQuestHeroWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!HeroImage) return;

    AnimTime += InDeltaTime;

    FWidgetTransform Transform;
    Transform.Scale = FVector2D(1.f, 1.f);

    if (bAirborne)
    {
        // Small forward lean and squash/stretch while jumping, while keeping
        // the feet visually tied to the bottom pivot of the sprite.
        Transform.Translation = FVector2D(0.f, -3.f);
        Transform.Scale = FVector2D(1.02f, 0.98f);
        Transform.Angle = bFacingLeft ? 1.8f : -1.8f;
    }
    else if (bMoving)
    {
        // Subtle whole-body walk motion. This deliberately avoids fake limb
        // separation until we have a truly matching six-part source asset.
        const float Step = FMath::Sin(AnimTime * 9.0f);
        const float Lift = FMath::Abs(FMath::Sin(AnimTime * 9.0f));
        Transform.Translation = FVector2D(Step * 0.8f, -Lift * 2.0f);
        Transform.Scale = FVector2D(1.f + Lift * 0.008f, 1.f - Lift * 0.008f);
        Transform.Angle = Step * 0.8f;
    }
    else
    {
        const float Idle = FMath::Sin(AnimTime * 2.0f);
        Transform.Translation = FVector2D(0.f, Idle * 0.35f);
    }

    HeroImage->SetRenderTransform(Transform);
}

void UWordQuestHeroWidget::SetFacingLeft(bool bInFacingLeft)
{
    if (bFacingLeft == bInFacingLeft) return;

    bFacingLeft = bInFacingLeft;
    RefreshBrush();
}

void UWordQuestHeroWidget::SetMovementState(bool bInMoving, bool bInAir)
{
    bMoving = bInMoving;
    bAirborne = bInAir;
}

void UWordQuestHeroWidget::RefreshBrush()
{
    if (!HeroImage || !HeroTexture) return;

    FSlateBrush Brush;
    Brush.SetResourceObject(HeroTexture);
    Brush.DrawAs = ESlateBrushDrawType::Image;
    Brush.ImageSize = FVector2D(100.f, 300.f);

    // Exact crops from the original 1448 x 1086 four-view sheet:
    // second figure = left-facing, fourth figure = right-facing.
    const FBox2f UVRegion = bFacingLeft
        ? FBox2f(FVector2f(0.297652f, 0.151013f), FVector2f(0.475138f, 0.863720f))
        : FBox2f(FVector2f(0.791436f, 0.151013f), FVector2f(0.968923f, 0.863720f));

    Brush.SetUVRegion(UVRegion);
    HeroImage->SetBrush(Brush);
    HeroImage->SetColorAndOpacity(FLinearColor::White);
}

void UWordQuestHeroWidget::ResetHeroTransform()
{
    if (!HeroImage) return;

    FWidgetTransform Identity;
    Identity.Scale = FVector2D(1.f, 1.f);
    HeroImage->SetRenderTransform(Identity);
}
