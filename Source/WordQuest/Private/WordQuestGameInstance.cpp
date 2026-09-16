#include "WordQuestGameInstance.h"
#include "WordQuestCharacter.h"
#include "Kismet/GameplayStatics.h"

void UWordQuestGameInstance::StartNewAdventure()
{
    PlayerState = FWordQuestPlayerState();
    ResetShopStock();
}

void UWordQuestGameInstance::CollectSword()
{
    if (!PlayerState.bHasSword)
    {
        PlayerState.bHasSword = true;
        PlayerState.Damage = 1;
    }
}

bool UWordQuestGameInstance::ApplyEnemyHit()
{
    if (PlayerState.StarProtectionHits > 0)
    {
        --PlayerState.StarProtectionHits;
        return false;
    }
    PlayerState.CurrentHP = FMath::Max(0, PlayerState.CurrentHP - 1);
    return true;
}

void UWordQuestGameInstance::RewardNormalEnemy() { PlayerState.Coins += 3; }
void UWordQuestGameInstance::RewardBoss() { PlayerState.Coins += 10; }

bool UWordQuestGameInstance::BuyApple()
{
    if (bAppleBoughtThisVisit || PlayerState.Coins < 3 || PlayerState.CurrentHP >= PlayerState.MaxHP) return false;
    PlayerState.Coins -= 3;
    PlayerState.CurrentHP = FMath::Min(PlayerState.MaxHP, PlayerState.CurrentHP + 1);
    bAppleBoughtThisVisit = true;

    if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
    {
        Player->ShowFloatingText(TEXT("+1 HP"), FColor::Green, 160.f);
    }
    return true;
}

bool UWordQuestGameInstance::BuyStar()
{
    if (bStarBoughtThisVisit || PlayerState.Coins < 10) return false;
    PlayerState.Coins -= 10;
    PlayerState.StarProtectionHits += 2;
    bStarBoughtThisVisit = true;

    if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
    {
        Player->ShowFloatingText(TEXT("Star: 2 Blocks"), FColor::Cyan, 160.f);
    }
    return true;
}

bool UWordQuestGameInstance::BuyArmour()
{
    if (bArmourBoughtThisVisit || PlayerState.Coins < 25) return false;
    PlayerState.Coins -= 25;
    PlayerState.MaxHP += 10;
    PlayerState.CurrentHP += 10;
    bArmourBoughtThisVisit = true;

    if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
    {
        Player->ShowFloatingText(TEXT("+10 Max HP"), FColor::Yellow, 160.f);
    }
    return true;
}

bool UWordQuestGameInstance::IsGameOver() const { return PlayerState.CurrentHP <= 0; }

void UWordQuestGameInstance::ResetShopStock()
{
    bAppleBoughtThisVisit = false;
    bStarBoughtThisVisit = false;
    bArmourBoughtThisVisit = false;
}
