import Data.List
import Data.Char

--data ArvoreBInt = Folha | No Int ArvoreBInt ArvoreBInt deriving Show

data Arvorep a = Folha | No a (Arvorep a) (Arvorep a) deriving Show  


arvoreB =
    No 4    (No 2 Folha Folha) 
            (No 10  (No 5 Folha Folha)
                    (No 15 Folha Folha))

arvoreP =
    No 'c' (No 'b' Folha Folha) 
            (No 'd'    (No 'e' Folha Folha)
                        (No 'f' Folha Folha))


todosNos Folha = []
todosNos (No n Folha Folha) = [n]
todosNos (No n esq dir) = n : todosNos esq ++ todosNos dir

folhas Folha = []
folhas (No n Folha Folha) = [n]
folhas (No n esq dir) = folhas esq ++ folhas dir

soNos Folha = []
soNos (No n Folha Folha) = []
soNos (No n esq dir) = n : soNos esq ++ soNos dir

--aula 2
noUnico x = No x Folha Folha
insereArv x Folha = noUnico x
insereArv x (No a esq dir)
    | x == a = No a esq dir
    | x < a = No a (noUnico x) dir
    | x > a = No a esq (noUnico x)

ordena palavras = sortOn (map toLower) palavras

main :: IO()
main = do
    print "-------------------------------"
    print (todosNos arvoreB)
    print (todosNos arvoreP)
    print "-------------------------------"
    print (folhas arvoreB)
    print (folhas arvoreP)
    print "-------------------------------"
    print (soNos arvoreB)
    print (soNos arvoreP)
    print "-------------------------------"
    print "-------------aula 2------------"
    print (insereArv 2 (No 4 Folha Folha))
    print (insereArv 5 (No 4 Folha Folha))

    print "-------------------------------"
    print (ordena ["Asagga","aIUNI","Uafklna","unKJNASK"])
    print "-------------------------------"