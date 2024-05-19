# 小型语言词法分析器
## 第一部分 语言形式化描述
### 1.1 语言的字母表
$$ \Sigma=\{a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,+,-,*,/,<,=,(,),IF,\{,\},ELSE,1,2,3,4,5,6,7,8,9,0,;\} $$

### 1.2 语言的文法
定义语言的文法如下：
$$ G[S]=(V_N,V_T,P,S)\\
V_N=\{S,E,T,F,A,L,B,C,D,J,INT,VAR\}\\
V_T=\Sigma\\
P定义为:\\
S\rightarrow SA;|SJ;|A;|J;\\
\# 简单算术表达式\\
E\rightarrow E+T|E-T|T\\
T\rightarrow T*F|T/F|F\\
F\rightarrow (E)|INT|VAR\\
INT\rightarrow 1INT|2INT|3INT|4INT|5INT|6INT|7INT|8INT|9INT\\
|0|1|2|3|4|5|6|7|8|9\\
VAR\rightarrow aVAR|bVAR|cVAR|dVAR|eVAR|fVAR|gVAR|hVAR|iVAR|jVAR|kVAR|\\
lVAR|mVAR|nVAR|oVAR|pVAR|qVAR|rVAR|sVAR|tVAR|uVAR|vVAR|wVAR|xVAR|yVAR|\\
zVAR|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z\\
\# 赋值语句\\
A\rightarrow VAR=E\\
\# 逻辑表达式\\
L\rightarrow E<E|E\\
\# IF语句\\
J\rightarrow IF\{L\}\{S\}|IF\{L\}\{S\}ELSE\{S\} $$

在构建词法分析器时，暂不考虑运算优先级问题，可以适当对文法进行简化。简化后的文法如下，采用EBNF：
$$ P:\\
S\rightarrow \{(A;|J;)\}\\
E\rightarrow \{((INT|VAR)\{(+|-|*|/)(INT|VAR)\}|((INT|VAR)\{(+|-|*|/)(INT|VAR)\}))\}\\
A\rightarrow VAR:E\\
INT\rightarrow 0|(1|2|3|4|5|6|7|8|9)\{(0|1|2|3|4|5|6|7|8|9)\}\\
VAR\rightarrow \{(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z)\}\\
J\rightarrow IF\{L\}\{S\}|IF\{L\}\{S\}ELSE\{S\} \#此处的'\{'和'\}'是终结符\\ 
L\rightarrow E<E|E $$

## 第二部分 单词编码表
规定属性字长度为8位，其中前3位为类别号，后5位为属性值。

类别号表：
| 类别   | 类别号 |
| ------ | ------ |
| 关键字 | 000    |
| 标识符 | 001    |
| 常数   | 010    |
| 运算符 | 011    |
| 界限符 | 100    |

关键字表：
| 关键字 | 属性值 |
| ------ | ------ |
| IF     | 00000  |
| ELSE   | 00001  |

运算符表：
| 运算符 | 属性值 |
| ------ | ------ |
| +      | 00000  |
| -      | 00001  |
| *      | 00010  |
| /      | 00011  |
| =      | 00100  |
| <      | 00101  |
| (      | 00110  |
| )      | 00111  |

界限符表：
| 界限符 | 属性值 |
| ------ | ------ |
| {      | 00000  |
| }      | 00001  |
| ;      | 00010  |


## 第三部分 状态转换图
![状态转换图](./assets/状态转换图.png)

