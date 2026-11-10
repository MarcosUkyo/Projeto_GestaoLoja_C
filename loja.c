// =========================================================
// TRABALHO DE LINGUAGEM C: Sistema de Loja com Arquivos e Bubble Sort
// =========================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100     // Limite maximo de registros em cada vetor na memoria

// Estrutura para representar os Produtos
typedef struct {
    int id;
    char nome[50];       
    char categoria[30];
    float preco;
    int quantidade;      // Estoque atual
    char fornecedor[40]; 
} Produto;

// Estrutura para representar os Clientes
typedef struct {
    int id;
    char nome[50];
    char telefone[20];
} Cliente;

// Estrutura para representar o Histórico de Compras
typedef struct {
    int id;
    char cliente[50];    // Salvamos o nome para nao perder o historico se o cliente for apagado
    char produto[50];
    int quantidade;
    float total;
} Compra;

// ---------------------------------------------------------
// FUNCOES AUXILIARES DE LEITURA E TRATAMENTO
// ---------------------------------------------------------

// Le um texto do teclado com seguranca (substitui o scanf de strings)
void lerTexto(char *texto, int tam) {
    int i;
    fgets(texto, tam, stdin);           // Le a linha inteira do teclado
    texto[strcspn(texto, "\n")] = '\0'; // Remove a quebra de linha do final
    
    // O ponto e vírgula (;) e o nosso delimitador nos arquivos txt, entao evitamos conflito
    for (i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == ';') {          
            texto[i] = ' ';
        }
    }
}

// Le um numero inteiro de forma segura convertendo de string
int lerInt() {
    char buffer[50] = "0";
    fgets(buffer, 50, stdin);
    return atoi(buffer);
}

// Le um numero real aceitando virgula ou ponto
float lerFloat() {
    char buffer[50] = "0";
    int i;
    fgets(buffer, 50, stdin);
    for (i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == ',') {
            buffer[i] = '.';            // Padroniza virgula para ponto (padrao float em C)
        }
    }
    return atof(buffer);
}

// Converte caractere para minusculo (evitando problemas com acentos via cast)
char minus(char c) {
    return tolower((unsigned char)c);
}

// Compara dois textos ignorando maiusculas e minusculas (Retorna 0 se iguais)
int comparar(char *a, char *b) {
    int i = 0;
    while (a[i] != '\0' && minus(a[i]) == minus(b[i])) {
        i++;
    }
    return minus(a[i]) - minus(b[i]);
}

// Verifica se um pedaco de texto esta dentro de outro (Case-insensitive)
int contem(char *texto, char *pedaco) {
    char t[50], p[50];
    int i;
    for (i = 0; texto[i] != '\0'; i++) t[i] = minus(texto[i]);
    t[i] = '\0';
    for (i = 0; pedaco[i] != '\0'; i++) p[i] = minus(pedaco[i]);
    p[i] = '\0';
    return strstr(t, p) != NULL;          // strstr busca substring
}

// ---------------------------------------------------------
// MANIPULACAO DE ARQUIVOS (PERSISTENCIA DE DADOS)
// ---------------------------------------------------------

// Carrega os dados do produtos.txt para o vetor na memoria
int carregarProdutos(Produto *v) {
    FILE *arq = fopen("produtos.txt", "r");
    int n = 0;
    if (arq == NULL) {
        return 0;                       // Arquivo ainda nao existe
    }
    // Le linha por linha usando mascara de formato especifica para arquivos txt estruturados
    while (n < MAX && fscanf(arq, "%d;%49[^;];%29[^;];%f;%d;%39[^\n]\n", &v[n].id, v[n].nome,
                           v[n].categoria, &v[n].preco, &v[n].quantidade, v[n].fornecedor) == 6) {
        n++;
    }
    fclose(arq);
    return n;
}

// Tecnica de seguranca: Regrava produtos.txt usando um arquivo temporario
void salvarProdutos(Produto *v, int n) {
    FILE *temp = fopen("temp.txt", "w");
    int i;
    if (temp == NULL) {
        printf("Erro ao criar arquivo temporario!\n");
        return;
    }
    for (i = 0; i < n; i++) {
        fprintf(temp, "%d;%s;%s;%.2f;%d;%s\n", v[i].id, v[i].nome, v[i].categoria,
                v[i].preco, v[i].quantidade, v[i].fornecedor);
    }
    fclose(temp);
    remove("produtos.txt");             // Apaga o arquivo original corrompido ou desatualizado
    rename("temp.txt", "produtos.txt"); // Renomeia o temporario para se tornar o oficial
}

