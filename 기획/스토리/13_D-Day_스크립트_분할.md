# D-Day 실제 게임 스크립트 분할

이 문서는 [D-Day 축제 마지막 날 분기](./10_D-Day_축제마지막날_분기.md)의 대본을 실제 게임 스크립트 제작 단위로 쪼갠 문서다.

점수·플래그 판정 기준은 [D-1/D-Day 조건 수치화](./12_D-1_D-Day_조건_수치화.md)를 따른다.

## 1. 문서 목적

- D-Day를 StoryFlow Scene 또는 VN 스크립트 단위로 나눈다.
- 각 장면의 진입 조건, 선택지, 상태 변경, 다음 장면을 명확히 한다.
- 긴 대본 문서인 `10`을 그대로 구현하지 않고, 제작 가능한 스크립트 블록으로 변환한다.
- 이 문서는 구현용 초안이며, 실제 엔진 스크립트 문법은 이후 StoryFlow 에셋 구조에 맞춰 조정한다.

배경 원화와 AI 이미지 생성 기준은 [장소와 배경 묘사](./14_장소와_배경_묘사.md)를 따른다. 이 문서의 `@bg` 아래 배경 지시는 실제 제작자가 어떤 변형 배경을 호출해야 하는지 빠르게 확인하기 위한 요약이다.

## 2. 스크립트 표기 규칙

```text
# SceneID
@enter 조건
@bg 배경
@music 음악
@sfx 효과음
@flag 상태 변경
@choice 선택지
@goto 다음 SceneID
@end 종료
```

표기 의미:

| 표기 | 의미 |
| --- | --- |
| `# SceneID` | 장면 고유 ID 후보 |
| `@enter` | 진입 조건 |
| `@bg` | 배경/장소 |
| `@music` | 음악 방향 |
| `@sfx` | 주요 효과음 |
| `@flag` | 플래그/점수 변경 |
| `@choice` | 플레이어 선택지 |
| `@goto` | 다음 장면 이동 |
| `@end` | 엔딩/루프/다음 날짜 연결 |

## 3. 전체 Scene 흐름

```text
DDAY_00_ENTRY_CAFE
        ↓
DDAY_01_DAY_CHOICE_HUB
        ↓
DDAY_02A_HAYEON_DAY
DDAY_02B_SOHA_DAY
DDAY_02C_SEORIN_DAY
DDAY_02D_MIRU_DAY
DDAY_02E_ALONE_STAGE_CHECK
        ↓
DDAY_03_PRE_CLOSE_ASSESS
        ↓
DDAY_04_ROUTE_GATE
        ├─ DDAY_05_TRUE_APPROACH
        ├─ DDAY_06_FAIL_HAYEON_MISSING
        ├─ DDAY_07_SAD_SOHA_ENTRY
        ├─ DDAY_08_SAD_SEORIN_ENTRY
        ├─ DDAY_09_SAD_MIRU_ENTRY
        ├─ DDAY_10_FAIL_ACCIDENT_UNLINKED
        └─ DDAY_11_FAIL_AVOIDANCE_LOOP
```

## 4. 공통 진입

### DDAY_00_ENTRY_CAFE

```text
# DDAY_00_ENTRY_CAFE
@enter Date == DDay
@enter D1CafeOperated == true
@enter D2PremonitionSeen == true
@enter D3RelationshipAssessmentDone == true
@bg 2학년 5반 쉬어가 카페 / 축제 마지막 날 오전
@music festival_day_light_low
```

배경 지시:

- `14`의 `2학년 5반 쉬어가 카페`와 `폐장 후 교실` 사이 단계.
- 책상 임시 카운터, 손글씨 메뉴판, 종이컵, 반쯤 떼어진 종이 장식, 입구 문구 카드가 보인다.
- 오전이지만 이미 정리의 기운이 섞여야 한다.

목적:

- 축제 2일차의 마지막 낮을 시작한다.
- `쉬어가 카페`가 이제 정리되는 공간임을 보여준다.
- 하연이 먼저 폐장 후 약속을 잡지 않는다는 원칙을 유지한다.

스크립트:

```text
반 친구: 오후엔 부스 닫고 돌아다녀도 된대!
반 친구: 컵 남은 거 이쪽에 모아줘.
재윤: 알겠어.
하연: 이 문구, 떼지 말까?
재윤: 왜?
하연: 아직 들어올 사람 있을 수도 있으니까.
재윤: 축제 끝나기 전까지?
하연: 응. 끝나기 전까지.
```

내레이션:

