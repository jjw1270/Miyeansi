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
	Summary					UMETA(Tooltip = "루프 이후 압축 재방문 후보"),
};

UENUM(BlueprintType, meta = (ShortToolTip = "상태 변경 연산"))
enum class EVNStateOp : uint8
{
	Set							UMETA(Tooltip = "대상 상태를 입력 값으로 교체"),
	Add							UMETA(Tooltip = "정수 상태에 값을 더하거나 엔딩 관람 횟수를 증가"),
	Remove						UMETA(Tooltip = "대상 상태 키 또는 ItemID 기록을 제거"),
	Max							UMETA(Tooltip = "현재 정수 값과 입력 값 중 더 큰 값으로 갱신"),
	Min							UMETA(Tooltip = "현재 정수 값과 입력 값 중 더 작은 값으로 갱신"),
};

UENUM(BlueprintType, meta = (ShortToolTip = "조건 비교 연산"))
enum class EVNCompareOp : uint8
{
	Equal						UMETA(Tooltip = "현재 값이 비교 값과 같으면 참"),
	NotEqual				UMETA(Tooltip = "현재 값이 비교 값과 다르면 참"),
	Greater					UMETA(Tooltip = "현재 정수 값이 비교 값보다 크면 참"),
	GreaterEqual			UMETA(Tooltip = "현재 정수 값이 비교 값 이상이면 참"),
	Less						UMETA(Tooltip = "현재 정수 값이 비교 값보다 작으면 참"),
	LessEqual				UMETA(Tooltip = "현재 정수 값이 비교 값 이하이면 참"),
	Exists					UMETA(Tooltip = "대상 키 또는 ItemID 기록이 존재하면 참"),
	NotExists				UMETA(Tooltip = "대상 키 또는 ItemID 기록이 없으면 참"),
};

UENUM(BlueprintType, meta = (ShortToolTip = "상태/조건이 조회하거나 변경할 영역"))
enum class EVNStateDomain : uint8
{
	Bool						UMETA(Tooltip = "BoolMap의 플래그 상태. Key를 사용"),
	Int							UMETA(Tooltip = "IntMap의 정수 상태. Key를 사용"),
	Name						UMETA(Tooltip = "NameMap의 이름형 상태. Key를 사용"),
	Fragment				UMETA(Tooltip = "기억 조각/사고 단서 보유 목록. Fragment ItemID를 사용"),
	SeenEvent				UMETA(Tooltip = "이미 본 이벤트 목록. Event ItemID를 사용"),
	Ending					UMETA(Tooltip = "엔딩 관람 기록. Ending ItemID를 사용"),
};