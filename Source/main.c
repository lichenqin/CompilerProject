/*-------------------------------------------*/
/* This is the main function of this compiler*/
/* 1. Scanner--main function getToken()      */
/* 2. Parser --main function parse()         */
/*    use Recursive Descent                  */
/* 3. Code Genertater                        */
/*-------------------------------------------*/

# include "type.h"
# include "scan.h"
# include "parse.h"

int main( int argc, char * argv[] ){
    
    if( argc != 2){
        printf("Usage: %s <filename>", argv[0]);
        exit(1);
    }
    
    const char * filename = (const char *)argv[1];
    char * outfile = str_conjunction(argv[1],".out");

    read = fopen(filename, "rb");
    if( read == NULL){
        printf("Can't open this file: %s", argv[1]);
        exit(1);
    }

    output = fopen(outfile, "w");
    if( output == NULL ){
        printf("Can't record.\n");
        exit(1);
    }

    /* Scanner Part*/
    //TokenType temp;
    //while( (temp=getToken()) != ENDFI );

    /* Parser Part*/
    TreeNode * syntaxTree;
    syntaxTree = parse();
    printf("\n************Syntax Tree************\n");
    printTree(syntaxTree);

    /* Code generater parter*/

    /*Close file handle*/
    fclose(read);
    fclose(output);

    /*free heap*/
    free(outfile);

    return 0;

}