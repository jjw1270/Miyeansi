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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "변경할 상태 저장소 영역. Bool/Int/Name은 Key를, Fragment/SeenEvent/Ending은 ItemID를 사용"))
	EVNStateDomain Domain = EVNStateDomain::Bool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "Bool, Int, Name 도메인에서 변경할 상태 키"))
	FName Key = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "Fragment, SeenEvent, Ending 도메인에서 변경할 VN ItemID"))
	FVNItemID ItemID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "상태 변경 방식. Set/Add/Remove/Max/Min 중 선택"))
	EVNStateOp Op = EVNStateOp::Set;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "Bool 도메인 값 또는 ItemID 기록의 추가/제거 여부"))
	bool BoolValue = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "Int 도메인 값, Add/Max/Min 비교값, Ending Add 횟수"))
	int32 IntValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "Name 도메인에 저장할 이름 값"))
	FName NameValue = NAME_None;
};