// Fill out your copyright notice in the Description page of Project Settings.


#include "BPFL.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMesh.h"
#include "json.hpp"
#include "AutomatedAssetImportData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Factories/TextureFactory.h"
#include "Misc/ScopedSlowTask.h"
#include "PSKXFactory.h"


void UBPFL::ImportTextures(TArray<FString> AllTexturesPath)
{
	auto AutomatedData = NewObject<UAutomatedAssetImportData>();
	AutomatedData->bReplaceExisting = false;
	auto TextureFactory = NewObject<UTextureFactory>();
	TextureFactory->NoCompression = true;
	TextureFactory->AutomatedImportData = AutomatedData;
	FScopedSlowTask ImportTask(AllTexturesPath.Num(), FText::FromString("Importing Textures"));
	ImportTask.MakeDialog(true);
	auto ActorIdx = -1;
	for (auto tx : AllTexturesPath)
	{
		ActorIdx++;
		FString TexGamePath, TexName;
		tx.Split(TEXT("\\"), &TexGamePath, &TexName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		FString PathForTextures = FString::Printf(TEXT("/Game/ValorantContent/Textures/%s"), *TexName.Replace(TEXT(".png"),TEXT("")));
		auto TexPackage = CreatePackage(*PathForTextures);
		auto bCancelled = false;
		auto NewTxName = TexName.Replace(TEXT(".png"),TEXT(""));
		auto CreatedTexture = TextureFactory->FactoryCreateFile(UTexture2D::StaticClass(), TexPackage, FName(*NewTxName), RF_Public | RF_Standalone, tx, NULL, GWarn, bCancelled); 
		if (CreatedTexture == nullptr)
		{
			continue;
		}
		auto Tex = CastChecked<UTexture2D>(CreatedTexture);
		/// tx 
		auto CompressionSetting = Tex->CompressionSettings;
		if (NewTxName.EndsWith("MRA") && CompressionSetting != TC_Masks)
		{
			Tex->SRGB = false;
			Tex->CompressionSettings = TC_Masks;
		}
		if (NewTxName.EndsWith("NM") && CompressionSetting != TC_Normalmap)
		{
			Tex->SRGB = false;
			Tex->CompressionSettings = TC_Normalmap;
		}
		if (NewTxName.EndsWith("DF") && CompressionSetting != TC_Normalmap)
		{
			Tex->SRGB = true;
			Tex->CompressionSettings = TC_Default;
		}
		ImportTask.DefaultMessage = FText::FromString(FString::Printf(TEXT("Importing Texture : %d of %d: %s"), ActorIdx + 1, AllTexturesPath.Num() + 1, *NewTxName));
		ImportTask.EnterProgressFrame();
		Tex->MarkPackageDirty();
		FAssetRegistryModule::AssetCreated(Tex);
		Tex->PreEditChange(nullptr);
		Tex->PostEditChange();
	}
}

void UBPFL::ImportMeshes(TArray<FString> AllMeshesPath, FString ObjectsPath)
{
	auto AutomatedData = NewObject<UAutomatedAssetImportData>();
	AutomatedData->bReplaceExisting = false;
	auto PSKFactory = NewObject<UPSKXFactory>();
	PSKFactory->AutomatedImportData = AutomatedData;
	FScopedSlowTask ImportTask(AllMeshesPath.Num(), FText::FromString("Importing Meshes"));
	ImportTask.MakeDialog(true); 
	int ActorIdx = -1;
	for (FString MPath : AllMeshesPath)
	{
		FString MeshGamePath, MeshName;
		MPath.Split(TEXT("\\"), &MeshGamePath, &MeshName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		MeshName = MeshName.Replace(TEXT(".pskx"), TEXT(".json"));
		FPaths Path;
		ActorIdx++;
		// JSON Stuff
		FString UmapJson;
		int LMRES = 256;
		std::string BodySetupProps = "CTF_UseDefault";
		int LMCoord = 0; 
		float LMDens = 0.0;
		FString Filename = Path.Combine(ObjectsPath, MeshName);
		FFileHelper::LoadFileToString(UmapJson, *Filename);
		auto Umap = nlohmann::json::parse(TCHAR_TO_UTF8(*UmapJson));
		auto BodySetup = Umap[0];
		auto StaticMeshPP = Umap[2];
		if (!BodySetup["Properties"]["CollisionTraceFlag"].is_null())
		{
			BodySetupProps = BodySetup["Properties"]["CollisionTraceFlag"].get<std::string>();
		}
		auto StaticProps = StaticMeshPP["Properties"];
		if (!StaticProps["LightMapResolution"].is_null())
		{
			LMRES = StaticProps["LightMapResolution"].get<int>();
		}
		if (!StaticProps["LightMapCoordinateIndex"].is_null())
		{
			LMCoord = StaticProps["LightMapCoordinateIndex"].get<int>();
		}
		if (!StaticProps["LightMapDensity"].is_null())
		{
			LMDens = StaticProps["LightMapDensity"].get<float>();
		}
		///// end json stuff
		MeshName = MeshName.Replace(TEXT(".json"), TEXT(""));
		FString PathForMeshes = FString::Printf(TEXT("/Game/ValorantContent/Meshes/%s"), *MeshName);
		auto MeshPackage = CreatePackage(*PathForMeshes);
		auto bCancelled = false;
		auto CreatedMesh = PSKFactory->FactoryCreateFile(UStaticMesh::StaticClass(), MeshPackage, FName(*MeshName), RF_Public | RF_Standalone, MPath, NULL, GWarn, bCancelled);
		if (CreatedMesh == nullptr)
		{
			continue;
		}
		auto Msh = CastChecked<UStaticMesh>(CreatedMesh);
		////////////
		//Msh->Modify();
		//Msh->SetLightMapResolution(LMRES);
		//Msh->SetLightMapCoordinateIndex(LMCoord);
		//Msh->SetLightmapUVDensity(LMDens);
		Msh->GetBodySetup()->CollisionTraceFlag = GetTraceFlag(BodySetupProps.c_str());
		ImportTask.DefaultMessage = FText::FromString(FString::Printf(TEXT("Importing Mesh : %d of %d: %s"), ActorIdx + 1, AllMeshesPath.Num() + 1, *MeshName));
		ImportTask.EnterProgressFrame();
		//Msh->Property
	}
}

ECollisionTraceFlag UBPFL::GetTraceFlag(FString tflag)
{
	//CTF_UseComplexAsSimple
	if (tflag == "CTF_UseDefault")
	{
		return CTF_UseDefault;
	}
	if (tflag == "CTF_UseSimpleAndComplex")
	{
		return CTF_UseSimpleAndComplex;
	}
	if (tflag == "CTF_UseSimpleAsComplex")
	{
		return CTF_UseSimpleAsComplex;
	}
	if (tflag == "CTF_UseComplexAsSimple")
	{
		return CTF_UseComplexAsSimple;
	}
	if (tflag == "CTF_MAX")
	{
		return CTF_MAX;
	}
	return CTF_UseSimpleAsComplex;
}

