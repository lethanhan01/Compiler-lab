# Hướng Dẫn Thực Hành & Yêu Cầu Tuần 2: Bộ Phân Tích Cú Pháp (KPL Parser)

Tài liệu này hướng dẫn chi tiết yêu cầu thực hành Tuần 2 của môn học Biên dịch (IT3323), tập trung vào việc hiện thực hóa bộ phân tích cú pháp theo phương pháp đệ quy xuống (Recursive Descent Parser) cho ngôn ngữ KPL.

---

## 1. Cấu trúc các tệp tin mã nguồn

Các tệp mã nguồn nằm trong thư mục `week2/incompleted/`:

| STT | Tên tệp tin | Mô tả nhiệm vụ |
| :---: | :--- | :--- |
| 1 | `Makefile` | Kịch bản biên dịch dự án bằng `gcc` |
| 2 | `scanner.c`, `scanner.h` | Bộ phân tích từ vựng (Scanner), đọc và trả về từng token (`getToken()`, `getValidToken()`) |
| 3 | `reader.c`, `reader.h` | Quản lý việc mở file và đọc từng ký tự từ tệp mã nguồn KPL |
| 4 | `charcode.c`, `charcode.h` | Bảng phân loại mã ký tự (chữ cái, chữ số, dấu phân cách...) |
| 5 | `token.c`, `token.h` | Định nghĩa các cấu trúc dữ liệu token, bảng từ khóa và hàm in token |
| 6 | `error.c`, `error.h` | Quản lý danh sách mã lỗi cú pháp và hàm hiển thị thông báo lỗi |
| 7 | `parser.c`, `parser.h` | **Tệp trọng tâm cần hoàn thiện**: Duyệt các cấu trúc ngữ pháp KPL theo phương pháp đệ quy xuống |
| 8 | `main.c` | Điểm vào chương trình chính (`main`), tiếp nhận đường dẫn file nguồn và kích hoạt parser |

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

*Cách 1: Chuyển hướng đầu ra vào tệp tạm thời rồi dùng `diff`*
```bash
./parser ../test/example1.kpl > my_result1.txt
diff -u my_result1.txt ../test/result1.txt
```

*Cách 2: So sánh trực tiếp qua Process Substitution (trên Bash/WSL/Linux)*
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

| Tệp kiểm thử | Tệp kết quả mẫu | Nội dung đặc trưng kiểm thử | Lệnh kiểm tra |
| :--- | :--- | :--- | :--- |
| `example1.kpl` | `result1.txt` | Khung chương trình rỗng | `diff -u <(./parser ../test/example1.kpl) ../test/result1.txt` |
| `example2.kpl` | `result2.txt` | Hàm đệ quy tính giai thừa (`Function F`), vòng lặp `For`, lệnh gọi I/O | `diff -u <(./parser ../test/example2.kpl) ../test/result2.txt` |
| `example3.kpl` | `result3.txt` | Thuật toán Bubble Sort: Khai báo mảng, kiểu, biến, `Procedure`, `While`, `If` | `diff -u <(./parser ../test/example3.kpl) ../test/result3.txt` |
| `example4.kpl` | `result4.txt` | Khai báo mảng, thủ tục với tham số tham biến `VAR` | `diff -u <(./parser ../test/example4.kpl) ../test/result4.txt` |
| `example5.kpl` | `result5.txt` | Khai báo biến `char`, hàm đệ quy và các câu lệnh kết hợp | `diff -u <(./parser ../test/example5.kpl) ../test/result5.txt` |
| `example6.kpl` | `result6.txt` | Chương trình kiểm thử cú pháp tổng hợp 1 | `diff -u <(./parser ../test/example6.kpl) ../test/result6.txt` |
| `example7.kpl` | `result7.txt` | Chương trình kiểm thử cú pháp tổng hợp 2 | `diff -u <(./parser ../test/example7.kpl) ../test/result7.txt` |

---

## 4. Lưu ý khi cài đặt

1. **Khái niệm `lookAhead`**: Biến toàn cục `lookAhead` luôn chứa token tiếp theo cần kiểm tra.
2. **Hàm `eat(TokenType)`**: Dùng để khớp token hiện tại với loại token mong đợi. Nếu đúng, token được in ra và con trỏ chuyển sang token kế tiếp; nếu sai, báo lỗi cú pháp tương ứng.
3. **Phân tích đệ quy xuống**: Mỗi hàm `compile*()` tương ứng với một ký hiệu chưa kết thúc (Non-terminal) trong văn phạm BNF đã loại bỏ đệ quy trái và nhân tử trái. Khi gặp các nhánh rẽ $\varepsilon$ (rỗng), cần kiểm tra xem `lookAhead->tokenType` có thuộc tập `FOLLOW` của ký hiệu đó hay không.
