// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BPFL.generated.h"

class AActor;
class UPSKXFactory;
class USCS_Node;
class UBrushComponent;

UCLASS()
class UNREALPSKPSA_API UBPFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = ProjectSettings)
	static void ImportTextures(TArray<FString> AllTexturesPath);
	UFUNCTION(BlueprintCallable, Category = ProjectSettings)
	static void ImportMeshes(TArray<FString> AllMeshesPath, FString ObjectsPath);
	static ECollisionTraceFlag GetTraceFlag(FString tflag);
};
