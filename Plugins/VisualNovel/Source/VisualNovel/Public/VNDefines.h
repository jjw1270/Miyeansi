// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "VNDefines.generated.h"

UENUM(BlueprintType)
enum class EVNDaySlot : uint8
{
	None						UMETA(Tooltip = "슬롯 없음, 자동 컷신, 프롤로그/엔딩"),
	Morning					UMETA(Tooltip = "등교/오전 자동 이벤트"),
	Lunch						UMETA(Tooltip = "점심 이벤트"),
	AfterSchool			UMETA(Tooltip = "방과 후 핵심 선택"),
	Evening					UMETA(Tooltip = "저녁 정산/자동 이벤트"),
	Night						UMETA(Tooltip = "특수 컷신, 폐장 후, 루프 전환"),
};

UENUM(BlueprintType, meta = (ShortToolTip = "이벤트 재생 방식"))
enum class EVNEventRunMode : uint8
{
	Normal					UMETA(Tooltip = "일반 선택 이벤트"),
	Auto						UMETA(Tooltip = "조건 충족 시 선택 없이 바로 실행"),
	Once						UMETA(Tooltip = "1회만 실행"),
	Repeatable				UMETA(Tooltip = "반복 실행 가능"),
	Summary				UMETA(Tooltip = "루프 이후 압축 재방문 후보"),
};

UENUM(BlueprintType, meta = (ShortToolTip = "상태 변경 연산"))
enum class EVNStateOp : uint8
{
	Set,
	Add,
	Remove,
	Max,
	Min,
};

UENUM(BlueprintType, meta = (ShortToolTip = "조건 비교 연산"))
enum class EVNCompareOp : uint8
{
	Equal,
	NotEqual,
	Greater,
	GreaterEqual,
	Less,
	LessEqual,
	Exists,
	NotExists,
};

UENUM(BlueprintType, meta = (ShortToolTip = "상태/조건이 조회하거나 변경할 영역"))
enum class EVNStateDomain : uint8
{
	Bool,
	Int,
	Name,
	Fragment,
	SeenEvent,
	Ending,
};
