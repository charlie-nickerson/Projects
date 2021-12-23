import sys

from psParser import read
from stacks import Stacks
from colors import *

testinput1 = """
    /x 4 def
    /g { x stack } def
    /f { /x 7 def g } def
    f
    """

testinput2 = """
    /x 4 def
    (static_y) dup 7 120 put /x exch def
    /g { x stack } def
    /f { /x (dynamic_x) def g } def
    f
    """

testinput3 = """
    /m 50 def
    /n 100 def
    /egg1 {/m 25 def n} def
    /chic
    	{ /n 1 def
	      /egg2 { n stack} def
	      m  n
	      egg1
	      egg2
	    } def
    n
    chic
        """

testinput4 = """
    /x 10 def
    /A { x } def
    /C { /x 40 def A stack } def
    /B { /x 30 def /A { x } def C } def
    B
    """

testinput5 = """
    /x 2 def
    /n 5  def
    /A { 1  n -1 1 {pop x mul} for} def
    /C { /n 3 def /x 40 def A stack } def
    /B { /x 30 def /A { x } def C } def
    B
    """

testinput6 = """
    /mytrue 1 1 eq def
    /myfalse 1 2 eq def 
    /out mytrue def 
    /xand { mytrue eq {pop myfalse} {mytrue eq { myfalse } { mytrue } ifelse} ifelse dup /x exch def stack} def 
    /myput { out dup /x exch def xand } def 
    /f { /out myfalse def myput } def 
    myfalse  f
    """

testinput7 = """
    /x [1 2 3 4] def
    /A { 0 1 x length 1 sub {x exch get} for add add add } def
    /C { /x [10 20 30 40 50] def A stack } def
    /B { /x [6 7 8 9] def /A { x } def C } def
    B
    """

testinput8 = """
    /x [2 3 4 5] def
    /a 10 def  
    /A { x } def
    /C { /x [a 2 mul a 3 mul dup a 4 mul] def A  a x stack } def
    /B { /x [6 7 8 9] def /A { x } def /a 5 def C } def
    B
    """

testinput9 = """
    /x [1 2 3] def
    /g { x stack } def
    /f { /x 7 def g } def
    f
    """

testinput10 = """
    /x 45 def
    /n {2 3 sub}  def
    /A { 1  n -1 1 {pop x mul} for} def
    /C { /n [[2 3 4] [5 6 7]] def /x 40 def A stack } def
    /B { /x 30 def /A { x } def C } def
    B
    """

testinput11 = """
    /l 44 def
    /n 5  def
    /A { 1  n -1 1 {pop l mul} for} def
    /C { /n 3 def /l 40 def A stack } def
    /B { /l 30 def /A { l } def C } def
    B
    """

tests = [testinput1,testinput2,testinput3,testinput4,testinput5,testinput6,testinput7,testinput8,testinput9,testinput10,testinput11]

# program start
if __name__ == '__main__':
    
    psstacks_s = Stacks("static")  
    psstacks_d = Stacks("dynamic")  
    testnum = 1
    for testcase in tests:
        try:
            print(BOLD+BLUE+"\n-- TEST {} --".format(testnum))
            expr_list = read(testcase)
            print(BOLD+CYAN+"\nSTATIC")
            # interpret using static scoping rule
            for expr in expr_list:
                expr.eval(psstacks_s)
            print(BOLD+CYAN+"\nDYNAMIC")
            # interpret using dynamic scoping rule
            for expr in expr_list:
                expr.eval(psstacks_d)
            testnum += 1
            # clear the Stack objects 
            psstacks_s.clearBoth()
            psstacks_d.clearBoth()
        except (SyntaxError, NameError, TypeError, Exception) as err:
            print(type(err).__name__ + ':', err)