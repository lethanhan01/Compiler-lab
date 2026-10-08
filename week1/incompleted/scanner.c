/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"


extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/
/* 
Ở đây em bổ sung điều kiện currentChar != EOF bởi vì:
Khi đọc đến cuối file sẽ gặp khoảng trắng/xuống dòng cuối, readChar() đọc EOF (-1). 
Biểu thức charCodes[-1] truy cập bộ nhớ ngoài mảng nhận giá trị 0 (CHAR_SPACE)
làm vòng lặp vô hạn và chương trình bị treo 
*/ 
void skipBlank() {
  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_SPACE))
    readChar();
}

void skipComment() {
  // TODO
  int state = 0;
  while ((currentChar != EOF) && (state < 2)) {
    switch (charCodes[currentChar]) {
    case CHAR_TIMES:
      state = 1;
      break;
    case CHAR_RPAR:
      if (state == 1) state = 2;
      else state = 0;
      break;
    default:
      state = 0;
    }
    readChar();
  }
  if (state != 2)
    error(ERR_ENDOFCOMMENT, lineNo, colNo);
}
/*
AI (Gemini): Cài đặt hàm skipComment bỏ qua chú thích trong KPL
*/


Token* readIdentKeyword(void) {
  // TODO
  Token *token = makeToken(TK_NONE, lineNo, colNo);
  int count = 0;

  while ((currentChar != EOF) && 
         ((charCodes[currentChar] == CHAR_LETTER) || (charCodes[currentChar] == CHAR_DIGIT))) {
    if (count < MAX_IDENT_LEN) {
      token->string[count++] = currentChar;
    } else {
      error(ERR_IDENTTOOLONG, token->lineNo, token->colNo);
      return token;
    }
    readChar();
  }
  token->string[count] = '\0';

  token->tokenType = checkKeyword(token->string);
  if (token->tokenType == TK_NONE)
    token->tokenType = TK_IDENT;

  return token;
}
/*
AI (Gemini): Viết hàm readIdentKeyword đọc định danh/từ khóa KPL cho tôi
*/

Token* readNumber(void) {
  // TODO
  Token *token = makeToken(TK_NUMBER, lineNo, colNo);
  int count = 0;

  while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_DIGIT)) {
    token->string[count++] = currentChar;
    readChar();
  }
  token->string[count] = '\0';
  token->value = atoi(token->string);
  return token;
}
/*
AI (Gemini): Cài đặt hàm readNumber giúp tôi đọc hằng số số trong KPL
*/

Token* readConstChar(void) {
  // TODO
   Token *token = makeToken(TK_CHAR, lineNo, colNo);

  readChar();
  if (currentChar == EOF) {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
    
  token->string[0] = currentChar;
  token->string[1] = '\0';

  readChar();
  if (currentChar == EOF) {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }

  if (charCodes[currentChar] == CHAR_SINGLEQUOTE) {
    readChar();
    return token;
  } else {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDCHARCONSTANT, token->lineNo, token->colNo);
    return token;
  }
}
/*
AI (Gemini): Cài đặt hàm readConstChar giúp tôi đọc hằng số ký tự trong KPL
*/

void skipSingleComment(void) {
  while ((currentChar != EOF) && (currentChar != '\n'))
    readChar();
}

Token* readConstString(void) {
  Token *token = makeToken(TK_STRING, lineNo, colNo);
  int count = 0;

  readChar();
  while ((currentChar != EOF) && (charCodes[currentChar] != CHAR_DOUBLEQUOTE)) {
    if (currentChar == '\n') {
      token->tokenType = TK_NONE;
      error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
      return token;
    }
    if (count < MAX_STRING_LEN) {
      token->string[count++] = currentChar;
    } else {
      token->tokenType = TK_NONE;
      error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
      return token;
    }
    readChar();
  }

  if (currentChar == EOF) {
    token->tokenType = TK_NONE;
    error(ERR_INVALIDSYMBOL, token->lineNo, token->colNo);
    return token;
  }

  token->string[count] = '\0';
  readChar();
  return token;
}

