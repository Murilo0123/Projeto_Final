#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/* ======================= Config DLL ======================= */
static HMODULE g_hDll = NULL;

/* Conven��o de chamada (Windows): __stdcall */
#ifndef CALLCONV
#  define CALLCONV WINAPI
#endif

/* ======================= Assinaturas da DLL ======================= */
typedef int (CALLCONV *AbreConexaoImpressora_t)(int, const char *, const char *, int);
typedef int (CALLCONV *FechaConexaoImpressora_t)(void);
typedef int (CALLCONV *ImpressaoTexto_t)(const char *, int, int, int);
typedef int (CALLCONV *Corte_t)(int);
typedef int (CALLCONV *ImpressaoQRCode_t)(const char *, int, int);
typedef int (CALLCONV *ImpressaoCodigoBarras_t)(int, const char *, int, int, int);
typedef int (CALLCONV *AvancaPapel_t)(int);
typedef int (CALLCONV *AbreGavetaElgin_t)(int, int, int);
typedef int (CALLCONV *AbreGaveta_t)(int, int, int);
typedef int (CALLCONV *SinalSonoro_t)(int, int, int);
typedef int (CALLCONV *ImprimeXMLSAT_t)(const char *, int);
typedef int (CALLCONV *ImprimeXMLCancelamentoSAT_t)(const char *, const char *, int);
typedef int (CALLCONV *InicializaImpressora_t)(void);

/* ======================= Ponteiros ======================= */
static AbreConexaoImpressora_t        AbreConexaoImpressora        = NULL;
static FechaConexaoImpressora_t       FechaConexaoImpressora       = NULL;
static ImpressaoTexto_t               ImpressaoTexto               = NULL;
static Corte_t                        Corte                        = NULL;
static ImpressaoQRCode_t              ImpressaoQRCode              = NULL;
static ImpressaoCodigoBarras_t        ImpressaoCodigoBarras        = NULL;
static AvancaPapel_t                  AvancaPapel                  = NULL;
static AbreGavetaElgin_t              AbreGavetaElgin              = NULL;
static AbreGaveta_t                   AbreGaveta                   = NULL;
static SinalSonoro_t                  SinalSonoro                  = NULL;
static ImprimeXMLSAT_t                ImprimeXMLSAT                = NULL;
static ImprimeXMLCancelamentoSAT_t    ImprimeXMLCancelamentoSAT    = NULL;
static InicializaImpressora_t         InicializaImpressora         = NULL;

/* ======================= Configura��o ======================= */
static int   g_tipo      = 1;
static char  g_modelo[64] = "i9";
static char  g_conexao[128] = "USB";
static int   g_parametro = 0;
static int   g_conectada = 0;

