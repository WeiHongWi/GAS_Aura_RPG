// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/LoadScreenSaveGame.h"

FSavedMap ULoadScreenSaveGame::GetSavedMapFromMapName(const FString& InMapName)
{
	for (FSavedMap SaveMap : SavedMaps) {
		if (SaveMap.MapAssetName == InMapName) {
			return SaveMap;
		}
	}
	return FSavedMap();
}

bool ULoadScreenSaveGame::HasMap(const FString& InMapName)
{
	for (FSavedMap SaveMap : SavedMaps) {
		if (SaveMap.MapAssetName == InMapName) {
			return true;
		}
	}
	return false;
}
