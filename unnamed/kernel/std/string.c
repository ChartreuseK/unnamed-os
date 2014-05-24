#include "std/string.h"

#include "io/serialconsole.h"

#define NULL 0


int strcmp(const char *s1, const char *s2)
{
    for(;*s1 == *s2; s1++, s2++)
    {
        if(*s1 == '\0') // Both strings terminated equal
        {
            return 0;
        }
    }
    
    // The strings were not equal
    return *s1 - *s2;
    
}

char *strtok(char *str, const char *delim)
{
    static char *last = NULL;
    
    char *working = NULL;
    char *token = NULL;
    const char *curdelim = NULL;
    
    // Not sure what standard behavior for this is
    if(delim == NULL)
    {
        if(str == NULL) return last;
        else            return str;
    }
        
    // Get our working string
    if(str == NULL) working = last;
    else            working = str;
    
    if(!working) return NULL;
    
    
    
    // Now we need to strip of any leading delimiters
    for(;;working++)
    {
        
        for(curdelim = delim; *working != *curdelim; curdelim++)
        {
            if(!*curdelim) // If we've run out of delimiters
            {
                // We've found our first non delimter character, now to get out of here
                
                // A perfectly valid use of a goto! No way! :)
                goto strtok_firstchar; 
            }
        }
        
        if(!*working)
        {
            // The string contained nothing but delimiters
            return NULL;
        }
    }
strtok_firstchar:
    
    
    // working now points to the first non delimiter character of the string
    token = working;
    
    do
    {
        for(curdelim = delim; (*working != *curdelim) && *curdelim; curdelim++)
        {
            
        }
        if(*working == *curdelim && *curdelim)
        {
            // We hit another delimiter
            last = working + 1;
            *working = '\0';
            return token;
        }
        
    } while(*working++);
    
    // If we got here we went through the remaining string without hitting a delimiter
    last = NULL;
    return token;
    
}

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
        str[i++] = '-';
        
    str[i] = '\0';
    
    // Now reverse the string
    reverse(str, i);   
    return str;
}







char *uitoa(uint64_t num, char *str, int base)
{
    int i = 0;
    
    // If the num is 0 then str = "0"
    if(num == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }

    // Now we'll do the conversion, this will give us a reverse order string
    for(; num; i++)
    {
        unsigned int digit = ( num % base );
        
        if( digit >= 10 )
            str[i] = 'A' + (digit - 10);
        else
            str[i] = '0' + (digit);
            
        num /= base;
    }
      
    str[i] = '\0';
    
    // Now reverse the string
    reverse(str, i);   
    return str;
}
