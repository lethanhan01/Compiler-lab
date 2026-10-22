# Hướng Dẫn Thực Hành & Yêu Cầu Tuần 2: Bộ Phân Tích Cú Pháp (KPL Parser)

Tài liệu này hướng dẫn chi tiết yêu cầu thực hành Tuần 2 của môn học Biên dịch (IT3323), tập trung vào việc hiện thực hóa bộ phân tích cú pháp theo phương pháp đệ quy xuống (Recursive Descent Parser) cho ngôn ngữ KPL.

---

## 1. Cấu trúc các tệp tin mã nguồn

Các tệp mã nguồn nằm trong thư mục `week2/incompleted/`:

| STT | Tên tệp tin                | Mô tả nhiệm vụ                                                                                  |
| :-: | :------------------------- | :---------------------------------------------------------------------------------------------- |
|  1  | `Makefile`                 | Kịch bản biên dịch dự án bằng `gcc`                                                             |
|  2  | `scanner.c`, `scanner.h`   | Bộ phân tích từ vựng (Scanner), đọc và trả về từng token (`getToken()`, `getValidToken()`)      |
|  3  | `reader.c`, `reader.h`     | Quản lý việc mở file và đọc từng ký tự từ tệp mã nguồn KPL                                      |
|  4  | `charcode.c`, `charcode.h` | Bảng phân loại mã ký tự (chữ cái, chữ số, dấu phân cách...)                                     |
|  5  | `token.c`, `token.h`       | Định nghĩa các cấu trúc dữ liệu token, bảng từ khóa và hàm in token                             |
|  6  | `error.c`, `error.h`       | Quản lý danh sách mã lỗi cú pháp và hàm hiển thị thông báo lỗi                                  |
|  7  | `parser.c`, `parser.h`     | **Tệp trọng tâm cần hoàn thiện**: Duyệt các cấu trúc ngữ pháp KPL theo phương pháp đệ quy xuống |
|  8  | `main.c`                   | Điểm vào chương trình chính (`main`), tiếp nhận đường dẫn file nguồn và kích hoạt parser        |

---

## 2. Hướng dẫn biên dịch và kiểm thử

### 2.1. Biên dịch bộ phân tích cú pháp

Mở terminal (WSL / Linux) và di chuyển vào thư mục chứa mã nguồn:

```bash
cd week2/incompleted
make clean
make
```

Sau khi biên dịch thành công, tệp thực thi `parser` sẽ được tạo ra tại `week2/incompleted/parser`.

### 2.2. Kiểm thử thủ công từng tệp

Chạy chương trình parser với một tệp mã nguồn KPL bất kỳ:

```bash
./parser ../test/example1.kpl
```

### 2.3. Đối chiếu kết quả với output chuẩn

So sánh kết quả chạy thực tế với file kết quả mẫu trong thư mục `week2/test/`:

_Cách 1: Chuyển hướng đầu ra vào tệp tạm thời rồi dùng `diff`_

```bash
./parser ../test/example1.kpl > my_result1.txt
diff -u my_result1.txt ../test/result1.txt
```

_Cách 2: So sánh trực tiếp qua Process Substitution (trên Bash/WSL/Linux)_

```bash
diff -u <(./parser ../test/example1.kpl) ../test/result1.txt
```

> Nếu lệnh không in ra bất kỳ dòng khác biệt nào (hoặc trả về exit code 0), bộ phân tích của bạn đã vượt qua ca kiểm thử.

---

## 3. Nội dung các bài tập thực hành

Hầu hết công việc cần thực hiện bằng cách thay thế các đoạn ghi chú `// TODO` trong tệp `week2/incompleted/parser.c`.

### Bài tập 1: Phân tích cú pháp phần Khai báo và Thân rỗng

**Mục tiêu**: Nhận dạng và phân tích cú pháp cho các khai báo hằng (`CONST`), kiểu (`TYPE`), biến (`VAR`) và thân chương trình không chứa lệnh thực thi.

#### Checklist các hàm cần cài đặt trong `parser.c`:

- [ ] **Khai báo hằng (`Const`):**
  - `compileConstDecls()`:
    $$\text{ConstDecls} \to \text{ConstDecl}\ \text{ConstDecls} \mid \varepsilon$$
  - `compileConstDecl()`:
    $$\text{ConstDecl} \to \text{Ident}\ \texttt{=}\ \text{Constant}\ \texttt{;}$$
  - `compileConstant()`:
    $$\text{Constant} \to [\texttt{+} \mid \texttt{-}]\ \text{Constant2} \mid \text{ConstChar}$$
  - `compileConstant2()`:
    $$\text{Constant2} \to \text{ConstIdent} \mid \text{Number}$$
  - `compileUnsignedConstant()`:
    $$\text{UnsignedConstant} \to \text{Number} \mid \text{ConstIdent} \mid \text{ConstChar}$$
- [ ] **Khai báo kiểu (`Type`):**
  - `compileTypeDecls()`:
    $$\text{TypeDecls} \to \text{TypeDecl}\ \text{TypeDecls} \mid \varepsilon$$
  - `compileTypeDecl()`:
    $$\text{TypeDecl} \to \text{Ident}\ \texttt{=}\ \text{Type}\ \texttt{;}$$
  - `compileType()`:
    $$\text{Type} \to \text{BasicType} \mid \text{TypeIdent} \mid \texttt{ARRAY}\ \texttt{(.}\ \text{Number}\ \texttt{.)}\ \texttt{OF}\ \text{Type}$$
  - `compileBasicType()`:
    $$\text{BasicType} \to \texttt{INTEGER} \mid \texttt{CHAR}$$
- [ ] **Khai báo biến (`Var`):**
  - `compileVarDecls()`:
    $$\text{VarDecls} \to \text{VarDecl}\ \text{VarDecls} \mid \varepsilon$$
  - `compileVarDecl()`:
    $$\text{VarDecl} \to \text{Ident}\ \texttt{:}\ \text{Type}\ \texttt{;}$$
- [ ] **Thân chương trình rỗng:**
  - Đảm bảo `compileBlock5()` và `compileStatements()` xử lý được trường hợp danh sách câu lệnh rỗng (khi gặp ngay `END`).

#### Ca kiểm thử bài 1:

- `../test/example1.kpl`: Chương trình KPL tối giản với thân hàm rỗng.
- `../testparser.kpl`: Kiểm thử đầy đủ khai báo `const`, `type`, `var`.

```bash
./parser ../test/example1.kpl
diff -u <(./parser ../test/example1.kpl) ../test/result1.txt
```

---

### Bài tập 2: Phân tích cú pháp các Câu lệnh và Biểu thức

**Mục tiêu**: Bổ sung cú pháp cho các loại câu lệnh gán, gọi thủ tục, rẽ nhánh điều kiện, vòng lặp và tính toán biểu thức số học / logic.

#### Checklist các hàm cần cài đặt trong `parser.c`:

- [ ] **Các loại câu lệnh (`Statements`):**
  - `compileStatements()` & `compileStatements2()`:
    $$\text{Statements} \to \text{Statement}\ \text{Statements2}$$
    $$\text{Statements2} \to \texttt{;}\ \text{Statement}\ \text{Statements2} \mid \varepsilon$$
  - `compileAssignSt()`:
    $$\text{AssignSt} \to \text{Variable}\ \texttt{:=}\ \text{Expression}$$
  - `compileCallSt()`:
    $$\text{CallSt} \to \texttt{CALL}\ \text{ProcedureIdent}\ \text{Arguments}$$
  - `compileGroupSt()`:
    $$\text{GroupSt} \to \texttt{BEGIN}\ \text{Statements}\ \texttt{END}$$
  - `compileIfSt()` & `compileElseSt()`:
    $$\text{IfSt} \to \texttt{IF}\ \text{Condition}\ \texttt{THEN}\ \text{Statement}\ \text{ElseSt}$$
    $$\text{ElseSt} \to \texttt{ELSE}\ \text{Statement} \mid \varepsilon$$
  - `compileWhileSt()`:
    $$\text{WhileSt} \to \texttt{WHILE}\ \text{Condition}\ \texttt{DO}\ \text{Statement}$$
  - `compileForSt()`:
    $$\text{ForSt} \to \texttt{FOR}\ \text{VariableIdent}\ \texttt{:=}\ \text{Expression}\ \texttt{TO}\ \text{Expression}\ \texttt{DO}\ \text{Statement}$$