## 第四部分 词法分析算法
对于给定的文法，可以通过状态转换图得到词法分析算法。算法如下：
```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define keynum 2

int var_pointer = 0, num_pointer = 0, pointer = 0;
char var_table_name[32][101], num_table_value[32][101];
int var_table_addr[32];
const char keyword[keynum][100] = {"IF", "ELSE"};

void dtob(int d, char b[6])
{
    b[5] = '\0';
    int i;
    for (i = 4; i >= 0 && d > 0; i--)
    {
        b[i] = d % 2 + '0';
        d /= 2;
    }
    for (; i >= 0; i--)
    {
        b[i] = '0';
    }
}
int search_var(char var_name[100])
{
    for (int i = 0; i < var_pointer; i++)
    {
        if (strcmp(var_table_name[i], var_name) == 0)
        {
            return i;
        }
    }
    return -1;
}
int search_num(char num_value[100])
{
    for (int i = 0; i < num_pointer; i++)
    {
        if (strcmp(num_table_value[i], num_value) == 0)
        {
            return i;
        }
    }
    return -1;
}
int scanner(FILE *program, FILE *num, FILE *var, FILE *result)
{
    char c;
    c = fgetc(program);
    while (c != EOF)
    {
        while (c == ' ' || c == '\n' || c == '\t')
        {
            c = fgetc(program);
        }
        if (c >= 'a' && c <= 'z')
        {
            char var_name[100];
            memset(var_name, 0, 100);
            int i;
            for (i = 0; i < 100 && c >= 'a' && c <= 'z'; i++)
            {
                var_name[i] = c;
                c = fgetc(program);
            }
            if (i >= 100 && c >= 'a' && c <= 'z')
            {
                fprintf(stderr, "Variable name is too long\n");
                return EXIT_FAILURE;
            }
            fflush(var);
            int var_pos = search_var(var_name);
            if (var_pos != -1)
            {
                char b[6];
                dtob(var_pos, b);
                fprintf(result, "001%s", b);
            }
            else
            {
                if (var_pointer > 31)
                {
                    fprintf(stderr, "Too many variables\n");
                    return EXIT_FAILURE;
                }
                strncpy(var_table_name[var_pointer], var_name, 100);
                var_table_addr[var_pointer] = pointer;
                char b[6];
                dtob(var_pointer, b);
                fprintf(result, "001%s", b);
                var_pointer++;
                pointer += 4;
            }
        }
        else if (c >= '0' && c <= '9')
        {
            char num_value[100];
            memset(num_value, 0, 100);
            int i;
            for (int i = 0; i < 100 && c >= '0' && c <= '9'; i++)
            {
                num_value[i] = c;
                c = fgetc(program);
                if (num_value[i] == '0' && c >= '0' && c <= '9')
                {
                    fprintf(stderr, "Number can't start with 0\n");
                    return EXIT_FAILURE;
                }
            }
            if (i >= 100 && c >= '0' && c <= '9')
            {
                fprintf(stderr, "Number is too long\n");
                return EXIT_FAILURE;
            }
            int num_pos = search_num(num_value);
            if (num_pos != -1)
            {
                char b[6];
                dtob(num_pos, b);
                fprintf(result, "010%s", b);
            }
            else
            {
                if (num_pointer > 31)
                {
                    fprintf(stderr, "Too many numbers\n");
                    return EXIT_FAILURE;
                }
                strncpy(num_table_value[num_pointer], num_value, 100);
                char b[6];
                dtob(num_pointer, b);
                fprintf(result, "010%s", b);
                num_pointer++;
            }
        }
        else if (c == '+')
        {
            fprintf(result, "01100000");
            c = fgetc(program);
        }
        else if (c == '-')
        {
            fprintf(result, "01100001");
            c = fgetc(program);
        }
        else if (c == '*')
        {
            fprintf(result, "01100010");
            c = fgetc(program);
        }
        else if (c == '/')
        {
            fprintf(result, "01100011");
            c = fgetc(program);
        }
        else if (c == '=')
        {
            fprintf(result, "01100100");
            c = fgetc(program);
        }
        else if (c == '<')
        {
            fprintf(result, "01100101");
            c = fgetc(program);
        }
        else if (c == '(')
        {
            fprintf(result, "01100110");
            c = fgetc(program);
        }
        else if (c == ')')
        {
            fprintf(result, "01100111");
            c = fgetc(program);
        }
        else if (c == '{')
        {
            fprintf(result, "10000000");
            c = fgetc(program);
        }
        else if (c == '}')
        {
            fprintf(result, "10000001");
            c = fgetc(program);
        }
        else if (c == ';')
        {
            fprintf(result, "10000010");
            c = fgetc(program);
        }
        else if (c >= 'A' && c <= 'Z')
        {
            char kw[100];
            memset(kw, 0, 100);
            int i;
            for (i = 0; i < 100 && c >= 'A' && c <= 'Z'; i++)
            {
                kw[i] = c;
                c = fgetc(program);
            }
            if (i >= 100 && c >= 'A' && c <= 'Z')
            {
                fprintf(stderr, "Keyword is too long\n");
                return EXIT_FAILURE;
            }
            int flag = 0;
            for (int j = 0; j < keynum; j++)
            {
                if (strcmp(kw, keyword[j]) == 0)
                {
                    flag = 1;
                    char b[6];
                    dtob(j, b);
                    fprintf(result, "000%s", b);
                    break;
                }
            }
            if (flag == 0)
            {
                fprintf(stderr, "Invalid keyword %s\n", kw);
                return EXIT_FAILURE;
            }
        }
        else if (c == EOF)
        {
            break;
        }
        else
        {
            fprintf(stderr, "Invalid character %c\n", c);
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int main()
{
    FILE *program;
    program = fopen("pwd/program.txt", "r");
    if (program == NULL)
    {
        fprintf(stderr, "File not found\n");
        return EXIT_FAILURE;
    }
    FILE *num;
    num = fopen("pwd/number.txt", "w");
    FILE *var;
    var = fopen("pwd/variable.txt", "w");
    FILE *result;
    result = fopen("pwd/result.txt", "w");
    int return_val = scanner(program, num, var, result);
    for (int i = 0; i < var_pointer; i++)
    {
        fprintf(var, "%s,%d\n", var_table_name[i], var_table_addr[i]);
    }
    for (int i = 0; i < num_pointer; i++)
    {
        fprintf(num, "%s\n", num_table_value[i]);
    }
    fclose(program);
    fclose(num);
    fclose(var);
    fclose(result);
    return return_val;
}
```