```text
문구 카드는 아직 입구에 붙어 있었다.

잠깐 앉아도 괜찮습니다.

오늘은 그 문장이,
재윤에게 하는 말처럼 보였다.

쉬어가도 되지만,
머물러 숨으면 안 되는 날이었다.
```

상태 처리:

```text
@flag DDayCafeClosed = true
@goto DDAY_01_DAY_CHOICE_HUB
```

## 5. 낮 선택 허브

### DDAY_01_DAY_CHOICE_HUB

```text
# DDAY_01_DAY_CHOICE_HUB
@enter DDayCafeClosed == true
@bg 학교 축제 운동장 / 낮
@music festival_day_choice
```

배경 지시:

- `14`의 `축제 낮` 기준.
- 밝은 천막과 학생 소음은 유지하되, 화면 한쪽에 조용한 가장자리 동선이 보여야 한다.
- 선택 허브이므로 어느 캐릭터 방향으로도 갈 수 있는 열린 구도가 좋다.

선택지:

```text
@choice [하연과 축제를 둘러본다] -> DDAY_02A_HAYEON_DAY
@choice [소하와 마지막으로 오래된 길을 걷는다] -> DDAY_02B_SOHA_DAY
@choice [서린의 방송 마무리를 돕는다] -> DDAY_02C_SEORIN_DAY
@choice [미루의 도움 인원 마무리를 확인한다] -> DDAY_02D_MIRU_DAY
@choice [혼자 야외 소무대 쪽을 본다] -> DDAY_02E_ALONE_STAGE_CHECK
```

주의:

- 낮 선택은 한 번만 가능하다.
- 선택한 축만 보정한다.
- `DDayDayChoiceUsed = true`를 공통으로 설정한다.

## 6. 낮 선택 A: 하연

### DDAY_02A_HAYEON_DAY

```text
# DDAY_02A_HAYEON_DAY
@enter DDayDayChoiceUsed != true
@bg 축제 운동장 가장자리 / 낮
@music hayeon_day_soft
```

배경 지시:

- `14`의 `운동장과 축제 중심부` 중 가장자리 기준.
- 하연은 축제 중심보다 그늘, 사람 흐름이 느슨한 길, 멀리 보이는 소무대 방향과 맞다.
- 화면은 밝지만 소리가 조금 낮아지는 느낌으로, 하연의 속도를 읽는 장면임을 보여준다.

목적:

- 하연 신뢰와 동선 이해를 마지막으로 보정한다.
- 하연이 조용한 곳을 보는 감각을 재윤이 다시 확인한다.
- 낮에는 고백하지 않는다.

핵심 대사:

```text
재윤: 사람 많네.
하연: 어제보다 더 많은 것 같아.
재윤: 괜찮아?
하연: 괜찮아. 오래 있으면 조금 시끄럽긴 한데.

재윤: 저쪽으로 돌까?
하연: 어디?
재윤: 사람 덜 몰리는 쪽.

하연: 알아봤네.
재윤: 조금은.
하연: 조금이면 됐어.
```

D-2 성공 시 추가:

```text
재윤: 혼자 있고 싶은 거랑, 아무도 오지 않았으면 하는 건 다르다고 했잖아.
하연: 기억했네.
재윤: 응.
하연: 그럼 됐어.
```

상태 처리:

```text
@if HaYeonTrust >= 5 || HaYeonPaceRead >= 2
  @flag HaYeonTrust += 1
  @flag HaYeonPaceRead += 1
  @flag DDayHayeonDaySuccess = true
@endif

@if D2PremonitionPassed == true && DDayHayeonDaySuccess == true
  @flag AccidentClueHaYeon = true
@endif

@flag DDayDayChoiceUsed = true
@flag DDayDayChoice = Hayeon
@goto DDAY_03_PRE_CLOSE_ASSESS
```

실패/약한 진행:

```text
@if HaYeonTrust <= 4 && HaYeonPaceRead <= 1
  @flag DDayHayeonDaySuccess = false
@endif
```

## 7. 낮 선택 B: 소하

### DDAY_02B_SOHA_DAY

```text
# DDAY_02B_SOHA_DAY
@enter DDayDayChoiceUsed != true
@bg 운동장 옆 오래된 길 / 낮
@music soha_memory_warm
```

배경 지시:

- `14`의 `소하의 오래된 길` 기준.
- 따뜻하고 익숙하지만 앞으로 나아가지 못하는 느낌을 준다.
- 오래된 담장, 색 바랜 표지, 등굣길을 떠올리게 하는 생활감을 넣고, 지나친 데이트 거리처럼 꾸미지 않는다.

