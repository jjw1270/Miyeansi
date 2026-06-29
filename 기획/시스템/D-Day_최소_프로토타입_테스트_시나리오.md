# D-Day 최소 프로토타입 테스트 시나리오

이 문서는 `VisualNovelPlugin` 1차 구현이 실제로 동작하는지 확인하기 위한 최소 테스트 명세다.

스토리 정본은 [D-Day 스크립트 분할](../스토리/13_D-Day_스크립트_분할.md)과 [D-1/D-Day 조건 수치화](../스토리/12_D-1_D-Day_조건_수치화.md)를 따른다. 이 문서는 그 내용을 개발 검증용 상태값과 기대 결과로 옮긴다.

## 1. 목표

1차 프로토타입에서 반드시 증명해야 하는 흐름은 다음 네 가지다.

1. `DDay_03Assess`에서 D-Day 결과를 계산한다.
2. `DDay_04Gate`에서 `DDayResult`에 따라 다른 Scene으로 간다.
3. 진엔딩 조건 충족 시 `DDay_04TrueChoice`에서 최종 선택 후 `DDay_05True`로 진입하고 `TE_00_TimeSkip`로 넘어간다.
4. 실패 루프 1종 이상이 D-25 복귀 상태를 만든다.

이 테스트가 통과하면 `FVNStoryState`, `FVNCondition`, `FVNStateChange`, `UVNDialogueShot`, `UVNChoiceShot`, 프로젝트 전용 분기의 최소 연결이 검증된다.

## 2. 테스트 범위

포함:

- `FVNStoryState` 초기화와 상태 주입
- `EvaluateDDayResult()`에 해당하는 프로젝트 평가 로직
- `NameMap["DDayResult"]` 저장
- `DDay_04Gate` 조건 분기
- `UVNDialogueShot`의 `OnEnter` / `OnComplete` 상태 변경
- `UVNChoiceShot`의 `ResultKey` / `OnSelect` 상태 변경
- 실패 루프의 `LoopCount`, `ReturnDay` 변경

제외:

- 실제 UI 애니메이션 품질
- 캐릭터 스프라이트/배경 리소스 로딩 품질
- 크레딧 연출
- 전체 4주 이벤트 데이터 완성
- 선택지별 커스텀 출력 핀

## 3. 공통 테스트 전제

### 3.1 시작 위치

| 항목 | 값 |
|---|---|
| `CurrentDay` | `DDay` |
| `CurrentWeek` | `FinalWeek` |
| `CurrentSlot` | `Evening` |
| 시작 Scene | `DDay_03Assess` |
| 다음 Gate Scene | `DDay_04Gate` |

### 3.2 공통 기본 상태

각 테스트는 아래 값을 기본으로 두고, 케이스별 차이만 덮어쓴다.

| 영역 | 키 | 기본값 |
|---|---|---|
| BoolMap | `IsHiddenCollapseActive` | false |
| BoolMap | `HasHayeonD2Premonition` | true |
| BoolMap | `DidChooseHayeonOnDDay` | true |
| BoolMap | `IsHayeonBoothD1Complete` | true |
| BoolMap | `HasConfirmedHayeonMutualFeelings` | false |
| BoolMap | `IsSohaResolved` | true |
| BoolMap | `IsSeorinResolved` | true |
| BoolMap | `IsMiruResolved` | true |
| Fragments | `Fragment:HasSohaClue` | present |
| Fragments | `Fragment:HasSeorinClue` | present |
| Fragments | `Fragment:HasMiruClue` | present |
| Fragments | `Fragment:HasHayeonClue` | present |
| BoolMap | `HasAcceptedAccidentMemory` | false |
| BoolMap | `IsDDayTrueEnding` | false |
| BoolMap | `CanEnterTrueEndingEpilogue` | false |
| IntMap | `HayeonTrust` | 6 |
| IntMap | `HayeonPace` | 3 |
| IntMap | `SohaPressure` | 0 |
| IntMap | `SeorinControl` | 0 |
| IntMap | `MiruDepend` | 0 |
| IntMap | `Avoid` | 0 |
| IntMap | `LoopCount` | 0 |
| NameMap | `DDayChoice` | `Hayeon` |
| NameMap | `DDayResult` | `None` |
| NameMap | `ReturnDay` | `None` |

`ClueScore`는 저장 키가 아니라 평가 시 계산값이다. 1차에서는 다음처럼 계산한다.

```text
ClueScore =
  Fragments.Contains(Fragment:HasSohaClue) ? 1 : 0
+ Fragments.Contains(Fragment:HasSeorinClue) ? 1 : 0
+ Fragments.Contains(Fragment:HasMiruClue) ? 1 : 0
+ Fragments.Contains(Fragment:HasHayeonClue) ? 1 : 0
```

