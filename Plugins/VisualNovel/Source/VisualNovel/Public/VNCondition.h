// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "VNItemIDs.h"
#include "VNDefines.h"
#include "VNCondition.generated.h"

USTRUCT(BlueprintType, meta = (ShortToolTip = "미연시 진행 조건"))
struct VISUALNOVEL_API FVNCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	EVNStateDomain Domain = EVNStateDomain::Bool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FName Key = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FVNItemID ItemID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	EVNCompareOp Op = EVNCompareOp::Equal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	bool BoolValue = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	int32 IntValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FName NameValue = NAME_None;
};

USTRUCT(BlueprintType, meta = (ShortToolTip = "All/Any/None 조합 조건"))
struct VISUALNOVEL_API FVNConditionSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	TArray<FVNCondition> All;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	TArray<FVNCondition> Any;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	TArray<FVNCondition> None;
};