목적:

- 과거 도피를 마지막으로 정리한다.
- D-3에서 이미 정리했다면 확인 장면으로 사용한다.
- D-3에서 정리하지 못했더라도 압박이 너무 높지 않으면 마지막 보정이 가능하다.

D-3 정리 성공 시:

```text
소하: 예전 같으면, 너 이런 날엔 나랑 먼저 돌았어.
재윤: 알아.
소하: 그럼 오늘은?
재윤: 마지막에 가야 할 데가 있어.
소하: 하연이한테?
재윤: 응.
소하: 이제야 제대로 말하네.
```

D-3 미정리 보정 선택:

```text
@choice [예전처럼 편한 쪽으로 돌아간다] -> DDAY_07_SAD_SOHA_ENTRY
@choice [지금 가야 할 사람을 말한다] -> DDAY_02B_SOHA_RESOLVE
```

#### DDAY_02B_SOHA_RESOLVE

```text
재윤: 편해서 돌아가는 거랑, 좋아해서 가는 건 다르잖아.
소하: 이제 그런 말도 하네.
재윤: 늦었지.
소하: 늦었어. 그래도 지금 말했잖아.
```

상태 처리:

```text
@if SoHaResolved == true
  @flag DDaySoHaConfirmed = true
@elseif SoHaPressure <= 2
  @flag SoHaResolved = true
  @flag AccidentClueSoHa = true
  @flag DDaySoHaConfirmed = true
@else
  @flag SoHaPressure = 3
@endif

@flag DDayDayChoiceUsed = true
@flag DDayDayChoice = SoHa
@goto DDAY_03_PRE_CLOSE_ASSESS
```

## 8. 낮 선택 C: 서린

### DDAY_02C_SEORIN_DAY

```text
# DDAY_02C_SEORIN_DAY
@enter DDayDayChoiceUsed != true
@bg 방송실 / 낮
@music seorin_order_cold
```

배경 지시:

- `14`의 `방송실과 학생회 업무 공간` 기준.
- 믹서, 마이크, 헤드폰, 폐장 큐시트, 라벨링된 케이블, D-9 상담 기준 메모가 보여야 한다.
- 차가운 통제실이 아니라, 서린이 실제 업무를 정리해 둔 좁고 바쁜 공간이다.

목적:

- 정답과 순서 뒤에 숨는 길을 마지막으로 정리한다.
- 폐장 방송과 소무대 위험 단서를 보강할 수 있다.

D-3 정리 성공 시:

```text
서린: 폐장 방송은 이 순서면 돼.
재윤: 제가 확인 안 해도 괜찮아요?
서린: 네가 오늘 확인해야 할 건 이게 아니잖아.
재윤: 그래도 틀리면요?
서린: 틀리면 내가 고친다.
서린: 너는 네가 말해야 할 걸 말해.
```

D-3 미정리 보정 선택:

```text
@choice [서린이 정한 순서를 따른다] -> DDAY_08_SAD_SEORIN_ENTRY
@choice [내가 확인해야 할 것을 고른다] -> DDAY_02C_SEORIN_RESOLVE
```

#### DDAY_02C_SEORIN_RESOLVE

```text
재윤: 틀리지 않는 쪽만 고르면, 제가 고른 게 아니잖아요.
서린: 이제야 그걸 말하네요.
재윤: D-9에 선배가 말했잖아요. 맞는 답이 아니라, 제가 고른 이유를 보라고.
서린: 기억하고 있었네.
재윤: 오늘은 제가 확인해야 할 게 따로 있어요.
서린: 그럼 가요. 방송은 내가 할 테니까.
```

상태 처리:

```text
@if SeoRinResolved == true
  @flag DDaySeoRinConfirmed = true
  @flag AccidentClueSeoRin = true
@elseif SeoRinControl <= 2
  @flag SeoRinResolved = true
  @flag AccidentClueSeoRin = true
  @flag DDaySeoRinConfirmed = true
@else
  @flag SeoRinControl = 3
@endif

@flag DDayDayChoiceUsed = true
@flag DDayDayChoice = SeoRin
@goto DDAY_03_PRE_CLOSE_ASSESS
```

## 9. 낮 선택 D: 미루

### DDAY_02D_MIRU_DAY

```text
# DDAY_02D_MIRU_DAY
@enter DDayDayChoiceUsed != true
@bg 축제 도움 인원 대기 장소 / 낮
@music miru_soft_anxious
```

배경 지시:

- `14`의 `미루의 축제 도움 인원 대기 장소` 기준.
- 복도 옆 임시 책상, 체크리스트, 명찰, 여분 안내 표지, 테이프와 끈, 분실물 박스를 배치한다.
- 미루가 갇힌 느낌보다, 혼자 해보려고 나갈 수 있는 문과 복도 동선이 보여야 한다.

목적:

- 미루의 의존을 대신 책임지지 않고, 미루가 자기 일을 끝낼 수 있게 한다.
- 출입 제한 표지와 고정끈 단서를 보강할 수 있다.

D-3 정리 성공 시:

```text
미루: 저, 표지 확인하고 보고까지 했어요.
재윤: 잘했네.
미루: 그래도 마지막에 선배가 한 번 봐주시면—
재윤: 네가 먼저 본 게 맞아.
재윤: 내가 다시 정해주지 않아도 돼.
미루: 그럼 저, 혼자 한 거 맞죠?
재윤: 응. 네가 한 거야.
```

D-3 미정리 보정 선택:

```text
@choice [마지막까지 대신 봐준다] -> DDAY_09_SAD_MIRU_ENTRY
@choice [먼저 끝낸 일을 인정한다] -> DDAY_02D_MIRU_RESOLVE
```

#### DDAY_02D_MIRU_RESOLVE

```text
재윤: 내가 확인해줘서 맞는 게 아니야.
재윤: 네가 확인했고, 네가 보고했으니까 맞는 거야.
미루: 그래도 불안하면요?
재윤: 불안해도 네가 한 일이 사라지진 않아.
미루: 그럼... 저 혼자 끝낸 거네요.
재윤: 응.
```

상태 처리:

```text
@if MiRuResolved == true
  @flag DDayMiRuConfirmed = true
  @flag AccidentClueMiRu = true
@elseif MiRuDependence <= 2
  @flag MiRuResolved = true
  @flag AccidentClueMiRu = true
  @flag DDayMiRuConfirmed = true
@else
  @flag MiRuDependence = 3
@endif

@flag DDayDayChoiceUsed = true
@flag DDayDayChoice = MiRu
@goto DDAY_03_PRE_CLOSE_ASSESS
```

## 10. 낮 선택 E: 혼자 소무대 확인

### DDAY_02E_ALONE_STAGE_CHECK

```text
# DDAY_02E_ALONE_STAGE_CHECK
@enter DDayDayChoiceUsed != true
@bg 학교 뒤편 야외 소무대 / 낮
@music stage_empty_day
```

배경 지시:

- `14`의 `낮의 소무대` 기준.
- 낮은 단차, 검은 전선, 임시 조명 프레임, 기울어진 출입 제한 표지가 보이지만 아직 노골적인 사고 장소처럼 보이면 안 된다.
- 하연 조건이 부족하면 물리 단서만 선명하고, 하연 조건이 충분하면 D-2 창밖 시선과 연결된다.

목적:

- 물리적 사고 단서를 보강한다.
- 하지만 하연과 공유하지 않으면 감정 연결은 회복되지 않는다.
- 회피가 높은 상태에서는 오히려 회피가 더 오른다.

스크립트:

```text
소무대는 낮에도 조용했다.

표지는 제대로 붙어 있었고,
끈은 풀리지 않았고,
프레임은 아직 제자리에 있었다.

전부 확인했는데도,
재윤은 여전히 누구에게 가려 했는지 알 수 없었다.
```

상태 처리:

```text
@if AccidentClueScore >= 2
  @flag MissingPhysicalAccidentClueOne = true
@endif

@if AccidentClueSeoRin == false
  @flag AccidentClueSeoRin = true
@elseif AccidentClueMiRu == false
  @flag AccidentClueMiRu = true
@endif

@if AccidentClueHaYeon == false
  @flag DDayAloneCheckNoHeartLink = true
@endif

@if Avoidance >= 3
  @flag Avoidance += 1
@endif

@flag DDayDayChoiceUsed = true
@flag DDayDayChoice = AloneStage
@goto DDAY_03_PRE_CLOSE_ASSESS
```

주의:

- `AccidentClueHaYeon`은 혼자 조사로 획득하지 않는다.
- 이 선택은 사고를 이해하는 데 도움이 되지만, 하연에게 말하러 가는 감정 조건을 대신하지 않는다.

## 11. 폐장 전 상태 평가

### DDAY_03_PRE_CLOSE_ASSESS

```text
# DDAY_03_PRE_CLOSE_ASSESS
@enter DDayDayChoiceUsed == true
@bg 축제 폐장 직전 / 운동장과 교실 사이
@music festival_close_fade
@sfx distant_school_broadcast
```

