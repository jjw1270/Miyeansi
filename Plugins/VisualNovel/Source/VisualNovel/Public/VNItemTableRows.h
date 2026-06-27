// Copyright (c) 2026 장윤제. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ItemTableRow.h"
#include "VNDefines.h"
#include "VNItemIDs.h"
#include "VNItemTableRows.generated.h"

USTRUCT(BlueprintType, meta = (ShortToolTip = "VN 캐릭터 Item Row"))
struct VISUALNOVEL_API FVNCharacterTableRow : public FItemTableRow
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "대사창 이름, 포커스 UI 등에 사용할 기본 색상"))
	FLinearColor NameColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "기본 스탠딩/표정 리소스 ID. 전용 ItemType 확정 전까지 범용 FItemID로 보관"))
	FItemID DefaultSpriteID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "대사창/음성/텍스트 스타일을 프로젝트 데이터에서 찾기 위한 스타일 ID"))
	FName DialogueStyleID = NAME_None;

public:
	FVNCharacterTableRow()
		: FItemTableRow(EItemType::Character)
	{
	}
};

USTRUCT(BlueprintType, meta = (ShortToolTip = "VN 기억 조각/단서 Item Row"))
struct VISUALNOVEL_API FVNFragmentTableRow : public FItemTableRow
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "도감/선택지 UI에서 사용할 아이콘 리소스 ID"))
	FItemID IconID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "이 단서와 관련된 캐릭터 ID"))
	FVNCharacterID RelatedCharacterID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "이 단서가 연결되는 엔딩 ID"))
	FVNEndingID RelatedEndingID;

public:
	FVNFragmentTableRow()
		: FItemTableRow(EItemType::Fragment)
	{
	}
};

USTRUCT(BlueprintType, meta = (ShortToolTip = "VN 이벤트 Item Row"))
struct VISUALNOVEL_API FVNEventTableRow : public FItemTableRow
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (MultiLine = true, ToolTip = "허브 목록, 제작 검수, 디버그에 사용할 이벤트 요약"))
	FText Summary;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "이벤트 분류, 검색, 제작 상태 필터에 사용할 태그"))
	TArray<FName> CategoryTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "이벤트가 기본적으로 배치되는 날짜 ID"))
	FName DefaultDayID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "이벤트가 기본적으로 배치되는 하루 슬롯"))
	EVNDaySlot DefaultSlot = EVNDaySlot::None;

public:
	FVNEventTableRow()
		: FItemTableRow(EItemType::Event)
	{
	}
};

USTRUCT(BlueprintType, meta = (ShortToolTip = "VN 엔딩 Item Row"))
struct VISUALNOVEL_API FVNEndingTableRow : public FItemTableRow
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "Primary/Sad/Loop/Hidden 같은 프로젝트별 엔딩 분류 ID"))
	FName EndingTypeID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "갤러리 대표 이미지 또는 썸네일 리소스 ID"))
	FItemID RepresentativeImageID = FItemID::Zero;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VisualNovel", meta = (ToolTip = "엔딩 갤러리/회상 목록에 표시할지 여부"))
	bool ShouldShowInGallery = true;

public:
	FVNEndingTableRow()
		: FItemTableRow(EItemType::Ending)
	{
	}
};
