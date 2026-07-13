import sys
import os
from itertools import combinations


def ler_gramatica_do_arquivo(nome_arquivo):
    """Lê a especificação da gramática a partir de um arquivo .glc"""
    if not os.path.exists(nome_arquivo):
        print(f"Erro: O arquivo '{nome_arquivo}' não foi encontrado.")
        sys.exit(1)

    try:
        with open(nome_arquivo, 'r', encoding='utf-8') as f:
            # Filtra linhas vazias para evitar erros de leitura
            linhas = [linha.strip() for linha in f if linha.strip()]
    except Exception as e:
        print(f"Erro ao ler o arquivo: {e}")
        sys.exit(1)

    try:
        indice = 0

        # Preenche Terminais
        numero_terminais = int(linhas[indice])
        indice += 1
        terminais = set()
        for _ in range(numero_terminais):
            if linhas[indice] == '&':
                print("Erro: '&' não pode ser declarado como terminal, pois representa a palavra vazia.")
                sys.exit(1)
            terminais.add(linhas[indice])
            indice += 1

        # Preenche Não-terminais
        numero_nao_terminais = int(linhas[indice])
        indice += 1
        nao_terminais = set()
        for _ in range(numero_nao_terminais):
            nao_terminal = linhas[indice]
            if nao_terminal in terminais:
                print(f"Erro: o símbolo '{nao_terminal}' foi declarado como terminal e como não-terminal.")
                sys.exit(1)
            nao_terminais.add(nao_terminal)
            indice += 1

        # Símbolo inicial
        simbolo_inicial = linhas[indice]
        indice += 1
        if simbolo_inicial not in nao_terminais:
            print(f"Erro: o símbolo inicial '{simbolo_inicial}' não está entre os não-terminais declarados.")
            sys.exit(1)

        # Regras de produção
        numero_regras = int(linhas[indice])
        indice += 1
        regras = []
        for i in range(numero_regras):
            partes = linhas[indice].split()
            if not partes:
                print(f"Erro: regra vazia na linha {indice + 1} do arquivo.")
                sys.exit(1)

            lado_esquerdo = partes[0]
            if lado_esquerdo not in nao_terminais:
                print(f"Erro: lado esquerdo '{lado_esquerdo}' (regra {i + 1}) não é um não-terminal declarado.")
                sys.exit(1)

            lado_direito = partes[1:]

            # '&' representa explicitamente a palavra vazia; normaliza para RHS vazio
            if lado_direito == ['&']:
                lado_direito = []

            # Valida se todo símbolo do RHS foi declarado (terminal ou não-terminal)
            for simbolo in lado_direito:
                if simbolo not in terminais and simbolo not in nao_terminais:
                    print(f"Erro: símbolo '{simbolo}' usado na regra {i + 1} não foi declarado "
                          f"como terminal nem como não-terminal.")
                    sys.exit(1)

            regras.append((lado_esquerdo, lado_direito))
            indice += 1

        return terminais, nao_terminais, simbolo_inicial, regras

    except IndexError:
        print("Erro de formatação: arquivo terminou antes do esperado. "
              "Verifique se o número de linhas corresponde aos contadores (t, n, r).")
        sys.exit(1)
    except ValueError:
        print("Erro de formatação: esperava um número inteiro onde não havia. "
              "Verifique as linhas de contagem (t, n, r).")
        sys.exit(1)


def gerar_nome_livre(base, nao_terminais, terminais):
    """Gera um nome de símbolo que não colide com nenhum terminal ou não-terminal existente."""
    if base not in nao_terminais and base not in terminais:
        return base
    contador = 1
    while True:
        candidato = f"{base}_{contador}"
        if candidato not in nao_terminais and candidato not in terminais:
            return candidato
        contador += 1


def calcular_geradores(terminais, nao_terminais, regras):
    """Calcula o conjunto de não-terminais que derivam alguma cadeia de terminais."""
    geradores = set()
    alterou = True
    while alterou:
        alterou = False
        for lado_esquerdo, lado_direito in regras:
            if lado_esquerdo in geradores:
                continue
            if all(simbolo in terminais or simbolo in geradores for simbolo in lado_direito): 
                geradores.add(lado_esquerdo)
                alterou = True
    return geradores