## 4. D-Day 결과 평가 기준

테스트용 평가 순서는 `13_D-Day_스크립트_분할.md`의 `EvaluateDDayResult()`와 같아야 한다.

```text
if IsHiddenCollapseActive == true:
    return HiddenCollapse

if HasHayeonD2Premonition == false:
    return HayeonMiss

if HayeonTrust < 6 or HayeonPace < 3:
    return HayeonMiss

if IsSohaResolved == false and SohaPressure == 3:
    return SohaSad

if IsSeorinResolved == false and SeorinControl == 3:
    return SeorinSad

if IsMiruResolved == false and MiruDepend == 3:
    return MiruSad

if IsSohaResolved == false or IsSeorinResolved == false or IsMiruResolved == false:
    return RelationUnresolved

if ClueScore < 3 or Fragments.Contains(Fragment:HasHayeonClue) == false:
    return AccFail

if Avoid >= 4:
    return AvoidLoop

return True
```

## 5. 필수 테스트 케이스

### TC-DDAY-001: 진엔딩 조건 충족

목적:

- 모든 핵심 조건이 충족되었을 때 `True` 결과와 진엔딩 Scene 진입을 검증한다.

입력 상태:

- 공통 기본 상태 그대로 사용

기대 결과:

| 단계 | 기대값 |
|---|---|
| `EvaluateDDayResult()` | `True` |
| `NameMap["DDayResult"]` | `True` |
| `DDay_04Gate` 출력 | `DDay_04TrueChoice` |
| `DDay_04TrueChoice` 선택 | `[하연에게 말하러 간다]` 선택 시 `DDay_05True` |
| `DDay_05True` 완료 후 | `HasConfirmedHayeonMutualFeelings = true`, `HasAcceptedAccidentMemory = true`, `IsDDayTrueEnding = true`, `CanEnterTrueEndingEpilogue = true` |
| 다음 Scene | `TE_00_TimeSkip` |

통과 기준:

- 프로젝트 전용 D-Day 분기 또는 평가 Shot이 `DDayResult == True`를 읽고 `DDay_04TrueChoice`로 보낸다.
- 최종 선택 Scene에서 `[하연에게 말하러 간다]`를 선택하면 `DDay_05True`로 보낸다.
- `DDay_05True`의 `OnComplete` 또는 대응 상태 변경이 진엔딩 후일담 진입 조건을 켠다.

### TC-DDAY-002: 하연 조건 부족 실패 루프

목적:

- 하연의 D-2 예감 이해가 부족하면 다른 조건이 충분해도 하연 실패 루프로 가는지 검증한다.

입력 상태:

| 영역 | 키 | 값 |
|---|---|---|
| BoolMap | `HasHayeonD2Premonition` | false |
| IntMap | `HayeonTrust` | 8 |
| IntMap | `HayeonPace` | 4 |
| BoolMap | `IsSohaResolved` | true |
| BoolMap | `IsSeorinResolved` | true |
| BoolMap | `IsMiruResolved` | true |
| Fragments | `Fragment:HasHayeonClue` | present |

기대 결과:

| 단계 | 기대값 |
|---|---|
| `EvaluateDDayResult()` | `HayeonMiss` |
| `NameMap["DDayResult"]` | `HayeonMiss` |
| `DDay_04Gate` 출력 | `DDay_06HayeonMiss` |
| 실패 처리 후 | `LoopCount += 1`, `ReturnDay = D25` |
| 다음 흐름 | D-25 월요일 허브 복귀 |

통과 기준:

- `HasHayeonD2Premonition == false`가 다른 성공 조건보다 먼저 실패를 만든다.
- 실패 루프는 D+3로 가지 않고 D-25로 돌아간다.

### TC-DDAY-REL-001: 기존 관계 미정리 약화판 루프

목적:

- 하연 조건은 충분하지만 기존 관계 하나가 약하게 미정리된 경우 `HayeonMiss`가 아니라 `RelationUnresolved`로 가는지 검증한다.

입력 상태:

| 영역 | 키 | 값 |
|---|---|---|
| BoolMap | `IsSohaResolved` | false |
| IntMap | `SohaPressure` | 2 |
| BoolMap | `IsSeorinResolved` | true |
| BoolMap | `IsMiruResolved` | true |
| BoolMap | `HasHayeonD2Premonition` | true |
| IntMap | `HayeonTrust` | 6 |
| IntMap | `HayeonPace` | 3 |
| Fragments | `Fragment:HasHayeonClue` | present |
| IntMap | `Avoid` | 2 |

추가 계산:

```text
ClueScore = 3
```

기대 결과:

