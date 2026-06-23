from __future__ import annotations

import html
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent
PROJECT_ROOT = ROOT.parent
OUTPUT = PROJECT_ROOT / 'index.html'
LEGACY_OUTPUT = ROOT / '통합_기획서.html'

EXCLUDE_NAMES = {
    '통합_기획서.html',
}

CATEGORY_ORDER = {
    '기획': 0,
    '스토리': 1,
    '시스템': 2,
}


def read_text(path: Path) -> str:
    return path.read_text(encoding='utf-8-sig')


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def doc_category(path: Path) -> str:
    r = path.relative_to(ROOT)
    if len(r.parts) == 1:
        return '기획'
    return r.parts[0]


def sort_key(path: Path):
    category = doc_category(path)
    return (CATEGORY_ORDER.get(category, 99), rel(path))


def slugify(value: str, fallback: str) -> str:
    value = re.sub(r'[`*_\[\]()]', '', value).strip().lower()
    value = re.sub(r'[^0-9a-zA-Z가-힣]+', '-', value).strip('-')
    return value or fallback


def inline_md(text: str) -> str:
    text = html.escape(text)
    text = re.sub(r'`([^`]+)`', r'<code>\1</code>', text)
    text = re.sub(r'\*\*([^*]+)\*\*', r'<strong>\1</strong>', text)
    text = re.sub(r'\[([^\]]+)\]\(([^)]+)\)', r'<a href="\2">\1</a>', text)
    return text


def render_table(lines: list[str]) -> str:
    rows = []
    for line in lines:
        cells = [c.strip() for c in line.strip().strip('|').split('|')]
        rows.append(cells)
    if len(rows) < 2:
        return ''.join(f'<p>{inline_md(line)}</p>' for line in lines)
    header = rows[0]
    body = rows[2:] if re.match(r'^\s*:?-{3,}:?\s*$', rows[1][0] if rows[1] else '') else rows[1:]
    out = ['<div class="table-wrap"><table><thead><tr>']
    out.extend(f'<th>{inline_md(cell)}</th>' for cell in header)
    out.append('</tr></thead><tbody>')
    for row in body:
        out.append('<tr>')
        width = max(len(header), len(row))
        for i in range(width):
            out.append(f'<td>{inline_md(row[i]) if i < len(row) else ""}</td>')
        out.append('</tr>')
    out.append('</tbody></table></div>')
    return ''.join(out)


def render_markdown(markdown: str, doc_id: str) -> tuple[str, list[dict]]:
    lines = markdown.splitlines()
    out: list[str] = []
    headings: list[dict] = []
    paragraph: list[str] = []
    list_stack: list[str] = []
    in_code = False
    code_lines: list[str] = []
    table_lines: list[str] = []
    used_ids: set[str] = set()

    def unique_id(base: str) -> str:
        candidate = base
        i = 2
        while candidate in used_ids:
            candidate = f'{base}-{i}'
            i += 1
        used_ids.add(candidate)
        return candidate

    def flush_paragraph():
        nonlocal paragraph
        if paragraph:
            text = ' '.join(p.strip() for p in paragraph).strip()
            if text:
                out.append(f'<p>{inline_md(text)}</p>')
            paragraph = []

    def close_lists():
        nonlocal list_stack
        while list_stack:
            out.append(f'</{list_stack.pop()}>')

    def flush_table():
        nonlocal table_lines
        if table_lines:
            out.append(render_table(table_lines))
            table_lines = []

    for raw in lines + ['']:
        line = raw.rstrip('\n')

        if in_code:
            if line.strip().startswith('```'):
                out.append('<pre><code>' + html.escape('\n'.join(code_lines)) + '</code></pre>')
                code_lines = []
                in_code = False
            else:
                code_lines.append(line)
            continue

        if line.strip().startswith('```'):
            flush_paragraph(); flush_table(); close_lists()
            in_code = True
            code_lines = []
            continue

        if not line.strip():
            flush_paragraph(); flush_table(); close_lists()
            continue

        heading = re.match(r'^(#{1,6})\s+(.+)$', line)
        if heading:
            flush_paragraph(); flush_table(); close_lists()
            level = len(heading.group(1))
            text = heading.group(2).strip()
            hid = unique_id(f'{doc_id}-{slugify(text, "heading")}')
            headings.append({'level': level, 'text': text, 'id': hid})
            out.append(f'<h{min(level + 1, 6)} id="{hid}">{inline_md(text)}</h{min(level + 1, 6)}>')
            continue

        if line.startswith('|') and line.endswith('|'):
            flush_paragraph(); close_lists()
            table_lines.append(line)
            continue
        else:
            flush_table()

        quote = re.match(r'^>\s?(.*)$', line)
        if quote:
            flush_paragraph(); close_lists()
            out.append(f'<blockquote>{inline_md(quote.group(1))}</blockquote>')
            continue

        li = re.match(r'^(\s*)([-*]|\d+\.)\s+(.+)$', line)
        if li:
            flush_paragraph(); flush_table()
            kind = 'ol' if li.group(2).endswith('.') and li.group(2)[:-1].isdigit() else 'ul'
            if not list_stack or list_stack[-1] != kind:
                close_lists()
                out.append(f'<{kind}>')
                list_stack.append(kind)
            out.append(f'<li>{inline_md(li.group(3).strip())}</li>')
            continue

        paragraph.append(line)

    flush_paragraph(); flush_table(); close_lists()
    return '\n'.join(out), headings


