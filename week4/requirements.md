# Yêu Cầu Thực Hành Tuần 4: Phân Tích Ngữ Nghĩa (2) - Xây Dựng Bảng Ký Hiệu

Tài liệu tóm tắt yêu cầu và hướng dẫn thực hành Tuần 4 môn Biên dịch (IT3323). Mục tiêu trọng tâm là tích hợp bảng ký hiệu (Symbol Table) vào bộ phân tích cú pháp (Parser) để thu thập thông tin và đăng ký các đối tượng định danh (chương trình, hằng, kiểu, biến, hàm, thủ tục, tham số).

---

## 1. Cấu trúc các tệp tin mã nguồn

Các tệp mã nguồn nằm tại thư mục `lab/week4/incompleted/`:

| Tên tệp tin | Mô tả nhiệm vụ |
| :--- | :--- |
| `Makefile` | Kịch bản biên dịch chương trình bằng `gcc` |
| `symtab.h`, `symtab.c` | Định nghĩa cấu trúc dữ liệu bảng ký hiệu và các hàm quản lý phạm vi, đối tượng |
| `parser.h`, `parser.c` | **Tệp trọng tâm**: Duyệt cú pháp và đăng ký đối tượng vào bảng ký hiệu |
| `scanner.h`, `scanner.c` | Bộ phân tích từ vựng (Scanner) |
| `reader.h`, `reader.c` | Đọc ký tự từ tệp nguồn KPL |
| `charcode.h`, `charcode.c` | Bảng phân loại mã ký tự |
| `token.h`, `token.c` | Định nghĩa token và từ khóa |
| `error.h`, `error.c` | Quản lý mã lỗi và thông báo lỗi |
| `debug.h`, `debug.c` | Hàm in cấu trúc bảng ký hiệu (`printObject`) để kiểm tra kết quả |
| `main.c` | Điểm vào chương trình, nhận file nguồn và kích hoạt `compile()` |

---

## 2. Hướng dẫn biên dịch và kiểm thử

### 2.1. Biên dịch

Mở terminal tại thư mục mã nguồn:

```bash
cd lab/week4/incompleted
make clean
make
```

Tệp thực thi `parser` sẽ được tạo ra tại `lab/week4/incompleted/parser`.

### 2.2. Kiểm thử thủ công từng bài kiểm tra

Chạy parser với từng file mã nguồn KPL và đối chiếu kết quả đầu ra với file mẫu tương ứng trong thư mục `lab/week4/tests/`:

```bash
# Test 1
./parser ../tests/example1.kpl > my_result1.txt
diff -u my_result1.txt ../tests/result1.txt

# Test 2
./parser ../tests/example2.kpl > my_result2.txt
diff -u my_result2.txt ../tests/result2.txt

# Test 3
./parser ../tests/example3.kpl > my_result3.txt
diff -u my_result3.txt ../tests/result3.txt

# Test 4
./parser ../tests/example4.kpl > my_result4.txt
diff -u my_result4.txt ../tests/result4.txt

# Test 5
./parser ../tests/example5.kpl > my_result5.txt
diff -u my_result5.txt ../tests/result5.txt

# Test 6
./parser ../tests/example6.kpl > my_result6.txt
diff -u my_result6.txt ../tests/result6.txt
```

> **Lưu ý**: Nếu lệnh `diff` không in ra nội dung khác biệt, chương trình đã vượt qua ca kiểm thử.

---

## 3. Nội dung công việc chi tiết

Hoàn thiện các hàm có đánh dấu `// TODO` trong `symtab.c` và `parser.c`. Giả định mã nguồn đầu vào hợp lệ, tập trung xây dựng chính xác cây đối tượng bảng ký hiệu để khớp với kết quả in của `printObject`.

### 3.1. Hoàn thiện trong `symtab.c`