## 第五部分 测试计划（报告）
为了对词法分析器进行测试，我写了一个代码生成器，代码如下：
```C
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define S 0
#define E 1
#define T 2
#define F 3
#define INT 4
#define VAR 5
#define A 6
#define L 7
#define J 11
#define N 12
#define LEVEL 5

FILE *output;

int generate(int op, int level)
{
    switch (op)
    {
    case S: {
        int o;
        if (level >= LEVEL)
            o = rand() % 2 + 2;
        else
            o = rand() % 4;
        switch (o)
        {
        case 0: {
            generate(S, level + 1);
            generate(A, level + 1);
            fprintf(output, ";");
            break;
        }
        case 1: {
            generate(S, level + 1);
            generate(J, level + 1);
            fprintf(output, ";");
            break;
        }
        case 2: {
            generate(A, level + 1);
            fprintf(output, ";");
            break;
        }
        case 3: {
            generate(J, level + 1);
            fprintf(output, ";");
            break;
        }
        }
        break;
    }
    case E: {
        int o;
        if (level >= LEVEL)
            o = 2;
        else
            o = rand() % 3;
        switch (o)
        {
        case 0: {
            generate(E, level + 1);
            fprintf(output, "+");
            generate(T, level + 1);
            break;
        }
        case 1: {
            generate(E, level + 1);
            fprintf(output, "-");
            generate(T, level + 1);
            break;
        }
        case 2: {
            generate(T, level + 1);
            break;
        }
        }
        break;
    }
    case T: {
        int o;
        if (level >= LEVEL)
            o = 2;
        else
            o = rand() % 3;
        switch (o)
        {
        case 0: {
            generate(T, level + 1);
            fprintf(output, "*");
            generate(F, level + 1);
            break;
        }
        case 1: {
            generate(T, level + 1);
            fprintf(output, "/");
            generate(F, level + 1);
            break;
        }
        case 2: {
            generate(F, level + 1);
            break;
        }
        }
        break;
    }
    case F: {
        int o;
        if (level >= LEVEL)
            o = rand() % 2 + 1;
        else
            o = rand() % 3;
        switch (o)
        {
        case 0: {
            fprintf(output, "(");
            generate(E, level + 1);
            fprintf(output, ")");
            break;
        }
        case 1: {
            generate(INT, level + 1);
            break;
        }
        case 2: {
            generate(VAR, level + 1);
            break;
        }
        }
        break;
    }
    case INT: {
        fprintf(output, "%d", rand() % 100);
        break;
    }
    case VAR: {
        fprintf(output, "%c", 'a' + rand() % 26);
        break;
    }
    case A: {
        generate(VAR, level + 1);
        fprintf(output, "=");
        generate(E, level + 1);
        break;
    }
    case L: {
        int o = rand() % 2;
        switch (o)
        {
        case 0: {
            generate(E, level + 1);
            fprintf(output, "<");
            generate(E, level + 1);
            break;
        }
        case 1: {
            generate(E, level + 1);
        }
        }
        break;
    }
    case J: {
        int o = rand() % 2;
        switch (o)
        {
        case 0: {
            fprintf(output, "IF{");
            generate(L, level);
            fprintf(output, "}{");
            generate(S, level);
            fprintf(output, "}");
            break;
        }
        case 1: {
            fprintf(output, "IF{");
            generate(L, level);
            fprintf(output, "}{");
            generate(S, level);
            fprintf(output, "}ELSE{");
            generate(S, level);
            fprintf(output, "}");
            break;
        }
        }
        break;
    }
    }
    return 0;
}

int main()
{
    output = fopen("pwd/program.txt", "w");
    srand(time(0));
    generate(S, 0);
    return 0;
}
```
该代码生成器可以生成一个随机的测试源代码。为了编写方便，该测试程序仅会使用单个字符的变量名。我手动修改了生成的测试代码，使其更加复杂。最终得到测试代码如下：
```
IF{av+69+s}{c=69;q=83-z;}ELSE{IF{h+x}{e=b;z=79;};};e=n+h*74+x/58;
```
词法分析器产生的结果如下：
单词串（经过手动换行处理）：
```
000000001000000000100000011000000100000001100000
001000011000000110000000001000100110010001000000
100000100010001101100100010000010110000100100100
100000101000000100000001100000000000000010000000
001001010110000000100110100000011000000000100111
011001000010100010000010001001000110010001000010
100000101000000110000010100000011000001000100111
011001000010100101100000001001010110001001000011
0110000000100110011000110100010010000010
```
变量表：
```
av,0
s,4
c,8
q,12
z,16
h,20
x,24
e,28
b,32
n,36
```
常数表：
```
69
83
79
74
58
```
当程序出错时，词法分析器会进行报错。例如，我在测试代码中加入非法字符：
```
IF@{av+69+s}{c=69;q=83-z;}ELSE{IF{h+x}{e=b;z=79;};};e=n+h*74+x/58;
```
程序报错：
```
Invalid character @
```