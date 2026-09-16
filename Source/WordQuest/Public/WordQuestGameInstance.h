#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WordQuestTypes.h"
#include "WordQuestGameInstance.generated.h"

UCLASS()
class WORDQUEST_API UWordQuestGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadOnly, Category="Word Quest") FWordQuestPlayerState PlayerState;

    UFUNCTION(BlueprintCallable, Category="Word Quest") void StartNewAdventure();
    UFUNCTION(BlueprintCallable, Category="Word Quest") void CollectSword();
    UFUNCTION(BlueprintCallable, Category="Word Quest") bool ApplyEnemyHit();
    UFUNCTION(BlueprintCallable, Category="Word Quest") void RewardNormalEnemy();
    UFUNCTION(BlueprintCallable, Category="Word Quest") void RewardBoss();
    UFUNCTION(BlueprintCallable, Category="Word Quest|Shop") bool BuyApple();
    UFUNCTION(BlueprintCallable, Category="Word Quest|Shop") bool BuyStar();
    UFUNCTION(BlueprintCallable, Category="Word Quest|Shop") bool BuyArmour();
    UFUNCTION(BlueprintPure, Category="Word Quest") bool IsGameOver() const;

    UFUNCTION(BlueprintPure, Category="Word Quest|Shop") bool IsAppleBoughtThisVisit() const { return bAppleBoughtThisVisit; }
    UFUNCTION(BlueprintPure, Category="Word Quest|Shop") bool IsStarBoughtThisVisit() const { return bStarBoughtThisVisit; }
    UFUNCTION(BlueprintPure, Category="Word Quest|Shop") bool IsArmourBoughtThisVisit() const { return bArmourBoughtThisVisit; }
    UFUNCTION(BlueprintPure, Category="Word Quest|Shop") bool CanBuyApple() const;
    UFUNCTION(BlueprintPure, Category="Word Quest|Shop") bool CanBuyStar() const;
    UFUNCTION(BlueprintPure, Category="Word Quest|Shop") bool CanBuyArmour() const;

private:
    bool bAppleBoughtThisVisit = false;
    bool bStarBoughtThisVisit = false;
    bool bArmourBoughtThisVisit = false;

public:
    UFUNCTION(BlueprintCallable, Category="Word Quest|Shop") void ResetShopStock();
};
