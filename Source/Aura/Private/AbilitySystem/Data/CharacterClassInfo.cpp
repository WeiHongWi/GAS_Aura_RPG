// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterDefaultInfo UCharacterClassInfo::GetCharacterDefaultInfo(ECharacterClass CharacterClass)
{
	return CharacterInfo.FindChecked(CharacterClass);
}
