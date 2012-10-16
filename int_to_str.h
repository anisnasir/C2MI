#ifndef INT_TO_STR_H_
#define INT_TO_STR_H_


char* int_to_str(int n)
{
  char *str = NULL;                    /* pointer to the string */
  int length = 1;                      /* Number of characters in string(at least 1 for terminator */
  int temp = 0;
  int sign = 1;                        /* Indicates sign of n */

  /* Check for negative value */
  if(n<0)
  {
    sign = -1;
    n = -n;
    ++length;                          /* For the minus character */
  }

  /* Increment length by number of digits in n */
  temp = n;
  do
  {
    ++length;
  }
  while((temp /= 10)>0);

  str = (char*)malloc(length);        /* Allocate space required */

  if(sign<0)                          /* If it was negative      */
    str[0] = '-';                     /* Insert the minus sign   */

  str[--length] = '\0';               /* Add the terminator to the end */

  /* Add the digits starting from the end */
  do
  {
    str[--length] = '0'+n%10;
  }while((n /= 10) > 0);

  return str;
}

#endif /*INT_TO_STR_H_*/