| 단계 | 기대값 |
|---|---|
| `EvaluateDDayResult()` | `RelationUnresolved` |
| `NameMap["DDayResult"]` | `RelationUnresolved` |
| `DDay_04Gate` 출력 | `DDay_10RelationUnresolved` |
| 실패 처리 후 | `IsRelationUnresolvedLoop = true`, `LoopCount += 1`, `ReturnDay = D25` |

통과 기준:

- 하연 조건은 충분하므로 `HayeonMiss`로 보내지 않는다.
- `SohaPressure == 3`이 아니므로 소하 완전 새드엔딩이 아니라 관계 미정리 루프로 보낸다.

### TC-DDAY-003: 사고 단서 부족 실패 루프

목적:

- 하연 감정선은 충분하지만 사고 단서가 부족하면 `AccFail`로 가는지 검증한다.

입력 상태:

| 영역 | 키 | 값 |
|---|---|---|
| BoolMap | `HasHayeonD2Premonition` | true |
| IntMap | `HayeonTrust` | 6 |
| IntMap | `HayeonPace` | 3 |
| BoolMap | `IsSohaResolved` | true |
| BoolMap | `IsSeorinResolved` | true |
| BoolMap | `IsMiruResolved` | true |
| Fragments | `Fragment:HasSohaClue` | present |
| Fragments | `Fragment:HasSeorinClue` | present |
| Fragments | `Fragment:HasMiruClue` | absent |
| Fragments | `Fragment:HasHayeonClue` | absent |

계산:

```text
ClueScore = 2
```

기대 결과:

| 단계 | 기대값 |
|---|---|
| `EvaluateDDayResult()` | `AccFail` |
| `NameMap["DDayResult"]` | `AccFail` |
| `DDay_04Gate` 출력 | `DDay_11AccFail` |
| 실패 처리 후 | `LoopCount += 1`, `ReturnDay = D25` |

통과 기준:

- `ClueScore < 3` 또는 `Fragment:HasHayeonClue` 미보유가 `AccFail`을 만든다.
- 이 실패는 하연 감정 부족이 아니라 사고 의미 미연결 실패로 분류된다.

### TC-DDAY-004: 회피 누적 실패 루프

목적:

- 하연/관계/단서 조건이 충분해도 회피가 높으면 `AvoidLoop`가 우선되는지 검증한다.

입력 상태:

| 영역 | 키 | 값 |
|---|---|---|
| BoolMap | `HasHayeonD2Premonition` | true |
| IntMap | `HayeonTrust` | 8 |
| IntMap | `HayeonPace` | 4 |
| BoolMap | `IsSohaResolved` | true |
| BoolMap | `IsSeorinResolved` | true |
| BoolMap | `IsMiruResolved` | true |
| Fragments | `Fragment:HasSohaClue` | present |
| Fragments | `Fragment:HasSeorinClue` | present |
| Fragments | `Fragment:HasMiruClue` | present |
| Fragments | `Fragment:HasHayeonClue` | present |
| IntMap | `Avoid` | 4 |

기대 결과:

| 단계 | 기대값 |
|---|---|
| `EvaluateDDayResult()` | `AvoidLoop` |
| `NameMap["DDayResult"]` | `AvoidLoop` |
| `DDay_04Gate` 출력 | `DDay_12Avoid` |
| 실패 처리 후 | `HasSeenAvoidLoop = true`, `LoopCount += 1`, `ReturnDay = D25` |

통과 기준:

- `Avoid >= 4`가 진엔딩 직전의 마지막 가드로 작동한다.
- 이 케이스는 관계 실패나 단서 실패가 아니라 마지막 선택 회피 실패로 분류된다.

### TC-DDAY-005: 히든 붕괴 우선순위 가드

목적:

- `HiddenCollapse`가 켜져 있으면 모든 일반 결과보다 먼저 히든 붕괴로 가는지 검증한다.

입력 상태:

| 영역 | 키 | 값 |
|---|---|---|
| BoolMap | `IsHiddenCollapseActive` | true |
| IntMap | `HayeonTrust` | 8 |
| IntMap | `HayeonPace` | 4 |
| Fragments | `Fragment:HasHayeonClue` | present |
| IntMap | `Avoid` | 0 |

기대 결과:

| 단계 | 기대값 |
|---|---|
| `EvaluateDDayResult()` | `HiddenCollapse` |
| `NameMap["DDayResult"]` | `HiddenCollapse` |
| `DDay_04Gate` 출력 | `DDay_13HiddenCollapse` |
| 처리 후 | `LoopCount += 1`, `ReturnDay = D25`, `BrokenLoop` 종료 |

통과 기준:

- `HiddenCollapse`가 가장 먼저 평가된다.
- 진엔딩 조건이 충분해도 `DDay_05True`로 가지 않는다.