- [ ] **Biểu thức và Điều kiện (`Expressions & Conditions`):**
  - `compileCondition()` & `compileCondition2()`:
    $$\text{Condition} \to \text{Expression}\ \text{Condition2}$$
    $$\text{Condition2} \to (\texttt{=} \mid \texttt{!=} \mid \texttt{<=} \mid \texttt{<} \mid \texttt{>=} \mid \texttt{>})\ \text{Expression}$$
  - `compileExpression()`, `compileExpression2()`, `compileExpression3()`:
    $$\text{Expression} \to [\texttt{+} \mid \texttt{-}]\ \text{Expression2}$$
    $$\text{Expression2} \to \text{Term}\ \text{Expression3}$$
    $$\text{Expression3} \to (\texttt{+} \mid \texttt{-})\ \text{Term}\ \text{Expression3} \mid \varepsilon$$
  - `compileTerm()` & `compileTerm2()`:
    $$\text{Term} \to \text{Factor}\ \text{Term2}$$
    $$\text{Term2} \to (\texttt{*} \mid \texttt{/})\ \text{Factor}\ \text{Term2} \mid \varepsilon$$
  - `compileFactor()`:
    $$\text{Factor} \to \text{UnsignedConstant} \mid \text{Variable} \mid \text{FunctionApplication} \mid \texttt{(}\ \text{Expression}\ \texttt{)}$$
  - `compileIndexes()`:
    $$\text{Indexes} \to \texttt{(.}\ \text{Expression}\ \texttt{.)}\ \text{Indexes} \mid \varepsilon$$
  - `compileArguments()` & `compileArguments2()`:
    $$\text{Arguments} \to \texttt{(}\ \text{Expression}\ \text{Arguments2}\ \texttt{)} \mid \varepsilon$$
    $$\text{Arguments2} \to \texttt{,}\ \text{Expression}\ \text{Arguments2} \mid \varepsilon$$

#### Ca kiểm thử bài 2:

- Kiểm thử câu lệnh gán, điều kiện rẽ nhánh và vòng lặp `for`/`while` với các biểu thức toán học.

---

### Bài tập 3: Dịch chương trình với đầy đủ sơ đồ cú pháp KPL

**Mục tiêu**: Hoàn thiện toàn bộ các cấu trúc còn lại của bộ phân tích cú pháp KPL, hỗ trợ định nghĩa chương trình con (Hàm `FUNCTION`, Thủ tục `PROCEDURE`), danh sách tham số (tham trị, tham biến `VAR`), cấu trúc mảng và kiểm thử với toàn bộ test suite.

#### Checklist các hàm cần cài đặt trong `parser.c`:

- [ ] **Khai báo chương trình con (`SubDecls`):**
  - `compileSubDecls()`:
    $$\text{SubDecls} \to (\text{FunDecl} \mid \text{ProcDecl})\ \text{SubDecls} \mid \varepsilon$$
  - `compileFuncDecl()`:
    $$\text{FunDecl} \to \texttt{FUNCTION}\ \text{Ident}\ \text{Params}\ \texttt{:}\ \text{BasicType}\ \texttt{;}\ \text{Block}\ \texttt{;}$$
  - `compileProcDecl()`:
    $$\text{ProcDecl} \to \texttt{PROCEDURE}\ \text{Ident}\ \text{Params}\ \texttt{;}\ \text{Block}\ \texttt{;}$$
- [ ] **Danh sách tham số (`Params`):**
  - `compileParams()`:
    $$\text{Params} \to \texttt{(}\ \text{Param}\ \text{Params2}\ \texttt{)} \mid \varepsilon$$
  - `compileParams2()`:
    $$\text{Params2} \to \texttt{;}\ \text{Param}\ \text{Params2} \mid \varepsilon$$
  - `compileParam()`:
    $$\text{Param} \to [\texttt{VAR}]\ \text{Ident}\ \texttt{:}\ \text{BasicType}$$

#### Danh sách ca kiểm thử trọn bộ (Test Suite):

