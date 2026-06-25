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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "검사할 상태 저장소 영역. Bool/Int/Name은 Key를, Fragment/SeenEvent/Ending은 ItemID를 사용"))
	EVNStateDomain Domain = EVNStateDomain::Bool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "Bool, Int, Name 도메인에서 검사할 상태 키"))
	FName Key = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "Fragment, SeenEvent, Ending 도메인에서 검사할 VN ItemID"))
	FVNItemID ItemID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "조건 비교 방식. Equal/Greater/Exists 등 도메인에 맞는 연산을 선택"))
	EVNCompareOp Op = EVNCompareOp::Equal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "Bool 도메인 비교 기준값. ItemID 존재 조건에서는 Equal/NotEqual 기준으로도 사용 가능"))
	bool BoolValue = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "Int 도메인 비교 기준값"))
	int32 IntValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "Name 도메인 비교 기준값"))
	FName NameValue = NAME_None;
};

USTRUCT(BlueprintType, meta = (ShortToolTip = "All/Any/None 조합 조건"))
struct VISUALNOVEL_API FVNConditionSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "모두 참이어야 통과하는 조건 목록"))
	TArray<FVNCondition> All;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "비어 있으면 통과, 값이 있으면 하나 이상 참이어야 통과하는 조건 목록"))
	TArray<FVNCondition> Any;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "모두 거짓이어야 통과하는 금지 조건 목록"))
	TArray<FVNCondition> None;
};