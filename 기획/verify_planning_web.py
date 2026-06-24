from __future__ import annotations

import json
import re
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parent
PROJECT_ROOT = ROOT.parent
ROOT_HTML = PROJECT_ROOT / 'index.html'
PLANNING_HTML = ROOT / '통합_기획서.html'


def read_text(path: Path) -> str:
    return path.read_text(encoding='utf-8-sig')


def markdown_files() -> list[Path]:
    return sorted(ROOT.rglob('*.md'))


def missing_markdown_links() -> list[dict[str, object]]:
    missing: list[dict[str, object]] = []
    for path in markdown_files():
        text = read_text(path)
        for match in re.finditer(r'\[[^\]]+\]\(([^)]+)\)', text):
            link = match.group(1).split('#', 1)[0]
            if not link or re.match(r'^[a-zA-Z][a-zA-Z0-9+.-]*:', link):
                continue
            target = (path.parent / link).resolve()
            if not target.exists():
                missing.append({
                    'file': str(path.relative_to(ROOT)),
                    'line': text[:match.start()].count('\n') + 1,
                    'link': match.group(1),
                })
    return missing


def html_payload(html_text: str) -> list[dict[str, object]]:
    match = re.search(
        r'<script id="doc-data" type="application/json">(.*?)</script>',
        html_text,
        flags=re.DOTALL,
    )
    if not match:
        raise AssertionError('doc-data script를 찾을 수 없음')
    return json.loads(match.group(1))


def verify() -> dict[str, object]:
    root_html = read_text(ROOT_HTML)
    planning_html = read_text(PLANNING_HTML)
    payload = html_payload(root_html)
    schedule_doc = next((doc for doc in payload if doc.get('path') == '개발_일정.md'), None)
    errors: list[str] = []
    missing_links = missing_markdown_links()

    if root_html != planning_html:
        errors.append('index.html과 기획/통합_기획서.html 내용이 다름')
    if len(payload) != len(markdown_files()):
        errors.append('Markdown 문서 수와 HTML payload 문서 수가 다름')
    if missing_links:
        errors.append('Markdown 내부 링크 누락 존재')
    if schedule_doc is None:
        errors.append('개발 일정 문서가 HTML payload에 없음')
    else:
        schedule_html = str(schedule_doc.get('html', ''))
        if 'TC-DDAY-001' not in schedule_html:
            errors.append('개발 일정 문서에 D-Day 테스트 기준이 없음')
        if 'status-select' not in schedule_html:
            errors.append('개발 일정 상태가 드롭다운으로 렌더링되지 않음')
        if 'task-checkbox' in schedule_html:
            errors.append('개발 일정 문서에 체크박스가 남아 있음')
        if 'GitHub Pages' not in schedule_html:
            errors.append('개발 일정 문서에 공유 방식 안내가 없음')
        if '로컬 편집 방식' not in schedule_html:
            errors.append('개발 일정 문서에 로컬 편집 방식 안내가 없음')
        if '공유 페이지에는 반영되지 않는다' not in schedule_html:
            errors.append('개발 일정 문서에 로컬 상태 주의 문구가 없음')
    if '개발 일정 보기' not in root_html:
        errors.append('홈 빠른 시작에 개발 일정 바로가기가 없음')
    if '체크박스형 작업 상태' in root_html:
        errors.append('홈 빠른 시작에 오래된 체크박스 문구가 남아 있음')
    if '__STATUS_CLASSES__' in root_html:
        errors.append('상태 클래스 placeholder가 치환되지 않음')
    if 'bindStatusSelects' not in root_html or 'localStorage' not in root_html:
        errors.append('상태 드롭다운 로컬 저장 스크립트가 없음')
    if 'bindTaskCheckboxes' in root_html:
        errors.append('체크박스 저장 스크립트가 남아 있음')

    return {
        'md_count': len(markdown_files()),
        'payload_count': len(payload),
        'missing_links': len(missing_links),
        'html_identical': root_html == planning_html,
        'has_schedule_doc': schedule_doc is not None,
        'has_status_selects': 'status-select' in root_html,
        'has_local_storage_status': 'localStorage' in root_html and 'bindStatusSelects' in root_html,
        'has_checkbox_storage': 'bindTaskCheckboxes' in root_html,
        'errors': errors,
    }


def main() -> int:
    result = verify()
    print(json.dumps(result, ensure_ascii=False, indent=2))
    return 1 if result['errors'] else 0


if __name__ == '__main__':
    sys.exit(main())
