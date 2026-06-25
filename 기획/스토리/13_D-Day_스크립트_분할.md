# D-Day 실제 게임 스크립트 분할

이 문서는 [D-Day 축제 마지막 날 분기](./10_D-Day_축제마지막날_분기.md)의 대본을 실제 게임 스크립트 제작 단위로 쪼갠 문서다.

점수·플래그 판정 기준은 [D-1/D-Day 조건 수치화](./12_D-1_D-Day_조건_수치화.md)를 따른다.

## 1. 문서 목적

- D-Day를 StoryFlow Scene 또는 VN 스크립트 단위로 나눈다.
- 각 장면의 진입 조건, 선택지, 상태 변경, 다음 장면을 명확히 한다.
- 긴 대본 문서인 `10`을 그대로 구현하지 않고, 제작 가능한 스크립트 블록으로 변환한다.
- 이 문서는 StoryFlow/VN 제작 기준이며, 실제 엔진 입력 필드는 시스템 문서의 VisualNovelPlugin 설계를 따른다.

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
| `# SceneID` | 장면 고유 ID |
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
DDay_00Entry
        ↓
DDay_01Choice
        ↓
DDay_02Hayeon
DDay_02Soha
DDay_02Seorin
DDay_02Miru
DDay_02Alone
        ↓
DDay_03Assess
        ↓
DDay_04Gate
        ├─ DDay_04TrueChoice
        │       └─ DDay_05True
        ├─ DDay_06HayeonMiss
        ├─ DDay_07SohaSad
        ├─ DDay_08SeorinSad
        ├─ DDay_09MiruSad
        ├─ DDay_10RelationUnresolved
        ├─ DDay_11AccFail
        ├─ DDay_12Avoid
        └─ DDay_13HiddenCollapse
```

## 4. 공통 진입

### DDay_00Entry

```text
# DDay_00Entry
@enter Date == DDay
@enter IsHayeonBoothD1Complete == true
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
@flag IsDDayCafeClosed = true
@goto DDay_01Choice
```

## 5. 낮 선택 허브

### DDay_01Choice

```text
# DDay_01Choice
@enter IsDDayCafeClosed == true
@bg 학교 축제 운동장 / 낮
@music festival_day_choice
```

배경 지시:

- `14`의 `축제 낮` 기준.
- 밝은 천막과 학생 소음은 유지하되, 화면 한쪽에 조용한 가장자리 동선이 보여야 한다.
- 선택 허브이므로 어느 캐릭터 방향으로도 갈 수 있는 열린 구도가 좋다.

선택지:

```text
@choice [하연과 축제를 둘러본다] -> DDay_02Hayeon
@choice [소하와 마지막으로 오래된 길을 걷는다] -> DDay_02Soha
@choice [서린의 방송 마무리를 돕는다] -> DDay_02Seorin
@choice [미루의 도움 인원 마무리를 확인한다] -> DDay_02Miru
@choice [혼자 야외 소무대 쪽을 본다] -> DDay_02Alone
```

주의:

- 낮 선택은 한 번만 가능하다.
- 선택한 축만 보정한다.
- `DidUseDDayChoice = true`를 공통으로 설정한다.

## 6. 낮 선택 A: 하연

### DDay_02Hayeon

```text
# DDay_02Hayeon
@enter DidUseDDayChoice != true
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
@if HayeonTrust >= 5 || HayeonPace >= 2
  @flag HayeonTrust += 1
  @flag HayeonPace += 1
  @flag DidChooseHayeonOnDDay = true
@endif

@if HasHayeonD2Premonition == true && DidChooseHayeonOnDDay == true
  @flag HasHayeonClue = true
@endif

@flag DidUseDDayChoice = true
@flag DDayChoice = Hayeon
@goto DDay_03Assess
```

실패/약한 진행:

```text
@if HayeonTrust <= 4 && HayeonPace <= 1
  @flag DidChooseHayeonOnDDay = false
@endif
```

## 7. 낮 선택 B: 소하

### DDay_02Soha

```text
# DDay_02Soha
@enter DidUseDDayChoice != true
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
@choice [예전처럼 편한 쪽으로 돌아간다] -> DDay_07SohaSad
@choice [지금 가야 할 사람을 말한다] -> DDay_02SohaResolve
```

#### DDay_02SohaResolve

```text
재윤: 편해서 돌아가는 거랑, 좋아해서 가는 건 다르잖아.
소하: 이제 그런 말도 하네.
재윤: 늦었지.
소하: 늦었어. 그래도 지금 말했잖아.
```

상태 처리:

```text
@if IsSohaResolved == true
  @flag DidChooseSohaOnDDay = true
