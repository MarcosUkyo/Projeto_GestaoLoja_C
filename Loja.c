// Loja: produtos, clientes e historico de compras (3 arquivos txt)
// Trabalho de ordenacao (bubble sort) + arquivos

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100     // maximo de registros em cada vetor

typedef struct {
    int id;
    char nome[50];       // se precisar de nomes maiores, aumente aqui
    char categoria[30];
    float preco;
    int quantidade;      // estoque
} Produto;

typedef struct {
    int id;
    char nome[50];
    char telefone[20];
} Cliente;

typedef struct {
    int id;
    char cliente[50];    // guardo o nome, assim o historico nao se perde se apagar o cliente
    char produto[50];
    int quantidade;
    float total;
} Compra;

// ---------------------------------------------------------
// FUNCOES AUXILIARES
// ---------------------------------------------------------

// le um texto do teclado
void lerTexto(char *texto, int tam) {
    int i;
    fgets(texto, tam, stdin);                   // le a linha
    texto[strcspn(texto, "\n")] = '\0';         // tira o enter do final
    for (i = 0; texto[i] != '\0'; i++) {
        if (texto[i] == ';') {                  // ; e o separador do arquivo, entao troca
            texto[i] = ' ';
        }
    }
}

// le um numero inteiro
int lerInt() {
    char buffer[50] = "0";
    fgets(buffer, 50, stdin);
    return atoi(buffer);
}

// le um numero real (aceita virgula)
float lerFloat() {
    char buffer[50] = "0";
    int i;
    fgets(buffer, 50, stdin);
    for (i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == ',') {
            buffer[i] = '.';                    // troca virgula por ponto
        }
    }
    return atof(buffer);
}

// letra minuscula (o cast evita problema com acento)
char minus(char c) {
    return tolower((unsigned char)c);
}

// compara dois textos ignorando maiuscula/minuscula (0 = iguais)
int comparar(char *a, char *b) {
    int i = 0;
    while (a[i] != '\0' && minus(a[i]) == minus(b[i])) {
        i++;
    }
    return minus(a[i]) - minus(b[i]);
}

// ve se o pedaco aparece dentro do texto (ignorando maiuscula/minuscula)
int contem(char *texto, char *pedaco) {
    char t[50], p[50];
    int i;
    for (i = 0; texto[i] != '\0'; i++) t[i] = minus(texto[i]);
    t[i] = '\0';
    for (i = 0; pedaco[i] != '\0'; i++) p[i] = minus(pedaco[i]);
    p[i] = '\0';
    return strstr(t, p) != NULL;                // strstr procura um texto dentro do outro
}

// ---------------------------------------------------------
// LER E SALVAR OS ARQUIVOS
// ---------------------------------------------------------

// le produtos.txt para o vetor e retorna quantos leu
int carregarProdutos(Produto *v) {
    FILE *arq = fopen("produtos.txt", "r");
    int n = 0;
    if (arq == NULL) {
        return 0;               // arquivo ainda nao existe
    }
    // le linha por linha no formato id;nome;categoria;preco;quantidade
    while (n < MAX && fscanf(arq, "%d;%49[^;];%29[^;];%f;%d\n", &v[n].id, v[n].nome,
                             v[n].categoria, &v[n].preco, &v[n].quantidade) == 5) {
        n++;
    }
    fclose(arq);
    return n;
}

// regrava produtos.txt usando um arquivo temporario
// (se der erro no meio, o original nao e corrompido)
void salvarProdutos(Produto *v, int n) {
    FILE *temp = fopen("temp.txt", "w");
    int i;
    if (temp == NULL) {
        printf("Erro ao criar arquivo temporario!\n");
        return;
    }
    for (i = 0; i < n; i++) {
        fprintf(temp, "%d;%s;%s;%.2f;%d\n", v[i].id, v[i].nome, v[i].categoria,
                v[i].preco, v[i].quantidade);
    }
    fclose(temp);
    remove("produtos.txt");                 // apaga o original
    rename("temp.txt", "produtos.txt");     // o temporario vira o original
}

// le clientes.txt (id;nome;telefone)
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

// le historico.txt (id;cliente;produto;quantidade;total)
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

