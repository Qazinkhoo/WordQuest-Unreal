# Word Quest — Unreal Engine

A 2D educational side-scrolling adventure for Malaysian Year 4 English using British English.

## Core game design

- 10 themed stages: Whispering Forest, Sunny Meadow, Crystal Cave, Misty Mountain, Ancient Ruins, Moonlit Marsh, Frozen Valley, Sky Kingdom, Dragon Pass and Word Castle.
- Five waves per stage. Waves 1–4 use 1 HP normal enemies; Wave 5 is a 3 HP boss.
- The hero starts at 10/10 HP and 0 damage, then collects a sword near the beginning of Stage 1 to permanently gain 1 damage.
- Correct English answers attack the enemy. Wrong answers cause 1 damage unless Star protection is active.
- Normal enemies reward 3 coins and bosses reward 10 coins.
- HP, maximum HP, coins, sword and unused Star protection persist between stages. There is no automatic healing between stages.
- A shop opens after each boss. Apple costs 3 coins and restores 1 HP; Star costs 10 coins and protects against the next two enemy hits; Armour costs 25 coins and permanently adds 10 maximum HP and 10 current HP.
- Each shop item can be purchased once per shop visit and restocks at the next shop.
- Questions are Year 4 level, use British English, are randomised, and do not repeat during an adventure.
- Reaching 0 HP triggers game over rather than automatic healing.

## Current foundation

The repository contains the Unreal C++ module, persistent player-state/shop mechanics, Paper2D dependency and the first Year 4 question subsystem. Visual levels, characters, widgets, animation and audio will be added in subsequent development passes.

## Local Unreal workflow

Clone the repository to the Windows PC that has Unreal Engine installed. Open `WordQuest.uproject`. Unreal may ask to build the C++ module or generate project files the first time. Generated Unreal folders are intentionally excluded from Git.