// Carrega os dados de clientes.txt
int carregarClientes(Cliente *v) {
    FILE *arq = fopen("clientes.txt", "r");
    int n = 0;
    if (arq == NULL) {
        return 0;
    }
    while (n < MAX && fscanf(arq, "%d;%49[^;];%19[^\n]\n", &v[n].id, v[n].nome, v[n].telefone) == 3) {
        n++;
    }
    fclose(arq);
    return n;
}

// Carrega os dados de historico.txt
int carregarCompras(Compra *v) {
    FILE *arq = fopen("historico.txt", "r");
    int n = 0;
    if (arq == NULL) {
        return 0;
    }
    while (n < MAX && fscanf(arq, "%d;%49[^;];%49[^;];%d;%f\n", &v[n].id, v[n].cliente,
                           v[n].produto, &v[n].quantidade, &v[n].total) == 5) {
        n++;
    }
    fclose(arq);
    return n;
}

// Cria os 3 arquivos principais vazios (Zera a base de dados se confirmado)
void criarArquivos() {
    FILE *arq;
    char resp[10];

    printf("Isso apaga tudo que ja existe. Continuar? (s/n): ");
    lerTexto(resp, 10);
    if (minus(resp[0]) != 's') {
        printf("Cancelado.\n");
        return;
    }
    arq = fopen("produtos.txt", "w");   // Modo "w" limpa ou cria arquivo vazio
    if (arq != NULL) fclose(arq);
    arq = fopen("clientes.txt", "w");
    if (arq != NULL) fclose(arq);
    arq = fopen("historico.txt", "w");
    if (arq != NULL) fclose(arq);
    printf("Arquivos criados com sucesso!\n");
}

// ---------------------------------------------------------
// FUNCOES DE PRODUTOS
// ---------------------------------------------------------

int acharProduto(Produto *v, int n, int id) {
    int i;
    for (i = 0; i < n; i++) {
        if (v[i].id == id) {
            return i; // Retorna o indice no vetor
        }
    }
    return -1; // Nao encontrado
}

void cabecalhoProdutos() {
    printf("\n%-5s %-25s %-18s %10s %6s  %-15s\n", "ID", "NOME", "CATEGORIA", "PRECO", "QTD", "FORNECEDOR");
    printf("-------------------------------------------------------------------------------------\n");
}

void linhaProduto(Produto *p) {
    printf("%-5d %-25.25s %-18.18s %10.2f %6d  %-15.15s\n", p->id, p->nome, p->categoria,
           p->preco, p->quantidade, p->fornecedor);
}

void mostrarProdutos(Produto *v, int n) {
    int i;
    cabecalhoProdutos();
    for (i = 0; i < n; i++) {
        linhaProduto(&v[i]);
    }
    printf("\nTotal: %d produto(s)\n", n);
}

void inserirProduto() {
    Produto v[MAX], novo;
    int n = carregarProdutos(v), i, maior = 0;
    FILE *arq;

    if (n >= MAX) {
        printf("Limite de produtos atingido!\n");
        return;
    }
    printf("Nome: ");
    lerTexto(novo.nome, 50);
    printf("Categoria: ");
    lerTexto(novo.categoria, 30);
    if (strlen(novo.nome) == 0 || strlen(novo.categoria) == 0) {
        printf("Nome e categoria nao podem ficar vazios!\n");
        return;
    }
    printf("Preco: ");
    novo.preco = lerFloat();
    printf("Quantidade: ");
    novo.quantidade = lerInt();
    printf("Fornecedor: ");
    lerTexto(novo.fornecedor, 40);
    if (strlen(novo.fornecedor) == 0) {
        printf("Fornecedor nao pode ficar vazio!\n");
        return;
    }

    // Valida duplicidade e calcula ID sequencial automatico
    for (i = 0; i < n; i++) {
        if (comparar(v[i].nome, novo.nome) == 0) {       
            printf("Esse produto ja esta cadastrado!\n");
            return;
        }
        if (v[i].id > maior) {                          
            maior = v[i].id;
        }
    }
    novo.id = maior + 1; // ID automatico incremental

    arq = fopen("produtos.txt", "a");   // Modo "a" (append) adiciona no final do arquivo
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }
    fprintf(arq, "%d;%s;%s;%.2f;%d;%s\n", novo.id, novo.nome, novo.categoria,
            novo.preco, novo.quantidade, novo.fornecedor);
    fclose(arq);
    printf("Produto cadastrado com o ID %d!\n", novo.id);
}

