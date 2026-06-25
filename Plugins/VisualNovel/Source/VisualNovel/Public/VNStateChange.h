// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "VNItemIDs.h"
#include "VNDefines.h"
#include "VNStateChange.generated.h"

USTRUCT(BlueprintType, meta = (ShortToolTip = "미연시 진행 상태 변경 명령"))
struct VISUALNOVEL_API FVNStateChange
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
	EVNStateOp Op = EVNStateOp::Set;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	bool BoolValue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	int32 IntValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FName NameValue = NAME_None;
};