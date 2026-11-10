# 🛒 Projeto_GestaoLoja_C

Sistema de gestão de loja em **linguagem C**, feito para o terminal. Cadastra produtos e clientes, registra compras com baixa automática de estoque e guarda tudo em **arquivos de texto**. Os produtos podem ser ordenados com **Bubble Sort**.

> **Atividade Avaliativa 04** — une algoritmos de ordenação (plano de aula 11) com manipulação de arquivos (plano 8), usando structs, ponteiros (plano 9) e menu com `switch/case` (plano 12).

Este README tem duas partes: a **primeira** explica o que o projeto faz e como usá-lo; a **segunda** ensina os conceitos de C usados no código, com trechos reais do `Loja.c`.

---

## 📑 Sumário

1. [O que o programa faz](#-o-que-o-programa-faz)
2. [Como compilar e executar](#-como-compilar-e-executar)
3. [Menu e funcionalidades](#-menu-e-funcionalidades)
4. [Arquivos gerados e formato dos dados](#-arquivos-gerados-e-formato-dos-dados)
5. [Aprendendo com o código](#-aprendendo-com-o-código)
6. [Limitações conhecidas](#-limitações-conhecidas)
7. [Ideias para evoluir](#-ideias-para-evoluir)
8. [Licença](#-licença)

---

## 🎯 O que o programa faz

| Recurso | Descrição |
|---|---|
| **ID automático** | O usuário nunca digita o ID. O programa usa o maior ID existente + 1. |
| **Sem duplicatas** | Dois produtos (ou clientes) com o mesmo nome não são aceitos, sem diferenciar maiúsculas de minúsculas. |
| **CRUD de produtos** | Criar, listar, inserir, alterar e excluir. |
| **Busca** | Por nome, categoria ou fornecedor, aceitando só um pedaço do texto. |
| **Ordenação** | Nome A–Z, nome Z–A, preço menor → maior e preço maior → menor, com Bubble Sort. |
| **Exportar CSV** | Gera `produtos.csv`, que abre no Excel ou no Google Planilhas. |
| **Clientes** | Cadastro e listagem. |
| **Compras** | Vincula cliente e produto, calcula o total e **baixa o estoque**. |
| **Histórico** | Lista todas as vendas e soma o faturamento. |

### Dados de exemplo da atividade

```
-------------------------------------------------------------------------------
| ID | Produto          | Categoria       | Preço (R$) | Qtd | Fornecedor     |
-------------------------------------------------------------------------------
| 1  | Camiseta Básica  | Vestuário       | 49.90      | 30  | Moda Brasil    |
| 2  | Mouse Óptico USB | Informática     | 59.99      | 15  | TecnoShop      |
| 3  | Shampoo 300ml    | Higiene Pessoal | 22.50      | 40  | Beleza Pura    |
| 4  | Café 500g        | Alimentos       | 18.00      | 25  | Grãos do Sul   |
-------------------------------------------------------------------------------
```

---

## ⚙️ Como compilar e executar

Você precisa de um compilador C, como o **GCC**.

```bash
# Clonar o repositório
git clone https://github.com/<seu-usuario>/Projeto_GestaoLoja_C.git
cd Projeto_GestaoLoja_C

# Compilar
gcc Loja.c -o loja

# Executar (Linux/macOS)
./loja

# Executar (Windows)
loja.exe
```

**Primeiro uso:** escolha a opção `1` para criar os arquivos `.txt`. Depois use a `2` para cadastrar os produtos.

> 💡 Os arquivos `.txt` são criados na **pasta de onde você executa o programa**, não na pasta do código-fonte.

---

## 🧭 Menu e funcionalidades

```
===== SISTEMA DE LOJA =====
1  - Criar arquivos       -> Cria os 3 arquivos txt vazios
2  - Inserir produto      -> Cadastra um produto (ID automatico)
3  - Listar produtos      -> Mostra todos em tabela formatada
4  - Alterar produto      -> Muda os dados pelo ID
5  - Excluir produto      -> Remove do arquivo pelo ID
6  - Buscar produto       -> Busca por nome, categoria ou fornecedor
7  - Ordenar produtos     -> Usa Bubble Sort (nao altera o arquivo)
8  - Exportar CSV         -> Gera arquivo 'produtos.csv'
9  - Inserir cliente      -> Cadastra cliente (ID automatico)
10 - Listar clientes      -> Mostra todos em tabela
11 - Registrar compra     -> Vincula cliente/produto e baixa estoque
12 - Listar historico     -> Exibe todas as vendas realizadas
0  - Sair
```

### Exemplo de uso

```
Escolha uma opcao: 2
Nome: Camiseta Basica
Categoria: Vestuario
Preco: 49,90
Quantidade: 30
Fornecedor: Moda Brasil
Produto cadastrado com o ID 1!
```

O preço aceita **vírgula ou ponto** (`49,90` ou `49.90`).

### Observações de comportamento

- ⚠️ A opção **1** apaga tudo que já existe e pede confirmação (`s/n`) antes.
- A **ordenação** (opção 7) só mostra o resultado na tela. O arquivo não é modificado.
- Ao registrar uma compra, o nome do cliente e do produto é **copiado** para o histórico. Assim, o histórico continua legível mesmo se o produto for excluído depois.

---

## 🗂️ Arquivos gerados e formato dos dados

| Arquivo | Conteúdo |
|---|---|
| `produtos.txt` | Cadastro de produtos |
| `clientes.txt` | Cadastro de clientes |
| `historico.txt` | Compras realizadas |
| `produtos.csv` | Exportação dos produtos (opção 8) |
| `temp.txt` | Temporário, existe só durante gravações seguras |

Cada linha é um registro, e os campos são separados por **ponto e vírgula (`;`)**:

```
# produtos.txt  →  id;nome;categoria;preco;quantidade;fornecedor
1;Camiseta Basica;Vestuario;49.90;30;Moda Brasil
2;Mouse Optico USB;Informatica;59.99;15;TecnoShop

# clientes.txt  →  id;nome;telefone
1;Maria Silva;11999998888

# historico.txt →  id;cliente;produto;quantidade;total
1;Maria Silva;Camiseta Basica;2;99.80
```

Como o `;` é o delimitador, a função `lerTexto()` troca qualquer `;` digitado por espaço. Assim ele não bagunça as colunas.

---

## 📚 Aprendendo com o código

Esta parte explica **por que** o código é escrito assim. Os trechos vêm do próprio `Loja.c`.

### 1. Structs: agrupando dados relacionados

Um produto tem várias informações de tipos diferentes. A `struct` junta tudo em um tipo só:

```c
typedef struct {
    int id;
    char nome[50];
    char categoria[30];
    float preco;
    int quantidade;
    char fornecedor[40];
} Produto;
```

- `typedef` permite escrever `Produto` em vez de `struct Produto`.
- Strings em C são vetores de `char`. `nome[50]` guarda até **49 caracteres**, e o último espaço é do `'\0'` que marca o fim do texto.
- O projeto tem três structs: `Produto`, `Cliente` e `Compra`.

### 2. Arquivos: persistindo dados entre execuções

Variáveis somem quando o programa fecha. Arquivos guardam os dados. O fluxo é sempre **abrir → usar → fechar**:

```c
FILE *arq = fopen("produtos.txt", "a");   // abre
if (arq == NULL) { /* deu erro */ }       // sempre teste!
fprintf(arq, "%d;%s;...\n", ...);         // escreve
fclose(arq);                              // fecha
```

| Modo | Significado | Onde é usado |
|---|---|---|
| `"r"` | leitura (o arquivo precisa existir) | `carregarProdutos()` |
| `"w"` | escrita, **apaga** o conteúdo anterior | `criarArquivos()`, `salvarProdutos()` |
| `"a"` | *append*, acrescenta no final | `inserirProduto()` |

**Lendo registros com `fscanf`:**

```c
fscanf(arq, "%d;%49[^;];%29[^;];%f;%d;%39[^\n]\n", ...)
```

- `%d`, `%f`: lê um inteiro ou um real.
- `%49[^;]`: lê **até 49 caracteres, parando no próximo `;`**. É assim que se lê um texto com espaços.
- O limite (`49`) protege contra *buffer overflow*, que acontece quando o texto digitado é maior que o vetor.
- O retorno de `fscanf` é a **quantidade de campos lidos**. O `== 6` no `while` garante que a linha veio completa.

### 3. O padrão "carregar → modificar → salvar"

Quase toda função de produtos segue o mesmo caminho:

```
carregarProdutos()  →  vetor na memória  →  altera o vetor  →  salvarProdutos()
```

O vetor `Produto v[MAX]` na memória é mais fácil de manipular do que o arquivo. Para ordenar, buscar ou excluir, basta trabalhar no vetor.

### 4. Arquivo temporário: gravação segura

Em `salvarProdutos()`, o programa **não** grava direto sobre `produtos.txt`:

```c
FILE *temp = fopen("temp.txt", "w");   // 1. escreve tudo em um arquivo novo
/* ... fprintf de todos os produtos ... */
fclose(temp);
remove("produtos.txt");                // 2. apaga o original
rename("temp.txt", "produtos.txt");    // 3. o temporário vira o oficial
```

**Por quê?** Se o programa travasse no meio de uma gravação direta, o arquivo ficaria pela metade e os dados seriam perdidos. Com o temporário, o original só é trocado quando o novo já está completo.

### 5. Ponteiros: onde eles aparecem

**a) Vetores são passados como ponteiros.** Em `void mostrarProdutos(Produto *v, int n)`, o parâmetro `v` é o endereço do primeiro elemento. Por isso as funções **alteram o vetor original**, sem copiá-lo.

**b) Ponteiro para um elemento, com o operador `->`:**

```c
Produto *p = &v[pos];          // p guarda o endereço do produto encontrado
lerTexto(p->categoria, 30);    // p->campo é o mesmo que (*p).campo
p->preco = lerFloat();         // altera direto no vetor
```

**c) Strings percorridas por ponteiro**, como na função `comparar`:

```c
int comparar(char *a, char *b) {
    int i = 0;
    while (a[i] != '\0' && minus(a[i]) == minus(b[i])) i++;
    return minus(a[i]) - minus(b[i]);
}
```

O retorno funciona como o `strcmp`:

| Retorno | Significado |
|---|---|
| `0` | textos iguais |
| `< 0` | `a` vem antes de `b` no alfabeto |
| `> 0` | `a` vem depois de `b` |

Como cada letra passa por `minus()` (minúscula), `"CAFÉ"` e `"café"` são considerados iguais. Isso é o **case-insensitive**.

### 6. Bubble Sort: ordenando produtos

A ideia é comparar vizinhos e trocar quando estão fora de ordem. A cada volta, o maior elemento "flutua" para o final.

```c
for (i = 0; i < n - 1; i++) {
    for (j = 0; j < n - 1 - i; j++) {
        if (criterio == 1)      trocar = comparar(v[j].nome, v[j+1].nome) > 0;  // A-Z
        else if (criterio == 2) trocar = comparar(v[j].nome, v[j+1].nome) < 0;  // Z-A
        else if (criterio == 3) trocar = v[j].preco > v[j+1].preco;             // menor → maior
        else                    trocar = v[j].preco < v[j+1].preco;             // maior → menor

        if (trocar) { aux = v[j]; v[j] = v[j+1]; v[j+1] = aux; }
    }
}
```

**Um único Bubble Sort para 4 ordenações:** só muda a **condição de troca**. Inverter o sinal (`>` para `<`) inverte a ordem.

**Passo a passo com preços `[50, 20, 40]`:**

```
Volta 1:  [50,20,40] → troca 50↔20 → [20,50,40] → troca 50↔40 → [20,40,50]
Volta 2:  [20,40,50] → nada a trocar → pronto ✅
```

Complexidade: **O(n²)**. É simples e bom para aprender, mas lento para milhares de itens. Com no máximo 100 produtos (`MAX`), isso não é problema.

### 7. ID automático e verificação de duplicatas

Os dois acontecem no mesmo laço de `inserirProduto()`:

```c
for (i = 0; i < n; i++) {
    if (comparar(v[i].nome, novo.nome) == 0) {   // já existe?
        printf("Esse produto ja esta cadastrado!\n");
        return;
    }
    if (v[i].id > maior) maior = v[i].id;        // guarda o maior ID
}
novo.id = maior + 1;
```

Usar **maior ID + 1**, e não "quantidade + 1", evita IDs repetidos depois de exclusões. Exemplo: com os IDs 1, 2, 3, excluir o 2 deixa a quantidade em 2. O novo ID seria 3 (repetido). Com o maior ID + 1, o novo ID é 4.

### 8. Entrada de dados segura

O `scanf` deixa o `\n` no buffer e quebra leituras seguintes. Por isso o projeto lê **tudo como texto** com `fgets` e converte depois:

```c
int lerInt() {
    char buffer[50] = "0";
    fgets(buffer, 50, stdin);
    return atoi(buffer);       // texto → int
}
```

`lerTexto()` também remove o `\n` do final com `texto[strcspn(texto, "\n")] = '\0';`.

### 9. Excluindo de um vetor

Não existe "remover elemento" em um vetor de C. A solução é **deslocar** os elementos seguintes uma posição para trás e salvar com `n - 1`:

```c
for (i = pos; i < n - 1; i++) v[i] = v[i + 1];
salvarProdutos(v, n - 1);
```

### 10. Mapa das funções

```
Loja.c
├── Auxiliares   lerTexto · lerInt · lerFloat · minus · comparar · contem
├── Arquivos     carregarProdutos · salvarProdutos · carregarClientes
│                carregarCompras · criarArquivos
├── Produtos     acharProduto · inserirProduto · listarProdutos · alterarProduto
│                excluirProduto · buscarProduto · exportarCSV
├── Ordenação    bubbleSort · ordenarProdutos
├── Clientes     inserirCliente · listarClientes
├── Compras      registrarCompra · listarHistorico
└── main         menu com do-while + switch/case
```

---

## ⚠️ Limitações conhecidas

Conhecer as limitações também faz parte de aprender. Estas são as do projeto:

- **Máximo de 100 registros** por tipo (`#define MAX 100`).
- **Tamanho dos textos é limitado** (nome: 49 caracteres, categoria: 29, fornecedor: 39). O que passar disso é cortado ao ler.
- **A comparação case-insensitive** funciona bem para letras ASCII. Letras acentuadas (`É`, `Ç`) não são tratadas como equivalentes às minúsculas (`é`, `ç`).
- **Preço e quantidade não são validados** (aceita valor negativo, por exemplo). Texto inválido vira `0`, por causa do `atoi`/`atof`.
- **A busca** (`contem`) usa buffers de 50 caracteres.
- **Excluir um cliente ou produto** não afeta o histórico, de propósito.
- **Sem controle de acesso**: qualquer pessoa com acesso aos `.txt` pode editá-los.

---

## 🚀 Ideias para evoluir

- Trocar o vetor fixo por **alocação dinâmica** (`malloc`/`realloc`/`free`), removendo o limite de 100.
- Implementar **Quick Sort** ou **Merge Sort** e comparar o tempo com o Bubble Sort.
- Validar preço e quantidade (não aceitar negativos).
- Permitir **compras com vários itens** e emitir um recibo.
- Adicionar alterar e excluir **clientes**.
- Relatório de **estoque baixo** e **produto mais vendido**.
- Separar o código em vários arquivos (`produtos.c`, `clientes.c`, `main.c`) com `.h`.

---

## 📄 Licença

Distribuído sob a **Licença MIT**. Veja o arquivo [`LICENSE`](LICENSE).

© 2026 Marcos André Aragão da Cunha