void listarProdutos() {
    Produto v[MAX];
    int n = carregarProdutos(v);
    if (n == 0) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }
    mostrarProdutos(v, n);
}

void alterarProduto() {
    Produto v[MAX];
    int n = carregarProdutos(v), i, pos;
    Produto *p;                        
    char nome[50];

    printf("ID do produto: ");
    pos = acharProduto(v, n, lerInt());
    if (pos == -1) {
        printf("Produto nao encontrado!\n");
        return;
    }
    p = &v[pos];

    printf("Novo nome: ");
    lerTexto(nome, 50);
    for (i = 0; i < n; i++) {           
        if (i != pos && comparar(v[i].nome, nome) == 0) {
            printf("Ja existe outro produto com esse nome!\n");
            return;
        }
    }
    printf("Nova categoria: ");
    lerTexto(p->categoria, 30);
    printf("Novo fornecedor: ");
    lerTexto(p->fornecedor, 40);
    if (strlen(nome) == 0 || strlen(p->categoria) == 0 || strlen(p->fornecedor) == 0) {
        printf("Campos nao podem ficar vazios!\n");
        return;                         
    }
    strcpy(p->nome, nome);
    printf("Novo preco: ");
    p->preco = lerFloat();
    printf("Nova quantidade: ");
    p->quantidade = lerInt();

    salvarProdutos(v, n);               // Grava as mudancas usando arquivo temporario
    printf("Produto alterado com sucesso!\n");
}

void excluirProduto() {
    Produto v[MAX];
    int n = carregarProdutos(v), i, pos;

    printf("ID do produto: ");
    pos = acharProduto(v, n, lerInt());
    if (pos == -1) {
        printf("Produto nao encontrado!\n");
        return;
    }
    // Desloca os elementos seguintes para cobrir a posicao excluida
    for (i = pos; i < n - 1; i++) {     
        v[i] = v[i + 1];
    }
    salvarProdutos(v, n - 1);           // Salva o vetor reduzindo o tamanho em 1
    printf("Produto excluido!\n");
}

void buscarProduto() {
    Produto v[MAX];
    int n = carregarProdutos(v), i, tipo, achou = 0;
    char busca[50];

    printf("Buscar por: 1 - nome | 2 - categoria | 3 - fornecedor: ");
    tipo = lerInt();
    printf("Digite o texto de busca: ");
    lerTexto(busca, 50);

    for (i = 0; i < n; i++) {
        if ((tipo == 1 && contem(v[i].nome, busca)) ||
            (tipo == 2 && contem(v[i].categoria, busca)) ||
            (tipo == 3 && contem(v[i].fornecedor, busca))) {
            if (achou == 0) {
                cabecalhoProdutos();    
            }
            linhaProduto(&v[i]);
            achou++;
        }
    }
    if (achou == 0) {
        printf("Nenhum produto encontrado.\n");
    }
}

// ---------------------------------------------------------
// ALGORITMO DE ORDENACAO: BUBBLE SORT
// ---------------------------------------------------------
void bubbleSort(Produto *v, int n, int criterio) {
    int i, j, trocar;
    Produto aux;
    
    // O algoritmo da bolha compara elementos vizinhos e empurra os maiores/menores para o fim
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            if (criterio == 1) trocar = comparar(v[j].nome, v[j + 1].nome) > 0;       // A-Z
            else if (criterio == 2) trocar = comparar(v[j].nome, v[j + 1].nome) < 0;  // Z-A
            else if (criterio == 3) trocar = v[j].preco > v[j + 1].preco;           // Menor -> Maior
            else trocar = v[j].preco < v[j + 1].preco;                              // Maior -> Menor

            if (trocar) {               // Realiza a troca fisica na memoria RAM
                aux = v[j];
                v[j] = v[j + 1];
                v[j + 1] = aux;
            }
        }
    }
}