def remover_inuteis(terminais, nao_terminais, simbolo_inicial, regras):
    """Remove símbolos não-geradores e inalcançáveis a partir do símbolo inicial."""
    # 1) não-geradores
    geradores = calcular_geradores(terminais, nao_terminais, regras)
    regras = [
        (lado_esquerdo, lado_direito) for lado_esquerdo, lado_direito in regras
        if lado_esquerdo in geradores and all(
            simbolo in terminais or simbolo in geradores for simbolo in lado_direito
        )
    ]
    nao_terminais = {nt for nt in nao_terminais if nt in geradores}

    # 2) inalcançáveis
    alcancaveis = set()
    if simbolo_inicial in nao_terminais:
        pilha = [simbolo_inicial]
        alcancaveis.add(simbolo_inicial)
        while pilha:
            atual = pilha.pop()
            for lado_esquerdo, lado_direito in regras:
                if lado_esquerdo != atual:
                    continue
                for simbolo in lado_direito:
                    if simbolo in nao_terminais and simbolo not in alcancaveis:
                        alcancaveis.add(simbolo)
                        pilha.append(simbolo)

    nao_terminais = {nt for nt in nao_terminais if nt in alcancaveis}
    regras = [
        (lado_esquerdo, lado_direito) for lado_esquerdo, lado_direito in regras
        if lado_esquerdo in alcancaveis and all(
            simbolo in terminais or simbolo in alcancaveis for simbolo in lado_direito
        )
    ]
    return nao_terminais, regras


def remover_producoes_vazias(simbolo_inicial, regras):
    """Elimina produções vazias, preservando a linguagem.
    Mantém simbolo_inicial -> [] apenas se a palavra vazia pertencer à linguagem."""

    # 1. Descobre quais símbolos são anuláveis
    anulaveis = set()
    alterou = True
    while alterou:
        alterou = False
        for lado_esquerdo, lado_direito in regras:
            if lado_esquerdo in anulaveis:
                continue
            if not lado_direito:
                anulaveis.add(lado_esquerdo)
                alterou = True
            elif all(simbolo in anulaveis for simbolo in lado_direito):
                anulaveis.add(lado_esquerdo)
                alterou = True

    # 2. Gera as novas regras considerando todas as combinações de remoção
    novas_regras = []
    vistas = set()
    for lado_esquerdo, lado_direito in regras:
        if not lado_direito:
            continue  # descarta toda regra vazia original

        # Posições (índices) dos símbolos anuláveis nesta regra
        posicoes_anulaveis = [i for i, simbolo in enumerate(lado_direito) if simbolo in anulaveis]

        # combinations gera, para cada tamanho, todas as combinações de índices a remover
        for tamanho in range(len(posicoes_anulaveis) + 1):
            for indices_remover in combinations(posicoes_anulaveis, tamanho):

                novo_lado_direito = [
                    simbolo for i, simbolo in enumerate(lado_direito) if i not in indices_remover
                ]

                if novo_lado_direito:
                    chave = (lado_esquerdo, tuple(novo_lado_direito))
                    if chave not in vistas:
                        vistas.add(chave)
                        novas_regras.append((lado_esquerdo, novo_lado_direito))
                elif lado_esquerdo == simbolo_inicial:
                    chave = (lado_esquerdo, ())
                    if chave not in vistas:
                        vistas.add(chave)
                        novas_regras.append((lado_esquerdo, []))

    return novas_regras


def remover_producoes_unitarias(nao_terminais, regras):
    """Elimina produções unitárias (A -> B, com B não-terminal)."""
    nao_terminais_ativos = {lado_esquerdo for lado_esquerdo, _ in regras} | nao_terminais
    pares_unitarios = {nt: {nt} for nt in nao_terminais_ativos}
    alterou = True
    while alterou:
        alterou = False
        for lado_esquerdo, lado_direito in regras:
            if len(lado_direito) == 1 and lado_direito[0] in nao_terminais:
                producao_origem, producao_destino = lado_esquerdo, lado_direito[0]
                for nt, alcancaveis in pares_unitarios.items():
                    if producao_origem in alcancaveis and producao_destino not in alcancaveis:
                        alcancaveis.add(producao_destino)
                        alterou = True

    regras_finais = []
    vistas = set()
    for origem, alcancaveis in pares_unitarios.items():
        for destino in alcancaveis:
            for lado_esquerdo, lado_direito in regras:
                if lado_esquerdo == destino and not (
                    len(lado_direito) == 1 and lado_direito[0] in nao_terminais
                ):
                    chave = (origem, tuple(lado_direito))
                    if chave not in vistas:
                        vistas.add(chave)
                        regras_finais.append((origem, lado_direito))
    return regras_finais


