// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LevelupInfo.h"

int32 ULevelupInfo::FindLevelFromAmountOfExp(int32 Exp)
{
	int level = 1;
	bool bFoundTheLevel = false;
	while (!bFoundTheLevel) {
		if (LevelupInfo.Num() - 1 <= level) return level;
		if ((Exp -= LevelupInfo[level].RequiredExperience) >= 0) {
			level++;
		}
		else {
			bFoundTheLevel = true;
		}
	}

	return level;
}