void ordenarProdutos() {
    Produto v[MAX];
    int n = carregarProdutos(v), criterio;

    if (n == 0) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }
    printf("1 - Nome A-Z\n2 - Nome Z-A\n3 - Preco menor -> maior\n4 - Preco maior -> menor\n");
    printf("Ordenar por criterio: ");
    criterio = lerInt();
    if (criterio < 1 || criterio > 4) {
        printf("Opcao invalida!\n");
        return;
    }
    bubbleSort(v, n, criterio); // Executa o Bubble Sort
    mostrarProdutos(v, n);      // Mostra ordenado apenas na tela (o arquivo original nao muda)
}

void exportarCSV() {
    Produto v[MAX];
    int n = carregarProdutos(v), i;
    FILE *csv = fopen("produtos.csv", "w");

    if (csv == NULL) {
        printf("Erro ao criar o CSV!\n");
        return;
    }
    fprintf(csv, "id,nome,categoria,preco,quantidade,fornecedor\n");
    for (i = 0; i < n; i++) {
        fprintf(csv, "%d,\"%s\",\"%s\",%.2f,%d,\"%s\"\n", v[i].id, v[i].nome, v[i].categoria,
                v[i].preco, v[i].quantidade, v[i].fornecedor);
    }
    fclose(csv);
    printf("Arquivo 'produtos.csv' gerado com sucesso!\n");
}

// ---------------------------------------------------------
// FUNCOES DE CLIENTES
// ---------------------------------------------------------

void inserirCliente() {
    Cliente v[MAX], novo;
    int n = carregarClientes(v), i, maior = 0;
    FILE *arq;

    if (n >= MAX) {
        printf("Limite de clientes atingido!\n");
        return;
    }
    printf("Nome: ");
    lerTexto(novo.nome, 50);
    printf("Telefone: ");
    lerTexto(novo.telefone, 20);
    if (strlen(novo.nome) == 0 || strlen(novo.telefone) == 0) {
        printf("Nome e telefone nao podem ficar vazios!\n");
        return;
    }
    for (i = 0; i < n; i++) {
        if (comparar(v[i].nome, novo.nome) == 0) {
            printf("Esse cliente ja esta cadastrado!\n");
            return;
        }
        if (v[i].id > maior) {
            maior = v[i].id;
        }
    }
    novo.id = maior + 1;

    arq = fopen("clientes.txt", "a");
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }
    fprintf(arq, "%d;%s;%s\n", novo.id, novo.nome, novo.telefone);
    fclose(arq);
    printf("Cliente cadastrado com o ID %d!\n", novo.id);
}

void listarClientes() {
    Cliente v[MAX];
    int n = carregarClientes(v), i;

    if (n == 0) {
        printf("Nenhum cliente cadastrado.\n");
        return;
    }
    printf("\n%-5s %-25s %-15s\n", "ID", "NOME", "TELEFONE");
    printf("----------------------------------------------\n");
    for (i = 0; i < n; i++) {
        printf("%-5d %-25.25s %-15.15s\n", v[i].id, v[i].nome, v[i].telefone);
    }
    printf("\nTotal: %d cliente(s)\n", n);
}

// ---------------------------------------------------------
// HISTORICO DE COMPRAS E INTEGRACAO DE ESTOQUE
// ---------------------------------------------------------