- [ ] **`lookupObject(char *name)`**:
  - Tra cứu đối tượng theo tên từ phạm vi hiện tại (`symtab->currentScope`).
  - Duyệt ngược dần ra các phạm vi bao ngoài (`scope = scope->outer`) qua hàm `findObject(scope->objList, name)`.
  - Nếu không tìm thấy trong các scope lồng nhau, tiếp tục tìm trong danh sách đối tượng toàn cục (`symtab->globalObjectList`).
  - Trả về con trỏ `Object*` nếu tìm thấy, hoặc `NULL` nếu không tồn tại.

---

### 3.2. Hoàn thiện trong `parser.c`

#### A. Khai báo Chương trình và Phạm vi chính (`Program Scope`)

- [ ] **`compileProgram()`**:
  - Đọc từ khóa `PROGRAM` và lấy tên chương trình từ `lookAhead->string`.
  - Khởi tạo đối tượng chương trình: `createProgramObject(...)`.
  - Đi vào phạm vi của chương trình: `enterBlock(program->progAttrs->scope)`.
  - Gọi `compileBlock()` để duyệt toàn bộ khai báo và thân chương trình.
  - Đi ra khỏi phạm vi chương trình: `exitBlock()`.

#### B. Khai báo Hằng, Kiểu và Biến (`compileBlock`, `compileBlock2`, `compileBlock3`)

- [ ] **`compileBlock()` (Khai báo hằng)**:
  - Khi gặp `KW_CONST`, lặp qua các khai báo: lấy tên định danh, tạo đối tượng hằng bằng `createConstantObject(name)`.
  - Gán giá trị hằng từ kết quả của `compileConstant()`.
  - Đăng ký đối tượng vào bảng ký hiệu bằng `declareObject(obj)`.
- [ ] **`compileBlock2()` (Khai báo kiểu)**:
  - Khi gặp `KW_TYPE`, lặp qua các khai báo: lấy tên định danh, tạo đối tượng kiểu bằng `createTypeObject(name)`.
  - Gán kiểu thực tế từ kết quả của `compileType()`.
  - Đăng ký đối tượng vào bảng ký hiệu bằng `declareObject(obj)`.
- [ ] **`compileBlock3()` (Khai báo biến)**:
  - Khi gặp `KW_VAR`, lặp qua các khai báo: lấy tên định danh, tạo đối tượng biến bằng `createVariableObject(name)`.
  - Gán kiểu biến từ kết quả của `compileType()`.
  - Đăng ký đối tượng vào bảng ký hiệu bằng `declareObject(obj)`.

#### C. Khai báo Hàm, Thủ tục và Tham số (`Subroutines & Parameters`)

- [ ] **`compileFuncDecl()`**:
  - Lấy tên hàm, tạo đối tượng hàm: `createFunctionObject(name)`.
  - Đăng ký đối tượng hàm vào scope hiện tại: `declareObject(funcObj)`.
  - Chuyển vào scope của hàm: `enterBlock(funcObj->funcAttrs->scope)`.
  - Duyệt danh sách tham số: `compileParams()`.
  - Duyệt kiểu trả về bằng `compileBasicType()` và gán vào `funcObj->funcAttrs->returnType`.
  - Duyệt khối lệnh cục bộ của hàm bằng `compileBlock()`.
  - Thoát khỏi scope hàm: `exitBlock()`.
- [ ] **`compileProcDecl()`**:
  - Lấy tên thủ tục, tạo đối tượng thủ tục: `createProcedureObject(name)`.
  - Đăng ký đối tượng thủ tục vào scope hiện tại: `declareObject(procObj)`.
  - Chuyển vào scope của thủ tục: `enterBlock(procObj->procAttrs->scope)`.
  - Duyệt danh sách tham số: `compileParams()`.
  - Duyệt khối lệnh cục bộ của thủ tục bằng `compileBlock()`.
  - Thoát khỏi scope thủ tục: `exitBlock()`.
- [ ] **`compileParam()`**:
  - Xác định loại tham số: `PARAM_REFERENCE` (nếu có từ khóa `VAR`) hoặc `PARAM_VALUE` (mặc định).
  - Lấy tên tham số, tạo đối tượng tham số: `createParameterObject(name, paramKind, symtab->currentScope->owner)`.
  - Duyệt kiểu cơ bản của tham số bằng `compileBasicType()` và gán vào `paramObj->paramAttrs->type`.
  - Đăng ký tham số bằng `declareObject(paramObj)` (tự động thêm vào cả `paramList` của chương trình con và `objList` của scope hiện tại).

