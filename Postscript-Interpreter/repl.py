"""Parts of this code was adopted from https://composingprograms.com/. 
The code has been changed according to Postscript syntax. 
https://creativecommons.org/licenses/by-sa/3.0/
"""
try:
    import readline  # history and arrow keys for CLI
except ImportError:
    pass  # but not everyone has it
import sys

from psParser import read
from stacks import Stacks


# program start
if __name__ == '__main__':
    """Run a read-eval-print loop.
    `python3 repl.py` to start an interactive REPL using dynamic scoping.
    `python3 repl.py --static` to start an interactive REPL using dynamic scoping.
    `python3 repl.py --read` to interactively read expressions and print their Python representations.
    """
    static = len(sys.argv) >= 2 and ('--static' in sys.argv)
    read_only = len(sys.argv) >= 2 and ('--read' in sys.argv)

    #create the PostScript stacks
    if static:
        psstacks = Stacks("static")
    else:
        psstacks = Stacks("dynamic")
    while True:
        try:
            # `input` prints the prompt, waits, and returns the user's input.
            user_input = input('> ')
            expr_list = read(user_input)
            for expr in expr_list:
                if read_only:
                    print(repr(expr))
                else:
                    expr.eval(psstacks)
                psstacks.cleanTop()
            #print('opstack ', psstacks.opstack)
            #print('dictstack ' , psstacks.dictstack)
        except (SyntaxError, NameError, TypeError, Exception) as err:
            print(type(err).__name__ + ':', err)
        except (KeyboardInterrupt, EOFError):  # Ctrl-C, Ctrl-D
            print()  # blank line
            break  # exit while loop (and end program)

