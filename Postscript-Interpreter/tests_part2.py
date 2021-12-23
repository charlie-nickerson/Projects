import unittest
from psParser import read
from psElements import  Literal, Name, StringExpr, ArrayExpr, CodeArray, Value, StrConstant, ArrayConstant, FunctionBody
from stacks import Stacks

class HW4GradingTests(unittest.TestCase):

    def setUp(self):
        #create the Stack object
        self.psstacks = Stacks()
        #clear the opstack and the dictstack
        self.psstacks.clearBoth() 
        self.psstacks.dictstack.append({})

    def compareObjectData(self,obj1,obj2):
        if type(obj1) != type(obj2):
            return False
        if isinstance(obj1,Literal):
            return obj1.value == obj2.value
        elif  isinstance(obj1,StringExpr):
            return obj1.value == obj2.value
        elif isinstance(obj1,ArrayExpr) or isinstance(obj1,CodeArray):
            for i in range(0,len(obj1.value)):
                if self.compareObjectData(obj1.value[i],obj2.value[i])== False:
                    return False
            return True
        elif isinstance(obj1,Name):
            return obj1.var_name == obj2.var_name
        elif isinstance(obj1,StrConstant):
            return obj1.value == obj2.value
        elif isinstance(obj1,ArrayConstant):
            for i in range(0,len(obj1.value)):
                if self.compareObjectData(obj1.value[i],obj2.value[i])== False:
                    return False
            return True
        elif isinstance(obj1,FunctionBody) :
            for i in range(0,len(obj1.value)):
                if self.compareObjectData(obj1.value[i],obj2.value[i])== False:
                    return False
            return True
        else:
            return obj1 == obj2

    def test_input1(self):
        testinput1 = """
            /x 3 def 
            /square {dup mul} def
            [x 1 x sub 1] /arr exch def  
            arr 0 get dup
            0 gt 
            {square} {dup} ifelse
        """
        opstackOutput = [9]
        expr_list = read(testinput1)
        for expr in expr_list:
            expr.eval(self.psstacks)
        self.assertEqual(len(self.psstacks.opstack),len(opstackOutput))
        self.assertEqual(self.psstacks.opstack,opstackOutput)
        #dictstackOutput = [{'/x': 3, '/square': FunctionBody([Name(dup), Name(mul)]), '/arr': ArrayConstant([3, -2, 1])}]

    def test_input2(self):
        testinput2 = """
            /x 1 def
            /y 2 def
            /x 10 def
            /y 20 def
            0 x 1 y {add} for
        """
        opstackOutput = [165]
        expr_list = read(testinput2)
        for expr in expr_list:
            expr.eval(self.psstacks)
        self.assertEqual(len(self.psstacks.opstack),len(opstackOutput))
        self.assertEqual(self.psstacks.opstack,opstackOutput)
        #dictstackOutput = [{'/x': 10, '/y': 20}]

    def test_input3(self):
        testinput3 = """
            /f {dup length} def
            [1 2 (322) (451) length]
            [1 -2 4 5 add (long) length]
            (123456)  f
        """
        opstackOutput = [ArrayConstant([1, 2, StrConstant('(322)'), 3]), ArrayConstant([1, -2, 9, 4]), StrConstant('(123456)'), 6]
        expr_list = read(testinput3)
        for expr in expr_list:
            expr.eval(self.psstacks)
        self.assertEqual(len(self.psstacks.opstack),len(opstackOutput))
        for i in range(0,len(opstackOutput)):
            self.assertTrue(self.compareObjectData(self.psstacks.opstack[i], opstackOutput[i]))
        #dictstackOutput =  [{'/f': FunctionBody([Name(dup), Name(length)])}]


    def test_input4(self):
        testinput4 = """
                /x 1 def
                /y 2 def
                1 dict begin
                /x 10 def
                1 dict begin /y 3 def x y end
                /y 20 def
                x y
                end
                x y
        """
        opstackOutput =  [10, 3, 10, 20, 1, 2]
        expr_list = read(testinput4)
        for expr in expr_list:
            expr.eval(self.psstacks)
        self.assertEqual(len(self.psstacks.opstack),len(opstackOutput))
        for i in range(0,len(opstackOutput)):
            self.assertTrue(self.compareObjectData(self.psstacks.opstack[i], opstackOutput[i]))
        #dictstackOutput =   [{'/x': 1, '/y': 2}]

    def test_input5(self):
        testinput5 = """
                1 2 3 4 5 count copy 15 1 1 5 {pop exch sub} for 0 eq
        """
        opstackOutput =   [1, 2, 3, 4, 5, True]
        expr_list = read(testinput5)
        for expr in expr_list:
            expr.eval(self.psstacks)
        self.assertEqual(len(self.psstacks.opstack),len(opstackOutput))
        for i in range(0,len(opstackOutput)):
            self.assertTrue(self.compareObjectData(self.psstacks.opstack[i], opstackOutput[i]))
        #dictstackOutput =   []

    def test_input6(self):
        testinput6 = """
                /str (CptS355) def 
                /loadarr {/arr exch def 0 1 arr length 1 sub {arr exch dup str exch get put } for arr} def
                [0 0 0 0 0 0 0] loadarr
        """ 
        opstackOutput =   [ArrayConstant([67, 112, 116, 83, 51, 53, 53])]
        expr_list = read(testinput6)
        for expr in expr_list:
            expr.eval(self.psstacks)
        self.assertEqual(len(self.psstacks.opstack),len(opstackOutput))
        for i in range(0,len(opstackOutput)):
            self.assertTrue(self.compareObjectData(self.psstacks.opstack[i], opstackOutput[i]))
        #dictstackOutput =   [{'/str': StrConstant((CptS355)), '/loadarr': FunctionBody([Name(/arr), Name(exch), Name(def), Literal(0), Literal(1), Name(arr), Name(length), Literal(1), Name(sub), CodeArray([Name(arr), Name(exch), Name(dup), Name(str), Name(exch), Name(get), Name(put)]), Name(for), Name(arr)]), '/arr': ArrayConstant([67, 112, 116, 83, 51, 53, 53])}]

    def test_input7(self):
        testinput7 = """
            (Vaccine)
            dup 0  get 86 eq
                {
                    (Vaccine) eq
                    { (yay) }
                    { (nope)  }
                    ifelse
                } if
        """
        opstackOutput = [StrConstant('(nope)')]
        expr_list = read(testinput7)
        for expr in expr_list:
            expr.eval(self.psstacks)
        self.assertEqual(len(self.psstacks.opstack),len(opstackOutput))
        for i in range(0,len(opstackOutput)):
            self.assertTrue(self.compareObjectData(self.psstacks.opstack[i], opstackOutput[i]))
        #dictstackOutput =   []

    def test_input8(self):
        testinput8 = """
                /n 5 def
                /fact {
                    0 dict begin
                    /n exch def
                    n 2 lt
                    { 1}
                    {n 1 sub fact n mul }
                    ifelse
                    end
                } def
                n fact
        """
        opstackOutput = [120]
        expr_list = read(testinput8)
        for expr in expr_list:
            expr.eval(self.psstacks)
        self.assertEqual(len(self.psstacks.opstack),len(opstackOutput))
        for i in range(0,len(opstackOutput)):
            self.assertTrue(self.compareObjectData(self.psstacks.opstack[i], opstackOutput[i]))
        #dictstackOutput =  [{'/n': 5, '/fact': FunctionBody([Literal(0), Name(dict), Name(begin), Name(/n), Name(exch), Name(def), Name(n), Literal(2), Name(lt), CodeArray([Literal(1)]), CodeArray([Name(n), Literal(1), Name(sub), Name(fact), Name(n), Name(mul)]), Name(ifelse), Name(end)])}]

    def test_input9(self):
        testinput9 = """
            /fact {
                    0 dict
                    begin
                        /n exch def
                        1
                        n -1 1 { mul /n n 1 sub def } for 
                    end
                } def
                6 fact
        """
        opstackOutput = [720]
        expr_list = read(testinput9)
        for expr in expr_list:
            expr.eval(self.psstacks)
        self.assertEqual(len(self.psstacks.opstack),len(opstackOutput))
        for i in range(0,len(opstackOutput)):
            self.assertTrue(self.compareObjectData(self.psstacks.opstack[i], opstackOutput[i]))
        #dictstackOutput = [{'/fact': FunctionBody([Literal(0), Name(dict), Name(begin), Name(/n), Name(exch), Name(def), Literal(1), Name(n), Literal(-1), Literal(1), CodeArray([Name(mul), Name(/n), Name(n), Literal(1), Name(sub), Name(def)]), Name(for), Name(end)])}]

    def test_input10(self):
        testinput10 = """
                /loadarray {/arr exch def 0 1 arr length 1 sub {arr exch get} for } def
                /sumArray {0 exch loadarray count n sub -1 1 {pop add} for /n n 1 add def } def
                /x 5 def
                /y 10 def
                /n 1 def
                [x y x y add] sumArray
        """
        opstackOutput = [30]
        expr_list = read(testinput10)
        for expr in expr_list:
            expr.eval(self.psstacks)
        self.assertEqual(len(self.psstacks.opstack),len(opstackOutput))
        for i in range(0,len(opstackOutput)):
            self.assertTrue(self.compareObjectData(self.psstacks.opstack[i], opstackOutput[i]))
        #dictstackOutput =  [{'/loadarray': FunctionBody([Name(/arr), Name(exch), Name(def), Literal(0), Literal(1), Name(arr), Name(length), Literal(1), Name(sub), CodeArray([Name(arr), Name(exch), Name(get)]), Name(for)]), '/sumArray': FunctionBody([Literal(0), Name(exch), Name(loadarray), Name(count), Name(n), Name(sub), Literal(-1), Literal(1), CodeArray([Name(pop), Name(add)]), Name(for), Name(/n), Name(n), Literal(1), Name(add), Name(def)]), '/x': 5, '/y': 10, '/n': 2, '/arr': ArrayConstant([5, 10, 15])}]

    def test_input11(self):
        testinput11 = """
            [49 50 51 52 53 54]
            [49 50 51 52 53 54] dup dup /arr1 exch def
            (12345)
            (12345) dup dup /str1 exch def
            arr1 1 get
            str1 1 get
            eq 
            pop
            arr1 4 100 put
            str1 4 100 put
            arr1 length
            str1 length 
            lt
        """
        opstackOutput = [ArrayConstant([49, 50, 51, 52, 53, 54]), ArrayConstant([49, 50, 51, 52, 100, 54]), ArrayConstant([49, 50, 51, 52, 100, 54]), StrConstant('(12345)'), StrConstant('(1234d)'), StrConstant('(1234d)'), False]
        expr_list = read(testinput11)
        for expr in expr_list:
            expr.eval(self.psstacks)
        self.assertEqual(len(self.psstacks.opstack),len(opstackOutput))
        for i in range(0,len(opstackOutput)):
            self.assertTrue(self.compareObjectData(self.psstacks.opstack[i], opstackOutput[i]))
        #dictstackOutput =  [{'/arr1': ArrayConstant([49, 50, 51, 52, 100, 54]), '/str1': StrConstant('(1234d)')}]


if __name__ == '__main__':
    unittest.main()

