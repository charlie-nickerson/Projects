-- Programmer: Charlie Nickerson
-- Class: Cpts 355


module HW2
     where

{- 1. longest & longestTail &  mylongest - 20%-}

{- (a) longest - 5% -}
longest:: [[a]] -> [a]
longest [] = []
longest (x:xs) | (length(x) >= length(longest xs)) = x
             | (length(x) < length(longest xs)) = longest xs

{- (b) longestTail - 10% -}

longestTail [] = []
longestTail (x:xs)   | (length (xs) == 0) = x 
longestTail (x:y:xs) | (length(x) >= length(y)) = longestTail(x:xs)
                     | (length(x) < length(y)) = longestTail(y:xs)

{- (c) mylongest - 5% -}

mylongest xs = foldr (\x acc -> if length(x) > length(acc) then x else acc) [] xs

-----------------------------------------------------------

{- 2. getMonths & monthlyCans 20% -}
{- (a) getMonths - 10%-}
-- please don't include the myCatsLog list in your dolution file. 
getMonths [] n b = []
getMonths iL n [] = []
getMonths ((date,log):xs) n b | (length (filter ((>n).snd) (filter ((==b).fst) log)) > 0) = date:(getMonths xs n b)
                              | otherwise = getMonths xs n b

{- (b) getMonths - 10%-}
getCans (t1, t2) = (t1, (foldr ((+).snd) 0 t2))

count iL = sum (map (\(_,x) -> x) iL)

monthlyCans [] = []
monthlyCans iL = map monthlyCansHelper iL
          where
               monthlyCansHelper lst = ((fst lst), (count (snd lst)))


-----------------------------------------------------------

{- 3. convert, sumGrades, and organize  - 23% -}



--define the Grade datatype
data Grade = LETTER Char |  SCORE Int | PASS | FAIL 
              deriving (Show, Eq, Ord)

{- (a) convert - 3% -}
convert :: Grade -> Grade
convert (LETTER 'A') = SCORE 4
convert (LETTER 'B') = SCORE 3
convert (LETTER 'C') = SCORE 2
convert (LETTER 'D') = SCORE 1
convert (LETTER 'F') = SCORE 0
convert PASS = PASS
convert FAIL = FAIL
convert (SCORE 10) = (SCORE 10)
convert (SCORE 4) = (SCORE 4)
convert (SCORE 3) = (SCORE 3)
convert (SCORE 2) = (SCORE 2)
convert (SCORE 1) = (SCORE 1)
convert (SCORE 0) = (SCORE 0)

addScores (SCORE a) (SCORE b) = SCORE (a+b)
addScores (SCORE a) (PASS) = (SCORE a)
addScores (PASS) (SCORE a) = (SCORE a)
addScores (SCORE a) (FAIL) = (SCORE a)
addScores (FAIL) (SCORE a) = (SCORE a)

{- (b) sumGrades - 10% -}

-- sumGrades :: [Grade] -> Grade
sumGrades [] = SCORE 0
sumGrades (x:xs) = foldr (addScores) (SCORE 0) (map (convert) (x:xs))
{- (c) organize - 10% -}

isLetter (LETTER _) = True
isLetter _ = False

isScore (SCORE _) = True
isScore _ = False

isOther (PASS) = True
isOther (FAIL) = True
isOther _ = False

organize [] = []
organize ((t1,t2):xs) = [filter ((isScore).snd) ((t1,t2):xs), filter ((isLetter).snd) ((t1,t2):xs), filter ((isOther).snd) ((t1,t2):xs)]
-----------------------------------------------------------

{-4 - createLevelTree & listify & isBalanced  - 27%-}

data Tree a = LEAF a | NODE a (Tree a) (Tree a)
               deriving (Show, Read, Eq)

data LevelTree a = LLEAF a | LNODE (a,Int,Int) (LevelTree a) (LevelTree a)
                    deriving (Show, Read, Eq)

{- (a) createLevelTree - 12% -}

height (LEAF a) = 1
height (NODE a t1 t2) = max (1 + height t1) (1 + height t2)


createLevelTree (LEAF a) = (LLEAF a)
createLevelTree (NODE a t1 t2) = LNODE (a,(height t1),(height t2)) (createLevelTree t1) (createLevelTree t2)
-- createLevelTree :: Tree a -> LevelTree a
-- createLevelTree a = 

{- (b) listify - 6% -}
listify (LLEAF a) = [(a,0,0)]
listify (LNODE a t1 t2) = [a] ++ (listify t1) ++ (listify t2)

{- (c) isBalanced - 10% -}
levelHeight (LLEAF a) = 1
levelHeight (LNODE a t1 t2) = max (1 + levelHeight t1) (1 + levelHeight t2)

convertBool False False = False
convertBool False True = False
convertBool True False = False
convertBool True True = True

isBalanced (LLEAF a) = True
isBalanced (LNODE a t1 t2) | (((levelHeight t1) - (levelHeight t2)) == 1) = (convertBool (isBalanced t1) (isBalanced t2))
                           | (((levelHeight t1) - (levelHeight t2)) == -1) = (convertBool (isBalanced t1) (isBalanced t2))
                           | (((levelHeight t1) - (levelHeight t2)) == 0) = (convertBool (isBalanced t1) (isBalanced t2))
                           | (((levelHeight t1) - (levelHeight t2)) > 1) = False
                           | (((levelHeight t1) - (levelHeight t2)) < -1) = False




{-Testing your tree functions - 4%-}
-- include your tree examples in the test file. 
-- PROGRAMMER NOTE: Trees are in the test file


 
