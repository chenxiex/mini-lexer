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
    while (c != '#')
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
        else if (c == '#')
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