def isolar_terminais(terminais, nao_terminais, regras):
    """TERM: isola terminais em regras com 2 ou mais símbolos no lado direito."""
    terminal_para_nao_terminal = {}
    novas_regras = []
    regras_extras = []
    for lado_esquerdo, lado_direito in regras:
        if len(lado_direito) >= 2:
            novo_lado_direito = []
            for simbolo in lado_direito:
                if simbolo in terminais:
                    if simbolo not in terminal_para_nao_terminal:
                        novo_nao_terminal = gerar_nome_livre(f"T_{simbolo}", nao_terminais, terminais)
                        terminal_para_nao_terminal[simbolo] = novo_nao_terminal
                        nao_terminais.add(novo_nao_terminal)
                        regras_extras.append((novo_nao_terminal, [simbolo]))
                    novo_lado_direito.append(terminal_para_nao_terminal[simbolo])
                else:
                    novo_lado_direito.append(simbolo)
            novas_regras.append((lado_esquerdo, novo_lado_direito))
        else:
            novas_regras.append((lado_esquerdo, lado_direito))
    novas_regras.extend(regras_extras)
    return novas_regras


def binarizar(nao_terminais, terminais, regras):
    """BIN: quebra regras com lado direito > 2 símbolos em uma cadeia de regras binárias."""
    regras_binarias = []
    contador = 1
    for lado_esquerdo, lado_direito in regras:
        lado_direito = list(lado_direito)
        if len(lado_direito) <= 2:
            regras_binarias.append((lado_esquerdo, lado_direito))
            continue
        lado_esquerdo_anterior = lado_esquerdo
        for i in range(len(lado_direito) - 2):
            novo_nao_terminal = gerar_nome_livre(f"V{contador}", nao_terminais, terminais)
            contador += 1
            nao_terminais.add(novo_nao_terminal)
            regras_binarias.append((lado_esquerdo_anterior, [lado_direito[i], novo_nao_terminal]))
            lado_esquerdo_anterior = novo_nao_terminal
        regras_binarias.append((lado_esquerdo_anterior, [lado_direito[-2], lado_direito[-1]]))
    return regras_binarias


def converter_para_fnc(terminais, nao_terminais, simbolo_inicial, regras):
    """Converte a GLC para a Forma Normal de Chomsky, na ordem tradicional:
    START -> remoção de inúteis -> DEL -> UNIT -> remoção de inúteis -> TERM -> BIN."""
    nao_terminais = set(nao_terminais)
    regras = list(regras)

    # Passo 1: novo símbolo inicial (garante que não aparece em nenhum lado direito)
    novo_simbolo_inicial = gerar_nome_livre(simbolo_inicial + "'", nao_terminais, terminais)
    nao_terminais.add(novo_simbolo_inicial)
    regras = [(novo_simbolo_inicial, [simbolo_inicial])] + regras
    simbolo_inicial = novo_simbolo_inicial

    # Passo 2: remoção de símbolos inúteis (limpeza inicial)
    nao_terminais, regras = remover_inuteis(terminais, nao_terminais, simbolo_inicial, regras)

    # Passo 3: eliminação de produções vazias
    regras = remover_producoes_vazias(simbolo_inicial, regras)

    # Passo 4: eliminação de produções unitárias
    regras = remover_producoes_unitarias(nao_terminais, regras)

    # Passo 5: remoção de símbolos inúteis (limpeza final, pós DEL/UNIT)
    nao_terminais, regras = remover_inuteis(terminais, nao_terminais, simbolo_inicial, regras)

    # Passo 6: TERM - isolar terminais em regras com 2+ símbolos
    regras = isolar_terminais(terminais, nao_terminais, regras)

    # Passo 7: BIN - binarizar regras com mais de 2 símbolos no lado direito
    regras = binarizar(nao_terminais, terminais, regras)

    return simbolo_inicial, nao_terminais, regras


def imprimir_gramatica(regras, titulo, simbolo_inicial):
    """Imprime as regras de produção de forma agrupada e ordenada."""
    print(f"\n=== {titulo} ===")

    # Agrupa os lados direitos correspondentes ao mesmo lado esquerdo
    regras_agrupadas = {}
    for lado_esquerdo, lado_direito in regras:
        lado_direito_str = " ".join(lado_direito) if lado_direito else "&"
        if lado_esquerdo not in regras_agrupadas:
            regras_agrupadas[lado_esquerdo] = []
        # Evita produções duplicadas acidentais na exibição
        if lado_direito_str not in regras_agrupadas[lado_esquerdo]:
            regras_agrupadas[lado_esquerdo].append(lado_direito_str)

    # Imprime primeiro o símbolo inicial
    if simbolo_inicial in regras_agrupadas:
        print(f"{simbolo_inicial} -> {' | '.join(regras_agrupadas[simbolo_inicial])}")

    for lado_esquerdo in sorted(regras_agrupadas.keys()):
        if lado_esquerdo != simbolo_inicial:
            lado_direito_combinado = " | ".join(regras_agrupadas[lado_esquerdo])
            print(f"{lado_esquerdo} -> {lado_direito_combinado}")


