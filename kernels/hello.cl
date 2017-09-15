__kernel void hello(__global char* string)
{
  string[0] = 'H';
  string[1] = 'e';
  string[2] = 'l';
  string[3] = 'l';
  string[4] = 'o';
  string[5] = ',';
  string[6] = ' ';
  string[7] = 'W';
  string[8] = 'o';
  string[9] = 'r';
  string[10] = 'l';
  string[11] = 'd';
  string[12] = '!';
  string[13] = '\0';
}

__kernel void goodbye(__global char* string)
{
  string[0] = 'G';
  string[1] = 'o';
  string[2] = 'o';
  string[3] = 'd';
  string[4] = 'b';
  string[5] = 'y';
  string[6] = 'e';
  string[7] = 'W';
  string[8] = 'o';
  string[9] = 'r';
  string[10] = 'l';
  string[11] = 'd';
  string[12] = '!';
  string[13] = '\0';
}

__kernel void mem_ptr(__global char* string, __global ulong * ptr)
{
    ulong test_me[1];
    test_me[0] = &string[0];
    ptr[0] = &string[0];
    //char * print_me = (char *)test_me[0];
    char * print_me = (char *)ptr[0];
    //ptr[1] = &string[1];
    //string[0] = 'h';
    //string[1] = 'e';
    //printf("string: %lu\n", &string);
    //printf("string: %lu\n", *(string));
    //printf("%lu\n", (char *)ptr[0]);
    
    //print_me[0] = 'I';
    
    printf("string: %lu\n", &string[0]);
    printf("string: %s\n", string);
    
    //printf("last:\n");
    //printf("%c\n", *(string));
    //printf("%c\n", print_me[2]);
    //&string[0] = (char *)test_me[0];
}

__kernel void hello_goodbye(__global ulong * dag_ptr, uint n)
{
    char * print_me;
    printf("uint n: %i\n",n);
    for(int i = 0; i < 2; i++){
        print_me = (char *)dag_ptr[i];
        printf("%i: %s\n",i,print_me);
    }
}
