// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "WeatherButtonStyle.h"

class FWeatherButtonCommands : public TCommands<FWeatherButtonCommands>
{
public:

	FWeatherButtonCommands()
		: TCommands<FWeatherButtonCommands>(TEXT("WeatherButton"), NSLOCTEXT("Contexts", "WeatherButton", "WeatherButton Plugin"), NAME_None, FWeatherButtonStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
