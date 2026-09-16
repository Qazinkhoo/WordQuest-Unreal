#include "WordQuestSwordPickup.h"
#include "WordQuestCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AWordQuestSwordPickup::AWordQuestSwordPickup()
{
    PrimaryActorTick.bCanEverTick = false;
    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    SetRootComponent(Trigger);
    Trigger->SetBoxExtent(FVector(50.f));
    Trigger->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    Visual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visual"));
    Visual->SetupAttachment(Trigger);
    Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWordQuestSwordPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
    Super::NotifyActorBeginOverlap(OtherActor);
    if (AWordQuestCharacter* Player = Cast<AWordQuestCharacter>(OtherActor))
    {
        Player->CollectSword();
        Destroy();
    }
}