| Tệp kiểm thử   | Tệp kết quả mẫu | Nội dung đặc trưng kiểm thử                                                   | Lệnh kiểm tra                                                  |
| :------------- | :-------------- | :---------------------------------------------------------------------------- | :------------------------------------------------------------- |
| `example1.kpl` | `result1.txt`   | Khung chương trình rỗng                                                       | `diff -u <(./parser ../test/example1.kpl) ../test/result1.txt` |
| `example2.kpl` | `result2.txt`   | Hàm đệ quy tính giai thừa (`Function F`), vòng lặp `For`, lệnh gọi I/O        | `diff -u <(./parser ../test/example2.kpl) ../test/result2.txt` |
| `example3.kpl` | `result3.txt`   | Thuật toán Bubble Sort: Khai báo mảng, kiểu, biến, `Procedure`, `While`, `If` | `diff -u <(./parser ../test/example3.kpl) ../test/result3.txt` |
| `example4.kpl` | `result4.txt`   | Khai báo mảng, thủ tục với tham số tham biến `VAR`                            | `diff -u <(./parser ../test/example4.kpl) ../test/result4.txt` |
| `example5.kpl` | `result5.txt`   | Khai báo biến `char`, hàm đệ quy và các câu lệnh kết hợp                      | `diff -u <(./parser ../test/example5.kpl) ../test/result5.txt` |
| `example6.kpl` | `result6.txt`   | Chương trình kiểm thử cú pháp tổng hợp 1                                      | `diff -u <(./parser ../test/example6.kpl) ../test/result6.txt` |
| `example7.kpl` | `result7.txt`   | Chương trình kiểm thử cú pháp tổng hợp 2                                      | `diff -u <(./parser ../test/example7.kpl) ../test/result7.txt` |

---

### Bài tập 4: Bài tập bổ sung (Mở rộng Parser)

**Mục tiêu**: Mở rộng hệ thống từ vựng (Scanner/Token) và cú pháp (Parser) của ngôn ngữ KPL để hỗ trợ kiểu dữ liệu chuỗi ký tự (`STRING`), toán tử chia lấy dư (`%`) và cấu trúc lặp kiểm tra điều kiện sau (`DO ... WHILE`).

#### 1. Các tệp cần chỉnh sửa:
- `week2/incompleted/token.h` & `token.c`:
  - Khai báo token loại mới `TK_STRING` (hằng chuỗi), từ khóa `KW_STRING`, toán tử `SB_MOD` (phép chia lấy dư `%`).
  - Thêm từ khóa `"STRING"` vào mảng `keywords` và cập nhật hằng số `KEYWORDS_COUNT`.
  - Cập nhật hàm `tokenToString()` tương ứng để in tên token ra màn hình.
- `week2/incompleted/scanner.c`:
  - Cập nhật hàm `getToken()` để nhận dạng ký tự `'%'` trả về `SB_MOD`.
  - Thêm logic đọc chuỗi ký tự (trong cặp dấu nháy kép `"` hoặc nháy đơn `'`) trả về token `TK_STRING`.
- `week2/incompleted/parser.h` & `parser.c`:
  - Khai báo prototype `void compileDoWhileSt(void);` trong `parser.h`.
  - Cài đặt các luật mở rộng ngữ pháp trong `parser.c`.

#### 2. Checklist chi tiết và Quy tắc cú pháp mở rộng (BNF):

- [x] **Mở rộng Kiểu dữ liệu và Hằng chuỗi (`String`):**
  - **Quy tắc BNF mở rộng:**
    $$\text{BasicType} \to \texttt{INTEGER} \mid \texttt{CHAR} \mid \texttt{STRING}$$
    $$\text{UnsignedConstant} \to \text{Number} \mid \text{ConstIdent} \mid \text{ConstChar} \mid \text{TK\_STRING}$$
    $$\text{Factor} \to \dots \mid \text{TK\_STRING}$$
  - **Cần làm trong `parser.c`:**
    - `compileBasicType()`: Thêm trường hợp `case KW_STRING: eat(KW_STRING); break;`.
    - `compileUnsignedConstant()`: Thêm trường hợp `case TK_STRING: eat(TK_STRING); break;`.
    - `compileFactor()`: Thêm trường hợp `case TK_STRING: compileUnsignedConstant(); break;`.

- [x] **Mở rộng Phép toán lấy phần dư (`%` - Modulo):**
  - **Quy tắc BNF mở rộng:**
    $$\text{Term2} \to (\texttt{*} \mid \texttt{/} \mid \texttt{\%})\ \text{Factor}\ \text{Term2} \mid \varepsilon$$
  - **Cần làm trong `parser.c`:**
    - `compileTerm2()`: Bổ sung nhánh `case SB_MOD: eat(SB_MOD); compileFactor(); compileTerm2(); break;` (cùng mức ưu tiên với phép nhân `*` và chia `/`).