def collect_docs() -> list[dict]:
    docs: list[dict] = []
    for path in sorted(ROOT.rglob('*.md'), key=sort_key):
        if path.name in EXCLUDE_NAMES:
            continue
        content = read_text(path)
        title_match = re.search(r'^#\s+(.+)$', content, flags=re.MULTILINE)
        title = title_match.group(1).strip() if title_match else path.stem
        doc_id = slugify(rel(path), path.stem)
        rendered, headings = render_markdown(content, doc_id)
        docs.append({
            'path': rel(path),
            'category': doc_category(path),
            'title': title,
            'id': doc_id,
            'html': rendered,
            'headings': headings,
            'plain': re.sub(r'\s+', ' ', content).strip(),
        })
    return docs


def build_html(docs: list[dict]) -> str:
    payload = []
    for d in docs:
        payload.append({
            'path': d['path'],
            'category': d['category'],
            'title': d['title'],
            'id': d['id'],
            'html': d['html'],
            'headings': d['headings'],
            'plain': d['plain'],
            'excerpt': d['plain'][:220],
        })

    data_json = json.dumps(payload, ensure_ascii=False).replace('</', '<\\/')

    html_text = r'''<!doctype html>
<html lang="ko">
<head>
  <meta charset="utf-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <title>너에게 닿는 마지막 여름 통합 기획서</title>
  <style>
    :root {
      --bg:#f4f7fb;
      --panel:#ffffff;
      --panel-soft:#f8fafc;
      --ink:#172033;
      --muted:#667085;
      --line:#d9e2ee;
      --accent:#4f46e5;
      --accent-2:#0ea5e9;
      --accent-soft:#eef2ff;
      --shadow:0 20px 50px rgba(15,23,42,.08);
      --shadow-soft:0 8px 24px rgba(15,23,42,.06);
      --radius:18px;
      --library:360px;
      --reader-max:1040px;
    }
    * { box-sizing:border-box; }
    html {
      scroll-behavior:smooth;
      width:100%;
      overflow-x:hidden;
      -webkit-text-size-adjust:100%;
    }
    body {
      margin:0;
      width:100%;
      overflow-x:hidden;
      min-height:100vh;
      background:
        radial-gradient(circle at 10% 0%, rgba(79,70,229,.12), transparent 28rem),
        radial-gradient(circle at 90% 0%, rgba(14,165,233,.12), transparent 28rem),
        linear-gradient(180deg,#f8fbff 0,var(--bg) 100%);
      color:var(--ink);
      font-family:"Pretendard","Inter","Segoe UI","Apple SD Gothic Neo","Malgun Gothic",sans-serif;
      line-height:1.72;
      letter-spacing:-.01em;
    }
    a { color:var(--accent); text-decoration:none; }
    a:hover { text-decoration:underline; text-underline-offset:3px; }
    .topbar {
      position:sticky;
      top:0;
      z-index:30;
      display:flex;
      align-items:center;
      justify-content:space-between;
      gap:18px;
      padding:14px 22px;
      background:rgba(255,255,255,.84);
      border-bottom:1px solid var(--line);
      backdrop-filter:blur(18px);
    }
    .brand { display:flex; align-items:center; gap:12px; min-width:0; }
    .logo {
      width:38px; height:38px; border-radius:13px;
      background:linear-gradient(135deg,var(--accent),var(--accent-2));
      box-shadow:0 10px 24px rgba(79,70,229,.24);
    }
    .brand h1 { margin:0; font-size:18px; line-height:1.1; letter-spacing:-.045em; overflow-wrap:anywhere; }
    .brand p { margin:2px 0 0; color:var(--muted); font-size:12px; }
    .top-actions { display:flex; gap:8px; flex-wrap:wrap; justify-content:flex-end; }
    button, .button {
      border:1px solid var(--line);
      background:var(--panel);
      color:var(--ink);
      border-radius:999px;
      padding:8px 12px;
      cursor:pointer;
      font:inherit;
      font-size:13px;
      text-decoration:none;
      box-shadow:var(--shadow-soft);
      transition:transform .12s ease, border-color .12s ease, background .12s ease, color .12s ease;
    }
    button:hover, .button:hover { border-color:var(--accent); color:var(--accent); text-decoration:none; transform:translateY(-1px); }
    button.active { border-color:var(--accent); color:var(--accent); font-weight:800; background:var(--accent-soft); }
    .shell {
      display:grid;
      grid-template-columns:var(--library) minmax(0,1fr);
      gap:22px;
      max-width:1520px;
      margin:0 auto;
      padding:22px;
      width:100%;
      min-width:0;
    }
    .library {
      position:sticky;
      top:82px;
      height:calc(100vh - 104px);
      overflow:auto;
      background:rgba(255,255,255,.92);
      border:1px solid var(--line);
      border-radius:24px;
      box-shadow:var(--shadow);
      padding:18px;
    }
    .library-head h2 { margin:0; font-size:20px; letter-spacing:-.045em; }
    .library-head p { margin:6px 0 14px; color:var(--muted); font-size:13px; }
    .reader { min-width:0; max-width:var(--reader-max); width:100%; margin:0 auto; }
    .search-box { display:grid; gap:10px; margin:14px 0; }
    input[type="search"] {
      width:100%;
      border:1px solid var(--line);
      border-radius:14px;
      padding:12px 14px;
      background:var(--panel);
      color:var(--ink);
      font:inherit;
      font-size:14px;
      outline:none;
      box-shadow:var(--shadow-soft);
    }
    input[type="search"]:focus { border-color:var(--accent); box-shadow:0 0 0 4px rgba(79,70,229,.12); }
    .chips { display:flex; flex-wrap:wrap; gap:7px; }
    .result { color:var(--muted); font-size:13px; margin:6px 0; }
    .doc-list { display:grid; gap:9px; }
    .doc-button {
      width:100%;
      border:1px solid var(--line);
      background:rgba(255,255,255,.88);
      border-radius:16px;
      padding:13px;
      text-align:left;
      display:grid;
      gap:6px;
      box-shadow:none;
    }
    .doc-button strong { display:block; line-height:1.3; font-size:14px; }
    .doc-button small { color:var(--muted); word-break:break-all; }
    .doc-button.active { border-color:var(--accent); background:var(--accent-soft); box-shadow:0 10px 28px rgba(79,70,229,.14); }
    .badge { display:inline-flex; width:max-content; align-items:center; border-radius:999px; padding:4px 9px; background:var(--accent-soft); color:var(--accent); font-size:12px; font-weight:800; }
    .dashboard, .doc-view {
      background:rgba(255,255,255,.96);
      border:1px solid var(--line);
      border-radius:26px;
      box-shadow:var(--shadow);
      min-width:0;
    }
    .dashboard {
      overflow:hidden;
    }
    .doc-view { overflow:visible; }
    .dashboard-head, .doc-head { padding:34px 38px; border-bottom:1px solid var(--line); background:linear-gradient(135deg,#ffffff 0,#f6f8ff 100%); }
    .dashboard h2, .doc-head h2 { margin:8px 0 0; font-size:clamp(30px,4vw,50px); line-height:1.13; letter-spacing:-.06em; overflow-wrap:anywhere; }
    .dashboard-head p, .doc-head p { max-width:760px; color:#475467; margin:14px 0 0; font-size:16px; }
    .dashboard-body, .doc-body { padding:32px 40px; }
    .doc-reading {
      display:grid;
      grid-template-columns:minmax(0,1fr) 260px;
      align-items:start;
      min-width:0;
    }
    .doc-side-toc {
      position:sticky;
      top:86px;
      max-height:calc(100vh - 108px);
      overflow:auto;
      border-left:1px solid var(--line);
      background:linear-gradient(180deg,#fbfdff 0,#ffffff 100%);
      padding:26px 18px;
    }
    .doc-side-toc strong { display:block; margin-bottom:10px; font-size:14px; letter-spacing:-.035em; }
    .doc-side-toc nav { display:grid; gap:3px; }
    .doc-side-toc a {
      display:block;
      color:#475467;
      font-size:13px;
      line-height:1.35;
      border-radius:9px;
      padding:6px 7px;
      word-break:keep-all;
    }
    .doc-side-toc a:hover { background:var(--accent-soft); color:var(--accent); text-decoration:none; }
    .doc-side-toc .lv2 { font-weight:800; color:#27364f; }
    .doc-side-toc .lv3 { padding-left:16px; }
    .doc-side-toc .lv4, .doc-side-toc .lv5, .doc-side-toc .lv6 { padding-left:26px; color:var(--muted); }
    .grid { display:grid; gap:14px; }
    .grid.cards { grid-template-columns:repeat(auto-fit,minmax(220px,1fr)); }
    .stat, .quick-card {
      background:var(--panel-soft);
      border:1px solid var(--line);
      border-radius:18px;
      padding:18px;
      box-shadow:var(--shadow-soft);
    }
    .stat b { display:block; color:var(--accent); font-size:32px; line-height:1; margin-bottom:8px; }
    .quick-card h3 { margin:0 0 8px; font-size:20px; }
    .quick-card p { margin:0 0 12px; color:var(--muted); font-size:14px; }
    .quick-card button { border-radius:12px; width:100%; text-align:left; background:var(--panel); }
    .doc-tools { display:flex; gap:8px; flex-wrap:wrap; margin-top:18px; }
    .path { color:var(--muted); font-size:13px; word-break:break-all; }
    .inline-toc {
      margin-top:18px;
      border:1px solid var(--line);
      border-radius:16px;
      background:rgba(255,255,255,.78);
      overflow:hidden;
    }
    .inline-toc summary { cursor:pointer; padding:12px 14px; font-weight:800; }
    .inline-toc nav { display:grid; grid-template-columns:repeat(auto-fit,minmax(220px,1fr)); gap:4px 10px; padding:0 14px 14px; }
    .inline-toc a { color:#344054; font-size:13px; padding:5px 6px; border-radius:8px; }
    .inline-toc a:hover { background:var(--accent-soft); color:var(--accent); text-decoration:none; }
    .inline-toc .lv3 { padding-left:16px; color:#4b5563; }
    .inline-toc .lv4, .inline-toc .lv5, .inline-toc .lv6 { padding-left:26px; color:var(--muted); }
    .doc-body { font-size:16px; min-width:0; overflow-wrap:break-word; }
    .doc-body h2, .doc-body h3, .doc-body h4, .doc-body h5, .doc-body h6 { line-height:1.28; letter-spacing:-.04em; scroll-margin-top:92px; color:#111827; overflow-wrap:anywhere; }
    .doc-body h2 { font-size:34px; border-bottom:2px solid var(--line); padding-bottom:12px; margin:0 0 18px; }
    .doc-body h3 { font-size:26px; margin:38px 0 14px; }
    .doc-body h4 { font-size:21px; margin:30px 0 10px; }
    .doc-body p { margin:11px 0; }
    .doc-body ul, .doc-body ol { padding-left:1.35rem; }
    .doc-body li { margin:5px 0; }
    .doc-body code { background:#eef2ff; color:#3730a3; border:1px solid #dfe3ff; border-radius:6px; padding:1px 5px; }
    .doc-body pre { background:#111827; color:#e5e7eb; border-radius:16px; padding:16px; overflow:auto; max-width:100%; -webkit-overflow-scrolling:touch; box-shadow:inset 0 0 0 1px rgba(255,255,255,.08); }
    .doc-body pre code { background:transparent; color:inherit; border:0; padding:0; }
    .doc-body blockquote { margin:14px 0; border-left:4px solid var(--accent); background:#f5f7ff; padding:12px 15px; border-radius:12px; color:#30364a; }
    .doc-body .table-wrap { overflow:auto; max-width:100%; margin:16px 0; border:1px solid var(--line); border-radius:14px; background:var(--panel); -webkit-overflow-scrolling:touch; }
    .doc-body table { width:100%; border-collapse:collapse; min-width:620px; background:var(--panel); }
    .doc-body th, .doc-body td { border:1px solid var(--line); padding:10px 11px; vertical-align:top; }
    .doc-body th { background:#f1f5f9; text-align:left; color:#344054; }
    .doc-body tr:nth-child(even) td { background:#fbfdff; }
    .doc-body img { max-width:100%; height:auto; }
    .doc-body.sectioned > h2:first-child { margin-bottom:24px; }
    .planning-section {
      margin:18px 0;
      border:1px solid var(--line);
      border-radius:18px;
      background:var(--panel);
      box-shadow:var(--shadow-soft);
      overflow:hidden;
    }
    .planning-section[open] { border-color:#c7d2fe; }
    .planning-section summary {
      display:grid;
      grid-template-columns:minmax(0,1fr) auto;
      gap:10px 14px;
      align-items:center;
      cursor:pointer;
      list-style:none;
      padding:16px 18px;
      background:linear-gradient(135deg,#ffffff 0,#f8faff 100%);
    }
    .planning-section summary::-webkit-details-marker { display:none; }
    .planning-section summary::after {
      content:'펼치기';
      grid-column:2;
      grid-row:1;
      align-self:start;
      color:var(--accent);
      background:var(--accent-soft);
      border-radius:999px;
      padding:4px 9px;
      font-size:12px;
      font-weight:800;
      white-space:nowrap;
    }
    .planning-section[open] summary::after { content:'접기'; }
    .planning-section h3 {
      grid-column:1;
      margin:0;
      border:0;
      padding:0;
      font-size:23px;
      line-height:1.28;
    }
    .planning-section h3.compact-date-title { font-size:28px; letter-spacing:-.03em; }
    .section-preview {
      grid-column:1 / -1;
      margin:0;
      color:#667085;
      font-size:14px;
      line-height:1.55;
    }
    .section-content {
      padding:4px 20px 20px;
      border-top:1px solid var(--line);
    }
    .section-content > :first-child { margin-top:14px; }
    .mobile-library-toggle { display:none; }
    .hidden { display:none!important; }
    @media (max-width:1120px) {
      .shell { grid-template-columns:300px minmax(0,1fr); padding:16px; gap:16px; }
      .library { top:76px; height:calc(100vh - 92px); }
      .dashboard-head, .doc-head { padding:28px; }
      .dashboard-body, .doc-body { padding:28px; }
      .doc-reading { grid-template-columns:1fr; }
      .doc-side-toc {
        position:relative;
        top:auto;
        max-height:none;
        border-left:0;
        border-bottom:1px solid var(--line);
        padding:20px 28px;
      }
      .doc-side-toc nav { grid-template-columns:repeat(auto-fit,minmax(220px,1fr)); }
    }
    @media (max-width:820px) {
      .topbar {
        align-items:stretch;
        flex-direction:column;
        gap:12px;
        padding:12px;
      }
      .brand { align-items:flex-start; gap:10px; }
      .logo { width:32px; height:32px; border-radius:10px; flex:0 0 auto; }
      .brand h1 { font-size:16px; line-height:1.22; }
      .brand p { font-size:11px; line-height:1.35; }
      .top-actions {
        display:grid;
        grid-template-columns:repeat(2,minmax(0,1fr));
        width:100%;
        gap:7px;
      }
      .top-actions button, .top-actions .button {
        width:100%;
        justify-content:center;
        text-align:center;
        padding:9px 10px;
        box-shadow:none;
      }
      .mobile-library-toggle { display:inline-flex; }
      .shell { grid-template-columns:1fr; padding:12px; gap:12px; }
      .library {
        position:relative;
        top:auto;
        height:auto;
        max-height:72vh;
        display:none;
        border-radius:18px;
        padding:14px;
      }
      .library.open { display:block; }
      .reader { max-width:none; }
      .dashboard, .doc-view { border-radius:18px; }
      .dashboard-head, .doc-head, .dashboard-body, .doc-body { padding:18px; }
      .dashboard h2, .doc-head h2 { font-size:clamp(26px,8vw,36px); letter-spacing:-.045em; }
      .dashboard-head p, .doc-head p { font-size:14px; line-height:1.6; }
      .doc-body { font-size:15px; line-height:1.72; }
      .doc-body h2 { font-size:26px; margin:0 0 16px; }
      .doc-body h3 { font-size:21px; margin:30px 0 12px; }
      .doc-body h4 { font-size:18px; margin:24px 0 10px; }
      .doc-body pre { margin-left:-4px; margin-right:-4px; padding:12px; border-radius:12px; font-size:12px; line-height:1.55; }
      .doc-body code { word-break:break-word; }
      .doc-body .table-wrap { margin-left:-4px; margin-right:-4px; border-radius:12px; }
      .doc-body table { min-width:560px; font-size:13px; }
      .doc-body th, .doc-body td { padding:8px 9px; }
      .inline-toc nav { grid-template-columns:1fr; padding:0 12px 12px; }
      .inline-toc a { font-size:12px; }
      .doc-side-toc { display:none; }
      .grid.cards { grid-template-columns:1fr; }
      .planning-section { border-radius:14px; margin:14px 0; }
      .planning-section summary { grid-template-columns:1fr; padding:14px; }
      .planning-section summary::after { grid-column:1; grid-row:auto; width:max-content; }
      .planning-section h3,
      .planning-section h3.compact-date-title { font-size:21px; }
      .section-content { padding:2px 14px 16px; }
    }
    @media (max-width:420px) {
      .top-actions { grid-template-columns:1fr; }
      .shell { padding:10px; }
      .dashboard-head, .doc-head, .dashboard-body, .doc-body { padding:15px; }
      .doc-body { font-size:14px; }
      .doc-body table { min-width:500px; }
      .doc-body ul, .doc-body ol { padding-left:1.1rem; }
    }
  </style>
</head>
<body>
  <header class="topbar">
    <div class="brand">
      <div class="logo" aria-hidden="true"></div>
      <div>
        <h1>너에게 닿는 마지막 여름 통합 기획서</h1>
        <p>문서 라이브러리에서 하나씩 골라 읽는 웹형 기획서</p>
      </div>
    </div>
    <div class="top-actions">
      <button id="mobileLibraryToggle" class="mobile-library-toggle">문서 목록</button>
      <button id="homeBtn">홈</button>
      <a class="button" href="기획/README.md">원본 안내</a>
    </div>
  </header>

  <div class="shell">
    <aside class="library" id="library">
      <div class="library-head">
        <h2>문서 라이브러리</h2>
        <p>검색하거나 카테고리를 골라 필요한 기획서를 연다.</p>
      </div>
      <div class="search-box">
        <input id="search" type="search" placeholder="검색: 하연, 플래그, 하루 행동…" />
        <div class="chips" id="chips">
          <button class="active" data-filter="all">전체</button>
          <button data-filter="기획">기획</button>
          <button data-filter="스토리">스토리</button>
          <button data-filter="시스템">시스템</button>
        </div>
        <div id="result" class="result"></div>
      </div>
      <div id="docList" class="doc-list"></div>
    </aside>

    <main class="reader" id="content"></main>
  </div>

  <script id="doc-data" type="application/json">__DATA__</script>
  <script>
    const docs = JSON.parse(document.getElementById('doc-data').textContent);
    const content = document.getElementById('content');
    const docList = document.getElementById('docList');
    const result = document.getElementById('result');
    const search = document.getElementById('search');
    const buttons = [...document.querySelectorAll('[data-filter]')];
    const library = document.getElementById('library');
    let filter = 'all';
    let currentId = null;

    const categories = docs.reduce((acc, doc) => {
      acc[doc.category] = (acc[doc.category] || 0) + 1;
      return acc;
    }, {});

    function norm(value) { return (value || '').toLowerCase().trim(); }
    function getFilteredDocs() {
      const q = norm(search.value);
      return docs.filter(doc => {
        const categoryMatch = filter === 'all' || doc.category === filter;
        const haystack = norm([doc.title, doc.path, doc.plain].join(' '));
        return categoryMatch && (!q || haystack.includes(q));
      });
    }
    function renderList() {
      const filtered = getFilteredDocs();
      result.textContent = filtered.length + '개 문서';
      docList.innerHTML = filtered.map(doc => `
        <button class="doc-button ${doc.id === currentId ? 'active' : ''}" data-doc="${doc.id}">
          <span class="badge">${doc.category}</span>
          <strong>${doc.title}</strong>
          <small>${doc.path}</small>
        </button>
      `).join('') || '<p class="result">검색 결과가 없습니다.</p>';
      docList.querySelectorAll('[data-doc]').forEach(btn => {
        btn.addEventListener('click', () => openDoc(btn.dataset.doc));
      });
    }
    function escapeHtml(value) {
      return String(value || '').replace(/[&<>"']/g, ch => ({
        '&': '&amp;',
        '<': '&lt;',
        '>': '&gt;',
        '"': '&quot;',
        "'": '&#39;'
      }[ch]));
    }
    function tocHtml(doc) {
      const headings = (doc.headings || []).filter(h => h.level > 1).slice(0, 120);
      if (!headings.length) return '';
      return `
        <details class="inline-toc" open>
          <summary>이 문서 목차</summary>
          <nav>${headings.map(h => `<a class="lv${h.level}" href="#${h.id}" title="${escapeHtml(h.text)}">${escapeHtml(headingLabel(doc, h.text))}</a>`).join('')}</nav>
        </details>
      `;
    }
    function sideTocHtml(doc) {
      const headings = (doc.headings || []).filter(h => h.level > 1).slice(0, 160);
      if (!headings.length) return '';
      return `
        <aside class="doc-side-toc" aria-label="현재 문서 목차">
          <strong>현재 문서 목차</strong>
          <nav>${headings.map(h => `<a class="lv${h.level}" href="#${h.id}" title="${escapeHtml(h.text)}">${escapeHtml(headingLabel(doc, h.text))}</a>`).join('')}</nav>
        </aside>
      `;
    }
    function isCalendarDoc(doc) {
      return doc?.path === '스토리/06_날짜별_진행_달력.md' || doc?.title === '날짜별 진행 달력';
    }
    function isContinuousReadingDoc(doc) {
      return doc?.path === '스토리/00_전체_스토리.md'
        || doc?.path === '스토리/07_전체_스토리_외전_실패루프.md'
        || doc?.title === '전체 스토리'
        || doc?.title === '전체 스토리 외전: 실패 루프와 새드엔딩';
    }
    function dateHeadingLabel(text) {
      const match = (text || '').match(/^(D(?:-\d+|\+\d+|-Day))\b/);
      return match ? match[1] : '';
    }
    function headingLabel(doc, text) {
      return isCalendarDoc(doc) ? (dateHeadingLabel(text) || text) : text;
    }
    function previewText(node) {
      const text = (node?.textContent || '').replace(/\s+/g, ' ').trim();
      if (!text) return '';
      return text.length > 140 ? text.slice(0, 140) + '…' : text;
    }
    function enhanceDocBody(doc) {
      const body = content.querySelector('.doc-body');
      if (!body) return false;
      if (isContinuousReadingDoc(doc)) return false;
      const headings = [...body.querySelectorAll('h3')];
      if (!headings.length) return false;
      headings.forEach(heading => {
        const originalHeadingText = heading.textContent.trim();
        const compactHeadingText = headingLabel(doc, originalHeadingText);
        if (compactHeadingText !== originalHeadingText) {
          heading.dataset.fullTitle = originalHeadingText;
          heading.title = originalHeadingText;
          heading.textContent = compactHeadingText;
          heading.classList.add('compact-date-title');
        }

        const section = document.createElement('details');
        section.className = 'planning-section';
        section.open = true;

        const summary = document.createElement('summary');
        const sectionContent = document.createElement('div');
        sectionContent.className = 'section-content';

        heading.parentNode.insertBefore(section, heading);
        section.appendChild(summary);
        section.appendChild(sectionContent);
        summary.appendChild(heading);

        while (section.nextSibling) {
          const node = section.nextSibling;
          if (node.nodeType === Node.ELEMENT_NODE && ['H2', 'H3'].includes(node.tagName)) break;
          sectionContent.appendChild(node);
        }

        const source = sectionContent.querySelector('p, li, blockquote');
        const preview = previewText(source);
        if (preview) {
          const previewEl = document.createElement('p');
          previewEl.className = 'section-preview';
          previewEl.textContent = preview;
          summary.appendChild(previewEl);
        }
      });
      body.classList.add('sectioned');
      return true;
    }
    function setAllSections(open) {
      content.querySelectorAll('.planning-section').forEach(section => {
        section.open = open;
      });
    }
    function revealHeading(hash) {
      const id = decodeURIComponent((hash || '').replace(/^#/, ''));
      if (!id) return false;
      const target = document.getElementById(id);
      if (!target) return false;
      const section = target.closest('details.planning-section');
      if (section) section.open = true;
      setTimeout(() => target.scrollIntoView({ behavior: 'smooth', block: 'start' }), 0);
      return true;
    }
    function normalizeDocPath(path) {
      const parts = [];
      String(path || '').split('/').forEach(part => {
        if (!part || part === '.') return;
        if (part === '..') parts.pop();
        else parts.push(part);
      });
      return parts.join('/');
    }
    function docFromMarkdownHref(href, currentDoc) {
      if (!href || !currentDoc) return null;
      if (/^[a-z][a-z0-9+.-]*:/i.test(href) || href.startsWith('#')) return null;
      const [rawPath, rawHash = ''] = href.split('#');
      if (!rawPath.toLowerCase().endsWith('.md')) return null;
      const decodedPath = decodeURIComponent(rawPath).replace(/\\/g, '/');
      const baseParts = (currentDoc.path || '').split('/');
      baseParts.pop();
      const base = baseParts.join('/');
      const normalized = normalizeDocPath((base ? base + '/' : '') + decodedPath);
      const doc = docs.find(d => d.path === normalized);
      return doc ? { doc, hash: rawHash ? decodeURIComponent(rawHash) : '' } : null;
    }
    function bindInternalMarkdownLinks(doc) {
      content.querySelectorAll('.doc-body a[href]').forEach(link => {
        const target = docFromMarkdownHref(link.getAttribute('href'), doc);
        if (!target) return;
        link.dataset.docLink = target.doc.id;
        link.title = `통합 기획서에서 ${target.doc.title} 열기`;
      });
    }
    function openDoc(id, headingHash = '') {
      const doc = docs.find(d => d.id === id) || docs[0];
      currentId = doc.id;
      content.innerHTML = `
        <article class="doc-view">
          <header class="doc-head">
            <span class="badge">${doc.category}</span>
            <h2>${doc.title}</h2>
            <p class="path">${doc.path}</p>
            <div class="doc-tools">
              <button id="copyPath">경로 복사</button>
              <button id="expandSections">모두 펼치기</button>
              <button id="collapseSections">모두 접기</button>
              <button id="backHome">홈으로</button>
            </div>
            ${tocHtml(doc)}
          </header>
          <div class="doc-reading">
            <div class="doc-body">${doc.html}</div>
            ${sideTocHtml(doc)}
          </div>
        </article>
      `;
      const hasSections = enhanceDocBody(doc);
      bindInternalMarkdownLinks(doc);
      document.getElementById('copyPath').addEventListener('click', async () => {
        try { await navigator.clipboard.writeText(doc.path); } catch (_) {}
      });
      const expandSections = document.getElementById('expandSections');
      const collapseSections = document.getElementById('collapseSections');
      if (hasSections) {
        expandSections.addEventListener('click', () => setAllSections(true));
        collapseSections.addEventListener('click', () => setAllSections(false));
      } else {
        expandSections.classList.add('hidden');
        collapseSections.classList.add('hidden');
      }
      document.getElementById('backHome').addEventListener('click', renderHome);
      renderList();
      if (headingHash && revealHeading('#' + headingHash)) {
        history.replaceState(null, '', '#' + headingHash);
      } else {
        location.hash = doc.id;
        window.scrollTo({ top: 0, behavior: 'smooth' });
      }
      if (window.innerWidth <= 820) library.classList.remove('open');
    }
    function quickCard(label, title, desc) {
      const doc = docs.find(d => d.title === title);
      if (!doc) return '';
      return `<div class="quick-card"><h3>${label}</h3><p>${desc}</p><button data-quick="${doc.id}">${title} 열기</button></div>`;
    }
    function renderHome() {
      currentId = null;
      content.innerHTML = `
        <section class="dashboard" id="home">
          <header class="dashboard-head">
            <span class="badge">Overview</span>
            <h2>문서 하나씩 빠르게 읽는 통합 기획서</h2>
            <p>왼쪽 문서 라이브러리에서 필요한 기획서를 고르면 이 영역에 해당 문서만 표시됩니다. 목차는 문서 상단의 접이식 목차로 확인합니다.</p>
          </header>
          <div class="dashboard-body">
            <div class="grid cards">
              <div class="stat"><b>${docs.length}</b>전체 문서</div>
              <div class="stat"><b>${categories['스토리'] || 0}</b>스토리 문서</div>
              <div class="stat"><b>${categories['시스템'] || 0}</b>시스템 문서</div>
              <div class="stat"><b>2패널</b>라이브러리 + 리더</div>
            </div>
            <h3>빠른 시작</h3>
            <div class="grid cards">
              ${quickCard('전체 스토리 보기', '전체 스토리', '첫 월요일부터 진엔딩 후 방학식까지 확정된 본편 흐름을 소설형으로 읽습니다.')}
              ${quickCard('실패 외전 보기', '전체 스토리 외전: 실패 루프와 새드엔딩', '하연 실패 루프와 새드엔딩을 짧고 선명한 외전 장면으로 확인합니다.')}
              ${quickCard('날짜 흐름 보기', '날짜별 진행 달력', 'D-32부터 축제 마지막 날, D+3 방학식까지의 전체 진행을 확인합니다.')}
              ${quickCard('핵심 시스템 보기', '미연시 코어 시스템 요구사항', '날짜, 회차, 플래그, 조건, 엔딩 기록 같은 런타임 상태를 확인합니다.')}
              ${quickCard('하루 행동 보기', '하루 행동 시스템 기획', '평일/주말 슬롯과 선택 행동 구조를 확인합니다.')}
              ${quickCard('캐릭터 관계 보기', '캐릭터와 관계', '재윤, 하연, 소하, 서린, 미루의 관계 축을 확인합니다.')}
            </div>
          </div>
        </section>
      `;
      renderList();
      location.hash = 'home';
    }

    content.addEventListener('click', event => {
      const quick = event.target.closest('[data-quick]');
      if (quick) {
        event.preventDefault();
        openDoc(quick.dataset.quick);
        return;
      }
      const markdownLink = event.target.closest('.doc-body a[href]');
      if (markdownLink) {
        const currentDoc = docs.find(d => d.id === currentId);
        const target = docFromMarkdownHref(markdownLink.getAttribute('href'), currentDoc);
        if (target) {
          event.preventDefault();
          openDoc(target.doc.id, target.hash);
          return;
        }
      }
      const tocLink = event.target.closest('.inline-toc a, .doc-side-toc a');
      if (tocLink && tocLink.hash) {
        event.preventDefault();
        revealHeading(tocLink.hash);
        history.replaceState(null, '', tocLink.hash);
      }
    });
    search.addEventListener('input', renderList);
    buttons.forEach(btn => btn.addEventListener('click', () => {
      buttons.forEach(b => b.classList.remove('active'));
      btn.classList.add('active');
      filter = btn.dataset.filter;
      renderList();
    }));
    document.getElementById('homeBtn').addEventListener('click', renderHome);
    document.getElementById('mobileLibraryToggle').addEventListener('click', () => library.classList.toggle('open'));

    const initial = decodeURIComponent(location.hash.replace(/^#/, ''));
    const initialDoc = docs.find(d => d.id === initial);
    const initialHeadingDoc = docs.find(d => (d.headings || []).some(h => h.id === initial));
    if (initial && initial !== 'home' && initialDoc) {
      openDoc(initialDoc.id);
    } else if (initial && initialHeadingDoc) {
      openDoc(initialHeadingDoc.id);
      revealHeading('#' + initial);
      history.replaceState(null, '', '#' + initial);
    } else {
      renderHome();
    }
  </script>
</body>
</html>
'''
    return html_text.replace('__DATA__', data_json)

def main() -> None:
    docs = collect_docs()
    html_text = build_html(docs)
    OUTPUT.write_text(html_text, encoding='utf-8')
    LEGACY_OUTPUT.write_text(html_text, encoding='utf-8')
    print(
        f'generated {OUTPUT.relative_to(PROJECT_ROOT)} '
        f'and {LEGACY_OUTPUT.relative_to(PROJECT_ROOT)} '
        f'from {len(docs)} markdown files'
    )


if __name__ == '__main__':
    main()