배경 지시:

- `14`의 `축제 폐장 직전`과 `폐장 후 교실` 사이 전환 배경.
- 운동장 불빛, 접히는 천막, 교실 안에 쌓인 의자, 떼어진 카페 문구 카드가 함께 보여야 한다.
- 밝은 중심과 어두워지는 학교 뒤편의 대비를 만든다.

목적:

- 낮 선택 이후 상태를 다시 평가한다.
- 플레이어에게 수치가 아니라 연출로 부족 축을 보여준다.
- 하연이 조용히 사라지는 순간을 배치한다.

공통 스크립트:

```text
축제 마지막 안내 방송이 곧 시작된다고 했다.
운동장 쪽 불빛은 아직 밝았지만,
교실 안에는 이미 정리된 의자들이 쌓여 있었다.

재윤은 하연을 찾았다.

조금 전까지 있던 자리에,
하연은 없었다.
```

상태 평가:

```text
@eval DDayFinalBranch = EvaluateDDayFinalBranch()
@goto DDAY_04_ROUTE_GATE
```

연출 분기 힌트:

| 부족 축 | 삽입 연출 |
| --- | --- |
| 하연 신뢰/동선 부족 | 하연이 사라졌다는 사실을 늦게 알아차림 |
| 소하 미정리 | 소하의 메시지가 화면 위에 남음 |
| 서린 미정리 | 폐장 방송 순서표가 선택지처럼 보임 |
| 미루 미정리 | 미루의 체크리스트 알림이 계속 울림 |
| 사고 단서 부족 | 소무대 이미지는 보이지만 문장이 끊김 |
| 회피 높음 | `내일 말해도 되지 않을까` 독백이 끼어듦 |

## 12. 최종 라우팅

### DDAY_04_ROUTE_GATE

```text
# DDAY_04_ROUTE_GATE
@enter DDayFinalBranch != None
```

라우팅:

```text
@if DDayFinalBranch == TrueEnding
  @goto DDAY_05_TRUE_APPROACH
@elseif DDayFinalBranch == HayeonMissingLoop
  @goto DDAY_06_FAIL_HAYEON_MISSING
@elseif DDayFinalBranch == SoHaSad
  @goto DDAY_07_SAD_SOHA_ENTRY
@elseif DDayFinalBranch == SeoRinSad
  @goto DDAY_08_SAD_SEORIN_ENTRY
@elseif DDayFinalBranch == MiRuSad
  @goto DDAY_09_SAD_MIRU_ENTRY
@elseif DDayFinalBranch == AccidentUnlinkedLoop
  @goto DDAY_10_FAIL_ACCIDENT_UNLINKED
@elseif DDayFinalBranch == AvoidanceLoop
  @goto DDAY_11_FAIL_AVOIDANCE_LOOP
@else
  @goto DDAY_06_FAIL_HAYEON_MISSING
@endif
```

분기 평가는 `12_D-1_D-Day_조건_수치화.md`의 우선순위를 따른다.

## 13. 진엔딩 성공 스크립트

### DDAY_05_TRUE_APPROACH

```text
# DDAY_05_TRUE_APPROACH
@enter DDayFinalBranch == TrueEnding
@bg 학교 뒤편 야외 소무대 / 폐장 후
@music true_end_confession_start
@sfx distant_festival_shutdown
```

배경 지시:

- `14`의 `D-Day 폐장 후 소무대` 진엔딩 구도.
- 하연은 무대 위 중앙이 아니라 낮은 계단 근처나 무대 옆에 서 있다.
- 전선, 단차, 조명 프레임, 출입 제한 표지는 보이되, 사고를 낭만화하지 않는다.
- 두 사람은 화려한 조명이 아니라 축제가 끝난 뒤 남은 조용한 빛 아래에 있어야 한다.

도착:

```text
하연: 어떻게 알았어?
재윤: 네가 있을 것 같았어.

하연은 소무대 쪽을 바라보다가,
재윤 쪽으로 시선을 돌렸다.

하연: 내가 부른 것도 아닌데.
재윤: 그래서 온 거야.
재윤: 이번엔 내가 먼저 와야 할 것 같아서.

하연: 지금은 알아?
재윤: 응. 누구한테 말하러 왔는지 알아.
```

고백 전 회상 변주:

```text
@if SoHaResolved == true
  @insert "예전으로 돌아가 숨고 싶었던 마음을 지나서 왔어."
@endif
@if SeoRinResolved == true
  @insert "맞는 답이 아니라, 내가 고른 답이라고 말하려고 왔어."
@endif
@if MiRuResolved == true
  @insert "누군가에게 필요한 사람인 척하면서 내 마음을 미루지 않으려고 왔어."
@endif
@if D2PremonitionPassed == true
  @insert "네가 조용한 데서 숨을 고르는 사람이라는 걸, 이제는 조금 알아."
@endif
```

재윤 선고백:

```text
재윤: 오래된 쪽으로 돌아가면 편했어.
재윤: 맞는 답을 찾는 척하면 덜 무서웠고.
재윤: 누가 나를 필요로 하면,
재윤: 내가 뭘 원하는지 안 봐도 됐어.

재윤: 그런데 그러면,
재윤: 나는 계속 아무 데도 못 가.

재윤: 이번에는 너한테 가고 싶었어.
재윤: 누가 시켜서도 아니고,
재윤: 네가 먼저 말해줘서도 아니고.

재윤: 내가 고른 거라고 말하고 싶어서.
재윤: 하연아, 나는 너를 좋아해.
```

하연 마음 확인:

```text
하연: 나도 말하려고 했어.
재윤: 오늘?
하연: 응. 끝나고, 조금 조용해지면.

재윤: 왜 말 안 했어?
하연: 네가 올 수 있으면... 좋겠다고 생각했어.
재윤: 내가?
하연: 응.

하연: 네가 나를 기다려준 적이 있었잖아.
하연: 억지로 끌고 오지 않고,
하연: 들어와도 된다고 말해준 적.

하연: 그래서 나도 기다렸어.
하연: 네가 돌아보는 걸.
하연: 네가 네 발로 오는 걸.

재윤: 많이 늦었지.
하연: 늦었어.
재윤: 미안.
하연: 그 말보다...
재윤: 응.
하연: 와줘서 다행이야.

하연: 나도... 좋아해.
```

사고 기억 회복:

```text
폐장 안내 방송의 끝부분이 다시 들렸다.

조명 프레임.
출입 제한 표지.
풀리지 않은 고정끈.
낮게 남은 소무대의 단차.

전부 같은 자리에 있었다.

사고는 사라지지 않았다.
아픈 기억도 사라지지 않았다.

다만 이번에는,
재윤이 어디로 가려 했는지 알았다.

하연에게.
말하기 위해서.
```

상태 처리:

```text
@flag HayeonMutualHeartConfirmed = true
@flag HayeonMissingFromAccidentMemory = false
@flag DDayTrueEndingReached = true
@flag SundayWakeUpAfterDDay = true
@end TrueEndingWakeUpSunday
```

다음 연결:

- D+2 일요일 병실 기상 컷신
- D+3 월요일 방학식 컷신
- 진엔딩 크레딧

## 14. 하연 실패 루프 스크립트

### DDAY_06_FAIL_HAYEON_MISSING

```text
# DDAY_06_FAIL_HAYEON_MISSING
@enter DDayFinalBranch == HayeonMissingLoop
@bg 학교 뒤편 야외 소무대 / 폐장 후
@music loop_fail_empty
@sfx distant_metal_creak
```

배경 지시:

- `14`의 `D-Day 폐장 후 소무대` 실패 구도.
- 진엔딩과 같은 장소지만 하연이 없다.
- 전선, 단차, 표지, 조명 프레임만 선명하고, 재윤이 왜 왔는지 설명하는 감정 단서는 비어 있어야 한다.

스크립트:

```text
축제 마지막 안내 방송이 끝났다.
운동장 쪽 불빛이 하나씩 꺼졌다.

재윤은 학교 뒤편 야외 소무대 앞에 서 있었다.

왜 여기까지 왔는지,
기억나지 않았다.

누가 불렀는지도,
누구에게 가려 했는지도,
떠오르지 않았다.
```

반복:

```text
표지는 제대로 붙어 있었다.
끈은 묶여 있었다.
프레임은 제자리에 있었다.

그러니까 이상했다.

전부 제자리에 있는데,
재윤만 왜 여기 서 있는지 알 수 없었다.

폐장 안내 방송의 끝부분이
멀리서 다시 울리는 것 같았다.

발밑이 비었다.
```

상태 처리:

```text
@flag HayeonMissingFromAccidentMemory = true
@flag DDayTrueEndingReached = false
@flag LoopCount += 1
@flag ReturnDate = D25
@end LoopToD25
```

## 15. 소하 새드엔딩 진입 스크립트

### DDAY_07_SAD_SOHA_ENTRY

