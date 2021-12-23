#Programmer: Charlie Nickerson
#Class: Cpts 355
#Assignment 5

from psElements import Value, StrConstant, ArrayConstant, FunctionBody
class Stacks:
    def __init__(self, scope):
        self.scope = scope
        #stack variables
        self.opstack = []  #assuming top of the stack is the end of the list
        self.dictstack = []  #assuming top of the stack is the end of the list
        
        #The builtin operators supported by our interpreter
        self.builtin_operators = {
            "add":self.add,
            "sub":self.sub,
            "mul":self.mul,
            "eq":self.eq,
            "lt":self.lt,
            "gt":self.gt,
            "length":self.length,
            "get":self.get,
            "put":self.put,
            "dup":self.dup,
            "exch":self.exch,
            "pop":self.pop,
            "copy":self.copy,
            "count":self.count,
            "clear":self.clear,
            "stack":self.stack,
            "def":self.psDef,
            "if":self.psIf,
            "ifelse":self.psIfelse,
            "for":self.psFor

             #TO-DO in part1
        }
    #-------  Operand Stack Operators --------------
    """
        Helper function. Pops the top value from opstack and returns it.
    """
    def opPop(self):
        return self.opstack.pop()

    """
       Helper function. Pushes the given value to the opstack.
    """
    def opPush(self,value):
        self.opstack.append(value)
        
    #------- Dict Stack Operators --------------
    
    """
       Helper function. Pops the top dictionary from dictstack and returns it.
    """   
    def dictPop(self):
        return self.dictstack.pop()

    """
       Helper function. Pushes the given dictionary onto the dictstack. 
    """   
    def dictPush(self,d):
        self.dictstack.append(d)

    """
       Helper function. Adds name:value pair to the top dictionary in the dictstack.
       (Note: If the dictstack is empty, first adds an empty dictionary to the dictstack then adds the name:value to that. 
    """   
    def define(self,name,value):
        if self.dictstack == []:
            self.dictPush((0,{name:value}))
        else:
            # print(self.dictstack[len(self.dictstack)-1][1])
            self.dictstack[len(self.dictstack)-1][1][name] = value


    """
       Helper function. Searches the dictstack for a variable or function and returns its value. 
       (Starts searching at the top of the opstack; if name is not found returns None and prints an error message.
        Make sure to add '/' to the begining of the name.)
    """
    def lookup(self,name):
        name = "/" + name
        if self.scope == "dynamic":
            for tuples in reversed(self.dictstack):
                if name in tuples[1].keys():
                    return tuples[1][name]
        elif self.scope == "static":
            return self.lookupHelper(name, len(self.dictstack)-1)
        
    def lookupHelper(self,name,index):
        if name in self.dictstack[index][1]:
            return self.dictstack[index][1][name]
        else:
            return self.lookupHelper(name, self.dictstack[index][0])

    #------- Arithmetic Operators --------------
    
    """
       Pops 2 values from opstack; checks if they are numerical (int); adds them; then pushes the result back to opstack. 
    """   
    def add(self):
        if len(self.opstack) > 1:
            op1 = self.opPop()
            op2 = self.opPop()
            if isinstance(op1,int) and isinstance(op2,int):
                self.opPush(op1 + op2)
            else:
                print("Error: add - one of the operands is not a number value")
                self.opPush(op2)
                self.opPush(op1)             
        else:
            print("Error: add expects 2 operands")
 
    """
       Pop 2 values from opstack; checks if they are numerical (int); subtracts them; and pushes the result back to opstack. 
    """   
    def sub(self):
        if len(self.opstack) > 1:
            op1 = self.opPop()
            op2 = self.opPop()
            if isinstance(op1,int) and isinstance(op2,int):
                self.opPush(op2 - op1)
            else:
                print("Error: sub - one of the operands is not a number value")
                print(op2)
                print(op1)
                print(type(op1))
                print(type(op2))
                self.opPush(op2)
                self.opPush(op1)             
        else:
            print("Error: sub expects 2 operands")

    """
        Pops 2 values from opstack; checks if they are numerical (int); multiplies them; and pushes the result back to opstack. 
    """    
    def mul(self):
        if len(self.opstack) > 1:
            op1 = self.opPop()
            op2 = self.opPop()
            if isinstance(op1,int) and isinstance(op2,int):
                self.opPush(op1 * op2)
            else:
                print("Error: mul - one of the operands is not a number value")
                self.opPush(op2)
                self.opPush(op1)             
        else:
            print("Error: mul expects 2 operands")

    #---------- Comparison Operators  -----------------
    """
       Pops the top two values from the opstack; pushes "True" is they are equal, otherwise pushes "False"
    """ 
    def eq(self):
        if len(self.opstack) > 1:
            op1 = self.opPop()
            op2 = self.opPop()
            if op1 == op2:
                self.opPush(True)
            else:
                self.opPush(False)          
        else:
            print("Error: eq expects 2 operands")

    """
       Pops the top two values from the opstack; pushes "True" if the bottom value is less than the top value, otherwise pushes "False"
    """ 
    def lt(self):
        if len(self.opstack) > 1:
            op1 = self.opPop()
            op2 = self.opPop()
            if isinstance(op1,int) and isinstance(op2,int):
                if op2 < op1:
                    self.opPush(True)
                else:
                    self.opPush(False)
            else:
                print("Error: lt - one of the operands is not a number value")
                self.opPush(op2)
                self.opPush(op1)             
        else:
            print("Error: lt expects 2 operands")

    """
       Pops the top two values from the opstack; pushes "True" if the bottom value is greater than the top value, otherwise pushes "False"
    """ 
    def gt(self):
        if len(self.opstack) > 1:
            op1 = self.opPop()
            op2 = self.opPop()
            if isinstance(op1,int) and isinstance(op2,int):
                if op2 > op1:
                    self.opPush(True)
                else:
                    self.opPush(False)
            else:
                print("Error: gt - one of the operands is not a number value")
                self.opPush(op2)
                self.opPush(op1)             
        else:
            print("Error: gt expects 2 operands")

    # ------- String and Array Operators --------------
    """ 
       Pops a string or array value from the operand opstack and calculates the length of it. Pushes the length back onto the opstack.
       The `length` method should support both ArrayConstant and StrConstant values.
    """
    def length(self):
        if len(self.opstack) > 0:
            opStr = self.opPop()
            if isinstance(opStr, ArrayConstant):
                self.opPush(len(opStr.value))
            elif isinstance(opStr, StrConstant):
                self.opPush(len(opStr.value)-2)
            else:
                print("Value at the top of the stack is neither an array or a string")
                self.opPush(opStr.value)
        else:
            print("length requires the opstack to no be empty")

    """ 
        Pops a StrConstant or an ArrayConstant and an index from the operand opstack.  
        If the argument is a StrConstant, pushes the ascii value of the the character in the string at the index onto the opstack;
        If the argument is an ArrayConstant, pushes the value at the `index` of array onto the opstack;
    """
    def get(self):
        if len(self.opstack) > 1:
            opIndex = self.opPop()
            opItr = self.opPop()
            if isinstance(opItr.value, str):
                self.opPush(ord(opItr.value[opIndex+1]))
            elif isinstance(opItr.value, list):
                self.opPush(opItr.value[opIndex])
            else:
                print("Types did not match")
        else:
            print("Not enough values in the opstack")

    """
    Pops a StrConstant or ArrayConstant value, an (zero-based) `index`, and an `item` from the opstack
    If the argument is a StrConstant, replaces the character at `index` of the StrConstant's string with the character having the ASCII value of `item`.
    If the argument is an ArrayConstant, replaces the element at `index` of the ArrayConstant's list with the value `item`.
    """
    def put(self):
        if len(self.opstack) > 2:
            opItem = self.opPop()
            opIndex = self.opPop()
            opItr = self.opPop()
            if isinstance(opItr, StrConstant) and isinstance(opIndex, int):
                opItr.value = opItr.value[0:opIndex+1] + str(chr(opItem)) + opItr.value[opIndex+2:]
            elif isinstance(opItr, ArrayConstant) and isinstance(opIndex, int):
                opItr.value[opIndex] = opItem
            else:
                print("Types don't match")
        else:
            print("Not enough items in the opstack")
            
    #------- Stack Manipulation and Print Operators --------------

    """
       This function implements the Postscript "pop operator". Calls self.opPop() to pop the top value from the opstack and discards the value. 
    """
    def pop (self):
        self.opPop()

    """
       Prints the opstack. The end of the list is the top of the stack. 
    """
    def stack(self):
        n = len(self.dictstack) - 1
        print("===**opstack**===")
        for values in self.opstack.__reversed__():
            print(values)
        print( "===**dictstack**===")
        for values in self.dictstack.__reversed__():
            print("----",n,"----",values[0],"----")
            for key, value in values[1].items():
                print(key,"  ", value)
            n = n - 1
        print("=================")

    """
       Copies the top element in opstack.
    """
    def dup(self):
        self.opPush(self.opstack[-1])

    """
       Pops an integer count from opstack, copies count number of values in the opstack. 
    """
    def copy(self):
        if len(self.opstack) > 1:
            count = self.opPop()
            index = abs(count - len(self.opstack)) 
            if isinstance(index, int):
                while count > 0:
                    self.opPush(self.opstack[index])
                    index = index + 1
                    count = count - 1
            else:
                print("count is either negative or is not an integer")
        else:
            print("op stack is too small")

    """
        Counts the number of elements in the opstack and pushes the count onto the top of the opstack.
    """
    def count(self):
        self.opPush(len(self.opstack))

    """
       Clears the opstack.
    """
    def clear(self):
        self.opstack = []
        
    """
       swaps the top two elements in opstack
    """
    def exch(self):
        optop = self.opstack[-1]
        opbottom = self.opstack[-2]
        self.opstack[-1] = opbottom
        self.opstack[-2] = optop
        
    """
       Pops a name and a value from opstack, adds the name:value pair to the top dictionary by calling define.  
    """
    def psDef(self):
        value = self.opPop()
        name = self.opPop()
        self.define(name, value)


    # ------- if/ifelse Operators --------------
    """
       Implements if operator. 
       Pops the `ifbody` and the `condition` from opstack. 
       If the condition is True, evaluates the `ifbody`.  
    """
    def psIf(self):
        ifbody = self.opPop()
        condition = self.opPop()
        if condition is True:
            self.dictPush((len(self.dictstack)-1, {}))
            ifbody.apply(self)
            self.dictPop()
        # TO-DO in part2

    """
       Implements ifelse operator. 
       Pops the `elsebody`, `ifbody`, and the condition from opstack. 
       If the condition is True, evaluate `ifbody`, otherwise evaluate `elsebody`. 
    """
    def psIfelse(self):
        elsebody = self.opPop()
        ifbody = self.opPop()
        condition = self.opPop()
        if condition is True:
            self.dictPush((len(self.dictstack)-1, {}))
            ifbody.apply(self)
            self.dictPop()
        else:
            self.dictPush((len(self.dictstack)-1, {}))
            elsebody.apply(self)
            self.dictPop()
        # TO-DO in part2


    #------- Loop Operators --------------
    """
       Implements for operator.   
       Pops the `loopbody`, `end`index, `increment`, `start` index arguments from opstack; 
       loop counter starts at `start` , incremented by `increment` value, and ends at `end`. 
       for each value of loop counter, push the counter value on opstack, and  evaluate the `loopbody`. 
    """   
    def psFor(self):
        loopbody = self.opPop()
        end = self.opPop()
        increment = self.opPop()
        start = self.opPop()
        counter = start
        if increment > 0:
            while counter < end + 1:
                self.opPush(counter)
                counter = counter + increment
                self.dictPush((len(self.dictstack)-1, {}))
                loopbody.apply(self)
                self.dictPop()
        elif increment < 0:
            while counter > end - 1:
                self.opPush(counter)
                counter = counter + increment
                self.dictPush((len(self.dictstack)-1, {}))
                loopbody.apply(self)
                self.dictPop()
        
        # TO-DO in part2

    #--- used in the setup of unittests 
    def clearBoth(self):
        self.opstack[:] = []
        self.dictstack[:] = []

    def cleanTop(self):
        if len(self.opstack)>1:
            if self.opstack[-1] is None:
                self.opstack.pop()