- [x] **Mở rộng Cấu trúc lặp `DO ... WHILE`:**
  - **Quy tắc BNF mở rộng:**
    $$\text{Statement} \to \dots \mid \text{DoWhileSt}$$
    $$\text{DoWhileSt} \to \texttt{DO}\ \text{Statement}\ \texttt{WHILE}\ \text{Condition}$$
  - **Cần làm trong `parser.c`:**
    - `compileStatement()`: Bổ sung nhánh `case KW_DO: compileDoWhileSt(); break;`.
    - Hiện thực hàm `compileDoWhileSt()`:
      ```c
      void compileDoWhileSt(void) {
        assert("Parsing a do-while statement ....");
        eat(KW_DO);
        compileStatement();
        eat(KW_WHILE);
        compileCondition();
        assert("Do-while statement parsed ....");
      }
      ```

#### 3. Mã nguồn KPL kiểm thử mẫu (`test/bonus.kpl`):

```pascal
Program BonusTest;
Const msg = "Hello KPL";
Var s : String;
    x : Integer;
    r : Integer;

Begin
  s := "Compiler Construction";
  x := 10;
  r := x % 3;
  Do
    Begin
      x := x - 1;
      r := r + 1;
    End
  While x > 0;
End.
```

#### 4. Lệnh biên dịch và kiểm thử:

```bash
cd week2/incompleted
make clean && make
./parser ../test/bonus.kpl
```

## 4. Lưu ý khi cài đặt

1. **Khái niệm `lookAhead`**: Biến toàn cục `lookAhead` luôn chứa token tiếp theo cần kiểm tra.
2. **Hàm `eat(TokenType)`**: Dùng để khớp token hiện tại với loại token mong đợi. Nếu đúng, token được in ra và con trỏ chuyển sang token kế tiếp; nếu sai, báo lỗi cú pháp tương ứng.
3. **Phân tích đệ quy xuống**: Mỗi hàm `compile*()` tương ứng với một ký hiệu chưa kết thúc (Non-terminal) trong văn phạm BNF đã loại bỏ đệ quy trái và nhân tử trái. Khi gặp các nhánh rẽ $\varepsilon$ (rỗng), cần kiểm tra xem `lookAhead->tokenType` có thuộc tập `FOLLOW` của ký hiệu đó hay không.

## Phụ lục: Văn phạm BNF (01 - 93)

Hệ thống luật văn phạm KPL đã được loại bỏ đệ quy trái và nhân tử trái phục vụ phân tích cú pháp LL(1), được phân nhóm theo từng thành phần cấu trúc:

### 1. Cấu trúc Chương trình và Khối lệnh (Program & Block)

```ebnf
01) Prog   ::= KW_PROGRAM Ident SB_SEMICOLON Block SB_PERIOD
02) Block  ::= KW_CONST ConstDecl ConstDecls Block2
03) Block  ::= Block2
04) Block2 ::= KW_TYPE TypeDecl TypeDecls Block3
05) Block2 ::= Block3
06) Block3 ::= KW_VAR VarDecl VarDecls Block4
07) Block3 ::= Block4
08) Block4 ::= SubDecls Block5
09) Block5 ::= KW_BEGIN Statements KW_END
```

### 2. Khai báo Hằng số (Constant Declarations)

```ebnf
10) ConstDecls        ::= ConstDecl ConstDecls
11) ConstDecls        ::= ε
12) ConstDecl         ::= Ident SB_EQ Constant SB_SEMICOLON

36) UnsignedConstant  ::= Number
37) UnsignedConstant  ::= ConstIdent
38) UnsignedConstant  ::= ConstChar

39) Constant          ::= SB_PLUS Constant2
40) Constant          ::= SB_MINUS Constant2
41) Constant          ::= Constant2
42) Constant          ::= ConstChar

43) Constant2         ::= ConstIdent
44) Constant2         ::= Number
```

### 3. Khai báo Kiểu dữ liệu (Type Declarations)

