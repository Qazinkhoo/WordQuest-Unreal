#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "WordQuestHUD.generated.h"

class USoundWaveProcedural;
class UFont;
class UTexture2D;

struct FWordQuestHUDMessage
{
    FString Text;
    FColor Color = FColor::White;
    FVector WorldLocation = FVector::ZeroVector;
    float StartTime = 0.f;
    float Duration = 1.35f;
};

UCLASS()
class WORDQUEST_API AWordQuestHUD : public AHUD
{
    GENERATED_BODY()

public:
    AWordQuestHUD();
    virtual void DrawHUD() override;

    void AddFloatingMessage(const FString& InText, const FColor& InColor, const FVector& InWorldLocation, float InDuration = 1.35f);

private:
    TArray<FWordQuestHUDMessage> FloatingMessages;
    void DrawFloatingMessages();
    void DrawMenuVoxelHero(float CenterX, float TopY, float Scale);
    void DrawMenuHeroSprite(float X, float Y, float W, float H);
    void DrawGameplayHeroSprite();
    void PlayFeedbackTone(float StartFrequency, float EndFrequency, float DurationSeconds, float Volume = 0.4f);
    UFont* GetHUDTextFont() const;

    bool bShopWasOpen = false;
    bool bHeroFacingLeft = false;

    UPROPERTY()
    TObjectPtr<UFont> HDFont;

    UPROPERTY()
    TObjectPtr<UTexture2D> MenuBackgroundTexture;

    UPROPERTY()
    TObjectPtr<UTexture2D> HeroFrontTexture;

    UPROPERTY()
    TObjectPtr<UTexture2D> HeroLeftTexture;

    UPROPERTY()
    TObjectPtr<UTexture2D> HeroBackTexture;

    UPROPERTY()
    TObjectPtr<UTexture2D> HeroRightTexture;

    UPROPERTY()
    TArray<TObjectPtr<USoundWaveProcedural>> ActiveFeedbackSounds;
};
