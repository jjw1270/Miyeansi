// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "VNItemIDs.h"
#include "StoryFlowDefines.h"
#include "VNDefines.h"
#include "VNStoryState.generated.h"

USTRUCT(BlueprintType, meta = (ShortToolTip = "캐릭터별 미연시 진행 상태"))
struct VISUALNOVEL_API FVNCharState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "일반 호감/친밀도. 진엔딩 조건 자체보다 관계 온도 표현에 사용"))
	int32 Affinity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "핵심 신뢰도. 캐릭터별 주요 선택과 진전 조건에 사용"))
	int32 Trust = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "캐릭터 루트 진행 단계. 이벤트 개방과 중복 방지에 사용"))
	int32 Phase = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "관계 균열, 압박, 의존 같은 캐릭터별 부정 누적 수치"))
	int32 Strain = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "마지막으로 해당 캐릭터 이벤트를 진행한 하루 슬롯"))
	EVNDaySlot LastSlot = EVNDaySlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "마지막으로 해당 캐릭터 이벤트를 진행한 날짜 ID"))
	FName LastDay = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "해당 캐릭터의 주요 갈등 또는 클라이맥스가 해결되었는지 여부"))
	bool IsResolved = false;
};

USTRUCT(BlueprintType, meta = (ShortToolTip = "엔딩별 관람 기록"))
struct VISUALNOVEL_API FVNEndingState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "이 엔딩을 한 번이라도 본 적이 있는지 여부"))
	bool HasBeenSeen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "이 엔딩을 본 총 횟수"))
	int32 SeenCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "이 엔딩을 마지막으로 본 날짜 ID"))
	FName LastSeenDay = NAME_None;
};

USTRUCT(BlueprintType, meta = (ShortToolTip = "저장 가능한 진행 상태의 루트 구조"))
struct VISUALNOVEL_API FVNStoryState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "현재 날짜 ID. 예: D25, D1, DDay, DPlus2"))
	FName CurrentDay = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "현재 주차 또는 구간 ID. 예: Week1, FinalWeek, TrueEnding"))
	FName CurrentWeek = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "현재 하루 행동 슬롯. 이벤트 허브와 선택 가능 이벤트 판정에 사용"))
	EVNDaySlot CurrentSlot = EVNDaySlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "실패 루프 횟수. 반복 이벤트 압축과 루프 연출에 사용"))
	int32 LoopCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "저장/로드 시 복원할 StoryFlow Scene/Shot 위치"))
	FStoryFlowRef CurrentFlowRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "플래그형 상태 저장소. 조건 키와 상태 변경 Key로 접근"))
	TMap<FName, bool> BoolMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "호감도, 신뢰, 회피 같은 정수 상태 저장소. 조건 키와 상태 변경 Key로 접근"))
	TMap<FName, int32> IntMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "선택 결과, 분기 결과 같은 이름형 상태 저장소. 조건 키와 상태 변경 Key로 접근"))
	TMap<FName, FName> NameMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "Character ItemID별 캐릭터 진행 상태"))
	TMap<FVNCharacterID, FVNCharState> CharMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "Fragment ItemID 기반 기억 조각/사고 단서 보유 목록"))
	TSet<FVNFragmentID> Fragments;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "Ending ItemID별 엔딩 관람/반복 기록"))
	TMap<FVNEndingID, FVNEndingState> EndingMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel", meta = (Tooltip = "Event ItemID 기반 이미 본 이벤트 목록"))
	TSet<FVNEventID> SeenEvents;
};