@elseif SohaPressure <= 2
  @flag IsSohaResolved = true
  @flag HasSohaClue = true
  @flag DidChooseSohaOnDDay = true
@else
  @flag SohaPressure = 3
@endif

@flag DidUseDDayChoice = true
@flag DDayChoice = Soha
@goto DDay_03Assess
```

## 8. 낮 선택 C: 서린

### DDay_02Seorin

```text
# DDay_02Seorin
@enter DidUseDDayChoice != true
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
@choice [서린이 정한 순서를 따른다] -> DDay_08SeorinSad
@choice [내가 확인해야 할 것을 고른다] -> DDay_02SeorinResolve
```

#### DDay_02SeorinResolve

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
@if IsSeorinResolved == true
  @flag DidChooseSeorinOnDDay = true
  @flag HasSeorinClue = true
@elseif SeorinControl <= 2
  @flag IsSeorinResolved = true
  @flag HasSeorinClue = true
  @flag DidChooseSeorinOnDDay = true
@else
  @flag SeorinControl = 3
@endif

@flag DidUseDDayChoice = true
@flag DDayChoice = Seorin
@goto DDay_03Assess
```

## 9. 낮 선택 D: 미루

### DDay_02Miru

```text
# DDay_02Miru
@enter DidUseDDayChoice != true
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
@choice [마지막까지 대신 봐준다] -> DDay_09MiruSad
@choice [먼저 끝낸 일을 인정한다] -> DDay_02MiruResolve
```

#### DDay_02MiruResolve

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
@if IsMiruResolved == true
  @flag DidChooseMiruOnDDay = true
  @flag HasMiruClue = true
@elseif MiruDepend <= 2
  @flag IsMiruResolved = true
  @flag HasMiruClue = true
  @flag DidChooseMiruOnDDay = true
@else
  @flag MiruDepend = 3
@endif

@flag DidUseDDayChoice = true
@flag DDayChoice = Miru
@goto DDay_03Assess
```

## 10. 낮 선택 E: 혼자 소무대 확인

### DDay_02Alone

```text
# DDay_02Alone
@enter DidUseDDayChoice != true
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
@if ClueScore >= 2
  @flag IsMissingPhysicalClue = true
@endif

@if HasSeorinClue == false
  @flag HasSeorinClue = true
@elseif HasMiruClue == false
  @flag HasMiruClue = true
@endif

@if HasHayeonClue == false
  @flag DidAvoidHeartOnDDay = true
@endif

@if Avoid >= 3
  @flag Avoid += 1
@endif

@flag DidUseDDayChoice = true
@flag DDayChoice = AloneStage
@goto DDay_03Assess
```

주의:

- `HasHayeonClue`은 혼자 조사로 획득하지 않는다.
- 이 선택은 사고를 이해하는 데 도움이 되지만, 하연에게 말하러 가는 감정 조건을 대신하지 않는다.

## 11. 폐장 전 상태 평가

### DDay_03Assess

```text
# DDay_03Assess
@enter DidUseDDayChoice == true
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
@eval DDayResult = EvaluateDDayResult()
@goto DDay_04Gate
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

### DDay_04Gate

```text
# DDay_04Gate
@enter DDayResult != None
```

라우팅:

```text
@if DDayResult == HiddenCollapse
  @goto DDay_13HiddenCollapse
@elseif DDayResult == True
  @goto DDay_04TrueChoice
@elseif DDayResult == HayeonMiss
  @goto DDay_06HayeonMiss
@elseif DDayResult == SohaSad
  @goto DDay_07SohaSad
@elseif DDayResult == SeorinSad
  @goto DDay_08SeorinSad
@elseif DDayResult == MiruSad
  @goto DDay_09MiruSad
@elseif DDayResult == RelationUnresolved
  @goto DDay_10RelationUnresolved
@elseif DDayResult == AccFail
  @goto DDay_11AccFail
@elseif DDayResult == AvoidLoop
  @goto DDay_12Avoid
@else
  @goto DDay_06HayeonMiss
@endif
```

분기 평가는 `12_D-1_D-Day_조건_수치화.md`의 우선순위를 따른다.

### DDay_04TrueChoice