/*
/*
Bài 2
Gemini:
Dựa trên kết quả Bài 1, chỉnh sửa mã nguồn để bộ phân tích từ vựng nhận biết thêm các thành phần sau:
1.	Kiểu dữ liệu string: thêm từ khóa STRING và hằng xâu ký tự.
2.	Phép toán lấy phần dư: thêm toán tử %.
3.	Comment một dòng: chuỗi // bắt đầu một comment kéo dài đến hết dòng, giống C++.
4.	Kiểu dữ liệu bytes: thêm từ khóa BYTES.
5.	Phép toán lũy thừa **: ví dụ như 2**10 biểu diễn 210. Lưu ý: cần phân biệt ** với toán tử nhân *.
6.	Cấu trúc lặp REPEAT … UNTIL: thêm hai từ khóa REPEAT và UNTIL cho câu lệnh có dạng:
REPEAT <statement> UNTIL <condition>;
*/

Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;
    // ....
    // TODO
    // ....
  case CHAR_MINUS:
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar();
    return token;
  case CHAR_TIMES:
    ln = lineNo; cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_TIMES)) {
      readChar();
      return makeToken(SB_EXP, ln, cn);
    } else {
      return makeToken(SB_TIMES, ln, cn);
    }
  case CHAR_SLASH:
    ln = lineNo; cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_SLASH)) {
      readChar();
      skipSingleComment();
      return getToken();
    } else {
      return makeToken(SB_SLASH, ln, cn);
    }
  case CHAR_EQ:
    token = makeToken(SB_EQ, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COMMA:
    token = makeToken(SB_COMMA, lineNo, colNo);
    readChar();
    return token;
  case CHAR_SEMICOLON:
    token = makeToken(SB_SEMICOLON, lineNo, colNo);
    readChar();
    return token;
  case CHAR_RPAR:
    token = makeToken(SB_RPAR, lineNo, colNo);
    readChar();
    return token;
  case CHAR_COLON:
    ln = lineNo; cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return makeToken(SB_ASSIGN, ln, cn);
    } else return makeToken(SB_COLON, ln, cn);
  case CHAR_EXCLAIMATION:
    ln = lineNo; cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return makeToken(SB_NEQ, ln, cn);
    } else {
      token = makeToken(TK_NONE, ln, cn);
      error(ERR_INVALIDSYMBOL, ln, cn);
      return token;
    }
  case CHAR_LT:
    ln = lineNo; cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return makeToken(SB_LE, ln, cn);
    } else return makeToken(SB_LT, ln, cn);
  case CHAR_GT:
    ln = lineNo; cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ)) {
      readChar();
      return makeToken(SB_GE, ln, cn);
    } else return makeToken(SB_GT, ln, cn);
  case CHAR_PERIOD:
    ln = lineNo; cn = colNo;
    readChar();
    if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_RPAR)) {
      readChar();
      return makeToken(SB_RSEL, ln, cn);
    } else return makeToken(SB_PERIOD, ln, cn);
  case CHAR_LPAR:
    ln = lineNo; cn = colNo;
    readChar();
    if (currentChar == EOF)
      return makeToken(SB_LPAR, ln, cn);
    switch (charCodes[currentChar]) {
    case CHAR_PERIOD:
      readChar();
      return makeToken(SB_LSEL, ln, cn);
    case CHAR_TIMES:
      readChar();
      skipComment();
      return getToken();
    default:
      return makeToken(SB_LPAR, ln, cn);
    }
  case CHAR_SINGLEQUOTE:
    return readConstChar();
  case CHAR_DOUBLEQUOTE:
    return readConstString();
  case CHAR_PERCENT:
    token = makeToken(SB_MOD, lineNo, colNo);
    readChar();
    return token;
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%d)\n", token->value); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_STRING: printf("TK_STRING(\"%s\")\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;
  case KW_STRING: printf("KW_STRING\n"); break;
  case KW_BYTES: printf("KW_BYTES\n"); break;
  case KW_REPEAT: printf("KW_REPEAT\n"); break;
  case KW_UNTIL: printf("KW_UNTIL\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  case SB_MOD: printf("SB_MOD\n"); break;
  case SB_EXP: printf("SB_EXP\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}



