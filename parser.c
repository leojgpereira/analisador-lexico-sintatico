/*
 parser.c : analisador sintatico (exemplo de automato com pilha)
 Autor: Edna A. Hoshino

Gramática da linguagem aceita pelo parser:

 S  -> Function S_ 
 S_ -> Function S_ 
      | epsilon
 Function -> Type Function_
 Type -> void 
       | int 
       | float
 Function_ -> main() { B } 
            | id() { B }
 B -> C B 
      | epsilon
 C -> id = E ; 
      | while (E) C 
      | { B }
 E -> TE'
 E'-> +TE' 
      | epsilon
 T -> FT'
 T'-> *FT' 
      | epsilon
 F -> (E) 
      | id 
      | num

 A saida do analisador apresenta o total de linhas processadas e uma mensagem de erro ou sucesso. 
 Atualmente, nao ha controle sobre a coluna e a linha em que o erro foi encontrado.
*/

#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"

/* variaveis globais */
int lookahead;
FILE *fin;

/* Exige que o próximo terminal seja t e avança o ponteiro da fita de entrada (i.e., pega o próximo terminal) */
void match(int t)
{
  if(lookahead==t){
    lookahead=lex();
  }
  else{
     printf("\nErro(linha=%d): token %s (cod=%d) esperado.## Encontrado \"%s\" ##\n", lines, terminalName[t], t, lexema);
    exit(1);
  }
}

// S  -> Function S_ 
void S(){
  Function();
  S_();
}

/* S_ -> Function S_ 
      | epsilon
*/
void S_(){
  if(lookahead==INT || lookahead==FLOAT || lookahead==VOID){
    Function();
    S_();
  }
}

// Function -> Type Function_
void Function(){
  Type();
  Function_();
}

/* Type -> void 
       | int 
       | float
*/
void Type(){  
  if(lookahead==INT){
    match(INT);
  }
  else if(lookahead==FLOAT){
    match(FLOAT);
  }
  else{
    match(VOID);
  }
}

/* Type_   -> INT
            | FLOAT
            | DOUBLE
            | CHAR          
*/
void Type_() {
  if(lookahead==INT) {
    match(INT);
  } else if(lookahead==FLOAT) {
    match(FLOAT);
  } else if(lookahead==DOUBLE) {
    match(DOUBLE);
  } else {
    match(CHAR);
  }
}

/* Function_ -> main() { B } 
            | id() { B }
*/
void Function_(){
  if(lookahead == MAIN){
    match(MAIN);
    match(ABRE_PARENT);
    match(FECHA_PARENT);
    match(ABRE_CHAVES);
    B();
    match(FECHA_CHAVES);
  }
  else{
    match(ID);
    match(ABRE_PARENT);
    match(FECHA_PARENT);
    match(ABRE_CHAVES);
    B();
    match(FECHA_CHAVES);
  }
}

/* B -> C B 
      | epsilon
*/
void B(){
  if(lookahead==ID || lookahead==WHILE || lookahead==ABRE_CHAVES || lookahead==INT || lookahead==DOUBLE || lookahead==FLOAT || lookahead==CHAR || lookahead==OP_MULT){
    C();
    B();
  }
}
/*
C    -> id = E ; 
      | *id = E;
      | while (E) C 
      | { B }
      | Type_ X id C_ ;
*/
void C(){
  if(lookahead==OP_MULT){ // * X id = E ;
    match(OP_MULT);
    X();
    match(ID);
    match(OP_ATRIB);
    E();
    match(PONTO_VIRG);
  } else if(lookahead==ID){ // id = E ;
    match(ID);
    match(OP_ATRIB);
    E();
    match(PONTO_VIRG);
  } else if(lookahead==WHILE){
    match(WHILE);
    match(ABRE_PARENT);
    E();
    match(FECHA_PARENT);
    C();
  } else if(lookahead==ABRE_CHAVES) {
    match(ABRE_CHAVES);
    B();
    match(FECHA_CHAVES);
  } else {
    Type_();
    X();
    match(ID);
    C_();
    match(PONTO_VIRG);
  }
}

// C_ -> VIRG ID
//    | C_
//    | epsilon
void C_() {
  if(lookahead==VIRG) {
    match(VIRG);
    X();
    match(ID);
    C_();
  }
}
/* X -> *X | epsilon */
void X() {
  if(lookahead==OP_MULT) {
    match(OP_MULT);
    X();
  }
}

// E-> T E_
void E(){
  T();
  E_();
}
// T -> FT'
void T(){
  F();
  T_();
}
// E_ -> + T E_ | epsilon
void E_(){
  if(lookahead==OP_ADIT){
    match(OP_ADIT);
    T();
    E_();
  }
}
/* T'-> *FT' 
      | epsilon
*/
void T_(){
  if(lookahead==OP_MULT){
    match(OP_MULT);
    F();
    T_();
  }
}
/*
 F -> (E) 
      | id 
      | num
      | *X id
*/
void F(){
  if(lookahead==ABRE_PARENT){
    match(ABRE_PARENT);
    E();
    match(FECHA_PARENT);
  } else if(lookahead==ID){
      match(ID);
  } else if(lookahead==NUM) {
    match(NUM);
  } else {
    match(OP_MULT);
    X();
    match(ID);
  }
}

/*******************************************************************************************
 parser(): 
 - efetua o processamento do automato com pilha AP
 - devolve uma mensagem para indicar se a "palavra" (programa) estah sintaticamente correta.
********************************************************************************************/
char *parser()
{
   lookahead=lex(); // inicializa lookahead com o primeiro terminal da fita de entrada (arquivo)
   S(); // chama a variável inicial da gramática.
   if(lookahead==FIM)
      return("Programa sintaticamente correto!");
   else
      return("Fim de arquivo esperado");
}

int main(int argc, char**argv)
{
  if(argc<2){
    printf("\nUse: compile <filename>\n");
    return 1;
  }
  else{
    printf("\nAnalisando lexica e sintaticamente o programa: %s", argv[1]);
    fin=fopen(argv[1], "r");
    if(!fin){
      printf("\nProblema na abertura do programa %s\n", argv[1]);
      return 1;
    }
    // chama o parser para processar o arquivo de entrada
    printf("\nTotal de linhas processadas: %d\nResultado: %s\n", lines, parser());
    fclose(fin);
    return 0;
  }
}

