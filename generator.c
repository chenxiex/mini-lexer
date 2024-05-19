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