```text
# DDay_04TrueChoice
@enter DDayResult == True
@bg 2학년 5반 폐장 후 교실
@music true_choice_silence
```

목적:

- 상위 기획의 최종 선택지 개방 구조를 실제 스크립트 단위에 반영한다.
- 조건을 모두 만족했더라도 진엔딩은 자동 보상이 아니라, 재윤이 마지막으로 직접 선택하는 장면이어야 한다.

선택지:

```text
@choice [하연에게 말하러 간다] -> DDay_05True
```

상태 처리:

```text
@flag DidChooseTrueEnding = true
```

## 13. 진엔딩 성공 스크립트

### DDay_05True

```text
# DDay_05True
@enter DDayResult == True
@enter DidChooseTrueEnding == true
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
@if IsSohaResolved == true
  @insert "예전으로 돌아가 숨고 싶었던 마음을 지나서 왔어."
@endif
@if IsSeorinResolved == true
  @insert "맞는 답이 아니라, 내가 고른 답이라고 말하려고 왔어."
@endif
@if IsMiruResolved == true
  @insert "누군가에게 필요한 사람인 척하면서 내 마음을 미루지 않으려고 왔어."
@endif
@if HasHayeonD2Premonition == true
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
@flag HasConfirmedHayeonMutualFeelings = true
@flag IsHayeonMissingFromMemory = false
@flag HasAcceptedAccidentMemory = true
@flag IsDDayTrueEnding = true
@flag CanEnterTrueEndingEpilogue = true
@jump TE_00_TimeSkip
```

다음 연결:

- [D+2/D+3 진엔딩 후일담 컷신](./16_D+2_D+3_진엔딩_후일담_컷신.md)
- [진엔딩 크레딧 콘티 초안](./11_진엔딩_크레딧_콘티_초안.md)

## 14. 하연 실패 루프 스크립트

### DDay_06HayeonMiss

```text
# DDay_06HayeonMiss
@enter DDayResult == HayeonMiss
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
@flag IsHayeonMissingFromMemory = true
@flag IsDDayTrueEnding = false
@flag LoopCount += 1
@flag ReturnDate = D25
@end LoopToD25
```

## 15. 소하 새드엔딩 진입 스크립트

### DDay_07SohaSad

```text
# DDay_07SohaSad
@enter DDayResult == SohaSad || SohaPressure == 3
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
@flag HasSeenSohaSadEnding = true
@flag SohaSadCount += 1
@flag LoopCount += 1
@end SadEndingSohaFakeCredit
```

## 16. 서린 새드엔딩 진입 스크립트

### DDay_08SeorinSad

```text
# DDay_08SeorinSad
@enter DDayResult == SeorinSad || SeorinControl == 3
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
@flag HasSeenSeorinSadEnding = true
@flag SeorinSadCount += 1
@flag LoopCount += 1
@end SadEndingSeorinFakeCredit
```

## 17. 미루 새드엔딩 진입 스크립트

### DDay_09MiruSad

```text
# DDay_09MiruSad
@enter DDayResult == MiruSad || MiruDepend == 3
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
@flag HasSeenMiruSadEnding = true
@flag MiruSadCount += 1
@flag LoopCount += 1
@end SadEndingMiruFakeCredit
```

## 18. 기존 관계 미정리 루프

### DDay_10RelationUnresolved

```text
# DDay_10RelationUnresolved
@enter DDayResult == RelationUnresolved
@bg 축제 폐장 후 복도
@music loop_relation_unresolved
```

배경 지시:

- 하연이 있을 방향은 어렴풋이 보이지만, 소하/서린/미루 중 정리하지 못한 관계의 목소리나 메시지가 화면 위에 남는다.
- 완전 새드엔딩처럼 한 인물의 세계에 갇히지는 않지만, 재윤이 하연에게 가는 선택을 끝까지 붙잡지 못하는 약한 실패다.

목적:

- 하연 신뢰와 동선은 충분하지만 기존 관계 정리만 부족한 케이스를 `HayeonMiss`와 분리한다.
- 원인은 하연을 못 찾는 것이 아니라, 다른 관계 뒤에 숨는 마음이 아직 남아 있다는 점을 보여준다.

상태 처리:

```text
@flag IsRelationUnresolvedLoop = true
@flag LoopCount += 1
@flag ReturnDate = D25
@end LoopToD25
```

## 19. 사고 의미 미연결 루프

### DDay_11AccFail

