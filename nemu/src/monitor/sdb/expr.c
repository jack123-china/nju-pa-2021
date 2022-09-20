#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>



int eval(char** str ,  int p , int q);
static bool check_parentheses(char **str, int startIdx , int endIdx);
static int getlowestSymbol(char ** str, int p ,int q);

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"-",'-'},            //减号
  {"\\*",'*'},          //乘号
  {"/",'/'},            //除法
  {"\\(",'('}, //左括号
  {"\\)",')'},
  {"\\w", TK_NUM}
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
 int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  printf(" make token string = %s \n",e );
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  while (e[position] != '\0') {
    /* Try all rules one by one. */
    printf("ss = %c \n",e[position]);
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

	char c = e[position];
	char* temp = &c;
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        Token t = {} ;
        switch (rules[i].token_type) {
	  case '+':
	  case '-':
	  case '*':
	  case '/':
          case '(':
	  case ')':
             t.type = rules[i].token_type;
	     strcat(t.str, temp);
	     tokens[nr_token] = t;
	     nr_token += 1;
	     printf("parse symboy === %d \n",t.type);
	     break;
	  case TK_NUM:
             //t.type = rules[i].token_type;
	     if (tokens[nr_token - 1].type == TK_NUM ){
		strcat(tokens[nr_token - 1].str, temp);
	     }else{
		t.type = rules[i].token_type;
		strcat(t.str, temp);
		tokens[nr_token] = t;
		nr_token += 1;
	     }
	     printf("parse num = %s , nr_token = %d \n",t.str,nr_token);
             break;
	  case TK_NOTYPE:
	  case TK_EQ:
             break;	     
          default:
	    printf("rules[i].token_type = %d \n",rules[i].token_type); 
	     TODO();
        }

        break;
      }
    }
    
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
   
  //char str[32][32];

  char *str[32] ;
  for (int i = 0 ; i < 32;i++){
      str[i] = (char*)malloc(32);
  }
  for (int i = 0 ;i < nr_token;i++){
     strcpy(str[i] , tokens[i].str);
  }
  
  int result = eval(str, 0 , nr_token - 1);
  
  /* TODO: Insert codes to evaluate the expression. */
 // TODO();
  for (int i = 0 ; i < 32;i++){
       free(str[i]);
  }
  printf("%d\n",result);
  return 0;
}

int eval(char** str ,  int p , int q) {
  if ( p > q ) {
    assert(0);
  } else if (p == q){
    if (tokens[p].type != TK_NUM ) {
    	assert(0);
    }
    int value = atoi(tokens[p].str);
  
    return value;
  } else if (check_parentheses(str, p, q) == true){
    printf(" p = %d , q = %d \n",p,q);
   printf(" str p = %s ,str  q = %s \n",tokens[p].str , tokens[q].str);
    return eval( str ,  p+1 , q-1);
  }else {
    int op = getlowestSymbol(str, p ,q);
    if (op < 0) {
	printf("最dd低由县级运算符不存在\n");
    	assert(0);
    }
    printf("lowest op = %d\n", op);
    int val1 = eval(str,p, op - 1);
    int val2 = eval(str, op + 1, q);
     printf("val1 = %d val2 = %d \n",val1 , val2);
    char s = str[op][0]; 
    switch (s) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      default: assert(0);
    }
  }
  
  
}


// 判断是否括号一组
static bool check_parentheses(char **str, int startIdx , int endIdx){
   if (startIdx == 2 && endIdx == 6 ){
      printf(" str[2]= %s , str[6]= %s\n ", str[2], str[6]);
   }
   if (strcmp(str[startIdx],"(") != 0 ||  strcmp(str[endIdx],")") != 0){
     return false;
   }

   if (startIdx == 2 && endIdx == 6 ){
   	printf(" str[2]===== %s , str[6]======= %s\n ", str[2], str[6]);
   }


   int arrIdx = 0;
   int len = endIdx - startIdx + 1 ;
   char arr[len];
   for (int i = 0; i < len;i++) {
      if (!strcmp(str[i],"(")) {
         arr[arrIdx] = '(';
	 arrIdx++;
      }else if (!strcmp(str[i],")")){
         if (arrIdx == 0){
            return false;
         }
         if (arr[arrIdx - 1] != '(') {
           return false;
         }
         arrIdx--;
      }
   }
   printf("addIdx = %d \n", arrIdx); 
   return (arrIdx == 0);
}

static int getlowestSymbol(char ** str, int p ,int q) {
   int len = q - p;
   if (len == 0 ){
      return -1;
   }
   int a[len];
   int count = 0;
   int ignore = 0 ;
   for(int i = p; i <= q;i++) {
      if ( !strcmp(str[i],"(") )
         ignore = 1;
      if (!strcmp(str[i],")")  )
         ignore = 0;
      if (ignore > 0)
         continue;
      if (!strcmp(str[i],"+")    || !strcmp(str[i],"-")  || !strcmp(str[i],"*")  || !strcmp(str[i],"/") ){
         a[count] = i;
         count++;
      }
   }
   if (count == 0 ){
     return getlowestSymbol(str, p+1, q-1);
   }

   int lowest = 1000000;
   int lowIdx = 1000000;
   for(int i = 0;i < count ;i++) {
      int idx = a[i];
      int sym = str[idx][0];
      if (sym < lowest) {
        lowest = sym;
        lowIdx = idx;
      }
   }
   return lowIdx ;
}

/*
static bool check_parentheses(char *str, int startIdx , int endIdx){
   if (str[startIdx] != '(' && str[endIdx] != ')' ) {
      return false; 
   }
  
   int arrIdx = 0;
   int len = endIdx - startIdx+1;
   char arr[len];
   for (int i = 0; i < len;i++) {
      if ('(' == str[i]) {
         arr[arrIdx] = '(';
         arrIdx++;
      }else if (')' == str[i] ){
         if (arrIdx == 0){
	    return false;
	 }
	 if (arr[arrIdx] != '('){
	   return false;
	 }
	 arrIdx--;
      }
   } 
   return (arrIdx == 0);
}

//返回最小符号的索引
static int getlowestSymbol(char * str, int p ,int q) {
   int len = q - p;
   if (len == 0 ){
      return -1;
   }
   int a[len];
   int count = 0;
   int ignore = 0 ;
   for(int i = p; i <= q;i++) {
      if (str[i] == '(')
	 ignore = 1;
      if (str[i] == ')')
	 ignore = 0;
      if (ignore > 0) 
	 continue;
      if (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/'){
         a[count] = i;
	 count++;
      }	      
   }
   if (count == 0 ){
     getlowestSymbol(str, p+1, q-1);
   }

   int lowest = 100;
   int lowIdx = 10000;
   for(int i = 0;i < count ;i++) {
      int idx = a[i];
      int sym = str[idx];
      if (sym < lowest) {
        lowest = sym;
	lowIdx = idx;
      }
   }
   return lowIdx ;
}*/

