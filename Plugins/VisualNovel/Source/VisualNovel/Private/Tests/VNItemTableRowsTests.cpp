// Copyright (c) 2026 장윤제. All rights reserved.

#if WITH_DEV_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"
#include "VNItemTableRows.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNItemTableRowsDefaultTypeTest,
	"VisualNovel.ItemTableRows.DefaultTypes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNItemTableRowsDefaultTypeTest::RunTest(const FString& _parameters)
{
	FVNCharacterTableRow character_row;
	FVNFragmentTableRow fragment_row;
	FVNEventTableRow event_row;
	FVNEndingTableRow ending_row;

	TestEqual(TEXT("Character row defaults to Character ItemType"), character_row.ItemID.GetType(), EItemType::Character);
	TestEqual(TEXT("Fragment row defaults to Fragment ItemType"), fragment_row.ItemID.GetType(), EItemType::Fragment);
	TestEqual(TEXT("Event row defaults to Event ItemType"), event_row.ItemID.GetType(), EItemType::Event);
	TestEqual(TEXT("Ending row defaults to Ending ItemType"), ending_row.ItemID.GetType(), EItemType::Ending);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FVNItemTableRowsMetadataTest,
	"VisualNovel.ItemTableRows.Metadata",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVNItemTableRowsMetadataTest::RunTest(const FString& _parameters)
{
	FVNCharacterTableRow character_row;
	character_row.DisplayName = FText::FromString(TEXT("하연"));
	character_row.Description = FText::FromString(TEXT("축제와 사고 기억의 핵심 캐릭터"));
	character_row.NameColor = FLinearColor::Red;
	character_row.DialogueStyleID = TEXT("Hayeon");

	TestEqual(TEXT("Character display name is stored"), character_row.DisplayName.ToString(), FString(TEXT("하연")));
	TestEqual(TEXT("Character dialogue style ID is stored"), character_row.DialogueStyleID, FName(TEXT("Hayeon")));
	TestEqual(TEXT("Character name color is stored"), character_row.NameColor, FLinearColor::Red);

	FVNFragmentTableRow fragment_row;
	fragment_row.RelatedCharacterID = FVNCharacterID(FItemID(EItemType::Character, 1, 1));
	fragment_row.RelatedEndingID = FVNEndingID(FItemID(EItemType::Ending, 1, 1));

	TestEqual(TEXT("Fragment related character uses Character type"), fragment_row.RelatedCharacterID.GetType(), EItemType::Character);
	TestEqual(TEXT("Fragment related ending uses Ending type"), fragment_row.RelatedEndingID.GetType(), EItemType::Ending);

	FVNEventTableRow event_row;
	event_row.DefaultDayID = TEXT("DDay");
	event_row.DefaultSlot = EVNDaySlot::Night;
	event_row.CategoryTags = { TEXT("Final"), TEXT("Auto") };

	TestEqual(TEXT("Event default day is stored"), event_row.DefaultDayID, FName(TEXT("DDay")));
	TestEqual(TEXT("Event default slot is stored"), event_row.DefaultSlot, EVNDaySlot::Night);
	TestEqual(TEXT("Event tags are stored"), event_row.CategoryTags.Num(), 2);

	FVNEndingTableRow ending_row;
	ending_row.EndingTypeID = TEXT("True");
	ending_row.ShouldShowInGallery = false;

	TestEqual(TEXT("Ending type ID is stored"), ending_row.EndingTypeID, FName(TEXT("True")));
	TestFalse(TEXT("Ending gallery flag is stored"), ending_row.ShouldShowInGallery);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