## 6. 선택지 Shot 검증 케이스

`DDay_01Choice`는 `UVNChoiceShot` 최소 검증용으로 사용한다.

### TC-CHOICE-001: 선택 결과 저장

입력:

| 필드 | 값 |
|---|---|
| `PromptText` | 오늘 축제를 누구와 둘러볼까? |
| `ResultKey` | `DDayChoice` |
| 선택 | `Hayeon` |

기대 결과:

| 상태 | 기대값 |
|---|---|
| `NameMap["DDayChoice"]` | `Hayeon` |
| `BoolMap["DidChooseHayeonOnDDay"]` | true |
| `HayeonTrust` | 기존 값보다 1 증가 |
| 다음 | StoryFlow 기본 `Next` 핀 |

통과 기준:

- `UVNChoiceShot`이 직접 Scene 점프를 하지 않고 선택 결과만 저장한다.
- 이후 분기는 프로젝트 전용 Branch나 다음 평가 Shot에서 처리된다.

### TC-CHOICE-002: 조건 미충족 선택지 비활성

입력:

| 영역 | 키 | 값 |
|---|---|---|
| BoolMap | `IsHayeonBoothD1Complete` | false |
| IntMap | `HayeonTrust` | 4 |
| IntMap | `HayeonPace` | 1 |

기대 결과:

| 선택지 | 기대 상태 |
|---|---|
| 하연을 찾는다 | 숨김 또는 비활성 |
| 혼자 소무대를 확인한다 | 표시 |

통과 기준:

- `ShowCond`와 `EnableCond`가 분리되어 동작한다.
- 비활성 선택지는 선택해도 `OnSelect`를 적용하지 않는다.

## 7. 테스트 데이터 제작 단위

1차에서 필요한 StoryFlow SceneAsset은 다음 정도면 충분하다.

| Asset | 포함 Scene |
|---|---|
| `SF_DDayPrototype` | `DDay_01Choice`, `DDay_03Assess`, `DDay_04Gate`, `DDay_04TrueChoice` |
| `SF_DDayTrue` | `DDay_05True` |
| `SF_DDayFail` | `DDay_06HayeonMiss`, `DDay_10RelationUnresolved`, `DDay_11AccFail`, `DDay_12Avoid` |
| `SF_DDayHidden` | `DDay_13HiddenCollapse` |
| `SF_TrueEndingStub` | `TE_00_TimeSkip` |

실제 전체 장면을 모두 만들 필요는 없다. 각 Scene은 테스트에 필요한 Shot과 상태 변경만 포함한다.

## 8. 구현 시 확인할 함수/기능

| 대상 | 확인 내용 |
|---|---|
| `FVNStoryState` | Bool/Int/Name/Fragment/SeenEvent/Ending 상태 저장 |
| `FVNStateChange` | Set/Add/Subtract/Increment 류 변경 적용 |
| `FVNConditionSet` | All/Any/None 평가 |
| `UVNDialogueShot` | 대사 표시, OnEnter/OnShow/OnComplete 적용 |
| `UVNChoiceShot` | 선택지 조건, ResultKey 저장, OnSelect 적용 |
| 프로젝트 전용 D-Day 분기 | `DDayResult` 값에 따라 다음 Scene 선택 |
| `UVNEventHubSubsystem` | 실패 후 `ReturnDay = D25` 기준 허브 복귀 |

## 9. 1차 완료 기준

다음이 모두 가능하면 D-Day 최소 프로토타입은 완료로 본다.

- `TC-DDAY-001`부터 `TC-DDAY-005`까지 기대 Scene으로 분기한다.
- `TC-CHOICE-001`, `TC-CHOICE-002`가 통과한다.
- 실패 루프 케이스가 D+3로 가지 않고 D-25 복귀 상태를 만든다.
- 진엔딩 케이스가 `TE_00_TimeSkip`로 넘어간다.
- 테스트에 Miyeansi 전용 키가 필요하더라도 `VisualNovelPlugin` 코드에는 하드코딩하지 않는다.
- 모든 상태 키는 [VisualNovelPlugin 데이터 상세 설계](./VisualNovelPlugin_데이터_상세설계.md)의 짧은 PascalCase 규칙을 따른다.

## 10. 다음 연결

이 문서 이후 개발 순서는 다음이 적합하다.

1. `VisualNovelPlugin` 모듈 생성
2. `FVNStoryState`, `FVNStateChange`, `FVNConditionSet` 구현
3. `UVNConditionEvaluator` 구현
4. `UVNDialogueShot`, `UVNChoiceShot` 구현
5. 프로젝트 전용 D-Day 분기 또는 평가 Shot 구현
6. 이 문서의 D-Day 테스트 데이터를 최소 에셋으로 제작
