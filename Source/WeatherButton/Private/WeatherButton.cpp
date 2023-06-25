// Copyright Epic Games, Inc. All Rights Reserved.

#include "WeatherButton.h"
#include "WeatherButtonStyle.h"
#include "WeatherButtonCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"


#include "Kismet/GameplayStatics.h"
#include "Engine/DirectionalLight.h"
#include "Engine/PostProcessVolume.h"
#include "Components/LightComponent.h"
#include "NiagaraComponent.h"





static const FName WeatherButtonTabName("WeatherButton");

#define LOCTEXT_NAMESPACE "FWeatherButtonModule"

void FWeatherButtonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FWeatherButtonStyle::Initialize();
	FWeatherButtonStyle::ReloadTextures();

	FWeatherButtonCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FWeatherButtonCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FWeatherButtonModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FWeatherButtonModule::RegisterMenus));
}

void FWeatherButtonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FWeatherButtonStyle::Shutdown();

	FWeatherButtonCommands::Unregister();
}

void FWeatherButtonModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::FromString(("Changing weather rain"));
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);


	AActor* FoundActor;
	TArray<UActorComponent*> ActorComponents;
	//UNiagaraComponent* NiagaraComponent = nullptr;

	FoundActor = FindActor(ADirectionalLight::StaticClass());

	if (FoundActor)
	{
		ADirectionalLight* Sun = Cast<ADirectionalLight>(FoundActor);
		Sun->GetLightComponent()->SetIntensity(1.f);
	}
	
	FoundActor = FindActor(APostProcessVolume::StaticClass());
	APostProcessVolume* PPVol;

	if (!FoundActor)
	{
		DialogText = FText::FromString(("PostProcessVolume Not Found, Creating one"));
		FMessageDialog::Open(EAppMsgType::Ok, DialogText);
		FoundActor = AddActor(APostProcessVolume::StaticClass());
		
	}

	
	PPVol = Cast<APostProcessVolume>(FoundActor);
	PPVol->Settings.AutoExposureBias = -4.f;
	PPVol->Settings.bOverride_AutoExposureBias = true;
	PPVol->bUnbound = true;


	// Create a new UNiagaraComponent and set its asset to a Niagara particle system
	UNiagaraComponent* NiagaraComponent = NewObject<UNiagaraComponent>(PPVol);
	if (NiagaraComponent)
	{
		NiagaraComponent->SetAsset(LoadObject<UNiagaraSystem>(nullptr, TEXT("/WeatherButton/PS"), nullptr, LOAD_None, nullptr));
		NiagaraComponent->RegisterComponent();

		// Get the actor's components as an array and check if the Niagara component is in the array
		ActorComponents.Empty();
		PPVol->GetComponents(ActorComponents);
		if (ActorComponents.Contains(NiagaraComponent))
		{
			// The Niagara component is added successfully
			DialogText = FText::FromString(("NiagaraComponent added successfully"));
			FMessageDialog::Open(EAppMsgType::Ok, DialogText);
		}
		else
		{
			// The Niagara component is not added to the actor's components
			DialogText = FText::FromString(("NiagaraComponent not added to the actor's components"));
			FMessageDialog::Open(EAppMsgType::Ok, DialogText);
		}
	}
	else
	{
		// Failed to create the Niagara component
		DialogText = FText::FromString(("Failed to create the NiagaraComponent"));
		FMessageDialog::Open(EAppMsgType::Ok, DialogText);
	}
	

}

void FWeatherButtonModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FWeatherButtonCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FWeatherButtonCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

AActor* FWeatherButtonModule::FindActor(TSubclassOf<AActor> ActorClass)
{
	TArray<AActor*> FoundActors;
	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (World)
	{
		UGameplayStatics::GetAllActorsOfClass(World, ActorClass, FoundActors);
		if (FoundActors.Num() > 0)
		{
			return FoundActors[0];
		}
	}

	return nullptr;
}

AActor* FWeatherButtonModule::AddActor(TSubclassOf<AActor> ActorClass)
{
	ULevel* Level = GEditor->GetEditorWorldContext().World()->GetCurrentLevel();
	return GEditor->AddActor(Level,ActorClass,FTransform());	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FWeatherButtonModule, WeatherButton)