/* ======================= Utilidades ======================= */
#define LOAD_FN(h, name)                                                         \
    do {                                                                         \
        name = (name##_t)GetProcAddress((HMODULE)(h), #name);                    \
        if (!(name)) {                                                           \
            fprintf(stderr, "Falha ao resolver s�mbolo %s (erro=%lu)\n",         \
                    #name, GetLastError());                                      \
            return 0;                                                            \
        }                                                                        \
    } while (0)

static void flush_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

/* ======================= Fun��es para manipular a DLL ======================= */
static int carregarFuncoes(void)
{
    g_hDll = LoadLibraryA("E1_Impressora01.dll");
    if (!g_hDll) {
        fprintf(stderr, "Erro ao carregar E1_Impressora01.dll (erro=%lu)\n", GetLastError());
        return 0;
    }

    LOAD_FN(g_hDll, AbreConexaoImpressora);
    LOAD_FN(g_hDll, FechaConexaoImpressora);
    LOAD_FN(g_hDll, ImpressaoTexto);
    LOAD_FN(g_hDll, Corte);
    LOAD_FN(g_hDll, ImpressaoQRCode);
    LOAD_FN(g_hDll, ImpressaoCodigoBarras);
    LOAD_FN(g_hDll, AvancaPapel);
    LOAD_FN(g_hDll, AbreGavetaElgin);
    LOAD_FN(g_hDll, AbreGaveta);
    LOAD_FN(g_hDll, SinalSonoro);
    LOAD_FN(g_hDll, ImprimeXMLSAT);
    LOAD_FN(g_hDll, ImprimeXMLCancelamentoSAT);
    LOAD_FN(g_hDll, InicializaImpressora);

    return 1;
}

static void liberarBiblioteca(void)
{
    if (g_hDll) {
        FreeLibrary(g_hDll);
        g_hDll = NULL;
    }
}

/* ======================= Funcoes a serem implementadas pelos alunos ======================= */

static void exibirMenu(void)
{
    // TODO: implementar exibicao do menu principal com as opcoes de impressao
    printf("1 - Configurar Conexao    \n2 - Abrir Conexao    \n3 - Impressao Texto\n4 - Impressao QRCode\n5 - Impressao Cod Barras\n6 - Impressao XML SAT   \n7 - Impressao XML Canc SAT  \n8 - Abrir Gaveta Elgin  \n9 - Abrir Gaveta  \n10 - Sinal Sonoro \n0 - Fechar Conexao e Sair \n\n");
}

static void configurarConexao(void)
{
    // TODO: pedir ao usuario tipo, modelo, conexao e parametro
    printf("Digite o tipo da impressora: \n"); 
    scanf("%i", &g_tipo); // recebe o tipo da impressora
    printf("Digite o modelo da impressora: \n");
    scanf("%s", &g_modelo); // recebe o modelo da impressora
    printf("Digite o tipo de conexao da impressora: \n");
    scanf("%s", &g_conexao); // recebe o tipo de conexao da impressora
    printf("Digite o parametro da impressora: \n");
    scanf("%i", &g_parametro); // recebe o tipo de parametro da impressora
}

static void abrirConexao(void)
{
    if(!AbreConexaoImpressora){ //verifica se a impressora esta conectada
        printf("A funcao AbreConexaoImpressora nao foi carregada!\n"); 
        return;
    } 

    int retorno = AbreConexaoImpressora(g_tipo, g_modelo, g_conexao, g_parametro);// pega os parametros que estao acima

    if(retorno != 0){
        printf("Erro ao abrir conexao. \n", retorno); // erro ao realizar a conexao com a impressora quando retorna um valor diferente de 0
        g_conectada = 0;
        return;
    }

    printf("Conexao estabelecida com sucesso!\n"); // a conexao com a impressora foi estabelecida pois o valor retornado foi 0
    g_conectada = 1;
}

static void fecharConexao(void)
{
    if (!FechaConexaoImpressora) { // erro ao carregar a função FechaConexaoImpressora
        printf("A funcao FechaConexaoImpressora nao foi carregada!\n");
        return;
    }

    if (!g_conectada) { // verifica se a impressora esta conectada
        printf("A impressora ja esta desconectada.\n");
        return;
    }

    int retorno = FechaConexaoImpressora();

    if (retorno != 0) { // erro ao fechar a conexao com a impressora caso valor retornado seja diferente de 0
        printf("Erro ao fechar conexao.", retorno);
        return;
    }

    printf("Conexao encerrada com sucesso!\n"); // encerra a conexao pois o valor retornado foi 0
    g_conectada = 0;
}

static void imprimirTexto(void)
{
    // TODO: solicitar texto do usuario e chamar ImpressaoTexto
    if(!g_conectada){
        printf("Nao ha conexao com a impressora.\n");
        return;
    }

    if(!ImpressaoTexto){
        printf("A funcao ImpressaoTexto nao foi carregada.\n");
        return;
    }

    char texto[1024];

    printf("Digite o texto a ser impresso: ");
    flush_entrada();
    fgets(texto, sizeof(texto), stdin);

    int retorno = ImpressaoTexto(texto, 0, 0, 0);

    if(retorno != 0){
        printf("Erro ao imprimir texto.\n", retorno);
        return;
    }

    printf("Texto impresso com sucesso.\n");

    // incluir AvancaPapel e Corte no final
    AvancaPapel(2);
    
    Corte(2);

  
}

static void imprimirQRCode(void)
{
    // TODO: solicitar conteudo do QRCode e chamar ImpressaoQRCode(texto, 6, 4)
    ImpressaoQRCode("texto", 6, 4);
    printf("QRcode impresso com sucesso.\n");
    // incluir AvancaPapel e Corte no final
    AvancaPapel(2);
    Corte(2);
    
}

static void imprimirCodigoBarras(void)
{
    // TODO: usar ImpressaoCodigoBarras(8, "{A012345678912", 100, 2, 3)
    // incluir AvancaPapel e Corte no final
	ImpressaoCodigoBarras(0, "12345678901", 80, 2, 1);
    /*int TCB;//tipo de codigo de barras

    switch()
    {
        case 0: // UPC-A
            TCB = ImpressaoCodigoBarras(0, "12345678901", 80, 2, 1);
            break;

        case 1: // UPC-E
            TCB = ImpressaoCodigoBarras(1, "123456", 80, 2, 1);
            break;

        case 2: // EAN-13 & JAN-13
            TCB = ImpressaoCodigoBarras(2, "7891234567895", 80, 2, 2);
            break;

        case 3: // EAN-8 & JAN-8
            TCB = ImpressaoCodigoBarras(3, "1234567", 80, 2, 2);
            break;

        case 4: // CODE 39
            TCB = ImpressaoCodigoBarras(4, "ABC123$+-", 80, 2, 3);
            break;

        case 5: // ITF
            TCB = ImpressaoCodigoBarras(5, "12345678", 80, 2, 4);
            break;

        case 6: // CODE BAR
            TCB = ImpressaoCodigoBarras(6, "A1234B", 80, 2, 1);
            break;

        case 7: // CODE 93
            TCB = ImpressaoCodigoBarras(7, "ABC123xyz", 80, 2, 2);
            break;

        case 8: // CODE 128
            TCB = ImpressaoCodigoBarras(8, "{A}0123456789", 80, 2, 3);
            break;

        default:
            printf("Tipo invalido");
    }
    if (TCB == 0){
        printf("Codigo de Barras impresso!\n");
    }
    else{
        printf("Erro ao imprimir (codigo: %d)\n", TCB);
    }*/
        
    AvancaPapel(2);
    Corte(2);
}

static void imprimirXMLSAT(void)
{
    // TODO: ler o arquivo ./XMLSAT.xml e enviar via ImprimeXMLSAT
    ImprimeXMLSAT("path=C:/Users/murilo_monteiro/Desktop/Projeto_Final-main/XMLSAT.xml", 455);
    // incluir AvancaPapel e Corte no final
    AvancaPapel(2);
    Corte(2);
}

static void imprimirXMLCancelamentoSAT(void)
{
    // TODO: ler o arquivo ./CANC_SAT.xml e chamar ImprimeXMLCancelamentoSAT
    ImprimeXMLCancelamentoSAT("path=C:/Users/murilo_monteiro/Desktop/Projeto_Final-main/CANC_SAT.xml","Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZjbh3btsZhF+sjHqEMR159i4agru9x6KsepK/q0E2e5xlU5cv3m1woYfgHyOkWDNcSdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQEVd7u+TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+ZfO5b/+vkyAHS6CYVFCDtYR9Hi5qgdk31v23w==",193);
    // incluir AvancaPapel e Corte no final
    AvancaPapel(2);
    Corte(2);
    
	/*usar assinatura abaixo:
        "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZ"
        "jbh3btsZhF+sjHqEMR159i4agru9x6KsepK/q0E2e5xlU5cv3m1woYfgHyOkWDNc"
        "SdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQE"
        "Vd7u+TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6"
        "p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+ZfO5b/+vkyAHS6C"
        "YVFCDtYR9Hi5qgdk31v23w==";
        */
}

static void abrirGavetaElginOpc(void)
{
    // TODO: chamar AbreGavetaElgin(1, 50, 50)
    AbreGavetaElgin(1, 50, 50);
}

static void abrirGavetaOpc(void)
{
    // TODO: chamar AbreGaveta(1, 5, 10)
    AbreGaveta(1, 5, 10);
}

static void emitirSinalSonoro(void)
{
    // TODO: chamar SinalSonoro(4, 50, 5)
    SinalSonoro(4, 50, 5);
}

/* ======================= Funcao principal ======================= */
int main(void)
{
    if (!carregarFuncoes()) {
        return 1;
    }

    int opcao;
    int fecharmenu=0;
    while (fecharmenu==0) {
        
        //construir o menu e chamar as funcoes aqui!!!
        printf("\n========================================================================================================================\n\nSelecione uma opcao: \n\n"); 
        exibirMenu();
        scanf("%i", &opcao);
        printf("\n========================================================================================================================\n\n");
        switch (opcao)
        {
        case 1:
            configurarConexao();
            break;
        case 2:
            abrirConexao();
            break;
        case 3:
        	carregarFuncoes();
            imprimirTexto();
            break;
        case 4:
        	carregarFuncoes();
            imprimirQRCode();
            break;
        case 5:
        	carregarFuncoes();
            imprimirCodigoBarras();
            break;
        case 6:
        	carregarFuncoes();
            imprimirXMLSAT();
            break;
        case 7:
        	carregarFuncoes();
            imprimirXMLCancelamentoSAT();
            break;
        case 8:
        	carregarFuncoes();
            abrirGavetaElginOpc();
            break;
        case 9:
        	carregarFuncoes();
            abrirGavetaOpc();
            break;
        case 10:carregarFuncoes();
            emitirSinalSonoro();
            break;
        case 0:
            fecharmenu=1;
            break;
        
        default:
            printf("Opcao inexistente selecionada");
            break;
        }
        
                
        
    }
}