```text
# DDAY_07_SAD_SOHA_ENTRY
@enter DDayFinalBranch == SoHaSad || SoHaPressure == 3
@bg 오래된 등굣길처럼 보이는 축제장 가장자리
@music sad_soha_warm_loop
```

배경 지시:

- `14`의 `소하의 오래된 길`을 축제장 가장자리와 겹쳐 보이게 한다.
- 따뜻한 색감은 유지하되, 같은 길을 반복하는 정체감이 있어야 한다.

스크립트:

```text
소하: 그냥 가자.
재윤: 어디로?
소하: 늘 가던 데.
소하: 축제 같은 거 끝나도, 우린 별로 달라질 거 없잖아.

재윤은 대답하지 못했다.

운동장의 불빛은 아직 밝았지만,
길 끝은 이상하게 월요일 아침처럼 환했다.
```

상태 처리:

```text
@flag EndingSoHaSadSeen = true
@flag SoHaSadCount += 1
@flag LoopCount += 1
@end SadEndingSoHaOrLoop
```

## 16. 서린 새드엔딩 진입 스크립트

### DDAY_08_SAD_SEORIN_ENTRY

```text
# DDAY_08_SAD_SEORIN_ENTRY
@enter DDayFinalBranch == SeoRinSad || SeoRinControl == 3
@bg 방송실 / 폐장 직전
@music sad_seorin_ordered
```

배경 지시:

- `14`의 `방송실`을 폐장 직전 버전으로 사용한다.
- 큐시트와 장비는 정돈되어 있지만, 창밖 축제 소리가 멀어지고 선택지가 닫히는 느낌을 준다.

스크립트:

```text
서린: 이 순서면 안전해.
재윤: 안전한 쪽이요?
서린: 네. 틀리지 않는 쪽.

모니터에는 폐장 안내 문구가 떠 있었다.
그러나 문장 아래의 선택지는 하나뿐이었다.

[정해진 순서를 따른다]
```

상태 처리:

```text
@flag EndingSeoRinSadSeen = true
@flag SeoRinSadCount += 1
@flag LoopCount += 1
@end SadEndingSeoRinOrLoop
```

## 17. 미루 새드엔딩 진입 스크립트

### DDAY_09_SAD_MIRU_ENTRY

```text
# DDAY_09_SAD_MIRU_ENTRY
@enter DDayFinalBranch == MiRuSad || MiRuDependence == 3
@bg 축제 도움 인원 대기실 / 문이 닫힌 복도
@music sad_miru_musicbox
```

배경 지시:

- `14`의 `미루의 축제 도움 인원 대기 장소` 실패 변형.
- 평소에는 열려 있던 복도 동선이 문과 그림자 때문에 좁아 보인다.
- 그래도 공포 공간이 아니라, 도움을 기다리며 멈춘 공간으로 보여야 한다.

스크립트:

```text
미루: 선배, 마지막으로 한 번만 더 봐주시면 안 돼요?
재윤: 마지막?
미루: 네. 방학식 지나면 끝나잖아요.
미루: 그러니까 오늘만요.

문밖에는 여름빛이 있었다.
그러나 재윤은 문손잡이를 잡지 못했다.
```

상태 처리:

```text
@flag EndingMiRuSadSeen = true
@flag MiRuSadCount += 1
@flag LoopCount += 1
@end SadEndingMiRuOrLoop
```

## 18. 사고 의미 미연결 루프

### DDAY_10_FAIL_ACCIDENT_UNLINKED

```text
# DDAY_10_FAIL_ACCIDENT_UNLINKED
@enter DDayFinalBranch == AccidentUnlinkedLoop
@bg 야외 소무대 / 폐장 후
@music loop_accident_unlinked
```

배경 지시:

- `14`의 실패 소무대 구도.
- 장소와 사고 단서는 충분히 보이지만, 하연에게 가려던 의미만 빠진 상태다.

목적:

- 장소와 단서는 충분히 보이지만 `누구에게 가려 했는지`가 비어 있는 실패다.
- 하연 조건이 완전히 부족한 실패와 달리, 물리 단서가 오히려 빈자리를 강조한다.

스크립트:

```text
조명 프레임.
출입 제한 표지.
묶여 있는 고정끈.
폐장 안내 방송의 끝부분.

재윤은 그것들을 모두 보았다.

전부 여기 있었다.
전부 맞았다.

그런데 왜 여기 왔는지만,
끝내 떠오르지 않았다.
```

상태 처리:

```text
@flag AccidentMeaningUnlinked = true
@flag LoopCount += 1
@flag ReturnDate = D25
@end LoopToD25
```

## 19. 회피 루프