def executar_cyk(regras_fnc, simbolo_inicial, tokens):
    """Executa o algoritmo CYK e imprime a tabela triangular"""

    # Se o usuário não digitou nada (Enter direto) ou digitou explicitamente '&'
    if not tokens or tokens == ['&']:
        # Verifica se o símbolo inicial tem permissão para gerar vazio []
        if (simbolo_inicial, []) in regras_fnc:
            print("RESULTADO: A palavra vazia (&) PERTENCE à linguagem gerada pela gramática!\n")
        else:
            print("RESULTADO: A palavra vazia (&) NÃO PERTENCE à linguagem gerada pela gramática.\n")
        return

    n = len(tokens)

    # Matriz CYK: tabela[tamanho][posicao]
    tabela = [[set() for _ in range(n - tam)] for tam in range(n)]

    # Base (comprimento 1)
    for i in range(n):
        for lado_esquerdo, lado_direito in regras_fnc:
            if len(lado_direito) == 1 and lado_direito[0] == tokens[i]:
                tabela[0][i].add(lado_esquerdo)

    # Indução (comprimento de 2 até n)
    for tam in range(1, n):
        for i in range(n - tam):
            for k in range(tam):
                for lado_esquerdo, lado_direito in regras_fnc:
                    if len(lado_direito) == 2:
                        B, C = lado_direito[0], lado_direito[1]
                        if B in tabela[k][i] and C in tabela[tam - 1 - k][i + k + 1]:
                            tabela[tam][i].add(lado_esquerdo)

    # Exibição da Tabela Triangular
    celulas_str = [["" for _ in range(n - tam)] for tam in range(n)]
    largura_maxima = 0

    for tam in range(n):
        for i in range(n - tam):
            conteudo = "{" + ", ".join(sorted(tabela[tam][i])) + "}" if tabela[tam][i] else "{}"
            celulas_str[tam][i] = conteudo
            largura_maxima = max(largura_maxima, len(conteudo))

    for token in tokens:
        largura_maxima = max(largura_maxima, len(token))

    largura_maxima += 3  # padding para legibilidade

    print("\n=== Tabela CYK ===")
    for tam in range(n - 1, -1, -1):
        linha_str = f"L{tam+1:<2} | "
        for i in range(n - tam):
            linha_str += f"{celulas_str[tam][i]:<{largura_maxima}}"
        print(linha_str)

    print("-" * (7 + largura_maxima * n))
    cabecalho_str = "W   | "
    for token in tokens:
        cabecalho_str += f"{token:<{largura_maxima}}"
    print(cabecalho_str)
    print()

    # Verificação de pertencimento
    if simbolo_inicial in tabela[n - 1][0]:
        print("RESULTADO: A palavra PERTENCE à linguagem gerada pela gramática!\n")
    else:
        print("RESULTADO: A palavra NÃO PERTENCE à linguagem gerada pela gramática.\n")


if __name__ == "__main__":
    # Verifica se o arquivo foi passado via linha de comando
    if len(sys.argv) > 1:
        nome_arquivo = sys.argv[1]
    else:
        nome_arquivo = input("Digite o nome ou caminho do arquivo .glc (ex: gramatica.glc): ").strip()

    print(f"\nCarregando a gramática de '{nome_arquivo}'...")
    terminais, nao_terminais, simbolo_inicial, regras = ler_gramatica_do_arquivo(nome_arquivo)

    # Processamento para FNC
    simbolo_inicial_fnc, nao_terminais_fnc, regras_fnc = converter_para_fnc(
        terminais, nao_terminais, simbolo_inicial, regras
    )
    imprimir_gramatica(regras_fnc, "Gramática Equivalente na FNC", simbolo_inicial_fnc)

    # Loop de testes no terminal
    print("\n" + "=" * 65)
    print("Gramática carregada! Pronto para testar palavras no CYK.")
    print("Digite os terminais separados por espaço (ou 'sair' para encerrar).")
    print("=" * 65)

    try:
        while True:
            linha = input("\nPalavra de teste > ").strip()
            if linha.lower() == 'sair':
                break

            tokens = [] if not linha else linha.split()
            executar_cyk(regras_fnc, simbolo_inicial_fnc, tokens)

    except (EOFError, KeyboardInterrupt):
        print("\nEncerrando o programa.")