```ebnf
13) TypeDecls  ::= TypeDecl TypeDecls
14) TypeDecls  ::= ε
15) TypeDecl   ::= Ident SB_EQ Type SB_SEMICOLON

30) Type       ::= KW_INTEGER
31) Type       ::= KW_CHAR
32) Type       ::= TypeIdent
33) Type       ::= KW_ARRAY SB_LSEL Number SB_RSEL KW_OF Type

34) BasicType  ::= KW_INTEGER
35) BasicType  ::= KW_CHAR
```

### 4. Khai báo Biến (Variable Declarations)

```ebnf
16) VarDecls ::= VarDecl VarDecls
17) VarDecls ::= ε
18) VarDecl  ::= Ident SB_COLON Type SB_SEMICOLON
```

### 5. Khai báo Chương trình con và Tham số (Subroutines & Parameters)

```ebnf
19) SubDecls  ::= FunDecl SubDecls
20) SubDecls  ::= ProcDecl SubDecls
21) SubDecls  ::= ε

22) FunDecl   ::= KW_FUNCTION Ident Params SB_COLON BasicType SB_SEMICOLON Block SB_SEMICOLON
23) ProcDecl  ::= KW_PROCEDURE Ident Params SB_SEMICOLON Block SB_SEMICOLON

24) Params    ::= SB_LPAR Param Params2 SB_RPAR
25) Params    ::= ε
26) Params2   ::= SB_SEMICOLON Param Params2
27) Params2   ::= ε

28) Param     ::= Ident SB_COLON BasicType
29) Param     ::= KW_VAR Ident SB_COLON BasicType
```

### 6. Các Câu lệnh (Statements)

```ebnf
45) Statements  ::= Statement Statements2
46) Statements2 ::= SB_SEMICOLON Statement Statements2
47) Statements2 ::= ε

48) Statement   ::= AssignSt
49) Statement   ::= CallSt
50) Statement   ::= GroupSt
51) Statement   ::= IfSt
52) Statement   ::= WhileSt
53) Statement   ::= ForSt
54) Statement   ::= ε

55) AssignSt    ::= Variable SB_ASSIGN Expression
56) AssignSt    ::= FunctionIdent SB_ASSIGN Expression
57) CallSt      ::= KW_CALL ProcedureIdent Arguments
58) GroupSt     ::= KW_BEGIN Statements KW_END
59) IfSt        ::= KW_IF Condition KW_THEN Statement ElseSt
60) ElseSt      ::= KW_ELSE Statement
61) ElseSt      ::= ε
62) WhileSt     ::= KW_WHILE Condition KW_DO Statement
63) ForSt       ::= KW_FOR VariableIdent SB_ASSIGN Expression KW_TO Expression KW_DO Statement
```

### 7. Biểu thức, Điều kiện và Thành phần con (Expressions, Conditions, Terms & Factors)

```ebnf
64) Arguments           ::= SB_LPAR Expression Arguments2 SB_RPAR
65) Arguments           ::= ε
66) Arguments2          ::= SB_COMMA Expression Arguments2
67) Arguments2          ::= ε

68) Condition           ::= Expression Condition2
69) Condition2          ::= SB_EQ Expression
70) Condition2          ::= SB_NEQ Expression
71) Condition2          ::= SB_LE Expression
72) Condition2          ::= SB_LT Expression
73) Condition2          ::= SB_GE Expression
74) Condition2          ::= SB_GT Expression

75) Expression          ::= SB_PLUS Expression2
76) Expression          ::= SB_MINUS Expression2
77) Expression          ::= Expression2
78) Expression2         ::= Term Expression3
79) Expression3         ::= SB_PLUS Term Expression3
80) Expression3         ::= SB_MINUS Term Expression3
81) Expression3         ::= ε

82) Term                ::= Factor Term2
83) Term2               ::= SB_TIMES Factor Term2
84) Term2               ::= SB_SLASH Factor Term2
85) Term2               ::= ε

86) Factor              ::= UnsignedConstant
87) Factor              ::= Variable
88) Factor              ::= FunctionApplication
89) Factor              ::= SB_LPAR Expression SB_RPAR

90) Variable            ::= VariableIdent Indexes
91) FunctionApplication ::= FunctionIdent Arguments

92) Indexes             ::= SB_LSEL Expression SB_RSEL Indexes
93) Indexes             ::= ε
```