### DDAY_11_FAIL_AVOIDANCE_LOOP

```text
# DDAY_11_FAIL_AVOIDANCE_LOOP
@enter DDayFinalBranch == AvoidanceLoop
@bg 축제 폐장 후 복도
@music loop_avoidance_quiet
```

배경 지시:

- `14`의 `루프/실패 복도` 기준.
- 평범한 학교 복도지만 사람이 빠져 있고, 형광등과 발소리가 과하게 또렷하다.
- 공포물처럼 만들지 말고 의미가 빠진 현실처럼 보이게 한다.

목적:

- 조건은 거의 맞지만 재윤이 마지막 말을 또 미루는 실패다.
- 하연에게 갈 수 있는 길을 알지만, 직접 말하지 못한다.

스크립트:

```text
재윤은 하연이 어디 있을지 알 것 같았다.

조용한 곳.
사람들이 다 빠져나간 뒤에도,
잠깐 숨을 고를 수 있는 자리.

알고 있었다.

그런데 발이 움직이지 않았다.

내일 말해도 되지 않을까.
그런 생각이 든 순간,
복도의 불이 하나씩 꺼졌다.
```

상태 처리:

```text
@flag AvoidanceLoopSeen = true
@flag LoopCount += 1
@flag ReturnDate = D25
@end LoopToD25
```

## 20. 평가 함수 초안

실제 구현 시 사용할 평가 함수 의사코드다.

```text
function EvaluateDDayFinalBranch():
    if HiddenCollapseCondition == true:
        return HiddenCollapse

    if D2PremonitionPassed == false:
        return HayeonMissingLoop

    if HaYeonTrust < 6 or HaYeonPaceRead < 3:
        return HayeonMissingLoop

    if SoHaResolved == false and SoHaPressure == 3:
        return SoHaSad

    if SeoRinResolved == false and SeoRinControl == 3:
        return SeoRinSad

    if MiRuResolved == false and MiRuDependence == 3:
        return MiRuSad

    if SoHaResolved == false or SeoRinResolved == false or MiRuResolved == false:
        return RelationshipUnresolvedLoop

    if AccidentClueScore < 3 or AccidentClueHaYeon == false:
        return AccidentUnlinkedLoop

    if Avoidance >= 4:
        return AvoidanceLoop

    return TrueEnding
```

## 21. 실제 제작 시 분리 권장 에셋

| 에셋 후보 | 포함 Scene |
| --- | --- |
| `SF_DDay_Common` | `DDAY_00_ENTRY_CAFE`, `DDAY_01_DAY_CHOICE_HUB`, `DDAY_03_PRE_CLOSE_ASSESS`, `DDAY_04_ROUTE_GATE` |
| `SF_DDay_DayChoice_Hayeon` | `DDAY_02A_HAYEON_DAY` |
| `SF_DDay_DayChoice_Soha` | `DDAY_02B_SOHA_DAY`, `DDAY_02B_SOHA_RESOLVE` |
| `SF_DDay_DayChoice_Seorin` | `DDAY_02C_SEORIN_DAY`, `DDAY_02C_SEORIN_RESOLVE` |
| `SF_DDay_DayChoice_Miru` | `DDAY_02D_MIRU_DAY`, `DDAY_02D_MIRU_RESOLVE` |
| `SF_DDay_DayChoice_Alone` | `DDAY_02E_ALONE_STAGE_CHECK` |
| `SF_DDay_TrueEnding` | `DDAY_05_TRUE_APPROACH` |
| `SF_DDay_FailLoops` | `DDAY_06_FAIL_HAYEON_MISSING`, `DDAY_10_FAIL_ACCIDENT_UNLINKED`, `DDAY_11_FAIL_AVOIDANCE_LOOP` |
| `SF_DDay_SadEntries` | `DDAY_07_SAD_SOHA_ENTRY`, `DDAY_08_SAD_SEORIN_ENTRY`, `DDAY_09_SAD_MIRU_ENTRY` |

## 22. 다음 연결

이 문서 이후 우선 작업은 두 갈래다.

1. 진엔딩 이후 [엔딩과 루프](./05_엔딩과_루프.md)의 D+2 병실 장면과 D+3 방학식 장면을 실제 컷신 단위로 분리한다.
2. 이 스크립트 분할을 시스템 문서의 StoryFlow Scene/Branch/Transition 설계와 맞춘다.

현재 기준에서는 D+2/D+3 컷신 분리가 더 먼저다. 그래야 `DDAY_05_TRUE_APPROACH` 이후의 연결이 완성된다.
