#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int add(int a, int b) 
{
    return a + b;
}

int sub(int a, int b) 
{
    return a - b;
}

int mul(int a, int b)
{
    return a * b;
}

int divide(int a, int b) 
{
    if(b == 0) 
    {
        printf("Div operation cant div for 0");
        return(EXIT_FAILURE);
    }
    return a / b;
}

typedef enum e_operation_type 
{
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_INVALID
} e_operation_type_t;

e_operation_type_t get_operation_type(const char* op_str)
{
    if(strcmp(op_str, "add") == 0 || strcmp(op_str, "+") == 0) 
    {
        return OP_ADD;
    }
    if(strcmp(op_str, "sub") == 0 || strcmp(op_str, "-") == 0) 
    {
        return OP_SUB;
    }
    if(strcmp(op_str, "mul") == 0 || strcmp(op_str, "*") == 0) 
    {
        return OP_MUL;
    }
    if(strcmp(op_str, "div") == 0 || strcmp(op_str, "/") == 0) 
    {
        return OP_DIV;
    }
    return OP_INVALID;
}

int main(int argc, char* argv[]) 
{
    int n0 = 0;
    int n1 = 0;

    e_operation_type_t op_type = OP_INVALID;
    
    int (*operation_func)(int a, int b) = NULL;

    for(int i = 1; i < argc; ++i)
    {
        if((strcmp(argv[i], "-n0") == 0) && ((i + 1) < argc)) 
        {
            n0 = atoi(argv[++i]);
        } 
        if((strcmp(argv[i], "-n1") == 0) && ((i + 1) < argc)) 
        {
            n1 = atoi(argv[++i]);
        }
        if((strcmp(argv[i], "-o") == 0) && ((i + 1) < argc)) 
        {
            op_type = get_operation_type(argv[++i]);
        }
    }

    switch(op_type) 
    {
        case OP_ADD: 
            operation_func = add;
            break;
        case OP_SUB:
            operation_func = sub;
            break;
        case OP_MUL:
            operation_func = mul;
            break;
        case OP_DIV:
            operation_func = divide;
            break;
        default:
            return(EXIT_FAILURE);
    }

    int result = operation_func(n0, n1);
    printf("Result operation: %d\n", result);

    return EXIT_SUCCESS;
}