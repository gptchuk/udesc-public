import System.IO
import Data.Char

type Doc = String
type Line = String
type Word' = String

data Tree = Node Word' [Int] Tree Tree | Leaf deriving Show

numLines :: [Line] -> [(Int, Line)]
numLines xs = zip [1..] xs

numWord _ [] = []
numWord n (x:xs) = (n,x) : numWord n xs

allNumWords :: [(Int, Line)] -> [(Int, Word')]
allNumWords [] = []
allNumWords ((x,y):xs) = numWord x (words y) ++ allNumWords xs

removeCh [] = []
removeCh ((x,y):xs) = (x, filter isAlpha y) : removeCh xs

removeNum [] = []
removeNum ((x,y):xs) = (x, filter (\x -> not (isDigit x)) y) : removeNum xs

removeVazio [] = []
removeVazio ((x,y):xs)
        | y == "" = removeVazio xs
        | otherwise = (x, y) : removeVazio xs

removeEsp x = removeVazio $ removeNum $ removeCh x

insOrd :: Int -> [Int] -> [Int]
insOrd e [] = [e]
insOrd e (x:xs)
      | e == x = x : xs
      | e < x = e : x : xs
      | e > x = x : insOrd e xs

ins :: Word' -> Int -> Tree -> Tree
ins palavra num Leaf = Node palavra [num] Leaf Leaf
ins palavraX numX (Node palavraY numY esq dir)
      | palavraX == palavraY = Node palavraY (insOrd numX numY) esq dir
      | palavraX < palavraY = Node palavraY numY (ins palavraX numX esq) dir
      | palavraX > palavraY = Node palavraY numY esq (ins palavraX numX dir)

duplas :: Doc -> [(Int, Word')]
duplas x = removeEsp $ allNumWords $ numLines $ lines x

mIndexTree :: [(Int, Word')] -> Tree
mIndexTree x = foldr (\(num, palavra) arvore -> ins palavra num arvore) Leaf x

makeIndexTree :: Doc -> Tree
makeIndexTree x = mIndexTree $ duplas x

todosNos Leaf = []
todosNos (Node palavra nums Leaf Leaf) = [(palavra, nums)]
todosNos (Node palavra nums esq dir) = todosNos esq ++ [(palavra, nums)] ++ todosNos dir



main :: IO()
main = do
    conteudo <- readFile "arquivo.txt"
    let arvore = todosNos $ makeIndexTree conteudo
    print arvore