```text
# DDay_11AccFail
@enter DDayResult == AccFail
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
@flag IsAccidentMemoryUnlinked = true
@flag LoopCount += 1
@flag ReturnDate = D25
@end LoopToD25
```

## 20. 회피 루프

### DDay_12Avoid

```text
# DDay_12Avoid
@enter DDayResult == AvoidLoop
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
@flag HasSeenAvoidLoop = true
@flag LoopCount += 1
@flag ReturnDate = D25
@end LoopToD25
```

## 21. 히든 붕괴 루프

### DDay_13HiddenCollapse

```text
# DDay_13HiddenCollapse
@enter DDayResult == HiddenCollapse
@bg 월요일 교실과 축제 마지막 밤이 뒤섞인 복도
@music hidden_collapse_noise
@sfx distant_festival_bell, broken_school_chime
```

배경 지시:

- 월요일 아침 교실, 축제 폐장 후 복도, 소무대 조명이 한 화면에 겹친다.
- 시간 붕괴를 설명 대사로 풀지 말고, 서로 맞지 않는 배경음과 조명으로 먼저 느끼게 한다.
- 공포 연출보다 `선택을 회피한 기억이 구조 자체를 망가뜨린다`는 불안감에 집중한다.

목적:

- 반복을 악용하거나 핵심 선택을 계속 피한 경우 정상 루프로 돌아가지 못하는 예외 실패다.
- 진엔딩/새드엔딩이 아니라, 플레이 구조가 무너지는 히든 실패로 취급한다.

스크립트:

```text
종소리가 울렸다.

분명 월요일 아침이어야 했다.

하지만 복도 끝에는 축제 조명이 아직 꺼지지 않은 채 매달려 있었고,
교실 칠판에는 폐장 안내 문구가 분필가루처럼 번져 있었다.

재윤은 책상 위에 놓인 자기 손을 내려다보았다.
어제의 손인지,
스물다섯 번째 날의 손인지,
깨어난 뒤의 손인지 알 수 없었다.

멀리서 하연의 목소리 같은 것이 들렸다.

이번에도,
말하지 않을 거야?

대답하려는 순간,
문장이 먼저 지워졌다.
```

상태 처리:

```text
@flag HasSeenHiddenCollapse = true
@flag IsHiddenCollapseActive = true
@flag LoopCount += 1
@flag ReturnDate = D25
@end BrokenLoop
```

## 22. 평가 함수 기준

실제 구현 시 사용할 평가 함수 의사코드다.

```text
function EvaluateDDayResult():
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

    if ClueScore < 3 or HasHayeonClue == false:
        return AccFail

    if Avoid >= 4:
        return AvoidLoop

    return True
```

## 23. 실제 제작 시 분리 권장 에셋

| 에셋 | 포함 Scene |
| --- | --- |
| `SF_DDay` | `DDay_00Entry`, `DDay_01Choice`, `DDay_03Assess`, `DDay_04Gate`, `DDay_04TrueChoice` |
| `SF_DDayHayeon` | `DDay_02Hayeon` |
| `SF_DDaySoha` | `DDay_02Soha`, `DDay_02SohaResolve` |
| `SF_DDaySeorin` | `DDay_02Seorin`, `DDay_02SeorinResolve` |
| `SF_DDayMiru` | `DDay_02Miru`, `DDay_02MiruResolve` |
| `SF_DDayAlone` | `DDay_02Alone` |
| `SF_DDayTrue` | `DDay_05True` |
| `SF_DDayFail` | `DDay_06HayeonMiss`, `DDay_10RelationUnresolved`, `DDay_11AccFail`, `DDay_12Avoid` |
| `SF_DDayHidden` | `DDay_13HiddenCollapse` |
| `SF_DDaySad` | `DDay_07SohaSad`, `DDay_08SeorinSad`, `DDay_09MiruSad` |

## 24. 다음 연결

이 문서 이후 우선 작업은 두 갈래다.

1. 진엔딩 이후 D+2 병실 장면과 D+3 방학식 장면은 [D+2/D+3 진엔딩 후일담 컷신](./16_D+2_D+3_진엔딩_후일담_컷신.md)을 따른다.
2. 이 스크립트 분할을 시스템 문서의 StoryFlow Scene/Branch/Transition 설계와 맞춘다.

현재 기준에서는 `DDay_05True` 이후 `TE_00_TimeSkip`로 넘기고, D+2/D+3 후일담이 끝난 뒤 크레딧으로 연결한다.
