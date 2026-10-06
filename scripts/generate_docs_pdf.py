#!/usr/bin/env python3
"""
generate_docs_pdf.py
Converts Markdown technical documentation with embedded Mermaid diagrams and tables
into publication-grade, styled PDF documents using Chrome headless rendering.
"""

import markdown
import re
import subprocess
import os
import html
import sys

def md_to_pdf(md_path, pdf_path, doc_title, subtitle):
    if not os.path.exists(md_path):
        print(f"Error: {md_path} not found.")
        return False

    with open(md_path, "r", encoding="utf-8") as f:
        md_text = f.read()

    raw_html = markdown.markdown(md_text, extensions=["tables", "fenced_code"])
    
    def clean_mermaid(m):
        raw_code = html.unescape(m.group(1))
        return f"<div class=\"mermaid\">\n{raw_code}\n</div>"
        
    processed_html = re.sub(
        r"<pre><code class=\"(?:language-)?mermaid\">(.*?)</code></pre>",
        clean_mermaid,
        raw_html,
        flags=re.DOTALL
    )

    css = """
    @page {
        size: A4;
        margin: 16mm 14mm 16mm 14mm;
    }
    body {
        font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
        color: #1e293b;
        line-height: 1.5;
        font-size: 9.5pt;
        background: #ffffff;
    }
    .header-banner {
        border-bottom: 3px solid #2563eb;
        padding-bottom: 12px;
        margin-bottom: 18pt;
    }
    .header-banner h1 {
        margin: 0 0 4px 0;
        font-size: 20pt;
        color: #0f172a;
        font-weight: 700;
        border: none;
        padding: 0;
    }
    .header-banner .subtitle {
        color: #475569;
        font-size: 11pt;
        font-weight: 500;
    }
    .badge {
        display: inline-block;
        padding: 2px 8px;
        background: #e0e7ff;
        color: #3730a3;
        border-radius: 4px;
        font-size: 8.5pt;
        font-weight: 600;
        margin-top: 4px;
    }
    h1 {
        font-size: 16pt;
        color: #0f172a;
        border-bottom: 2px solid #2563eb;
        padding-bottom: 4px;
        margin-top: 18pt;
        margin-bottom: 8pt;
        page-break-after: avoid;
    }
    h2 {
        font-size: 13pt;
        color: #1e3a8a;
        border-bottom: 1px solid #cbd5e1;
        padding-bottom: 3px;
        margin-top: 14pt;
        margin-bottom: 6pt;
        page-break-after: avoid;
    }
    h3 {
        font-size: 10.5pt;
        color: #1d4ed8;
        margin-top: 10pt;
        margin-bottom: 4pt;
        page-break-after: avoid;
    }
    table {
        width: 100%;
        border-collapse: collapse;
        margin: 10pt 0;
        font-size: 8pt;
        page-break-inside: avoid;
    }
    th {
        background-color: #0f172a;
        color: #ffffff;
        font-weight: 600;
        text-align: left;
        padding: 6px 7px;
        border: 1px solid #0f172a;
    }
    td {
        padding: 5px 7px;
        border: 1px solid #cbd5e1;
        vertical-align: top;
    }
    tr:nth-child(even) {
        background-color: #f8fafc;
    }
    pre {
        background-color: #0f172a;
        color: #f8fafc;
        padding: 10px;
        border-radius: 5px;
        font-size: 8pt;
        page-break-inside: avoid;
        line-height: 1.4;
        overflow-x: auto;
    }
    code {
        font-family: "SFMono-Regular", Consolas, Menlo, monospace;
        font-size: 8.5pt;
    }
    p > code, td > code, li > code {
        background-color: #f1f5f9;
        color: #0f172a;
        padding: 1px 4px;
        border-radius: 3px;
        border: 1px solid #e2e8f0;
    }
    .mermaid {
        text-align: center;
        margin: 12pt auto;
        padding: 10pt;
        background-color: #fcfcfc;
        border: 1px solid #e2e8f0;
        border-radius: 6px;
        page-break-inside: avoid;
        overflow: hidden;
    }
    .mermaid svg {
        max-width: 95% !important;
        height: auto !important;
        max-height: 380px !important;
    }
    blockquote {
        border-left: 4px solid #2563eb;
        margin: 10pt 0;
        padding: 6pt 12pt;
        background-color: #eff6ff;
        color: #1e3a8a;
        border-radius: 0 4px 4px 0;
    }
    ul, ol {
        margin-top: 4pt;
        margin-bottom: 8pt;
        padding-left: 18pt;
    }
    li {
        margin-bottom: 2pt;
    }
    """

    full_html = f"""<!DOCTYPE html>
    <html lang="en">
    <head>
    <meta charset="utf-8">
    <title>{doc_title}</title>
    <script src="https://cdn.jsdelivr.net/npm/mermaid@10/dist/mermaid.min.js"></script>
    <script>
    mermaid.initialize({{
        startOnLoad: true,
        theme: "neutral",
        flowchart: {{ useMaxWidth: true, htmlLabels: true }},
        sequence: {{ useMaxWidth: true, showSequenceNumbers: true, actorMargin: 25 }}
    }});
    </script>
    <style>{css}</style>
    </head>
    <body>
    <div class="header-banner">
        <h1>{doc_title}</h1>
        <div class="subtitle">{subtitle}</div>
        <span class="badge">UE24CS341A — Software Engineering</span>
        <span class="badge" style="background:#dcfce7; color:#166534;">Release 1.0 (Sprint 1)</span>
    </div>
    {processed_html}
    </body>
    </html>
    """

    temp_html = pdf_path + ".tmp.html"
    with open(temp_html, "w", encoding="utf-8") as f:
        f.write(full_html)

    chrome_cmd = [
        "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome",
        "--headless",
        "--disable-gpu",
        "--no-pdf-header-footer",
        "--run-all-compositor-stages-before-draw",
        "--virtual-time-budget=6000",
        f"--print-to-pdf={pdf_path}",
        temp_html
    ]
    subprocess.run(chrome_cmd, check=True)
    if os.path.exists(temp_html):
        os.remove(temp_html)
    print(f"Generated: {pdf_path} ({os.path.getsize(pdf_path):,} bytes)")
    return True

if __name__ == "__main__":
    docs = [
        (
            "docs/srs/srs-v0.1.md",
            "docs/srs/srs-v0.1.pdf",
            "Software Requirements Specification (SRS)",
            "Jackfruit — Basic ZIP-Style File Compression Tool (Huffman Coding)"
        ),
        (
            "docs/design/architecture-v0.1.md",
            "docs/design/architecture-v0.1.pdf",
            "Software Architecture and Design Specification (SAD)",
            "Jackfruit — Basic ZIP-Style File Compression Tool (Huffman Coding)"
        ),
        (
            "docs/design/archive-format.md",
            "docs/design/archive-format.pdf",
            "Jackfruit Archive Binary Format Specification (.jack)",
            "Binary Layout, Endianness, Metadata Dictionary & Bitstream Specification"
        ),
    ]

    for md, pdf, title, sub in docs:
        md_to_pdf(md, pdf, title, sub)