#### D. Duyệt giá trị Hằng và Kiểu dữ liệu

- [ ] **`compileUnsignedConstant()`**:
  - `TK_NUMBER`: Trả về `makeIntConstant(currentToken->value)`.
  - `TK_IDENT`: Tra cứu đối tượng hằng qua `lookupObject(currentToken->string)` và nhân bản giá trị bằng `duplicateConstantValue(obj->constAttrs->value)`.
  - `TK_CHAR`: Trả về `makeCharConstant(currentToken->string[0])`.
- [ ] **`compileConstant()`**:
  - Xử lý dấu cộng/trừ: nếu có `SB_PLUS` hoặc `SB_MINUS`, gọi `compileConstant2()`; nếu là dấu trừ, đảo dấu giá trị nguyên (`intValue = -intValue`).
  - Nếu không có dấu: gọi `compileConstant2()` hoặc xử lý `TK_CHAR`.
- [ ] **`compileConstant2()`**:
  - `TK_IDENT`: Tra cứu hằng qua `lookupObject(currentToken->string)` và nhân bản giá trị bằng `duplicateConstantValue(...)`.
  - `TK_NUMBER`: Trả về `makeIntConstant(currentToken->value)`.
- [ ] **`compileType()`**:
  - `KW_INTEGER`: Trả về `makeIntType()`.
  - `KW_CHAR`: Trả về `makeCharType()`.
  - `TK_IDENT`: Tra cứu đối tượng kiểu qua `lookupObject(currentToken->string)` và nhân bản kiểu bằng `duplicateType(obj->typeAttrs->actualType)`.
  - `KW_ARRAY`: Đọc kích thước mảng từ `TK_NUMBER`, duyệt kiểu phần tử qua `compileType()`, trả về `makeArrayType(arraySize, elementType)`.
- [ ] **`compileBasicType()`**:
  - `KW_INTEGER`: Trả về `makeIntType()`.
  - `KW_CHAR`: Trả về `makeCharType()`.

---

## 4. Bảng ca kiểm thử và đối chiếu kết quả

| Tệp kiểm thử | Tệp kết quả chuẩn | Nội dung kiểm thử đặc trưng | Lệnh kiểm tra |
| :--- | :--- | :--- | :--- |
| `tests/example1.kpl` | `tests/result1.txt` | Chương trình rỗng chỉ có tên chương trình | `diff -u <(./parser ../tests/example1.kpl) ../tests/result1.txt` |
| `tests/example2.kpl` | `tests/result2.txt` | Khai báo biến nguyên `Var N` và hàm đệ quy `Function F` có tham số | `diff -u <(./parser ../tests/example2.kpl) ../tests/result2.txt` |
| `tests/example3.kpl` | `tests/result3.txt` | Khai báo các biến đơn và thủ tục `Procedure HANOI` có 3 tham số | `diff -u <(./parser ../tests/example3.kpl) ../tests/result3.txt` |
| `tests/example4.kpl` | `tests/result4.txt` | Khai báo hằng `MAX`, kiểu `T`, mảng `Arr(10,Int)`, thủ tục và hàm có biến cục bộ | `diff -u <(./parser ../tests/example4.kpl) ../tests/result4.txt` |
| `tests/example5.kpl` | `tests/result5.txt` | Khai báo hằng, kiểu và hàm lồng nhau có hằng/kiểu mảng cục bộ | `diff -u <(./parser ../tests/example5.kpl) ../tests/result5.txt` |
| `tests/example6.kpl` | `tests/result6.txt` | Đầy đủ hằng ký tự, mảng đa chiều, thủ tục có tham số tham biến `VAR`, định danh lồng nhau | `diff -u <(./parser ../tests/example6.kpl) ../tests/result6.txt` |