// cria os 3 arquivos vazios (apaga o que tinha)
void criarArquivos() {
    FILE *arq;
    char resp[10];

    printf("Isso apaga tudo que ja existe. Continuar? (s/n): ");
    lerTexto(resp, 10);
    if (minus(resp[0]) != 's') {
        printf("Cancelado.\n");
        return;
    }
    arq = fopen("produtos.txt", "w");   // "w" cria o arquivo vazio
    if (arq != NULL) fclose(arq);
    arq = fopen("clientes.txt", "w");
    if (arq != NULL) fclose(arq);
    arq = fopen("historico.txt", "w");
    if (arq != NULL) fclose(arq);
    printf("Arquivos criados!\n");
}

// ---------------------------------------------------------
// PRODUTOS
// ---------------------------------------------------------

// procura um produto pelo id (retorna a posicao ou -1)
int acharProduto(Produto *v, int n, int id) {
    int i;
    for (i = 0; i < n; i++) {
        if (v[i].id == id) {
            return i;
        }
    }
    return -1;
}

// mostra o cabecalho e as linhas da tabela
void cabecalhoProdutos() {
    printf("\n%-5s %-25s %-20s %10s %6s\n", "ID", "NOME", "CATEGORIA", "PRECO", "QTD");
    printf("--------------------------------------------------------------------\n");
}

void linhaProduto(Produto *p) {
    printf("%-5d %-25.25s %-20.20s %10.2f %6d\n", p->id, p->nome, p->categoria,
           p->preco, p->quantidade);
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

    for (i = 0; i < n; i++) {
        if (comparar(v[i].nome, novo.nome) == 0) {      // mesmo nome = duplicado
            printf("Esse produto ja esta cadastrado!\n");
            return;
        }
        if (v[i].id > maior) {                          // guarda o maior id
            maior = v[i].id;
        }
    }
    novo.id = maior + 1;    // id automatico (maior + 1 nunca repete um id que ja existe)

    arq = fopen("produtos.txt", "a");   // "a" adiciona no final do arquivo
    if (arq == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }
    fprintf(arq, "%d;%s;%s;%.2f;%d\n", novo.id, novo.nome, novo.categoria, novo.preco, novo.quantidade);
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
    Produto *p;                         // ponteiro para o produto que vai ser alterado
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
    for (i = 0; i < n; i++) {           // ve se outro produto ja tem esse nome
        if (i != pos && comparar(v[i].nome, nome) == 0) {
            printf("Ja existe outro produto com esse nome!\n");
            return;
        }
    }
    printf("Nova categoria: ");
    lerTexto(p->categoria, 30);
    if (strlen(nome) == 0 || strlen(p->categoria) == 0) {
        printf("Nome e categoria nao podem ficar vazios!\n");
        return;                         // sai sem salvar, entao o arquivo nao muda
    }
    strcpy(p->nome, nome);
    printf("Novo preco: ");
    p->preco = lerFloat();
    printf("Nova quantidade: ");
    p->quantidade = lerInt();

    salvarProdutos(v, n);               // regrava o arquivo (com temporario)
    printf("Produto alterado!\n");
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
    for (i = pos; i < n - 1; i++) {     // puxa os proximos uma posicao para tras
        v[i] = v[i + 1];
    }
    salvarProdutos(v, n - 1);           // regrava com um produto a menos
    printf("Produto excluido!\n");
}

// busca por nome (1) ou categoria (2)
void buscarProduto() {
    Produto v[MAX];
    int n = carregarProdutos(v), i, tipo, achou = 0;
    char busca[50];

    printf("Buscar por: 1 - nome | 2 - categoria: ");
    tipo = lerInt();
    printf("Digite o texto (ou parte dele): ");
    lerTexto(busca, 50);

    for (i = 0; i < n; i++) {
        if ((tipo == 1 && contem(v[i].nome, busca)) || (tipo == 2 && contem(v[i].categoria, busca))) {
            if (achou == 0) {
                cabecalhoProdutos();    // cabecalho so no primeiro resultado
            }
            linhaProduto(&v[i]);
            achou++;
        }
    }
    if (achou == 0) {
        printf("Nenhum produto encontrado.\n");
    }
}

