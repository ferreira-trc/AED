//
// Tomás Oliveira e Silva, AED, November 2021
//
// matched-parenthesis verification
//

#include <iostream>
#include <cstring>
#include "aStack.h"

using std::cout;
using std::cerr;
using std::endl;

int check_parenthesis(const char *s)
{
  aStack<unsigned int> stack;

  for ( unsigned int i = 0; i < strlen(s); i++)
  {
    if (s[i] == '(')
    {
      stack.push(i);
    }
    if (s[i] == ')')
    {
      if (stack.size() == 0)
      {
        return -1;
      }
      else
      {
        stack.pop();
      }      
    }
  } 
  
  return stack.size();
}

int main(int argc,char **argv)
{
  if(argc == 1)
  {
    cerr << "usage: " << argv[0] << " [arguments...]" << endl;
    cerr << "example: " << argv[0] << " 'abc(def)ghi' 'x)(y'" << endl;
    return 1;
  }
  for(int i = 1;i < argc;i++)
  {
    cout << argv[i] << endl;
    int r = check_parenthesis(argv[i]);
    if(r == 0)
      cout << "  good "<< r << endl;
    else
      cout << "  bad "<< r << endl;
  }
  return 0;
}
