import System.IO
import Data.Char

type Doc = String

type Line = String

type Word' = String

data Tree = Node Word' [Int] Tree Tree | Leaf deriving (Eq)

numLines :: [Line] -> [(Int, Line)]
numLines xs = zip [1 ..] xs

numWord _ [] = []
numWord n (x : xs) = (n, x) : numWord n xs

allNumWords :: [(Int, Line)] -> [(Int, Word')]
allNumWords [] = []
allNumWords ((x, y) : xs) = numWord x (words y) ++ allNumWords xs

removeCh [] = []
removeCh ((x, y) : xs) = (x, filter isAlpha y) : removeCh xs

removeNum [] = []
removeNum ((x, y) : xs) = (x, filter (\x -> not (isDigit x)) y) : removeNum xs

removeVazio [] = []
removeVazio ((x, y) : xs)
  | y == "" = removeVazio xs
  | otherwise = (x, y) : removeVazio xs

removeEsp :: [(a, [Char])] -> [(a, String)]
removeEsp x = removeVazio $ removeNum $ removeCh x

insOrd :: Int -> [Int] -> [Int]
insOrd e [] = [e]
insOrd e (x : xs)
  | e == x = x : xs
  | e < x = e : x : xs
  | e > x = x : insOrd e xs

ins :: Word' -> Int -> Tree -> Tree
ins palavra num Leaf = Node palavra [num] Leaf Leaf
ins palavraX numX (Node palavraY numY esq dir)
  | palavraX == palavraY = Node palavraY (insOrd numX numY) esq dir
  | palavraX < palavraY = Node palavraY numY (ins palavraX numX esq) dir
  | palavraX> palavraY = Node palavraY numY esq (ins palavraX numX dir)

duplas :: Doc -> [(Int, Word')]
duplas x = removeEsp $ allNumWords $ numLines $ lines x

ordenaLista :: [(Int, Word')] -> [(Int, Word')]
ordenaLista (x:xs) = ordenaLista' x xs

ordenaLista' :: (Int, Word') -> [(Int, Word')] -> [(Int, Word')]
ordenaLista' e [] = [e]
ordenaLista' (num, palavra) ((numY, palavraY):xs)
  | palavra == palavraY = (num, palavra) : (numY, palavraY) : xs
  | (num, palavra) < (numY, palavraY) = (num, palavra) : (numY, palavraY) : xs
  | (num, palavra) > (numY, palavraY) = (numY, palavraY) : ordenaLista' (num, palavra) xs

makeIndexTree :: Doc -> Tree
makeIndexTree x = foldr (\(num, palavra) arvore -> ins palavra num arvore) Leaf (duplas x)

instance Show Tree where
    show Leaf = "Leaf"
    show (Node p ints left right) =
        show p ++ " " ++ show ints ++
        "\n├─ Left: " ++  indent2 (show left) ++
        "└─ Right: " ++ indent (show right)
        
      where
        indent x = unlines $ map ("    " ++) $ lines x
        indent2 x = unlines $ map ("|    " ++) $ lines x



main :: IO ()
main = do
  conteudo <- readFile "arquivo.txt"
  let arvore = makeIndexTree conteudo
  --print arvore
  putStrLn $ show arvore