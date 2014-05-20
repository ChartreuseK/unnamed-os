#include "std/string.h"

char *reverse(char *str, int length)
{
    char temp;
    int front = 0; 
    int back = length - 1;
    
    while(front < back)
    {
        temp = str[back];
        str[back] = str[front];
        str[front] = temp;
        
        front++;
        back--;
    }
    
    return str;
}

int strlen(const char *str)
{
    const char *temp = str;
    while(*temp++);     // Loop through the string till we hit the null
    return (temp - str);
}


char *itoa(int64_t num, char *str, int base)
{
    int i = 0;
    int isnegative = 0;
    
    // If the num is 0 then str = "0"
    if(num == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }
 
    
 
    
    // Only for base 10 will we treat the number as signed
    if(num < 0 && base == 10)
    {
        isnegative = 1;
        num = -num;
    }
    
    // Now we'll do the conversion, this will give us a reverse order string
    for(; num; i++)
    {
        int digit = ( num % base );
        
        if( digit >= 10 )
            str[i] = 'A' + (digit - 10);
        else
            str[i] = '0' + (digit);
            
        num /= base;
    }
      
        
    if(isnegative)
        str[++i] = '-';
        
    str[i + 1] = '\0';
    
    // Now reverse the string
    reverse(str, i);   
    return str;
}