// bubble sort: criterio 1 = nome A-Z, 2 = nome Z-A, 3 = preco menor->maior, 4 = preco maior->menor
void bubbleSort(Produto *v, int n, int criterio) {
    int i, j, trocar;
    Produto aux;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - 1 - i; j++) {
            if (criterio == 1) trocar = comparar(v[j].nome, v[j + 1].nome) > 0;
            else if (criterio == 2) trocar = comparar(v[j].nome, v[j + 1].nome) < 0;
            else if (criterio == 3) trocar = v[j].preco > v[j + 1].preco;
            else trocar = v[j].preco < v[j + 1].preco;

            if (trocar) {               // troca os dois de lugar
                aux = v[j];
                v[j] = v[j + 1];
                v[j + 1] = aux;
            }
        }
    }
}

// ordena so o vetor (na memoria) e mostra. o arquivo NAO muda
void ordenarProdutos() {
    Produto v[MAX];
    int n = carregarProdutos(v), criterio;

    if (n == 0) {
        printf("Nenhum produto cadastrado.\n");
        return;
    }
    printf("1 - Nome A-Z\n2 - Nome Z-A\n3 - Preco menor -> maior\n4 - Preco maior -> menor\n");
    printf("Ordenar por: ");
    criterio = lerInt();
    if (criterio < 1 || criterio > 4) {
        printf("Opcao invalida!\n");
        return;
    }
    bubbleSort(v, n, criterio);
    mostrarProdutos(v, n);
}

// gera produtos.csv separado por virgula
void exportarCSV() {
    Produto v[MAX];
    int n = carregarProdutos(v), i;
    FILE *csv = fopen("produtos.csv", "w");

    if (csv == NULL) {
        printf("Erro ao criar o CSV!\n");
        return;
    }
    fprintf(csv, "id,nome,categoria,preco,quantidade\n");
    for (i = 0; i < n; i++) {
        fprintf(csv, "%d,\"%s\",\"%s\",%.2f,%d\n", v[i].id, v[i].nome, v[i].categoria,
                v[i].preco, v[i].quantidade);
    }
    fclose(csv);
    printf("produtos.csv gerado com %d produto(s)!\n", n);
}

// ---------------------------------------------------------
// CLIENTES
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
// HISTORICO DE COMPRAS
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
    printf("%s | Preco: %.2f | Estoque: %d\n", prod->nome, prod->preco, prod->quantidade);

    printf("Quantidade: ");
    qtd = lerInt();
    if (qtd <= 0 || qtd > prod->quantidade) {
        printf("Quantidade invalida ou estoque insuficiente!\n");
        return;
    }

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

    arq = fopen("historico.txt", "a");
    if (arq == NULL) {
        printf("Erro ao abrir o historico!\n");
        return;
    }
    fprintf(arq, "%d;%s;%s;%d;%.2f\n", nova.id, nova.cliente, nova.produto, nova.quantidade, nova.total);
    fclose(arq);

    prod->quantidade = prod->quantidade - qtd;     // tira do estoque
    salvarProdutos(p, np);
    printf("Compra registrada! Total: R$ %.2f\n", nova.total);
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
    printf("\n%d compra(s) | Soma: R$ %.2f\n", n, soma);
}

// ---------------------------------------------------------
// MENU
// ---------------------------------------------------------
int main() {
    int opcao;

    do {
        printf("\n===== LOJA =====\n");
        printf("1  - Criar arquivos     -> cria os 3 arquivos txt vazios\n");
        printf("2  - Inserir produto    -> cadastra um produto (ID automatico)\n");
        printf("3  - Listar produtos    -> mostra todos em tabela\n");
        printf("4  - Alterar produto    -> muda os dados pelo ID\n");
        printf("5  - Excluir produto    -> remove pelo ID\n");
        printf("6  - Buscar produto     -> por nome ou categoria\n");
        printf("7  - Ordenar produtos   -> por nome ou preco (nao muda o arquivo)\n");
        printf("8  - Exportar CSV       -> gera o produtos.csv\n");
        printf("9  - Inserir cliente    -> cadastra um cliente (ID automatico)\n");
        printf("10 - Listar clientes    -> mostra todos em tabela\n");
        printf("11 - Registrar compra   -> liga cliente e produto, tira do estoque\n");
        printf("12 - Listar historico   -> mostra todas as compras\n");
        printf("0  - Sair\n");
        printf("Opcao: ");
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
            case 0:  printf("Saindo...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    return 0;
}