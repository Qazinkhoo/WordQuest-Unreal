#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WordQuestTypes.h"
#include "WordQuestGameMode.generated.h"

class AWordQuestEnemy;
class USoundWaveProcedural;

UCLASS()
class WORDQUEST_API AWordQuestGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AWordQuestGameMode();
    virtual void BeginPlay() override;

    UPROPERTY(BlueprintReadOnly, Category="Word Quest") int32 CurrentStage = 1;
    UPROPERTY(BlueprintReadOnly, Category="Word Quest") int32 CurrentWave = 1;
    UPROPERTY(BlueprintReadOnly, Category="Word Quest") bool bBattleActive = false;
    UPROPERTY(BlueprintReadOnly, Category="Word Quest") bool bStageClear = false;
    UPROPERTY(BlueprintReadOnly, Category="Word Quest") bool bShopOpen = false;
    UPROPERTY(BlueprintReadOnly, Category="Word Quest") bool bMainMenuOpen = false;
    UPROPERTY(BlueprintReadOnly, Category="Word Quest") bool bGameOver = false;
    UPROPERTY(BlueprintReadOnly, Category="Word Quest") bool bQuestComplete = false;
    UPROPERTY(BlueprintReadOnly, Category="Word Quest") bool bAnswerFeedbackActive = false;
    UPROPERTY(BlueprintReadOnly, Category="Word Quest") int32 SelectedAnswerIndex = INDEX_NONE;
    UPROPERTY(BlueprintReadOnly, Category="Word Quest") bool bSelectedAnswerCorrect = false;
    UPROPERTY(BlueprintReadOnly, Category="Word Quest") float AnswerFeedbackStartTime = 0.f;
    UPROPERTY(BlueprintReadOnly, Category="Word Quest") FWordQuestQuestion CurrentQuestion;
    UPROPERTY(BlueprintReadOnly, Category="Word Quest") TObjectPtr<AWordQuestEnemy> CurrentEnemy;

    UFUNCTION(BlueprintCallable) void StartEncounter(AWordQuestEnemy* Enemy);
    UFUNCTION(BlueprintCallable) bool SubmitAnswer(int32 AnswerIndex);
    UFUNCTION(BlueprintCallable) void AdvanceWave();
    UFUNCTION(BlueprintCallable) void OpenStageShop();
    UFUNCTION(BlueprintCallable) void BuyShopApple();
    UFUNCTION(BlueprintCallable) void BuyShopStar();
    UFUNCTION(BlueprintCallable) void BuyShopArmour();
    UFUNCTION(BlueprintCallable) void LeaveStageShop();
    UFUNCTION(BlueprintCallable) void StartStageTwo();
    UFUNCTION(BlueprintCallable) void StartStageThree();
    UFUNCTION(BlueprintCallable) void StartAdventureFromMenu();
    UFUNCTION(BlueprintCallable) void RestartCurrentStage();
    UFUNCTION(BlueprintCallable) void ReturnToMainMenu();

    UFUNCTION(BlueprintImplementableEvent) void OnQuestionChanged();
    UFUNCTION(BlueprintImplementableEvent) void OnBattleStateChanged();
    UFUNCTION(BlueprintImplementableEvent) void OnStageCleared();
    UFUNCTION(BlueprintImplementableEvent) void OnGameOver();

private:
    bool LoadDifferentQuestion();
    void ResolveSelectedAnswer();
    void PlayTone(float StartFrequency, float EndFrequency, float DurationSeconds, float Volume = 0.35f);
    FName GetBaseMapForStage(int32 Stage) const;

    FVector AdventureStartLocation = FVector::ZeroVector;

    UPROPERTY()
    TArray<TObjectPtr<USoundWaveProcedural>> ActiveFeedbackSounds;
};