# def main():
#     stacker = Stacks()
#     # stacker.opPush(1)
#     # stacker.psDict()
#     # print(stacker.opstack)
#     # print(stacker.opPop(), {})
#     # stacker.dictPush({})
#     # stacker.opPush("/x")
#     # stacker.opPush(10)
#     # stacker.psDef()
#     # stacker.opPush("/x")
#     # stacker.opPush(20)
#     # print(stacker.opstack)
#     # stacker.psDef()
#     # print(stacker.lookup('x'), "20")
#     # # stacker.opPush(True)
#     # stacker.opPush(1)
#     # stacker.opPush(3)
#     # stacker.opPush(4)
#     # stacker.opPush(3)
#     # print(stacker.opstack)
#     # stacker.copy()
#     # print(stacker.opstack)
#     # print(stacker.opPop(), "4")
#     # print(stacker.opPop(), "3") 
#     # print(stacker.opPop() , "1")
#     # print(stacker.opPop(), "4")
#     # print(stacker.opPop(), "3")
#     # print(stacker.opPop(), "1")
#     # print(stacker.opPop(), "True")
#     # stacker.opPush(ArrayConstant([0,1,2,100,4]))
#     # stacker.opPush(3)
#     # stacker.get()
#     # stacker.dictPush({'/v':3, '/x': 20})
#     # stacker.dictPush({'/v':4, '/x': 10})
#     # stacker.dictPush({'/v':5})
#     # print(stacker.lookup('x'))
#     # print(stacker.dictstack)
#     # stacker.opPush(StrConstant('(CptS355)'))
#     # stacker.length()
#     # stacker.opPush(StrConstant('(CptS355)'))
#     # stacker.opPush(3)
#     # stacker.get()
#     # print(stacker.opPop(), "83?")
#     # stacker.opPush(1)
#     # stacker.psDict()
#     # stacker.opPush("/x")
#     # print(stacker.opstack)
#     # stacker.opPush(3)
#     # stacker.psDef()
#     # stacker.opPush(1)
#     # stacker.psDict()
#     # print(stacker.dictstack)
#     # print(stacker.opstack)
#     # stacker.begin()
#     # stacker.opPush("/x")
#     # stacker.opPush(4)
#     # stacker.psDef()
#     # print(stacker.dictstack)
#     # stacker.end() 
#     # print(stacker.lookup('x'), 3)
#     # arr1 = ArrayConstant([3,5,5])
#     # stacker.opPush(arr1)
#     # stacker.dup()  #duplicating the array reference
#     # stacker.dup()  #duplicating the array reference
#     # stacker.dup()  #duplicating the array reference
#     # stacker.opPush(1)
#     # stacker.opPush(2)  # the item at index 1 will be replaced by 2
#     # print(stacker.opstack)
#     # stacker.put()  #put will not push back the changed array onto the opstack 
#     # stacker.opPush(2)
#     # stacker.opPush(2)  # the item at index 2 will be replaced by 2
#     # stacker.put()  #put will not push back the changed array onto the opstack 
#     # arr2 = stacker.opPop()
#     # print(arr2, arr1)  #we pop the string reference we copied with "dup"; check if it the same object
#     # print(arr2.value, [3,2,2]) 
#     str1 = StrConstant('(CptS321)')
#     stacker.opPush('/x')
#     stacker.opPush(str1)
#     stacker.psDef()  #defines x; x holds the string reference
#     stacker.opPush(stacker.lookup('x'))
#     stacker.opPush(6)
#     stacker.opPush(50)  # ascii value for '1'
#     stacker.put()  #put will not push back the changed string onto the opstack 
#     # # stacker.opPush(stacker.lookup('x'))
    

# main()