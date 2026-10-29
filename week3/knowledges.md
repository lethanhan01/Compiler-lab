---
title: "Experiment in Compiler Construction"
subtitle: "Phân tích Ngữ nghĩa (Phần 1) — Thiết kế và Xây dựng Bảng Ký hiệu (Symbol Table)"
author:
  - "Tài liệu do AI biên soạn dựa trên Slides bài giảng của TS. Nguyễn Hữu Đức"
  - "Bộ môn Hệ thống thông tin — Khoa Công nghệ Thông tin — Đại học Bách khoa Hà Nội"
date: "Học phần: Chương trình dịch (IT3323) — Tuần thực hành số 3"
titlepage: true
titlepage-color: "1E4D4E"
titlepage-text-color: "FFFFFF"
titlepage-rule-color: "D4DFB8"
titlepage-rule-height: 4
toc: true
toc-own-page: true
toc-title: "Mục lục"
numbersections: true
geometry:
  - a4paper
  - margin=1.8cm
  - includehead=true
  - includefoot=true
mainfont: "Latin Modern Roman"
sansfont: "Latin Modern Sans"
monofont: "Latin Modern Mono"
fontsize: 11pt
linestretch: 1.15
colorlinks: true
linkcolor: "1E4D4E"
urlcolor: "0B5394"
toccolor: "1E4D4E"
header-left: "IT3323 — Phân tích ngữ nghĩa (1): Bảng ký hiệu"
header-right: "Dựa trên Slides của TS. Nguyễn Hữu Đức"
footer-left: "AI biên soạn dựa trên bài giảng của TS. Nguyễn Hữu Đức (ĐHBK Hà Nội)"
footer-right: "Trang \\thepage"
code-block-font-size: \footnotesize
table-use-row-colors: true
header-includes:
  - \usepackage{graphicx}
  - \usepackage{booktabs}
  - \usepackage{tikz}
  - \usetikzlibrary{shapes.geometric, shapes.multipart, arrows.meta, positioning, calc, fit, backgrounds, matrix, decorations.pathreplacing}
  - \usepackage[most]{tcolorbox}
  - |
    \definecolor{kplteal}{HTML}{1E4D4E}
    \definecolor{kplsage}{HTML}{D4DFB8}
    \definecolor{kplred}{HTML}{C0392B}
    \definecolor{kplblue}{HTML}{2980B9}
    \definecolor{kplgold}{HTML}{F39C12}
    \definecolor{kplgray}{HTML}{F8F9F9}
    \newcommand{\StartSideBySide}[1]{\par\medskip\noindent\begin{minipage}[t]{#1\textwidth}\vspace{0pt}}
    \newcommand{\MidSideBySide}[1]{\end{minipage}\hfill\begin{minipage}[t]{#1\textwidth}\vspace{0pt}}
    \newcommand{\EndSideBySide}{\end{minipage}\par\medskip}
    \newtcolorbox{attributionbox}[1][]{
      enhanced,
      colback=kplteal!6!white,
      colframe=kplteal,
      boxrule=1.2pt,
      arc=3mm,
      left=4mm, right=4mm, top=3mm, bottom=3mm,
      fonttitle=\bfseries\large,
      title={#1}
    }
    \newtcolorbox{notebox}[1][]{
      enhanced,
      colback=kplblue!5!white,
      colframe=kplblue!85!black,
      boxrule=0.9pt,
      arc=2mm,
      left=3.5mm, right=3.5mm, top=2.5mm, bottom=2.5mm,
      fonttitle=\bfseries,
      title={#1}
    }
    \newtcolorbox{warnbox}[1][]{
      enhanced,
      colback=kplred!6!white,
      colframe=kplred!85!black,
      boxrule=1pt,
      arc=2mm,
      left=3.5mm, right=3.5mm, top=2.5mm, bottom=2.5mm,
      fonttitle=\bfseries,
      title={#1}
    }
---

\begin{attributionbox}[Thông tin Nguồn gốc và Ghi công Tài liệu (Document Attribution)]
\begin{itemize}
\item \textbf{Đơn vị biên soạn tài liệu:} Tài liệu hướng dẫn chi tiết, hệ thống sơ đồ trực quan \textbf{TikZ} và lời giải mẫu bằng ngôn ngữ C dưới đây được \textbf{AI (Google DeepMind Antigravity)} tổng hợp và biên soạn.
\item \textbf{Tài liệu gốc làm cơ sở:} Biên soạn trực tiếp dựa trên bộ Slides bài giảng \textbf{``Experiment in Compiler Construction -- Phân tích ngữ nghĩa (1)''} (\texttt{l4-semantics-analysis-1.pdf}, gồm 18 trang) và bộ mã nguồn khung trình biên dịch ngôn ngữ \textbf{KPL}.
\item \textbf{Tác giả bài giảng gốc và mã nguồn khung:} \textbf{TS. Nguyễn Hữu Đức} --- \emph{Bộ môn Hệ thống thông tin, Khoa Công nghệ Thông tin, Trường Đại học Bách khoa Hà Nội (Hanoi University of Technology -- HEDSPI)}.
\item \textbf{Cấu trúc trình bày:} Tài liệu được định dạng bằng \textbf{Pandoc} với giao diện \textbf{Eisvogel} và họ phông chữ chuẩn học thuật \textbf{Computer Modern (Latin Modern)}. Toàn bộ các khái niệm cốt lõi đều đặt \textbf{mã nguồn song song bên cạnh sơ đồ TikZ tương ứng} để người đọc dễ dàng đối chiếu.
\end{itemize}
\end{attributionbox}

---

# Tổng quan về Phân tích Ngữ nghĩa (Semantic Analysis)

_Nội dung chương này được biên soạn dựa trên **Slide 2, Slide 3 và Slide 4** trong bài giảng của **TS. Nguyễn Hữu Đức**._

## Vị trí và vai trò của Phân tích Ngữ nghĩa trong Trình biên dịch

Trong kiến trúc kinh điển của một trình biên dịch (Compiler), quá trình dịch một chương trình nguồn sang mã máy đích đi qua bốn pha nối tiếp nhau:

1. **Phân tích từ vựng (Lexical Analysis / Scanner):** Đọc dòng ký tự nguồn và gom nhóm chúng thành các đơn vị từ vựng hợp lệ gọi là _Token_ (từ khóa, định danh, hằng số, toán tử...).
2. **Phân tích cú pháp (Syntax Analysis / Parser):** Kiểm tra xem chuỗi các Token có tuân theo các quy tắc cấu trúc ngữ pháp phi ngữ cảnh (_Context-Free Grammar — CFG_) của ngôn ngữ lập trình hay không.
3. **Phân tích ngữ nghĩa (Semantic Analysis):** Kiểm tra tính hợp lệ về mặt **ý nghĩa logic và ngữ cảnh** của chương trình dựa trên các khai báo định danh và hệ thống kiểu dữ liệu.
4. **Sinh mã (Code Generation):** Chuyển đổi chương trình đã được kiểm chứng sang mã máy ảo hoặc hợp ngữ đích.

### Tại sao chỉ Phân tích Cú pháp là chưa đủ?

Bộ phân tích cú pháp (như chúng ta đã xây dựng ở Tuần 2) chỉ quan tâm đến **hình thức câu lệnh** (ví dụ: một phép gán có dạng `Biến := Biểu_thức;`, một lời gọi hàm có dạng `Tên_hàm(Danh_sách_đối_số)`). Tuy nhiên, văn phạm phi ngữ cảnh hoàn toàn **không thể trả lời** các câu hỏi phụ thuộc vào ngữ cảnh khai báo trước đó (Slide 3):

- **`"x"` là tên một biến hay một hàm?** (Nếu `"x"` là tên một thủ tục, ta không thể viết `x := 10;` dù câu lệnh đó đúng cú pháp phép gán!).
- **`"x"` đã được định nghĩa (khai báo) trước khi sử dụng hay chưa?**
- **`"x"` được định nghĩa ở đâu?** (Nằm ở phạm vi toàn cục của chương trình chính hay phạm vi cục bộ của một hàm con? Nếu có nhiều định danh cùng tên `"x"` ở các phạm vi lồng nhau thì câu lệnh hiện tại đang tham chiếu tới `"x"` nào?).
- **Biểu thức `"a + b"` có kiểu dữ liệu nhất quán không?** (Ví dụ: `"a"` là kiểu số nguyên `INTEGER` còn `"b"` lại là một mảng `ARRAY` hoặc hằng ký tự `CHAR` thì phép cộng có hợp lệ không?).

Bộ **Phân tích ngữ nghĩa** ra đời chính là để trả lời toàn bộ các câu hỏi trên, làm sáng tỏ ngữ nghĩa của chương trình và chặn đứng các lỗi logic tĩnh trước khi bước sang pha sinh mã.

## Đối chiếu Mã nguồn Lỗi Ngữ nghĩa và Sơ đồ Kiến trúc (Slide 3)

Để thấy rõ ranh giới giữa **Cú pháp (Syntax)** và **Ngữ nghĩa (Semantics)**, hãy quan sát đoạn chương trình KPL bên trái (vượt qua hoàn toàn bộ Parser ở Tuần 2 nhưng chứa 4 lỗi ngữ nghĩa nghiêm trọng) đặt cạnh sơ đồ kiến trúc bộ phân tích ngữ nghĩa ở bên phải:

\StartSideBySide{0.48}

```pascal
PROGRAM DemoSemanticErrors;
CONST c = 100;
TYPE  ArrType = ARRAY(. 5 .) OF INTEGER;
VAR   a : INTEGER;
      b : ArrType;

FUNCTION f(x : INTEGER) : INTEGER;
BEGIN
  f := x + 1;
END;

BEGIN
  (* Lỗi 1: 'z' chưa hề được khai báo! *)
  z := 10;

  (* Lỗi 2: 'c' là hằng số, không thể
     đứng ở vế trái của phép gán! *)
  c := 200;

  (* Lỗi 3: Biểu thức 'a + b' không nhất
     quán kiểu (INTEGER + ARRAY)! *)
  a := a + b;

  (* Lỗi 4: Hàm 'f' cần 1 tham số nhưng
     lời gọi hàm lại truyền 2 đối số! *)
  a := f(10, 20);
END.
```

\MidSideBySide{0.50}

\begin{center}
\resizebox{\linewidth}{!}{%
\begin{tikzpicture}[
node distance=0.65cm,
phase/.style={rectangle, rounded corners=3pt, minimum width=3.4cm, minimum height=0.95cm, align=center, font=\sffamily\bfseries\small, fill=kplteal!35, draw=kplteal!80, line width=0.8pt, text=black},
activephase/.style={rectangle, rounded corners=3pt, minimum width=3.4cm, minimum height=0.95cm, align=center, font=\sffamily\bfseries\small, fill=kplred!90, draw=kplred!60!black, line width=1.2pt, text=white},
qbox/.style={rectangle, rounded corners=3pt, draw=kplteal, fill=kplgray, text width=4.6cm, inner sep=5pt, font=\footnotesize},
arr/.style={-Stealth, line width=1.2pt, draw=kplteal!70},
qarr/.style={-Stealth, thick, dashed, draw=kplred!80!black}
]
\node[phase] (lex) {Lexical Analysis\\{\scriptsize (Phân tích từ vựng)}};
\node[phase, below=of lex] (syn) {Syntax Analysis\\{\scriptsize (Phân tích cú pháp -- CFG)}};
\node[activephase, below=of syn] (sem) {Semantic Analysis\\{\scriptsize (Phân tích ngữ nghĩa)}};
\node[phase, below=of sem] (codegen) {Code Generation\\{\scriptsize (Sinh mã đích)}};

\draw[arr] (lex) -- (syn);
\draw[arr] (syn) -- node[right, font=\scriptsize\itshape, text=kplteal!90!black] {Đúng cú pháp!} (sem);
\draw[arr] (sem) -- node[right, font=\scriptsize\itshape, text=kplred!85!black] {Đã kiểm tra kiểu \& phạm vi} (codegen);

\node[qbox, right=0.55cm of sem, yshift=0.3cm] (questions) {
\textbf{\textcolor{kplred!85!black}{Các câu hỏi ngữ nghĩa (Slide 3):}}\\[2pt]
$\bullet$ \texttt{"x"} là biến, hằng hay hàm?\\
$\bullet$ \texttt{"x"} đã được định nghĩa chưa?\\
$\bullet$ \texttt{"x"} được định nghĩa ở đâu?\\
$\bullet$ \texttt{"a + b"} có nhất quán kiểu?\\
$\bullet$ Tham số truyền vào \texttt{f(...)} có khớp khai báo không?
};

\draw[qarr] (sem.east) -- (questions.west);
\end{tikzpicture}%
}
\end{center}

\EndSideBySide

## Hai nhiệm vụ cốt lõi của Bộ Phân tích Ngữ nghĩa (Slide 4)

Theo **Slide 4** của TS. Nguyễn Hữu Đức, một bộ phân tích ngữ nghĩa trong trình biên dịch KPL đảm nhận hai nhóm nhiệm vụ chính có quan hệ mật thiết với nhau:

1. **Quản lý thông tin về các định danh (Identifier Management):**
   Thu thập, phân loại và lưu trữ thông tin chi tiết vào **Bảng ký hiệu (Symbol Table)** mỗi khi trình biên dịch duyệt qua phần khai báo của:
   - **Hằng (`Constant`):** Tên hằng, kiểu dữ liệu (`INT` hoặc `CHAR`), và giá trị cụ thể.
   - **Biến (`Variable`):** Tên biến, kiểu dữ liệu (nguyên, ký tự, hoặc mảng nhiều chiều), và phạm vi chứa biến.
   - **Kiểu người dùng định nghĩa (`User-defined Type`):** Tên kiểu mới (alias) và cấu trúc kiểu thực tế (`actualType`).
   - **Chương trình con (`Subprograms` — Hàm `Function` và Thủ tục `Procedure`):** Tên chương trình con, danh sách các tham số hình thức, kiểu trả về (đối với hàm), và phạm vi cục bộ (`scope`) của chương trình con đó.
2. **Kiểm tra một số luật ngữ nghĩa (Semantic Rule Checking):**
   Khi duyệt tới thân chương trình (`BEGIN ... END`), bộ phân tích ngữ nghĩa tra cứu Bảng ký hiệu để kiểm tra:
   - **Phạm vi định danh (Scope Rules):** Kiểm tra lỗi khai báo trùng tên trong cùng một block, lỗi sử dụng định danh chưa khai báo, và xác định đúng đối tượng gần nhất theo luật phạm vi lồng nhau.
   - **Nhất quán kiểu (Type Consistency / Type Checking):** Kiểm tra sự tương thích kiểu giữa hai vế của phép gán, giữa các toán hạng trong biểu thức số học/so sánh, chỉ số truy cập mảng, và đối số thực tế truyền vào lời gọi hàm/thủ tục.

\begin{center}
\resizebox{0.92\linewidth}{!}{%
\begin{tikzpicture}[
root/.style={rectangle, rounded corners=4pt, fill=kplteal, text=white, font=\bfseries\small, inner sep=7pt, align=center},
branch/.style={rectangle, rounded corners=3pt, fill=kplteal!15, draw=kplteal, line width=0.9pt, font=\bfseries\small, inner sep=6pt, align=center, minimum width=5.5cm},
leaf/.style={rectangle, rounded corners=2pt, fill=white, draw=kplteal!70, font=\footnotesize, inner sep=4.5pt, align=left, text width=5.2cm},
edge/.style={-Stealth, thick, draw=kplteal}
]
\node[root] (main) {NHIỆM VỤ CỦA BỘ PHÂN TÍCH NGỮ NGHĨA (SLIDE 4)};

\node[branch, below left=0.9cm and 0.4cm of main] (b1) {1. Quản lý thông tin các định danh\\{\scriptsize (Xây dựng Bảng ký hiệu -- Tuần 3)}};
\node[branch, below right=0.9cm and 0.4cm of main] (b2) {2. Kiểm tra các luật ngữ nghĩa\\{\scriptsize (Tra cứu \& Kiểm tra kiểu -- Tuần 4)}};

\draw[edge] (main.south) -- ++(0,-0.35) -| (b1.north);
\draw[edge] (main.south) -- ++(0,-0.35) -| (b2.north);

\node[leaf, below=0.45cm of b1] (l1) {
$\$ \textbf{Hằng (Constant):} \texttt{CONST c = 100;}\\
$\$ \textbf{Biến (Variable):} \texttt{VAR v : Integer;}\\
$\$ \textbf{Kiểu tự định nghĩa (Type):} \texttt{TYPE t = ...;}\\
$\$ \textbf{Chương trình con:} \texttt{FUNCTION}, \texttt{PROCEDURE}
};

\node[leaf, below=0.45cm of b2] (l2) {
$\$ \textbf{Phạm vi định danh (Scope Checking):}\\
\hspace*{0.8em}-- Khai báo duy nhất trong 1 block\\
\hspace*{0.8em}-- Tra cứu từ block trong ra block ngoài\\
$\$ \textbf{Nhất quán kiểu (Type Checking):}\\
\hspace\*{0.8em}-- Tương thích kiểu trong phép gán, biểu thức, lời gọi hàm
};

\draw[edge] (b1) -- (l1);
\draw[edge] (b2) -- (l2);
\end{tikzpicture}%
}
\end{center}

---

# Bảng Ký hiệu (Symbol Table) và Cấu trúc Phân cấp

_Nội dung chương này được biên soạn dựa trên **Slide 5 và Slide 6** trong bài giảng của **TS. Nguyễn Hữu Đức**._

## Khái niệm Bảng ký hiệu và Bộ thuộc tính của các Định danh (Slide 5)

**Bảng ký hiệu (Symbol Table)** là cấu trúc dữ liệu trung tâm lưu trữ toàn bộ thông tin về các định danh xuất hiện trong chương trình nguồn cùng các thuộc tính ngữ nghĩa của chúng. Theo **Slide 5**, mỗi nhóm định danh trong ngôn ngữ KPL cần lưu trữ bộ thông tin cụ thể sau:

| Loại định danh                           | Bộ thuộc tính lưu trữ trên Bảng ký hiệu (Slide 5)                    | Ví dụ trong ngôn ngữ KPL                             |
| :--------------------------------------- | :------------------------------------------------------------------- | :--------------------------------------------------- |
| **1. Hằng (Constant)**                   | `{tên, kiểu, giá trị}`                                               | `CONST max = 100; ch = 'a';`                         |
| **2. Kiểu người dùng định nghĩa (Type)** | `{tên, kiểu thực tế}`                                                | `TYPE Arr10 = ARRAY(. 10 .) OF INTEGER;`             |
| **3. Biến (Variable)**                   | `{tên, kiểu}` _(kèm con trỏ phạm vi phục vụ sinh mã)_                | `VAR count : INTEGER; a : Arr10;`                    |
| **4. Hàm (Function)**                    | `{tên, các tham số hình thức, kiểu trả về, các khai báo địa phương}` | `FUNCTION f(x : t) : t; VAR y : t; ...`              |
| **5. Thủ tục (Procedure)**               | `{tên, các tham số hình thức, các khai báo địa phương}`              | `PROCEDURE p(VAR k : INTEGER); ...`                  |
| **6. Tham số hình thức (Parameter)**     | `{tên, kiểu, tham biến / tham trị}`                                  | `x : INTEGER` (tham trị), `VAR k : CHAR` (tham biến) |

## Biểu diễn Phân cấp Bảng Ký hiệu trong KPL — Ví dụ Chuẩn (Slide 6)

Ngôn ngữ KPL (tương tự ngôn ngữ Pascal) là ngôn ngữ có **cấu trúc khối lồng nhau (nested block structure)**: bên trong chương trình chính (`PROGRAM`) có thể khai báo các hàm (`FUNCTION`) và thủ tục (`PROCEDURE`); bên trong mỗi hàm/thủ tục lại có thể khai báo các hằng, kiểu, biến địa phương và thậm chí cả các chương trình con cấp sâu hơn.

Do đó, trong trình biên dịch KPL, bảng ký hiệu **không phải** là một bảng phẳng duy nhất mà được biểu diễn theo **cấu trúc cây phân cấp (Hierarchical Structure)**. Mỗi nút chương trình, hàm hoặc thủ tục sở hữu một không gian tên (phạm vi — `Scope`) riêng chứa danh sách các đối tượng khai báo bên trong nó.

Dưới đây là **Ví dụ chuẩn trên Slide 6** của TS. Nguyễn Hữu Đức với mã nguồn KPL đặt song song bên cạnh sơ đồ phân cấp Bảng ký hiệu tương ứng:

\StartSideBySide{0.46}

```pascal
PROGRAM test;
CONST c = 100;
TYPE t = Integer;
VAR v : t;
FUNCTION f(x : t) : t;
  VAR y : t;
BEGIN
  y := x + 1;
  f := y;
END;

BEGIN
  v := 1;
  WriteLn (f(v));
END.
```

**Giải thích ánh xạ:**

- Định danh kiểu `t` được phân giải thành kiểu cơ sở `INT`.
- Khi khai báo `v : t;`, kiểu của biến `v` trong bảng ký hiệu được lưu trực tiếp là bản sao của kiểu thực tế `INT`.
- Hàm `f` có tham số `x : INT` và trả về `INT` (ký hiệu `f : FN : INT -> INT`), đồng thời mở ra một phạm vi con chứa tham số `x` và biến địa phương `y`.

\MidSideBySide{0.51}

\begin{center}
\resizebox{\linewidth}{!}{%
\begin{tikzpicture}[
objbox/.style={rectangle, rounded corners=3pt, draw=kplblue!80!black, fill=yellow!55!white, line width=0.9pt, minimum width=3.4cm, minimum height=0.68cm, font=\sffamily\small\bfseries, align=center},
line/.style={draw=black!80, line width=0.9pt}
]
% Root Program
\node[objbox] (prg) at (0, 0) {test : PRG};

% Level 1 objects inside test
\node[objbox, anchor=west] (c) at (0.8, -0.95) {c : CST = 100};
\node[objbox, anchor=west] (t) at (0.8, -1.85) {t : TY = INT};
\node[objbox, anchor=west] (v) at (0.8, -2.75) {v : VAR : INT};
\node[objbox, anchor=west, minimum height=0.8cm] (f) at (0.8, -3.70) {f : FN : INT $\rightarrow$ INT};

% Tree lines from prg
\draw[line] ($(prg.south west)+(0.45,0)$) |- (c.west);
\draw[line] ($(prg.south west)+(0.45,0)$) |- (t.west);
\draw[line] ($(prg.south west)+(0.45,0)$) |- (v.west);
\draw[line] ($(prg.south west)+(0.45,0)$) |- (f.west);

% Level 2 objects inside f
\node[objbox, anchor=west] (x) at (1.85, -4.75) {x : PAR : INT};
\node[objbox, anchor=west] (y) at (1.85, -5.65) {y : VAR : INT};

% Tree lines from f
\draw[line] ($(f.south west)+(0.55,0)$) |- (x.west);
\draw[line] ($(f.south west)+(0.55,0)$) |- (y.west);

% Scope annotations
\begin{scope}[on background layer]
\node[draw=kplteal!50, dashed, rounded corners=5pt, fill=kplteal!4, fit=(prg) (c) (t) (v) (f) (x) (y), inner sep=8pt, label={[font=\scriptsize\bfseries\sffamily, text=kplteal]above right:Scope Toàn cục (Program test)}] {};
\node[draw=kplred!60, dashed, rounded corners=4pt, fill=kplred!5, fit=(x) (y), inner sep=5pt, label={[font=\scriptsize\bfseries\sffamily, text=kplred!80!black]right:Scope Cục bộ (Hàm f)}] {};
\end{scope}
\end{tikzpicture}%
}
\end{center}

\EndSideBySide

## Ví dụ Mở rộng: Đa Phạm vi, Mảng Đa chiều và Che khuất Tên (Shadowing)

Trong thực tế (và cụ thể là trong tệp kiểm thử `incompleted/main.c` của bài thực hành tuần này), một chương trình KPL có thể có **nhiều chương trình con ngang hàng** (như hàm `f` và thủ tục `p`), **mảng nhiều chiều** (`ARRAY(. 10 .) OF ARRAY(. 10 .) OF INTEGER`), cả **tham trị** lẫn **tham biến (`VAR`)**, và đặc biệt là các định danh cục bộ **trùng tên với định danh toàn cục** (hiện tượng _Name Shadowing_ — che khuất tên).

Hãy đối chiếu chương trình KPL mô phỏng chính xác kịch bản của tệp `main.c` ở cột trái với cấu trúc cây Bảng ký hiệu đa phạm vi ở cột phải:

\StartSideBySide{0.47}

```pascal
PROGRAM PRG;
CONST c1 = 10;
      c2 = 'a';
TYPE  t1 = ARRAY(. 10 .) OF INTEGER;
VAR   v1 : INTEGER;
      v2 : ARRAY(. 10 .) OF t1;

FUNCTION f(p1 : INTEGER; VAR p2 : CHAR) : INTEGER;
BEGIN
  f := p1;
END;

PROCEDURE p(v1 : INTEGER);
  (* Khai báo lại c1, t1, v2 trùng tên với
     phạm vi toàn cục PRG -> Che khuất tên! *)
  CONST c1 = 'a';
        c3 = 10;
  TYPE  t1 = INTEGER;
        t2 = ARRAY(. 10 .) OF INTEGER;
  VAR   v2 : t2;
        v3 : CHAR;
BEGIN
  (* Tại đây:
     - 'c1' là hằng CHAR 'a' (cục bộ của p)
     - 'c2' là hằng CHAR 'a' (tìm ở PRG)
     - 'v1' là tham số INT (cục bộ của p) *)
END;

BEGIN
END.
```

\MidSideBySide{0.50}

\begin{center}
\resizebox{\linewidth}{!}{%
\begin{tikzpicture}[
prgbox/.style={rectangle, rounded corners=2pt, draw=kplteal, fill=kplteal!15, font=\sffamily\scriptsize\bfseries, minimum width=3.6cm, minimum height=0.48cm, align=left, inner xsep=4pt},
fbox/.style={rectangle, rounded corners=2pt, draw=kplblue!90!black, fill=kplblue!12, font=\sffamily\scriptsize\bfseries, minimum width=3.5cm, minimum height=0.46cm, align=left, inner xsep=4pt},
pbox/.style={rectangle, rounded corners=2pt, draw=kplred!85!black, fill=kplred!10, font=\sffamily\scriptsize\bfseries, minimum width=3.5cm, minimum height=0.46cm, align=left, inner xsep=4pt},
tline/.style={draw=black!75, line width=0.75pt}
]
\node[prgbox, fill=yellow!60] (prg) at (0,0) {Program PRG};
\node[prgbox, anchor=west] (c1) at (0.5,-0.58) {Const c1 = 10 (Int)};
\node[prgbox, anchor=west] (c2) at (0.5,-1.12) {Const c2 = 'a' (Char)};
\node[prgbox, anchor=west] (t1) at (0.5,-1.66) {Type t1 = Arr(10, Int)};
\node[prgbox, anchor=west] (v1) at (0.5,-2.20) {Var v1 : Int};
\node[prgbox, anchor=west] (v2) at (0.5,-2.74) {Var v2 : Arr(10, Arr(10, Int))};
\node[prgbox, anchor=west, fill=kplblue!25] (fnf) at (0.5,-3.32) {Function f : Int};

\node[fbox, anchor=west] (fp1) at (1.2,-3.88) {Param p1 : Int (VALUE)};
\node[fbox, anchor=west] (fp2) at (1.2,-4.42) {Param VAR p2 : Char (REF)};

\node[prgbox, anchor=west, fill=kplred!22] (prcp) at (0.5,-5.05) {Procedure p};
\node[pbox, anchor=west] (pv1) at (1.2,-5.61) {Param v1 : Int [che v1 ngoài]};
\node[pbox, anchor=west] (pc1) at (1.2,-6.15) {Const c1 = 'a' [che c1 ngoài]};
\node[pbox, anchor=west] (pc3) at (1.2,-6.69) {Const c3 = 10};
\node[pbox, anchor=west] (pt1) at (1.2,-7.23) {Type t1 = Int [che t1 ngoài]};
\node[pbox, anchor=west] (pt2) at (1.2,-7.77) {Type t2 = Arr(10, Int)};
\node[pbox, anchor=west] (pv2) at (1.2,-8.31) {Var v2 : Arr(10, Int) [che v2]};
\node[pbox, anchor=west] (pv3) at (1.2,-8.85) {Var v3 : Char};

\foreach \n in {c1,c2,t1,v1,v2,fnf,prcp} {
\draw[tline] ($(prg.south west)+(0.22,0)$) |- (\n.west);
}
\foreach \n in {fp1,fp2} {
\draw[tline] ($(fnf.south west)+(0.30,0)$) |- (\n.west);
}
\foreach \n in {pv1,pc1,pc3,pt1,pt2,pv2,pv3} {
\draw[tline] ($(prcp.south west)+(0.30,0)$) |- (\n.west);
}
\end{tikzpicture}%
}
\end{center}

\EndSideBySide

---

# Thiết kế Cấu trúc Dữ liệu Bảng Ký hiệu trong C

_Nội dung chương này được biên soạn dựa trên các **Slide 7, 8, 9, 10, 11, 12 và 13** trong bài giảng của **TS. Nguyễn Hữu Đức**, đối chiếu trực tiếp với tệp tiêu đề `incompleted/symtab.h`._

## Các thành phần cốt lõi của Bảng Ký hiệu (`SymTab_`, `Scope_`, `ObjectNode_`) [Slide 7]

Trong tệp `symtab.h`, toàn bộ bảng ký hiệu được neo giữ bởi biến toàn cục `SymTab* symtab;`. Ba cấu trúc dữ liệu nền tảng tạo nên bộ khung phân cấp của bảng ký hiệu bao gồm:

1. **`struct SymTab_` (Bảng ký hiệu tổng thể):**
   - `Object* program`: Trỏ tới đối tượng đại diện cho chương trình chính (`OBJ_PROGRAM`).
   - `Scope* currentScope`: Con trỏ động luôn trỏ tới **phạm vi (block) hiện tại đang được trình biên dịch duyệt qua**.
   - `ObjectNode* globalObjectList`: Danh sách liên kết chứa các chương trình con chuẩn dựng sẵn (built-in) của ngôn ngữ KPL gồm 2 hàm đọc (`READI`, `READC`) và 3 thủ tục in (`WRITEI`, `WRITEC`, `WRITELN`). Các đối tượng này nằm ngoài phạm vi `program` nhưng có hiệu lực toàn cục.
2. **`struct Scope_` (Phạm vi của một Block):**
   - `ObjectNode* objList`: Danh sách liên kết đơn chứa tất cả các đối tượng (`Object`) được khai báo trực tiếp bên trong block đó.
   - `Object* owner`: Con trỏ ngược trỏ về đối tượng sở hữu block này (có thể là `OBJ_PROGRAM`, `OBJ_FUNCTION`, hoặc `OBJ_PROCEDURE`).
   - `struct Scope_* outer`: Con trỏ trỏ ra **phạm vi bao ngoài trực tiếp** (riêng `Scope` của chương trình chính `program` và các hàm toàn cục có `outer = NULL`).
3. **`struct ObjectNode_` (Mắt xích danh sách liên kết đối tượng):**
   - Kết nối các con trỏ `Object* object` thành một danh sách liên kết đơn thông qua con trỏ `next`.

Hãy quan sát khai báo C trên **Slide 7** đặt cạnh sơ đồ liên kết con trỏ chi tiết:

\StartSideBySide{0.46}

```c
// Mắt xích danh sách liên kết đối tượng
struct ObjectNode_ {
  Object *object;
  struct ObjectNode_ *next;
};
typedef struct ObjectNode_ ObjectNode;

// Phạm vi của một block (Slide 7)
struct Scope_ {
  // Danh sách các đối tượng trong block
  ObjectNode *objList;
  // Hàm, thủ tục, chương trình
  // tương ứng sở hữu block
  Object *owner;
  // Phạm vi bao ngoài
  struct Scope_ *outer;
};
typedef struct Scope_ Scope;

// Bảng ký hiệu (Slide 7)
struct SymTab_ {
  // Chương trình chính
  Object* program;
  // Phạm vi hiện tại đang duyệt
  Scope* currentScope;
  // Các đối tượng toàn cục như
  // hàm WRITEI, WRITEC, WRITELN,
  // READI, READC
  ObjectNode *globalObjectList;
};
typedef struct SymTab_ SymTab;
```

\MidSideBySide{0.52}

\begin{center}
\resizebox{\linewidth}{!}{%
\begin{tikzpicture}[
structnode/.style={rectangle split, rectangle split parts=4, draw=kplteal!90!black, fill=kplteal!8, rounded corners=2pt, font=\ttfamily\scriptsize, align=left, line width=0.8pt},
scopenode/.style={rectangle split, rectangle split parts=4, draw=kplblue!90!black, fill=kplblue!8, rounded corners=2pt, font=\ttfamily\scriptsize, align=left, line width=0.8pt},
listnode/.style={rectangle split, rectangle split parts=2, draw=orange!85!black, fill=orange!10, rounded corners=2pt, font=\ttfamily\scriptsize, align=center, line width=0.75pt},
objpill/.style={rectangle, rounded corners=3pt, draw=kplred!80!black, fill=kplred!10, font=\sffamily\scriptsize\bfseries, inner sep=3.5pt},
ptr/.style={-Stealth, thick, draw=black!80},
backptr/.style={-Stealth, thick, dashed, draw=kplred!80!black}
]
% SymTab
\node[structnode] (symtab) at (0, 0) {
\textbf{struct SymTab\_}
\nodepart{two} program
\nodepart{three} currentScope
\nodepart{four} globalObjectList
};

% Program Object
\node[objpill] (progobj) at (3.6, 1.1) {Object: PRG (OBJ_PROGRAM)};

% Program Scope
\node[scopenode] (prgscope) at (3.6, -0.5) {
\textbf{Scope\_ (của PRG)}
\nodepart{two} objList
\nodepart{three} owner
\nodepart{four} outer = NULL
};

% Function Scope
\node[scopenode] (fnscope) at (3.6, -3.1) {
\textbf{Scope\_ (của hàm f)}
\nodepart{two} objList
\nodepart{three} owner (Object: f)
\nodepart{four} outer
};

% Global Object List nodes
\node[listnode] (g1) at (0.2, -2.5) {object \nodepart{two} next};
\node[listnode] (g2) at (0.2, -3.9) {object \nodepart{two} next};
\node[objpill, fill=green!12, draw=green!50!black] (readc) at (-2.1, -2.5) {READC};
\node[objpill, fill=green!12, draw=green!50!black] (writei) at (-2.1, -3.9) {WRITEI ...};

% ObjectNodes in PRG Scope
\node[listnode] (n1) at (6.7, -0.3) {object \nodepart{two} next};
\node[listnode] (n2) at (6.7, -1.7) {object \nodepart{two} NULL};
\node[objpill] (objc) at (8.7, -0.3) {Const c};
\node[objpill] (objf) at (8.7, -1.7) {Func f};

% Connections
\draw[ptr] (symtab.two east) -- (progobj.west);
\draw[ptr, draw=kplred, line width=1.1pt] (symtab.three east) to[out=0, in=180] node[above, sloped, font=\tiny\bfseries, text=kplred] {đang duyệt} (fnscope.one west);
\draw[ptr] (symtab.four south) -- (g1.north);
\draw[ptr] (g1.two south) -- (g2.one north);
\draw[ptr] (g1.one west) -- (readc.east);
\draw[ptr] (g2.one west) -- (writei.east);

\draw[ptr] (progobj.south) -- (prgscope.north);
\draw[backptr] (prgscope.three west) to[out=160, in=-150] (progobj.west);
\draw[backptr, line width=1pt] (fnscope.four east) to[out=20, in=-20] node[right, font=\scriptsize\ttfamily, text=kplred!85!black] {outer} (prgscope.four east);

\draw[ptr] (prgscope.two east) -- (n1.one west);
\draw[ptr] (n1.two south) -- (n2.one north);
\draw[ptr] (n1.one east) -- (objc.west);
\draw[ptr] (n2.one east) -- (objf.west);
\draw[ptr, dotted, thick] (objf.south) to[out=-90, in=0] (fnscope.one east);
\end{tikzpicture}%
}
\end{center}

\EndSideBySide

## Cơ chế Quản lý Phạm vi Động: `enterBlock`, `exitBlock`, `declareObject` [Slide 8]

Khi trình biên dịch duyệt từ đầu đến cuối tệp mã nguồn KPL, nó cần biết chính xác mỗi khai báo mới thuộc về block nào. Theo **Slide 8**:

- Bảng ký hiệu ghi nhớ block hiện đang duyệt trong biến `symtab->currentScope`.
- **Khi bắt đầu dịch một chương trình, hàm hoặc thủ tục:** Gọi hàm `void enterBlock(Scope* scope)` để cập nhật `symtab->currentScope = scope;`. Lưu ý rằng tại thời điểm hàm/thủ tục được khởi tạo bằng `createFunctionObject` hoặc `createProcedureObject`, trường `scope->outer` của nó đã được nối sẵn với `symtab->currentScope` của block bao ngoài.
- **Khi đăng ký một đối tượng mới vào block hiện tại:** Gọi hàm `void declareObject(Object* obj)` để nối `obj` vào cuối danh sách `symtab->currentScope->objList` (và nếu `obj` là tham số hình thức `OBJ_PARAMETER` thì nối thêm vào `paramList` của hàm/thủ tục chủ quản `owner`).
- **Khi kết thúc duyệt một hàm hay thủ tục (`END;`):** Gọi hàm `void exitBlock(void)` để lùi `symtab->currentScope` ra block bao ngoài: `symtab->currentScope = symtab->currentScope->outer;`.

Hãy quan sát mã nguồn C của 3 hàm này đặt cạnh sơ đồ biến đổi trạng thái `currentScope` qua 3 thời điểm:

\StartSideBySide{0.47}

```c
// Chuyển vào duyệt một phạm vi (block) mới
void enterBlock(Scope* scope) {
  symtab->currentScope = scope;
}

// Thoát khỏi block hiện tại, quay ra
// phạm vi bao ngoài trực tiếp
void exitBlock(void) {
  symtab->currentScope =
      symtab->currentScope->outer;
}

// Đăng ký một đối tượng vào block hiện tại
void declareObject(Object* obj) {
  if (obj->kind == OBJ_PARAMETER) {
    Object* owner =
        symtab->currentScope->owner;
    switch (owner->kind) {
    case OBJ_FUNCTION:
      addObject(&(owner->funcAttrs->paramList),
                obj);
      break;
    case OBJ_PROCEDURE:
      addObject(&(owner->procAttrs->paramList),
                obj);
      break;
    default:
      break;
    }
  }
  addObject(&(symtab->currentScope->objList),
            obj);
}
```

\MidSideBySide{0.50}

\begin{center}
\resizebox{\linewidth}{!}{%
\begin{tikzpicture}[
box/.style={rectangle, rounded corners=3pt, draw=kplteal, fill=kplteal!8, minimum width=2.5cm, minimum height=0.75cm, font=\sffamily\scriptsize\bfseries, align=center},
active/.style={rectangle, rounded corners=3pt, draw=kplred, fill=kplred!15, line width=1.1pt, minimum width=2.5cm, minimum height=0.75cm, font=\sffamily\scriptsize\bfseries, align=center},
cptr/.style={-Stealth, line width=1.1pt, draw=kplred},
outerptr/.style={-Stealth, thick, dashed, draw=kplblue!85!black}
]
% Step 1
\node[font=\sffamily\scriptsize\bfseries, text=kplteal!90!black, anchor=west] at (-1.8, 0.7) {Bước 1: \texttt{enterBlock(PRG->scope)} --- Duyệt khai báo toàn cục};
\node[active] (s1_prg) at (0, 0) {Scope(PRG)\\\texttt{outer = NULL}};
\node[font=\ttfamily\scriptsize\bfseries, text=kplred] (cs1) at (-2.4, 0) {currentScope};
\draw[cptr] (cs1) -- (s1_prg);

% Step 2
\node[font=\sffamily\scriptsize\bfseries, text=kplteal!90!black, anchor=west] at (-1.8, -1.2) {Bước 2: Tạo hàm \texttt{f} và gọi \texttt{enterBlock(f->scope)}};
\node[box] (s2_prg) at (0, -2.0) {Scope(PRG)\\\texttt{c, t, v, f}};
\node[active] (s2_f) at (3.4, -2.0) {Scope(f)\\\texttt{x, y}};
\node[font=\ttfamily\scriptsize\bfseries, text=kplred] (cs2) at (3.4, -3.05) {currentScope};
\draw[cptr] (cs2) -- (s2_f);
\draw[outerptr] (s2_f.west) -- node[above, font=\ttfamily\tiny] {outer} (s2_prg.east);

% Step 3
\node[font=\sffamily\scriptsize\bfseries, text=kplteal!90!black, anchor=west] at (-1.8, -3.95) {Bước 3: Kết thúc hàm \texttt{f}, gọi \texttt{exitBlock()}};
\node[active] (s3_prg) at (0, -4.75) {Scope(PRG)\\\texttt{c, t, v, f}};
\node[box, opacity=0.7] (s3_f) at (3.4, -4.75) {Scope(f)\\\texttt{x, y}};
\node[font=\ttfamily\scriptsize\bfseries, text=kplred] (cs3) at (-2.4, -4.75) {currentScope};
\draw[cptr] (cs3) -- (s3_prg);
\draw[outerptr] (s3_f.west) -- node[above, font=\ttfamily\tiny] {outer} (s3_prg.east);
\draw[-Stealth, thick, dotted, draw=kplred] (3.4, -5.3) to[out=180, in=-90] node[below, font=\tiny\itshape, text=kplred] {lùi theo con trỏ outer} (0, -5.15);
\end{tikzpicture}%
}
\end{center}

\EndSideBySide

## Biểu diễn Hằng số (`ConstantValue_`) và Kiểu dữ liệu (`Type_`) [Slides 9, 10]

Ngôn ngữ KPL hỗ trợ 2 kiểu dữ liệu cơ sở là số nguyên (`TP_INT`), ký tự (`TP_CHAR`) và 1 kiểu dữ liệu có cấu trúc là mảng (`TP_ARRAY`).

1. **Cấu trúc `Type_` (Slide 9):**
   - `enum TypeClass typeClass`: Cho biết đây là `TP_INT`, `TP_CHAR` hay `TP_ARRAY`.
   - `int arraySize` và `struct Type_ *elementType`: Chỉ có ý nghĩa khi `typeClass == TP_ARRAY`. Nhờ con trỏ tự trỏ `struct Type_ *elementType`, KPL biểu diễn được **mảng nhiều chiều bất kỳ** dưới dạng một chuỗi các cấu trúc `Type_` nối tiếp nhau và kết thúc ở một nút lá `TP_INT` hoặc `TP_CHAR`!
2. **Cấu trúc `ConstantValue_` (Slide 9):**
   - Vì một hằng số trong KPL chỉ có thể mang giá trị nguyên (`intValue`) **hoặc** giá trị ký tự (`charValue`) tại một thời điểm, `struct ConstantValue_` sử dụng một `union` vô danh để tiết kiệm bộ nhớ, đi kèm trường nhãn `enum TypeClass type` để phân biệt.

Hãy đối chiếu khai báo trong `symtab.h` (Slides 9–10) ở cột trái với sơ đồ tổ chức bộ nhớ trên Heap ở cột phải:

\StartSideBySide{0.48}

```c
// Phân loại kiểu (Slide 9)
enum TypeClass {
  TP_INT,
  TP_CHAR,
  TP_ARRAY
};

struct Type_ {
  enum TypeClass typeClass;
  // Chỉ sử dụng cho kiểu mảng
  int arraySize;
  struct Type_ *elementType;
};
typedef struct Type_ Type;

// Hằng số (Slide 9)
struct ConstantValue_ {
  enum TypeClass type;
  union {
    int intValue;
    char charValue;
  };
};
typedef struct ConstantValue_ ConstantValue;

// Các hàm tạo kiểu & hằng (Slide 10)
Type* makeIntType(void);
Type* makeCharType(void);
Type* makeArrayType(int arraySize,
                    Type* elementType);
Type* duplicateType(Type* type);

ConstantValue* makeIntConstant(int i);
ConstantValue* makeCharConstant(char ch);
ConstantValue* duplicateConstantValue(
                    ConstantValue* v);
```

\MidSideBySide{0.49}

\begin{center}
\resizebox{\linewidth}{!}{%
\begin{tikzpicture}[
tnode/.style={rectangle split, rectangle split parts=3, draw=kplteal!90!black, fill=kplteal!10, rounded corners=2pt, font=\ttfamily\scriptsize, align=left, line width=0.8pt},
cnode/.style={rectangle split, rectangle split parts=2, draw=purple!85!black, fill=purple!8, rounded corners=2pt, font=\ttfamily\scriptsize, align=left, line width=0.8pt},
ptr/.style={-Stealth, thick, draw=kplteal!90!black}
]
\node[font=\sffamily\scriptsize\bfseries, text=kplteal!90!black, anchor=west] at (-0.3, 1.45) {1. Biểu diễn đệ quy của mảng 2 chiều \texttt{v2: ARRAY(.10.) OF ARRAY(.10.) OF INT}:};

\node[tnode] (t1) at (0.8, 0.2) {
typeClass = TP_ARRAY
\nodepart{two} arraySize = 10
\nodepart{three} elementType $\bullet$
};

\node[tnode] (t2) at (3.9, 0.2) {
typeClass = TP_ARRAY
\nodepart{two} arraySize = 10
\nodepart{three} elementType $\bullet$
};

\node[tnode, fill=green!12, draw=green!50!black] (t3) at (6.9, 0.2) {
typeClass = TP_INT
\nodepart{two} {\color{gray}(không dùng)}
\nodepart{three} {\color{gray}(không dùng)}
};

\draw[ptr] (t1.three east) to[out=0, in=180] (t2.one west);
\draw[ptr] (t2.three east) to[out=0, in=180] (t3.one west);

\node[font=\sffamily\scriptsize\bfseries, text=purple!85!black, anchor=west] at (-0.3, -1.35) {2. Biểu diễn \texttt{ConstantValue\_} sử dụng \texttt{union} chia sẻ vùng nhớ:};

\node[cnode] (c1) at (1.5, -2.4) {
type = TP_INT
\nodepart{two} \textbf{union:} intValue = 100
};

\node[cnode] (c2) at (5.5, -2.4) {
type = TP_CHAR
\nodepart{two} \textbf{union:} charValue = 'a'
};
\end{tikzpicture}%
}
\end{center}

\EndSideBySide

## Biểu diễn Đối tượng (`Object_`) và 7 Cấu trúc Thuộc tính (`...Attributes_`) [Slides 11, 12, 13]

Mọi định danh trong bảng ký hiệu (dù là hằng, biến, kiểu, hàm, thủ tục, tham số hay tên chương trình) đều được gói gọn trong một kiểu cấu trúc thống nhất là `struct Object_` (**Slide 11**):

- `char name[MAX_IDENT_LEN]`: Chuỗi ký tự lưu tên định danh (với `MAX_IDENT_LEN = 15`).
- `enum ObjectKind kind`: Nhãn phân loại nhận 1 trong 7 giá trị: `OBJ_CONSTANT`, `OBJ_VARIABLE`, `OBJ_TYPE`, `OBJ_FUNCTION`, `OBJ_PROCEDURE`, `OBJ_PARAMETER`, `OBJ_PROGRAM`.
- Một `union` chứa 7 con trỏ trỏ tới 7 cấu trúc thuộc tính chuyên biệt (**Slide 12 và Slide 13**). Tại mỗi thời điểm, tùy thuộc vào giá trị của `obj->kind`, chỉ **một** trong 7 con trỏ thuộc tính trong `union` được cấp phát và sử dụng.

Hãy đối chiếu mã nguồn C trên **Slides 11, 12, 13** ở cột trái với sơ đồ kiến trúc đa hình của `Object_` ở cột phải:

\StartSideBySide{0.48}

```c
// Phân loại ký hiệu (Slide 11)
enum ObjectKind {
  OBJ_CONSTANT,  OBJ_VARIABLE,
  OBJ_TYPE,      OBJ_FUNCTION,
  OBJ_PROCEDURE, OBJ_PARAMETER,
  OBJ_PROGRAM
};

// Đối tượng trên bảng ký hiệu (Slide 11)
struct Object_ {
  char name[MAX_IDENT_LEN];
  enum ObjectKind kind;
  union {
    ConstantAttributes*  constAttrs;
    VariableAttributes*  varAttrs;
    TypeAttributes*      typeAttrs;
    FunctionAttributes*  funcAttrs;
    ProcedureAttributes* procAttrs;
    ProgramAttributes*   progAttrs;
    ParameterAttributes* paramAttrs;
  };
};

// 7 cấu trúc thuộc tính (Slides 12 & 13)
struct ConstantAttributes_ {
  ConstantValue* value;
};
struct VariableAttributes_ {
  Type *type;
  struct Scope_ *scope; // Dùng cho sinh mã
};
struct TypeAttributes_ {
  Type *actualType;
};
struct ParameterAttributes_ {
  enum ParamKind kind; // VALUE / REFERENCE
  Type* type;
  struct Object_ *function;
};
struct ProcedureAttributes_ {
  struct ObjectNode_ *paramList;
  struct Scope_* scope;
};
struct FunctionAttributes_ {
  struct ObjectNode_ *paramList;
  Type* returnType;
  struct Scope_ *scope;
};
struct ProgramAttributes_ {
  struct Scope_ *scope;
};
```

\MidSideBySide{0.49}

\begin{center}
\resizebox{\linewidth}{!}{%
\begin{tikzpicture}[
obj/.style={rectangle split, rectangle split parts=3, draw=kplteal!90!black, fill=kplteal!12, rounded corners=3pt, font=\ttfamily\scriptsize, align=left, line width=1pt},
attr/.style={rectangle, rounded corners=2pt, draw=kplblue!85!black, fill=kplblue!8, font=\ttfamily\scriptsize, align=left, inner sep=4pt, minimum width=4.6cm},
arr/.style={-Stealth, thick, draw=kplblue!85!black}
]
\node[obj] (o) at (0, 0) {
\textbf{struct Object\_}
\nodepart{two} char name[15]
\nodepart{three} enum ObjectKind kind\\
\textbf{union \{} ...7 con trỏ... \textbf{\}}
};

\node[attr] (a1) at (4.8, 3.0) {\textbf{ConstantAttributes\_}\\$\bullet$ \texttt{ConstantValue* value}};
\node[attr] (a2) at (4.8, 2.0) {\textbf{VariableAttributes\_}\\$\bullet$ \texttt{Type* type} \quad $\bullet$ \texttt{Scope* scope}};
\node[attr] (a3) at (4.8, 1.0) {\textbf{TypeAttributes\_}\\$\bullet$ \texttt{Type* actualType}};
\node[attr] (a4) at (4.8, 0.0) {\textbf{ParameterAttributes\_}\\$\bullet$ \texttt{kind} $\bullet$ \texttt{type} $\bullet$ \texttt{Object* function}};
\node[attr] (a5) at (4.8, -1.05) {\textbf{FunctionAttributes\_}\\$\bullet$ \texttt{paramList} $\bullet$ \texttt{returnType} $\bullet$ \texttt{scope}};
\node[attr] (a6) at (4.8, -2.1) {\textbf{ProcedureAttributes\_}\\$\bullet$ \texttt{paramList} \quad $\bullet$ \texttt{Scope* scope}};
\node[attr] (a7) at (4.8, -3.05) {\textbf{ProgramAttributes\_}\\$\bullet$ \texttt{Scope\* scope}};

\draw[arr] (o.three east) to[out=25, in=180] node[above, sloped, font=\tiny\ttfamily] {constAttrs} (a1.west);
\draw[arr] (o.three east) to[out=15, in=180] node[above, sloped, font=\tiny\ttfamily] {varAttrs} (a2.west);
\draw[arr] (o.three east) to[out=5, in=180] node[above, sloped, font=\tiny\ttfamily] {typeAttrs} (a3.west);
\draw[arr] (o.three east) -- node[above, font=\tiny\ttfamily] {paramAttrs} (a4.west);
\draw[arr] (o.three east) to[out=-5, in=180] node[below, sloped, font=\tiny\ttfamily] {funcAttrs} (a5.west);
\draw[arr] (o.three east) to[out=-15, in=180] node[below, sloped, font=\tiny\ttfamily] {procAttrs} (a6.west);
\draw[arr] (o.three east) to[out=-25, in=180] node[below, sloped, font=\tiny\ttfamily] {progAttrs} (a7.west);
\end{tikzpicture}%
}
\end{center}

\EndSideBySide

## Quy tắc Đặc biệt (Slide 13): Cơ chế "Đăng ký Kép" (Dual Registration) của Tham số Hình thức

Trên **Slide 13**, TS. Nguyễn Hữu Đức nhấn mạnh một lưu ý màu đỏ cực kỳ quan trọng:

\begin{warnbox}[Lưu ý đặc biệt trên Slide 13 về Tham số hình thức (\texttt{OBJ\_PARAMETER})]
\emph{``Các đối tượng tham số hình thức vừa được đăng ký trong danh sách tham số (\texttt{paramList}), vừa được đăng ký trong danh sách các đối tượng được định nghĩa trong block (\texttt{scope->objList}).''}
\end{warnbox}

### Tại sao một đối tượng tham số hình thức phải xuất hiện ở cả hai danh sách?

Hãy xét hàm `FUNCTION f(p1 : INTEGER; VAR p2 : CHAR) : INTEGER;`:

1. **Khi kiểm tra lời gọi hàm từ bên ngoài** (ví dụ câu lệnh `k := f(a, ch);` ở thân chương trình chính): Trình biên dịch cần duyệt qua danh sách tham số hình thức `f->funcAttrs->paramList` để đối chiếu số lượng tham số (2 tham số), kiểu của từng tham số (`INT`, `CHAR`) và cơ chế truyền (`PARAM_VALUE`, `PARAM_REFERENCE`).
2. **Khi dịch các câu lệnh bên trong thân hàm `f`** (ví dụ câu lệnh `f := p1 + 1;` trong thân hàm): Trình biên dịch đang đứng ở phạm vi `f->funcAttrs->scope` và tra cứu định danh `"p1"` trong danh sách đối tượng cục bộ `scope->objList`. Nhờ `"p1"` cũng nằm trong `scope->objList`, quá trình tìm kiếm biến/tham số trong thân hàm diễn ra hoàn toàn đồng nhất!

Hãy quan sát cách hàm `declareObject()` thực hiện cơ chế **Đăng ký Kép** ở bên trái và sơ đồ **Chia sẻ vùng nhớ đối tượng** ở bên phải:

\StartSideBySide{0.46}

```c
// Trích từ incompleted/main.c (dòng 44-58)
obj = createFunctionObject("f");
obj->funcAttrs->returnType = makeIntType();
declareObject(obj);

  // Chuyển currentScope vào scope của hàm f
  enterBlock(obj->funcAttrs->scope);

  obj = createParameterObject(
      "p1", PARAM_VALUE,
      symtab->currentScope->owner);
  obj->paramAttrs->type = makeIntType();
  // Lệnh declareObject dưới đây sẽ gắn obj
  // vào CẢ 2 danh sách:
  // 1) f->funcAttrs->paramList
  // 2) f->funcAttrs->scope->objList
  declareObject(obj);

  obj = createParameterObject(
      "p2", PARAM_REFERENCE,
      symtab->currentScope->owner);
  obj->paramAttrs->type = makeCharType();
  declareObject(obj);

  exitBlock();
```

\MidSideBySide{0.51}

\begin{center}
\resizebox{\linewidth}{!}{%
\begin{tikzpicture}[
nodebox/.style={rectangle split, rectangle split parts=2, draw=kplblue!90!black, fill=kplblue!10, rounded corners=2pt, font=\ttfamily\scriptsize, align=center},
snodebox/.style={rectangle split, rectangle split parts=2, draw=kplteal!90!black, fill=kplteal!10, rounded corners=2pt, font=\ttfamily\scriptsize, align=center},
sharedobj/.style={rectangle, rounded corners=3pt, draw=kplred!90!black, fill=yellow!45, line width=1.1pt, font=\ttfamily\scriptsize\bfseries, align=center, inner sep=5pt, minimum width=2.8cm},
p1/.style={-Stealth, thick, draw=kplblue!90!black},
p2/.style={-Stealth, thick, draw=kplteal!90!black}
]
% Top list: paramList
\node[font=\sffamily\scriptsize\bfseries, text=kplblue!90!black, anchor=west] at (-0.5, 2.1) {Danh sách 1: \texttt{f->funcAttrs->paramList} (Danh sách tham chiếu)};
\node[nodebox] (pn1) at (1.0, 1.3) {object \nodepart{two} next};
\node[nodebox] (pn2) at (4.2, 1.3) {object \nodepart{two} NULL};
\draw[p1] (pn1.two east) -- (pn2.one west);

% Shared Objects in the middle
\node[sharedobj] (objp1) at (1.0, -0.2) {Object "p1"\\{\normalfont\tiny (OBJ_PARAMETER, VALUE)}};
\node[sharedobj] (objp2) at (4.2, -0.2) {Object "p2"\\{\normalfont\tiny (OBJ_PARAMETER, REF)}};

% Bottom list: scope->objList
\node[font=\sffamily\scriptsize\bfseries, text=kplteal!90!black, anchor=west] at (-0.5, -2.4) {Danh sách 2: \texttt{f->funcAttrs->scope->objList} (Sở hữu thực sự)};
\node[snodebox] (sn1) at (1.0, -1.6) {object \nodepart{two} next};
\node[snodebox] (sn2) at (4.2, -1.6) {object \nodepart{two} next (biến y...)};
\draw[p2] (sn1.two east) -- (sn2.one west);

% Shared pointers
\draw[p1, line width=1.1pt] (pn1.one south) -- node[left, font=\tiny\sffamily, text=kplblue!90!black] {trỏ tới} (objp1.north);
\draw[p1, line width=1.1pt] (pn2.one south) -- node[left, font=\tiny\sffamily, text=kplblue!90!black] {trỏ tới} (objp2.north);

\draw[p2, line width=1.1pt] (sn1.one north) -- node[left, font=\tiny\sffamily, text=kplteal!90!black] {trỏ tới} (objp1.south);
\draw[p2, line width=1.1pt] (sn2.one north) -- node[left, font=\tiny\sffamily, text=kplteal!90!black] {trỏ tới} (objp2.south);
\end{tikzpicture}%
}
\end{center}

\EndSideBySide

---

# Một số Hàm Phụ trợ: Khởi tạo, Giải phóng Bộ nhớ và Gỡ rối

_Nội dung chương này được biên soạn dựa trên các **Slide 14, 15, 16 và 17** trong bài giảng của **TS. Nguyễn Hữu Đức**._

## Nhóm hàm Tạo Đối tượng (`create...Object`) [Slides 14, 15]

Theo **Slide 14 và Slide 15**, thư viện bảng ký hiệu cung cấp 7 hàm khởi tạo đối tượng tương ứng với 7 giá trị của `enum ObjectKind`:

1. `Object* createConstantObject(char *name);` — Tạo đối tượng hằng số (`OBJ_CONSTANT`).
2. `Object* createTypeObject(char *name);` — Tạo đối tượng kiểu (`OBJ_TYPE`).
3. `Object* createVariableObject(char *name);` — Tạo đối tượng biến (`OBJ_VARIABLE`), tự động ghi nhận `varAttrs->scope = symtab->currentScope`.
4. `Object* createParameterObject(char *name, enum ParamKind kind, Object* owner);` — Tạo đối tượng tham số hình thức (`OBJ_PARAMETER`), gắn `paramAttrs->function = owner`.
5. `Object* createFunctionObject(char *name);` — Tạo đối tượng hàm (`OBJ_FUNCTION`), khởi tạo `paramList = NULL` và tạo phạm vi mới `createScope(f, symtab->currentScope)`.
6. `Object* createProcedureObject(char *name);` — Tạo đối tượng thủ tục (`OBJ_PROCEDURE`), khởi tạo `paramList = NULL` và tạo phạm vi mới `createScope(p, symtab->currentScope)`.
7. `Object* createProgramObject(char *programName);` — Tạo đối tượng chương trình chính (`OBJ_PROGRAM`), tạo phạm vi gốc `createScope(program, NULL)` và gán `symtab->program = program` _(hàm này đã được TS. Nguyễn Hữu Đức viết mẫu sẵn trong `symtab.c`)_.

## Nhóm hàm Giải phóng Bộ nhớ và Sự khác biệt Sống còn giữa `freeObjectList` và `freeReferenceList` [Slide 16]

Vì ngôn ngữ C không có cơ chế thu gom rác tự động (Garbage Collection), khi kết thúc quá trình dịch (`cleanSymTab()`), toàn bộ cây bảng ký hiệu phải được giải phóng đệ quy thông qua 5 hàm trên **Slide 16**:

- `void freeType(Type* type);` — Giải phóng kiểu dữ liệu (đệ quy theo `elementType` nếu là `TP_ARRAY`).
- `void freeObject(Object* obj);` — Giải phóng cấu trúc thuộc tính con bên trong `obj` và bản thân `obj`.
- `void freeScope(Scope* scope);` — Giải phóng danh sách đối tượng `scope->objList` rồi giải phóng `scope`.
- `void freeObjectList(ObjectNode* objList);` — Giải phóng danh sách đối tượng độc quyền.
- `void freeReferenceList(ObjectNode* objList);` — Giải phóng danh sách các mắt xích tham chiếu.

### Tại sao phải tách riêng `freeObjectList` và `freeReferenceList`?

Như đã phân tích ở Mục 3.5 (Slide 13), mỗi đối tượng tham số hình thức (`OBJ_PARAMETER`) được trỏ đồng thời bởi **hai danh sách liên kết**:

1. Danh sách `scope->objList` của phạm vi hàm/thủ tục.
2. Danh sách `paramList` trong `funcAttrs` (hoặc `procAttrs`).

Nếu khi giải phóng hàm `f`, ta gọi `freeObjectList()` cho cả `scope->objList` lẫn `paramList`, thì đối tượng `Object` của mỗi tham số sẽ bị gọi `free(obj)` **hai lần liên tiếp** — gây ra lỗi **Double Free (Giải phóng kép)** làm sập chương trình ngay lập tức!

Do đó, thiết kế chuẩn trong `symtab.c` quy định:

- **`scope->objList` là chủ sở hữu thực sự:** Dùng `freeObjectList(scope->objList)` để giải phóng **cả** mắt xích `ObjectNode` **lẫn** đối tượng `Object` bên trong.
- **`paramList` chỉ là danh sách tham chiếu phụ:** Dùng `freeReferenceList(paramList)` để **chỉ giải phóng các mắt xích `ObjectNode`** mà **tuyệt đối không** gọi `freeObject(node->object)`.

Hãy quan sát mã nguồn C và sơ đồ TikZ đối chiếu trực tiếp hai hàm này:

\StartSideBySide{0.46}

```c
// Giải phóng danh sách đối tượng độc quyền
// (Dùng cho scope->objList và globalObjectList)
void freeObjectList(ObjectNode *objList) {
  ObjectNode* list = objList;
  while (list != NULL) {
    ObjectNode* node = list;
    list = list->next;
    // Giải phóng CẢ đối tượng bên trong
    // LẪN mắt xích ObjectNode
    freeObject(node->object);
    free(node);
  }
}

// Giải phóng danh sách tham chiếu
// (Chỉ dùng cho funcAttrs->paramList
//  và procAttrs->paramList)
void freeReferenceList(ObjectNode *objList) {
  ObjectNode* list = objList;
  while (list != NULL) {
    ObjectNode* node = list;
    list = list->next;
    // CHỈ giải phóng mắt xích ObjectNode,
    // KHÔNG gọi freeObject(node->object)!
    free(node);
  }
}
```

\MidSideBySide{0.51}

\begin{center}
\resizebox{\linewidth}{!}{%
\begin{tikzpicture}[
refnode/.style={rectangle, rounded corners=2pt, draw=kplblue!90!black, fill=kplblue!12, font=\ttfamily\scriptsize, inner sep=4pt},
ownnode/.style={rectangle, rounded corners=2pt, draw=kplred!90!black, fill=kplred!12, font=\ttfamily\scriptsize, inner sep=4pt},
objbox/.style={rectangle, rounded corners=3pt, draw=kplred!90!black, fill=yellow!40, font=\ttfamily\scriptsize\bfseries, inner sep=5pt}
]
% Reference list zone
\node[refnode] (rn1) at (0, 1.5) {ObjectNode 1};
\node[refnode] (rn2) at (2.8, 1.5) {ObjectNode 2};
\draw[-Stealth, thick] (rn1) -- (rn2);

% Shared objects
\node[objbox] (o1) at (0, 0) {Object "p1"};
\node[objbox] (o2) at (2.8, 0) {Object "p2"};

% Owner list zone
\node[ownnode] (on1) at (0, -1.5) {ObjectNode 1'};
\node[ownnode] (on2) at (2.8, -1.5) {ObjectNode 2'};
\draw[-Stealth, thick] (on1) -- (on2);

\draw[-Stealth, thick, draw=kplblue!90!black] (rn1) -- (o1);
\draw[-Stealth, thick, draw=kplblue!90!black] (rn2) -- (o2);
\draw[-Stealth, thick, draw=kplred!90!black] (on1) -- (o1);
\draw[-Stealth, thick, draw=kplred!90!black] (on2) -- (o2);

\begin{scope}[on background layer]
\node[draw=kplblue!90!black, dashed, line width=1pt, rounded corners=4pt, fill=kplblue!4, fit=(rn1) (rn2), inner sep=6pt, label={[font=\scriptsize\bfseries\sffamily, text=kplblue!90!black]above:Vùng xóa của \texttt{freeReferenceList(paramList)}}] {};
\node[draw=kplred!90!black, dashed, line width=1pt, rounded corners=4pt, fill=kplred!4, fit=(o1) (o2) (on1) (on2), inner sep=6pt, label={[font=\scriptsize\bfseries\sffamily, text=kplred!90!black]below:Vùng xóa của \texttt{freeObjectList(scope->objList)}}] {};
\end{scope}
\end{tikzpicture}%
}
\end{center}

\EndSideBySide

\begin{notebox}[Lưu ý nho nhỏ đối với trường \texttt{globalObjectList} trong \texttt{initSymTab()}]
Trong hàm \texttt{initSymTab()} (\texttt{symtab.c}, dòng 165--175), hai thủ tục chuẩn dựng sẵn \texttt{WRITEI} và \texttt{WRITEC} được khởi tạo trực tiếp mà không thông qua \texttt{enterBlock}/\texttt{declareObject}: các tham số \texttt{"i"} và \texttt{"ch"} của chúng được gắn trực tiếp vào \texttt{procAttrs->paramList} chứ không nằm trong \texttt{procAttrs->scope->objList}. Tuy nhiên, đối với mọi hàm và thủ tục trong chương trình KPL được khai báo qua \texttt{declareObject()}, tham số luôn nằm ở cả hai danh sách như quy tắc trên Slide 13.
\end{notebox}

## Nhóm hàm Hỗ trợ Gỡ rối (`debug.h`, `debug.c`) [Slide 17]

Để giúp người lập trình kiểm tra xem cây bảng ký hiệu đã được xây dựng đúng cấu trúc phân cấp chưa, tệp `debug.h` và `debug.c` cung cấp các hàm in thông tin kèm độ thụt lề (`int indent`) tăng dần theo độ sâu của block (**Slide 17**):

- `void printType(Type* type);` — In kiểu dữ liệu (`Int`, `Char`, hoặc `Arr(10,Int)`).
- `void printConstantValue(ConstantValue* value);` — In giá trị hằng (`10` hoặc `'a'`).
- `void printObject(Object* obj, int indent);` — In thông tin chi tiết của một đối tượng ở mức thụt lề `indent`; nếu đối tượng là `Program`, `Function` hay `Procedure` thì gọi đệ quy `printScope(scope, indent + 4)`.
- `void printObjectList(ObjectNode* objList, int indent);` — Duyệt và in từng đối tượng trong danh sách liên kết _(Lưu ý đính chính nhỏ: trên **Slide 17**, dòng thứ 3 ghi nhầm tên hàm thành `void freeObjectList(ObjectNode_ objList, int indent)`, trong mã nguồn `debug.h`và`debug.c`tên chính xác của hàm này là`void printObjectList(ObjectNode* objList, int indent)`)*.
- `void printScope(Scope* scope, int indent);` — In toàn bộ các đối tượng trong phạm vi `scope`.

---

# Hướng dẫn Thực hành Chi tiết & Lời giải Hoàn chỉnh (`symtab.c`)

_Nội dung chương này được biên soạn dựa trên **Slide 18 (Nhiệm vụ thực hành)** trong bài giảng của **TS. Nguyễn Hữu Đức**, phân tích và hoàn thiện toàn bộ 17 hàm đánh dấu `TODO` trong tệp `incompleted/symtab.c`._

## Cấu trúc các tệp mã nguồn trong thư mục `incompleted/` (Slide 18)

Dự án thực hành Tuần 3 trong thư mục `week3/incompleted/` gồm 6 tệp mã nguồn:

| STT | Tên tệp    | Vai trò trong chương trình                                                              | Trạng thái ban đầu              |
| :-: | :--------- | :-------------------------------------------------------------------------------------- | :------------------------------ |
|  1  | `Makefile` | Kịch bản biên dịch tự động tạo tệp thực thi `symtab` từ `main.o`, `symtab.o`, `debug.o` | Đã hoàn chỉnh                   |
|  2  | `symtab.h` | Khai báo toàn bộ các `enum`, `struct` và nguyên mẫu hàm của Bảng ký hiệu                | Đã hoàn chỉnh                   |
|  3  | `symtab.c` | Cài đặt các hàm khởi tạo, sao chép, so sánh, tìm kiếm và giải phóng Bảng ký hiệu        | **Cần lập trình 17 hàm `TODO`** |
|  4  | `debug.h`  | Khai báo nguyên mẫu các hàm in cấu trúc Bảng ký hiệu phục vụ gỡ rối                     | Đã hoàn chỉnh                   |
|  5  | `debug.c`  | Cài đặt chi tiết các hàm in `printType`, `printObject`, `printScope`                    | Đã hoàn chỉnh                   |
|  6  | `main.c`   | Chương trình chính tạo lập cây Bảng ký hiệu mẫu và gọi `printObject` kiểm thử           | Đã hoàn chỉnh                   |

---

## Phân tích Thuật toán và Lời giải Chi tiết cho 17 Hàm `TODO`

Dưới đây chúng ta chia 17 hàm `TODO` trong `symtab.c` thành **5 nhóm chức năng**, kèm theo giải thích từng bước và đối chiếu mã nguồn C với sơ đồ con trỏ TikZ.

### Nhóm 1: Các hàm tiện ích xử lý Kiểu dữ liệu (`duplicateType`, `compareType`, `freeType`)

Khi làm việc với cấu trúc `Type_`, điểm mấu chốt cần lưu ý là kiểu mảng (`TP_ARRAY`) có cấu trúc **đệ quy** thông qua con trỏ `type->elementType`:

1. **`Type* duplicateType(Type* type)` (Hàm TODO số 1):**
   - **Mục đích:** Tạo một bản sao độc lập hoàn toàn trên Heap (**Deep Copy — Sao chép sâu**) của kiểu `type`.
   - **Tại sao không được gán con trỏ trực tiếp (Shallow Copy)?** Vì mỗi đối tượng biến/kiểu/hàm khi bị hủy bởi `freeObject()` đều gọi `freeType()` lên con trỏ kiểu riêng của nó. Nếu hai biến cùng trỏ chung vào một vùng nhớ `Type_`, việc giải phóng biến thứ nhất sẽ làm hỏng con trỏ kiểu của biến thứ hai! Do đó, khi `type->typeClass == TP_ARRAY`, ta bắt buộc phải gọi đệ quy `resultType->elementType = duplicateType(type->elementType);`.
2. **`int compareType(Type* type1, Type* type2)` (Hàm TODO số 2):**
   - **Mục đích:** Kiểm tra xem hai kiểu dữ liệu `type1` và `type2` có hoàn toàn tương đương cấu trúc (_structural equivalence_) hay không (trả về `1` nếu giống nhau, `0` nếu khác nhau).
   - **Thuật toán:** Trước hết kiểm tra `type1->typeClass == type2->typeClass`. Nếu khác nhau thì trả về `0`. Nếu cùng là `TP_INT` hoặc `TP_CHAR` thì trả về `1`. Nếu cùng là `TP_ARRAY`, hai mảng chỉ tương đương kiểu khi chúng có **cùng kích thước** (`type1->arraySize == type2->arraySize`) **và** kiểu phần tử của chúng tương đương đệ quy (`compareType(type1->elementType, type2->elementType)`).
3. **`void freeType(Type* type)` (Hàm TODO số 3):**
   - **Mục đích:** Giải phóng hoàn toàn bộ nhớ của `type`.
   - **Thuật toán:** Kiểm tra `switch (type->typeClass)`. Nếu là `TP_INT` hoặc `TP_CHAR` thì chỉ cần `free(type);`. Nếu là `TP_ARRAY`, phải giải phóng đệ quy phần tử con trước (`freeType(type->elementType);`) rồi mới giải phóng nút cha (`free(type);`).

\StartSideBySide{0.48}

```c
Type* duplicateType(Type* type) {
  Type* resultType =
      (Type*) malloc(sizeof(Type));
  resultType->typeClass = type->typeClass;
  if (type->typeClass == TP_ARRAY) {
    resultType->arraySize = type->arraySize;
    resultType->elementType =
        duplicateType(type->elementType);
  }
  return resultType;
}

int compareType(Type* type1, Type* type2) {
  if (type1->typeClass == type2->typeClass) {
    if (type1->typeClass == TP_ARRAY) {
      if (type1->arraySize == type2->arraySize)
        return compareType(type1->elementType,
                           type2->elementType);
      else return 0;
    } else return 1;
  } else return 0;
}

void freeType(Type* type) {
  switch (type->typeClass) {
  case TP_INT:
  case TP_CHAR:
    free(type);
    break;
  case TP_ARRAY:
    freeType(type->elementType);
    free(type);
    break;
  }
}
```

\MidSideBySide{0.49}

\begin{center}
\resizebox{\linewidth}{!}{%
\begin{tikzpicture}[
orig/.style={rectangle, rounded corners=2pt, draw=kplblue!90!black, fill=kplblue!10, font=\ttfamily\scriptsize, align=center, inner sep=4pt, minimum width=2.4cm},
copy/.style={rectangle, rounded corners=2pt, draw=kplteal!90!black, fill=kplteal!12, font=\ttfamily\scriptsize, align=center, inner sep=4pt, minimum width=2.4cm},
arr/.style={-Stealth, thick}
]
\node[font=\sffamily\scriptsize\bfseries, text=kplblue!90!black, anchor=west] at (-1.3, 1.8) {Kiểu gốc \texttt{type}: \texttt{Arr(10, Int)}};
\node[orig] (o1) at (0, 1.0) {TP_ARRAY\\size = 10};
\node[orig] (o2) at (3.2, 1.0) {TP_INT};
\draw[arr, draw=kplblue!90!black] (o1) -- node[above, font=\tiny\ttfamily] {elementType} (o2);

\node[font=\sffamily\scriptsize\bfseries, text=kplred!85!black, anchor=west] at (-1.3, -0.05) {Sai lầm nếu chỉ copy nông (Shallow Copy):};
\node[copy, draw=kplred, fill=kplred!10] (s1) at (0, -0.8) {TP_ARRAY\\size = 10};
\draw[arr, dashed, draw=kplred, line width=1pt] (s1.east) to[out=10, in=-135] node[below right, font=\tiny\sffamily, text=kplred] {trỏ chung $\rightarrow$ Lỗi Double Free!} (o2.south west);

\node[font=\sffamily\scriptsize\bfseries, text=kplteal!90!black, anchor=west] at (-1.3, -1.9) {Đúng đắn với \texttt{duplicateType(type)} (Deep Copy):};
\node[copy] (c1) at (0, -2.7) {TP_ARRAY\\size = 10};
\node[copy] (c2) at (3.2, -2.7) {TP_INT (mới)};
\draw[arr, draw=kplteal!90!black] (c1) -- node[above, font=\tiny\ttfamily] {duplicateType} (c2);
\end{tikzpicture}%
}
\end{center}

\EndSideBySide

---

### Nhóm 2: Các hàm tiện ích xử lý Hằng số (`makeIntConstant`, `makeCharConstant`, `duplicateConstantValue`)

Ba hàm tiếp theo (Hàm TODO số 4, 5, 6) dùng để khởi tạo và sao chép cấu trúc `ConstantValue`:

- **`ConstantValue* makeIntConstant(int i)` (Hàm TODO số 4):** Cấp phát động 1 cấu trúc `ConstantValue`, đặt nhãn `value->type = TP_INT;` và lưu `value->intValue = i;`.
- **`ConstantValue* makeCharConstant(char ch)` (Hàm TODO số 5):** Cấp phát động 1 cấu trúc `ConstantValue`, đặt nhãn `value->type = TP_CHAR;` và lưu `value->charValue = ch;`.
- **`ConstantValue* duplicateConstantValue(ConstantValue* v)` (Hàm TODO số 6):** Cấp phát động vùng nhớ `ConstantValue` mới, sao chép trường `v->type` và dựa vào `v->type` (`TP_INT` hay `TP_CHAR`) để sao chép trường tương ứng trong `union`.

```c
ConstantValue* makeIntConstant(int i) {
  ConstantValue* value = (ConstantValue*) malloc(sizeof(ConstantValue));
  value->type = TP_INT;
  value->intValue = i;
  return value;
}

ConstantValue* makeCharConstant(char ch) {
  ConstantValue* value = (ConstantValue*) malloc(sizeof(ConstantValue));
  value->type = TP_CHAR;
  value->charValue = ch;
  return value;
}

ConstantValue* duplicateConstantValue(ConstantValue* v) {
  ConstantValue* value = (ConstantValue*) malloc(sizeof(ConstantValue));
  value->type = v->type;
  if (v->type == TP_INT)
    value->intValue = v->intValue;
  else
    value->charValue = v->charValue;
  return value;
}
```

---

### Nhóm 3: Các hàm khởi tạo Đối tượng (`createConstantObject` $\rightarrow$ `createParameterObject`)

Nhóm này gồm 6 hàm (Hàm TODO số 7 đến số 12). Mẫu thiết kế chung của các hàm này đã được gợi ý trực tiếp từ hàm mẫu `createProgramObject` (dòng 79–88 trong `symtab.c`):

1. Cấp phát bộ nhớ cho `Object* obj = (Object*) malloc(sizeof(Object));`.
2. Sao chép tên định danh: `strcpy(obj->name, name);`.
3. Gán nhãn phân loại `obj->kind = OBJ_...;`.
4. Cấp phát bộ nhớ cho cấu trúc thuộc tính tương ứng trong `union` (`constAttrs`, `typeAttrs`, `varAttrs`, `funcAttrs`, `procAttrs`, `paramAttrs`) và khởi tạo các trường bên trong:
   - Với **`createVariableObject`** (Hàm TODO số 9): Gán `obj->varAttrs->type = NULL;` và ghi nhận phạm vi hiện tại: `obj->varAttrs->scope = symtab->currentScope;`.
   - Với **`createFunctionObject`** (Hàm TODO số 10) và **`createProcedureObject`** (Hàm TODO số 11): Khởi tạo danh sách tham số rỗng `paramList = NULL` và gọi `createScope(obj, symtab->currentScope)` để tạo phạm vi cục bộ mới có `owner = obj` và `outer = symtab->currentScope`.
   - Với **`createParameterObject`** (Hàm TODO số 12): Gán `obj->paramAttrs->kind = kind;`, `obj->paramAttrs->function = owner;` (con trỏ tới hàm/thủ tục chủ quản).

Hãy đối chiếu mã nguồn C của 6 hàm khởi tạo ở cột trái với sơ đồ thiết lập liên kết hai chiều `owner <-> scope` ở cột phải:

\StartSideBySide{0.50}

```c
Object* createConstantObject(char *name) {
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_CONSTANT;
  obj->constAttrs = (ConstantAttributes*)
      malloc(sizeof(ConstantAttributes));
  return obj;
}

Object* createTypeObject(char *name) {
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_TYPE;
  obj->typeAttrs = (TypeAttributes*)
      malloc(sizeof(TypeAttributes));
  return obj;
}

Object* createVariableObject(char *name) {
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_VARIABLE;
  obj->varAttrs = (VariableAttributes*)
      malloc(sizeof(VariableAttributes));
  obj->varAttrs->type = NULL;
  obj->varAttrs->scope = symtab->currentScope;
  return obj;
}

Object* createFunctionObject(char *name) {
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_FUNCTION;
  obj->funcAttrs = (FunctionAttributes*)
      malloc(sizeof(FunctionAttributes));
  obj->funcAttrs->paramList = NULL;
  obj->funcAttrs->scope =
      createScope(obj, symtab->currentScope);
  return obj;
}

Object* createProcedureObject(char *name) {
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_PROCEDURE;
  obj->procAttrs = (ProcedureAttributes*)
      malloc(sizeof(ProcedureAttributes));
  obj->procAttrs->paramList = NULL;
  obj->procAttrs->scope =
      createScope(obj, symtab->currentScope);
  return obj;
}

Object* createParameterObject(char *name,
    enum ParamKind kind, Object* owner) {
  Object* obj = (Object*) malloc(sizeof(Object));
  strcpy(obj->name, name);
  obj->kind = OBJ_PARAMETER;
  obj->paramAttrs = (ParameterAttributes*)
      malloc(sizeof(ParameterAttributes));
  obj->paramAttrs->kind = kind;
  obj->paramAttrs->function = owner;
  return obj;
}
```

\MidSideBySide{0.47}

\begin{center}
\resizebox{\linewidth}{!}{%
\begin{tikzpicture}[
box/.style={rectangle split, rectangle split parts=3, draw=kplteal!90!black, fill=kplteal!10, rounded corners=3pt, font=\ttfamily\scriptsize, align=left, line width=0.85pt},
attr/.style={rectangle split, rectangle split parts=3, draw=kplblue!90!black, fill=kplblue!10, rounded corners=3pt, font=\ttfamily\scriptsize, align=left, line width=0.85pt},
scope/.style={rectangle split, rectangle split parts=3, draw=kplred!85!black, fill=kplred!10, rounded corners=3pt, font=\ttfamily\scriptsize, align=left, line width=0.85pt},
ptr/.style={-Stealth, thick, draw=black!85}
]
\node[font=\sffamily\scriptsize\bfseries, text=kplteal!90!black, align=center] at (1.6, 2.7) {Cấu trúc liên kết được tạo bởi\\\texttt{createFunctionObject("f")}};

\node[box] (fobj) at (0, 1.2) {
\textbf{Object "f"}
\nodepart{two} kind = OBJ_FUNCTION
\nodepart{three} funcAttrs $\bullet$
};

\node[attr] (fattr) at (3.6, 1.2) {
\textbf{FunctionAttributes}
\nodepart{two} paramList = NULL
\nodepart{three} returnType / scope $\bullet$
};

\node[scope] (fscope) at (3.6, -1.2) {
\textbf{Scope (của hàm f)}
\nodepart{two} objList = NULL
\nodepart{three} owner $\bullet$ \quad outer $\bullet$
};

\node[scope, fill=gray!15, draw=gray!80!black] (outer) at (0, -1.2) {
\textbf{symtab->currentScope}
\nodepart{two} {\color{gray}(Scope của PRG)}
\nodepart{three} ...
};

\draw[ptr] (fobj.three east) -- (fattr.one west);
\draw[ptr] (fattr.three south) -- node[right, font=\tiny\ttfamily] {scope} (fscope.one north);
\draw[ptr, dashed, draw=kplred!90!black] (fscope.three west) to[out=135, in=-60] node[below left, font=\tiny\ttfamily, text=kplred!90!black] {owner = obj} (fobj.south);
\draw[ptr, draw=kplblue!90!black] (fscope.two west) -- node[below, font=\tiny\ttfamily, text=kplblue!90!black] {outer} (outer.two east);
\end{tikzpicture}%
}
\end{center}

\EndSideBySide

---

### Nhóm 4: Các hàm Giải phóng Bộ nhớ (`freeObject`, `freeScope`, `freeObjectList`, `freeReferenceList`)

Đây là nhóm hàm đòi hỏi sự cẩn trọng cao nhất (Hàm TODO số 13, 14, 15, 16) để vừa giải phóng sạch 100% vùng nhớ đã `malloc`, vừa không giải phóng trùng lặp:

1. **Nguyên tắc giải phóng từ trong ra ngoài (Bottom-Up Deallocation) trong `freeObject(Object* obj)`:**
   Trước khi gọi `free(obj)`, ta phải kiểm tra `obj->kind` để giải phóng các thành phần con mà thuộc tính của `obj` đang nắm giữ, rồi giải phóng bản thân cấu trúc thuộc tính (`free(obj->...Attrs)`), và cuối cùng mới gọi `free(obj)`:
   - `OBJ_CONSTANT`: Giải phóng `free(obj->constAttrs->value)` $\rightarrow$ `free(obj->constAttrs)` $\rightarrow$ `free(obj)`.
   - `OBJ_TYPE`: Giải phóng `free(obj->typeAttrs->actualType)` (hoặc `freeType(obj->typeAttrs->actualType)`) $\rightarrow$ `free(obj->typeAttrs)` $\rightarrow$ `free(obj)`.
   - `OBJ_VARIABLE`: Giải phóng `free(obj->varAttrs->type)` (hoặc `freeType(obj->varAttrs->type)`) $\rightarrow$ `free(obj->varAttrs)` $\rightarrow$ `free(obj)`. _(Tuyệt đối **không** gọi `freeScope(obj->varAttrs->scope)` vì `varAttrs->scope` chỉ là con trỏ tham chiếu ngược về phạm vi chứa biến!)_.
   - `OBJ_PARAMETER`: Giải phóng `free(obj->paramAttrs->type)` (hoặc `freeType`) $\rightarrow$ `free(obj->paramAttrs)` $\rightarrow$ `free(obj)`. _(Không giải phóng `paramAttrs->function`)_.
   - `OBJ_FUNCTION`: Gọi `freeReferenceList(obj->funcAttrs->paramList)` (chỉ xóa các mắt xích `ObjectNode` của `paramList`), gọi `freeType(obj->funcAttrs->returnType)`, gọi `freeScope(obj->funcAttrs->scope)` (hàm này sẽ giải phóng toàn bộ đối tượng cục bộ và tham số trong `scope->objList`), sau đó `free(obj->funcAttrs)` $\rightarrow$ `free(obj)`.
   - `OBJ_PROCEDURE`: Gọi `freeReferenceList(obj->procAttrs->paramList)`, gọi `freeScope(obj->procAttrs->scope)`, sau đó `free(obj->procAttrs)` $\rightarrow$ `free(obj)`.
   - `OBJ_PROGRAM`: Gọi `freeScope(obj->progAttrs->scope)`, sau đó `free(obj->progAttrs)` $\rightarrow$ `free(obj)`.
2. **Hàm `freeScope(Scope* scope)` (Hàm TODO số 14):**
   Giải phóng danh sách đối tượng thuộc phạm vi `freeObjectList(scope->objList);` rồi giải phóng chính vùng nhớ phạm vi `free(scope);`.

```c
void freeObject(Object* obj) {
  switch (obj->kind) {
  case OBJ_CONSTANT:
    free(obj->constAttrs->value);
    free(obj->constAttrs);
    break;
  case OBJ_TYPE:
    free(obj->typeAttrs->actualType);
    free(obj->typeAttrs);
    break;
  case OBJ_VARIABLE:
    free(obj->varAttrs->type);
    free(obj->varAttrs);
    break;
  case OBJ_FUNCTION:
    freeReferenceList(obj->funcAttrs->paramList);
    freeType(obj->funcAttrs->returnType);
    freeScope(obj->funcAttrs->scope);
    free(obj->funcAttrs);
    break;
  case OBJ_PROCEDURE:
    freeReferenceList(obj->procAttrs->paramList);
    freeScope(obj->procAttrs->scope);
    free(obj->procAttrs);
    break;
  case OBJ_PROGRAM:
    freeScope(obj->progAttrs->scope);
    free(obj->progAttrs);
    break;
  case OBJ_PARAMETER:
    free(obj->paramAttrs->type);
    free(obj->paramAttrs);
    break;
  }
  free(obj);
}

void freeScope(Scope* scope) {
  freeObjectList(scope->objList);
  free(scope);
}

void freeObjectList(ObjectNode *objList) {
  ObjectNode* list = objList;
  while (list != NULL) {
    ObjectNode* node = list;
    list = list->next;
    freeObject(node->object);
    free(node);
  }
}

void freeReferenceList(ObjectNode *objList) {
  ObjectNode* list = objList;
  while (list != NULL) {
    ObjectNode* node = list;
    list = list->next;
    free(node);
  }
}
```

---

### Nhóm 5: Hàm tìm kiếm Đối tượng trong Danh sách (`findObject`)

Hàm cuối cùng cần cài đặt là **`Object* findObject(ObjectNode *objList, char *name)` (Hàm TODO số 17)**:

- **Mục đích:** Duyệt tuần tự qua danh sách liên kết `objList` để tìm xem có đối tượng nào mang tên trùng khớp với chuỗi `name` hay không.
- **Thuật toán:** Khởi tạo con trỏ duyệt `currentNode = objList`. Trong khi `currentNode != NULL`, dùng hàm chuẩn `strcmp(currentNode->object->name, name) == 0` để so sánh chuỗi. Nếu khớp, trả về ngay con trỏ `currentNode->object`. Nếu đi hết danh sách (`NULL`) mà không tìm thấy thì trả về `NULL`.

\StartSideBySide{0.48}

```c
Object* findObject(ObjectNode *objList,
                   char *name) {
  ObjectNode* currentNode = objList;
  while (currentNode != NULL) {
    if (strcmp(currentNode->object->name,
               name) == 0)
      return currentNode->object;
    currentNode = currentNode->next;
  }
  return NULL;
}
```

\MidSideBySide{0.49}

\begin{center}
\resizebox{\linewidth}{!}{%
\begin{tikzpicture}[
nodebox/.style={rectangle split, rectangle split parts=2, draw=kplteal!90!black, fill=kplteal!10, rounded corners=2pt, font=\ttfamily\scriptsize, align=center},
matchbox/.style={rectangle split, rectangle split parts=2, draw=green!60!black, fill=green!15, rounded corners=2pt, font=\ttfamily\scriptsize, align=center, line width=1pt},
ptr/.style={-Stealth, thick}
]
\node[font=\sffamily\scriptsize\bfseries, text=kplteal!90!black] at (2.8, 1.35) {Minh họa \texttt{findObject(objList, "v1")}:};

\node[nodebox] (n1) at (0, 0) {"c1" \nodepart{two} next};
\node[nodebox] (n2) at (2.6, 0) {"t1" \nodepart{two} next};
\node[matchbox] (n3) at (5.2, 0) {\textbf{"v1"} \nodepart{two} next};

\draw[ptr] (n1.two east) -- (n2.one west);
\draw[ptr] (n2.two east) -- (n3.one west);

\node[font=\tiny\ttfamily, text=kplred] at (0, -0.75) {strcmp != 0};
\node[font=\tiny\ttfamily, text=kplred] at (2.6, -0.75) {strcmp != 0};
\node[font=\tiny\ttfamily\bfseries, text=green!50!black] at (5.2, -0.75) {strcmp == 0 $\rightarrow$ return!};
\end{tikzpicture}%
}
\end{center}

\EndSideBySide

---

## Kiểm thử Chương trình với `main.c` và Kết quả Đầu ra Chuẩn

Trong tệp `incompleted/main.c`, TS. Nguyễn Hữu Đức đã xây dựng một kịch bản kiểm thử toàn diện:

1. Khởi tạo bảng ký hiệu (`initSymTab()`).
2. Tạo đối tượng chương trình chính `"PRG"` và tiến vào phạm vi của `"PRG"` (`enterBlock`).
3. Khai báo trong `"PRG"`: hằng nguyên `c1 = 10`, hằng ký tự `c2 = 'a'`, kiểu mảng `t1 = Arr(10, Int)`, biến nguyên `v1 : Int`, biến mảng 2 chiều `v2 : Arr(10, Arr(10, Int))`.
4. Khai báo hàm `f : Int`, vào block của `f` (`enterBlock`), khai báo tham trị `p1 : Int` và tham biến `VAR p2 : Char`, rồi thoát block của `f` (`exitBlock`).
5. Khai báo thủ tục `p`, vào block của `p` (`enterBlock`), khai báo tham trị `v1 : Int`, hai hằng `c1 = 'a'`, `c3 = 10`, hai kiểu `t1 = Int`, `t2 = Arr(10, Int)`, hai biến `v2 : Arr(10, Int)`, `v3 : Char`, rồi thoát block của `p` (`exitBlock`).
6. Thoát block của `"PRG"`, in toàn bộ cây bảng ký hiệu ra màn hình bằng `printObject(symtab->program, 0);` và giải phóng bộ nhớ bằng `cleanSymTab();`.

Hãy đối chiếu đoạn mã cuối của `main.c` và lệnh biên dịch ở cột trái với **Kết quả in chuẩn xác ra màn hình Console** ở cột phải:

\StartSideBySide{0.48}

**Lệnh biên dịch và thực thi:**

```bash
$ cd week3/incompleted
$ make
gcc -c -Wall main.c
gcc -c -Wall symtab.c
gcc -c -Wall debug.c
gcc main.o symtab.o debug.o -o symtab
$ ./symtab
```

**Trích đoạn kiểm thử trong `main.c`:**

```c
    obj = createVariableObject("v2");
    obj->varAttrs->type =
        makeArrayType(10, makeIntType());
    declareObject(obj);

    obj = createVariableObject("v3");
    obj->varAttrs->type = makeCharType();
    declareObject(obj);

    exitBlock(); // Thoát khỏi Procedure p

  exitBlock();   // Thoát khỏi Program PRG
  printObject(symtab->program, 0);
  cleanSymTab();

  return 0;
}
```

\MidSideBySide{0.49}

**Kết quả đầu ra chuẩn (`./symtab`):**

```text
Program PRG
    Const c1 = 10
    Const c2 = 'a'
    Type t1 = Arr(10,Int)
    Var v1 : Int
    Var v2 : Arr(10,Arr(10,Int))
    Function f : Int
        Param p1 : Int
        Param VAR p2 : Char
    Procedure p
        Param v1 : Int
        Const c1 = 'a'
        Const c3 = 10
        Type t1 = Int
        Type t2 = Arr(10,Int)
        Var v2 : Arr(10,Int)
        Var v3 : Char
```

\EndSideBySide

---

## Bảng Tổng hợp Tra cứu Nhanh 17 Hàm Thực hành trong `symtab.c`

| STT | Nguyên mẫu hàm trong `symtab.c`                           | Nhóm chức năng | Điểm cốt lõi cần nhớ khi cài đặt                                                              |
| :-: | :-------------------------------------------------------- | :------------- | :-------------------------------------------------------------------------------------------- |
|  1  | `Type* duplicateType(Type* type)`                         | Kiểu dữ liệu   | Cấp phát `Type` mới; nếu là `TP_ARRAY` phải gọi đệ quy cho `elementType`.                     |
|  2  | `int compareType(Type* type1, Type* type2)`               | Kiểu dữ liệu   | So sánh `typeClass`; nếu cùng là `TP_ARRAY` thì so sánh `arraySize` và đệ quy `elementType`.  |
|  3  | `void freeType(Type* type)`                               | Giải phóng     | Nếu là `TP_ARRAY` phải gọi `freeType(type->elementType)` trước khi `free(type)`.              |
|  4  | `ConstantValue* makeIntConstant(int i)`                   | Hằng số        | Cấp phát `ConstantValue`, gán `type = TP_INT` và `intValue = i`.                              |
|  5  | `ConstantValue* makeCharConstant(char ch)`                | Hằng số        | Cấp phát `ConstantValue`, gán `type = TP_CHAR` và `charValue = ch`.                           |
|  6  | `ConstantValue* duplicateConstantValue(ConstantValue* v)` | Hằng số        | Sao chép `type` và nhánh `intValue` hoặc `charValue` tương ứng.                               |
|  7  | `Object* createConstantObject(char *name)`                | Tạo đối tượng  | `kind = OBJ_CONSTANT`, cấp phát `constAttrs`.                                                 |
|  8  | `Object* createTypeObject(char *name)`                    | Tạo đối tượng  | `kind = OBJ_TYPE`, cấp phát `typeAttrs`.                                                      |
|  9  | `Object* createVariableObject(char *name)`                | Tạo đối tượng  | `kind = OBJ_VARIABLE`, cấp phát `varAttrs`, gán `scope = symtab->currentScope`.               |
| 10  | `Object* createFunctionObject(char *name)`                | Tạo đối tượng  | `kind = OBJ_FUNCTION`, `paramList = NULL`, `scope = createScope(obj, symtab->currentScope)`.  |
| 11  | `Object* createProcedureObject(char *name)`               | Tạo đối tượng  | `kind = OBJ_PROCEDURE`, `paramList = NULL`, `scope = createScope(obj, symtab->currentScope)`. |
| 12  | `Object* createParameterObject(...)`                      | Tạo đối tượng  | `kind = OBJ_PARAMETER`, gán `paramAttrs->kind = kind` và `paramAttrs->function = owner`.      |
| 13  | `void freeObject(Object* obj)`                            | Giải phóng     | Giải phóng thuộc tính con theo từng `obj->kind`, dùng `freeReferenceList` cho `paramList`.    |
| 14  | `void freeScope(Scope* scope)`                            | Giải phóng     | Gọi `freeObjectList(scope->objList)` rồi `free(scope)`.                                       |
| 15  | `void freeObjectList(ObjectNode *objList)`                | Giải phóng     | Giải phóng cả đối tượng `freeObject(node->object)` lẫn mắt xích `free(node)`.                 |
| 16  | `void freeReferenceList(ObjectNode *objList)`             | Giải phóng     | Chỉ giải phóng mắt xích `free(node)`, giữ nguyên `node->object`.                              |
| 17  | `Object* findObject(ObjectNode *objList, char *name)`     | Tra cứu        | Duyệt danh sách liên kết, dùng `strcmp(node->object->name, name) == 0`.                       |
