// Copyright Epic Games, Inc. All Rights Reserved.

#include "WeatherButtonCommands.h"

#define LOCTEXT_NAMESPACE "FWeatherButtonModule"

void FWeatherButtonCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "WeatherButton", "Execute WeatherButton action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
