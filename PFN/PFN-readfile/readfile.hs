import System.IO
import Data.Char

type Doc = String
type Linha = String
type Palavra = String

linhas :: Doc -> [Linha]
linhas = lines

numLinhas :: [Linha] -> [(Int, Linha)]
numLinhas xs = zip [1..] xs

numeraPalavra _ [] = []
numeraPalavra n (x:xs) = (n,x) : numeraPalavra n xs

numeraPalavras :: [(Int, Linha)] -> [(Int, Palavra)]
numeraPalavras [] = []
numeraPalavras ((x,y):xs) = numeraPalavra x (words y) ++ numeraPalavras xs

removeCh [] = []
removeCh ((x,y):xs) = (x, filter isAlpha y) : removeCh xs

removeNum [] = []
removeNum ((x,y):xs) = (x, filter (\x -> not (isDigit x)) y) : removeNum xs

removem3 [] = []
removem3 ((x,y):xs)
        | length y < 3 = removem3 xs
        | otherwise = (x, y) : removem3 xs

removeEsp x =  removeNum $ removeCh $ removem3 x


inserir :: (Int, Palavra) -> [(Int, Palavra)] -> [(Int, Palavra)]
inserir a [] = [a]
inserir (a, palavraX) ((b, palavraY):zs)
      | palavraX <= palavraY = (a, palavraX) : (b, palavraY) : zs
      | otherwise = (b, palavraY) : inserir (a, palavraX) zs

ordenar :: [(Int, Palavra)] -> [(Int, Palavra)]
ordenar [] = []
ordenar (x:xs) = inserir x (ordenar xs)

agrupar:: [(Int, Palavra)] -> [([Int], Palavra)]
agrupar ((x,y):xs) = agrupar2 ([x],y) xs

agrupar2 x [] = [x]
agrupar2 (a, palavraX) ((b, palavraY):xs)
      | palavraX == palavraY = agrupar2 (a++[b],palavraX) xs
      | otherwise = (a,palavraX) : agrupar2 ([b], palavraY) xs

eliminarRep :: [([Int], Palavra)] -> [([Int], Palavra)]
eliminarRep [] = []
eliminarRep (((x:xs),palavra):zs) = (elimRep x xs,palavra) : eliminarRep zs

elimRep x [] = [x]
elimRep x (y:ys) 
      | x == y = elimRep x ys
      | otherwise = x : elimRep y ys

main :: IO()
main = do
    conteudo <- readFile "alice.txt"
    let texto = eliminarRep $ agrupar $ ordenar $ removeEsp $ numeraPalavras $ numLinhas $ linhas conteudo
    print (texto)