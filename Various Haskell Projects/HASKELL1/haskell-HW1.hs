-- CptS 355 - Spring 2021 -- Homework1 - Haskell
-- Name: Charlie Nickerson
-- Collaborators: 

module HW1
     where

-- Q1(a) getUniqueRight [10 pts]: Define a recursive function getUniqueRight which takes a list as input and it
-- returns the unique values from the list.Your function should keep the last (i.e., rightmost) copies of the 
-- duplicate elements in the result. It should not sort the elements in the input list.  You may use the elem 
-- function in your implementation.

-- getUniqueRight:: Eq a => [a] -> [a]
getUniqueRightHelper [] buf = reverse buf
getUniqueRightHelper iL buf | ((elem (last iL) buf) == False) = (getUniqueRightHelper (init iL) (buf ++ ((last iL):[])))
                    | ((elem (last iL) buf) == True) = (getUniqueRightHelper (init iL) buf)

getUniqueRight [] = []
getUniqueRight iL = getUniqueRightHelper iL []

-- Q1(b) getUniqueLeft
getUniqueLeftHelper [] buf = reverse buf
getUniqueLeftHelper (x:xs) buf | ((elem x buf) == False) = (getUniqueLeftHelper xs (x:buf))
                         | ((elem x buf) == True) = (getUniqueLeftHelper xs buf)

getUniqueLeft [] = []
getUniqueLeft iL = getUniqueLeftHelper iL []

-- Q2(a) cansInLog
cansInLog [] = 0
cansInLog ((x,y):xs) = y + (cansInLog xs)                    

-- Q2(b) numCans
numCans [] y = 0
numCans (((day,year),log):xs) y | (y==year) = (cansInLog log) + (numCans xs y)
                                | otherwise = numCans xs y
-- Q2(c) getMonths

getMonthsHelper n b [] = False
getMonthsHelper n [] iL = False
getMonthsHelper n b ((brand,num):xs) | ((num > n) && (b == brand)) = True
                                     | ((num < n) && (b == brand)) = (getMonthsHelper n b xs)
                                     | (b /= brand) = (getMonthsHelper n b xs)
                                     | ((b == brand) && (num == n)) = (getMonthsHelper n b xs)

getMonths [] n b = []
getMonths iL n [] = []
getMonths ((date,log):xs) n b | ((getMonthsHelper n b log) == True) = date:(getMonths xs n b)
                              | ((getMonthsHelper n b log) == False) = (getMonths xs n b)
               
-- Q3 deepCount
deepCountHelper n [] = 0
deepCountHelper n (x:xs) | (n == x) = 1 + (deepCountHelper n xs)
                         | (n /= x) = (deepCountHelper n xs)

deepCount n [] = 0
deepCount n (x:xs) = (deepCountHelper n x) + (deepCount n xs)

-- Q4 clusterConsecutive
--clusterConsecutiveHelper [] = []




-- clusterConsecutiveHelper [] = []
-- clusterConsecutiveHelper (x:y:xs) | ((x+1) == y) = (x:y:(clusterConsecutiveHelper xs)):clusterConsecutiveHelper xs
--                                   | ((x+1) /= y) = x:(clusterConsecutiveHelper [])

clusterConsecutive [] = []
clusterConsecutive iL | (length iL == 1) = iL:[]
clusterConsecutive iL = let
     clusterHelper [] buf = ((reverse buf):[])
     clusterHelper (x:y:xs) buf | ((xs == []) && ((x+1) /= y)) = (x:buf):(clusterHelper [] (y:buf))
                                | ((xs == []) && ((x+1) == y)) = (clusterHelper [] (reverse(x:y:buf)))
                                | ((x+1) == y) = (clusterHelper (y:xs) (x:buf))
                                | ((x+1) /= y) = (reverse(x:buf)):(clusterHelper (y:xs) [])
                                
     in clusterHelper iL []


-- clusterConsecutive [] = []
-- clusterConsecutive (x:xs) = (length (clusterConsecutive (x:xs))





