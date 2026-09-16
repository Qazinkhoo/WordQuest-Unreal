#include "WordQuestEnemyWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "Widgets/SNullWidget.h"

TSharedRef<SWidget> UWordQuestEnemyWidget::RebuildWidget()
{
    MonsterImage = NewObject<UImage>(this, TEXT("MonsterImage"));
    if (!MonsterImage)
    {
        return SNullWidget::NullWidget;
    }

    MonsterImage->SetRenderTransformPivot(FVector2D(0.5f, 1.0f));
    return MonsterImage->TakeWidget();
}

void UWordQuestEnemyWidget::NativeConstruct()
{
    Super::NativeConstruct();
    RefreshMonster();
}

void UWordQuestEnemyWidget::SetMonsterWave(int32 InWaveNumber)
{
    WaveNumber = FMath::Clamp(InWaveNumber, 1, 4);
    RefreshMonster();
}

void UWordQuestEnemyWidget::RefreshMonster()
{
    if (!MonsterImage) return;

    const TCHAR* TexturePath = nullptr;

    switch (WaveNumber)
    {
    case 1:
        TexturePath = TEXT("/Game/UI/Enemies/Monster01_Zombie.Monster01_Zombie");
        break;
    case 2:
        TexturePath = TEXT("/Game/UI/Enemies/Monster02_Skeleton.Monster02_Skeleton");
        break;
    case 3:
        TexturePath = TEXT("/Game/UI/Enemies/Monster03_Spider.Monster03_Spider");
        break;
    case 4:
    default:
        TexturePath = TEXT("/Game/UI/Enemies/Monster04_Creeper.Monster04_Creeper");
        break;
    }

    MonsterTexture = LoadObject<UTexture2D>(nullptr, TexturePath);
    if (!MonsterTexture)
    {
        MonsterImage->SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    MonsterTexture->Filter = TF_Nearest;
    MonsterTexture->UpdateResource();

    FSlateBrush Brush;
    Brush.SetResourceObject(MonsterTexture);
    Brush.DrawAs = ESlateBrushDrawType::Image;
    Brush.ImageSize = FVector2D(
        static_cast<float>(MonsterTexture->GetSizeX()),
        static_cast<float>(MonsterTexture->GetSizeY())
    );

    MonsterImage->SetBrush(Brush);
    MonsterImage->SetColorAndOpacity(FLinearColor::White);
    MonsterImage->SetVisibility(ESlateVisibility::Visible);
}