void registrarCompra() {
    Produto p[MAX];
    Cliente c[MAX];
    Compra h[MAX], nova;
    int np = carregarProdutos(p), nc = carregarClientes(c), nh = carregarCompras(h);
    int i, idCliente, posCliente = -1, posProduto, qtd, maior = 0;
    Produto *prod;
    FILE *arq;

    printf("ID do cliente: ");
    idCliente = lerInt();
    for (i = 0; i < nc; i++) {
        if (c[i].id == idCliente) {
            posCliente = i;
        }
    }
    if (posCliente == -1) {
        printf("Cliente nao encontrado!\n");
        return;
    }

    printf("ID do produto: ");
    posProduto = acharProduto(p, np, lerInt());
    if (posProduto == -1) {
        printf("Produto nao encontrado!\n");
        return;
    }
    prod = &p[posProduto];
    printf("Produto: %s | Preco: R$ %.2f | Estoque disponivel: %d\n", prod->nome, prod->preco, prod->quantidade);

    printf("Quantidade desejada: ");
    qtd = lerInt();
    if (qtd <= 0 || qtd > prod->quantidade) {
        printf("Quantidade invalida ou estoque insuficiente!\n");
        return;
    }

    // Calcula ID do historico
    for (i = 0; i < nh; i++) {
        if (h[i].id > maior) {
            maior = h[i].id;
        }
    }
    nova.id = maior + 1;
    strcpy(nova.cliente, c[posCliente].nome);
    strcpy(nova.produto, prod->nome);
    nova.quantidade = qtd;
    nova.total = qtd * prod->preco;

    // Grava no arquivo de historico
    arq = fopen("historico.txt", "a");
    if (arq == NULL) {
        printf("Erro ao abrir o historico!\n");
        return;
    }
    fprintf(arq, "%d;%s;%s;%d;%.2f\n", nova.id, nova.cliente, nova.produto, nova.quantidade, nova.total);
    fclose(arq);

    // Atualiza o estoque do produto baixando a quantidade vendida
    prod->quantidade = prod->quantidade - qtd;     
    salvarProdutos(p, np); // Atualiza o arquivo de produtos com o novo estoque
    printf("Compra registrada com sucesso! Total: R$ %.2f\n", nova.total);
}

void listarHistorico() {
    Compra v[MAX];
    int n = carregarCompras(v), i;
    float soma = 0;

    if (n == 0) {
        printf("Nenhuma compra registrada.\n");
        return;
    }
    printf("\n%-5s %-20s %-20s %5s %10s\n", "ID", "CLIENTE", "PRODUTO", "QTD", "TOTAL");
    printf("-----------------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        printf("%-5d %-20.20s %-20.20s %5d %10.2f\n", v[i].id, v[i].cliente, v[i].produto,
               v[i].quantidade, v[i].total);
        soma = soma + v[i].total;
    }
    printf("\nTotal de %d compra(s) | Faturamento total: R$ %.2f\n", n, soma);
}

// ---------------------------------------------------------
// FUNCAO PRINCIPAL (MENU DE NAVEGACAO)
// ---------------------------------------------------------
int main() {
    int opcao;

    do {
        printf("\n===== SISTEMA DE LOJA =====\n");
        printf("1  - Criar arquivos       -> Cria os 3 arquivos txt vazios\n");
        printf("2  - Inserir produto      -> Cadastra um produto (ID automatico)\n");
        printf("3  - Listar produtos      -> Mostra todos em tabela formatada\n");
        printf("4  - Alterar produto      -> Muda os dados pelo ID\n");
        printf("5  - Excluir produto      -> Remove do arquivo pelo ID\n");
        printf("6  - Buscar produto       -> Busca por nome, categoria ou fornecedor\n");
        printf("7  - Ordenar produtos     -> Usa Bubble Sort (nao altera o arquivo)\n");
        printf("8  - Exportar CSV         -> Gera arquivo 'produtos.csv'\n");
        printf("9  - Inserir cliente      -> Cadastra cliente (ID automatico)\n");
        printf("10 - Listar clientes      -> Mostra todos em tabela\n");
        printf("11 - Registrar compra     -> Vincula cliente/produto e baixa estoque\n");
        printf("12 - Listar historico     -> Exibe todas as vendas realizadas\n");
        printf("0  - Sair\n");
        printf("Escolha uma opcao: ");
        opcao = lerInt();

        switch (opcao) {
            case 1:  criarArquivos();     break;
            case 2:  inserirProduto();    break;
            case 3:  listarProdutos();    break;
            case 4:  alterarProduto();    break;
            case 5:  excluirProduto();    break;
            case 6:  buscarProduto();     break;
            case 7:  ordenarProdutos();   break;
            case 8:  exportarCSV();       break;
            case 9:  inserirCliente();    break;
            case 10: listarClientes();    break;
            case 11: registrarCompra();   break;
            case 12: listarHistorico();   break;
            case 0:  printf("Encerrando o sistema...\n"); break;
            default: printf("Opcao invalida! Tente novamente.\n");
        }
    } while (opcao != 0);

    return 0;
}