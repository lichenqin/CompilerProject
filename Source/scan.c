# include "type.h"
# include "scan.h"

/* 作为 identifier\number 缓冲区使用*/
char String_buffer[Max_Va_Length];

/* 定义 identifier\number 缓冲区当前状态值*/
int string_position = 0;

/* 作为 line 缓冲区使用*/
char Line_buffer[Max_Line];

/* 定义 line 缓冲区当前的状态值*/
int line_position = 0;

/* 建立token类型表*/
const char * tokenTable[24]={"WHILE", "IF", "THEN", "ELSE", "DO",
                    "PLUS", "MINUS", "TIMES", "DIVID", "LP", "RP", "EQUAL", "ASIGN", "LT", "GT", "SEMI",
                    "INT10", "INT8", "INT16",
                    "FLO10", "FLO8", "FLO16",
                    "ID", "ERROR"};

/* 定义In_range内联函数*/
int In_range( char temp, char x, char y){
    if( temp >= x && temp <= y)
        return True;
    else
        return False;
}

/* 判断是否为间隔符*/
int Isinter( char temp){
    if( In_range(temp,'(','+') || In_range(temp,';','>')) return True;
    else if ( temp == '-' || temp == '/' )              return True;
    else if ( temp == '\n' || temp == ' ' || temp == '\t')  return True;
    else return False;
}

/*If it's legal word*/
int Isid( char temp ){
    if( In_range(temp, '0','9') || In_range(temp, 'a','z') || In_range(temp,'A','Z'))
        return True;
    else if( temp == '_')
        return True;
    else
        return False;
}

/* 打印字符类型 */
void printToken(State current_state){
    printf("%s:\t",tokenTable[current_state]);
}

/* printString 打印identifier或者number*/
void printString(){
    for(int i = 0; i < string_position; ++i)
        printf("%c",String_buffer[i]);
    printf("\n");
}

/* 安全模式下的strcat实现 避免指针溢出*/
char * str_conjunction(char * pre_array, const char * appendex){
    const char * char_tmp = pre_array;

    /*count total length*/
    int count = 0;
    while( *char_tmp++ != '\0' ) count++;    
    char_tmp = appendex;
    while ( *char_tmp++ != '\0' ) count++;

    /*allocate memorary for new string and copy*/
    char * array = (char *)calloc((count+1),sizeof(char));
    char * tmp = array;
    while( (*tmp++ = *pre_array++) != '\0');
    tmp--;
    while( (*tmp++ = *appendex++) != '\0');

    return array;
}

/* getToken是scanner的主函数*/
/*两个函数非常重要，fgetc fprintf*/
/*通过这两个函数 可以实现读取filename.tiny文件以及输出到.out文件中*/
/*由于linux的系统原因，feof存在延后性 所以使用fgets判断文件末尾*/
TokenType getToken(){
    
    /*temp用于从file中读取一个字符*/
    int temp = 0;
    char convert = '\0';
        
    State current_state = start;
    int accept = False;
    int save = True;
    string_position = 0;

    while( accept != True ){

        save = True;

        if( (temp=fgetc(read)) != -1)   convert = (char)temp;
        else{
            if( current_state != start ){
                save = False;
            }
            else{
                current_state = End;
            }
            //current_state = End;
        }

        switch (current_state){
            case start/* 开始状态 */:
                if(convert=='0')
                    current_state = zero;
                else if( In_range(convert,'1','9'))
                    current_state = Int10;
                else if( In_range(convert,'a','z') || In_range(convert, 'A','Z')){
                    if( convert == 'd') current_state = md;
                    else if( convert == 'e')    current_state = me;
                    else if( convert == 'i')    current_state = mi;
                    else if( convert == 't')    current_state = mt;
                    else if( convert == 'w')    current_state = mw;
                    else                        current_state = Id;
                }
                else if( Inrange(convert,'(','+') || In_range(convert,';','>') || convert=='-' || convert=='/'){
                    if( convert=='+')  current_state = Plus;
                    else if( convert=='-')  current_state = Minus;
                    else if( convert=='*')  current_state = Times;
                    else if( convert=='/')  current_state = Divide;
                    else if( convert=='(')  current_state = Lp;
                    else if( convert==')')  current_state = Rp;
                    else if( convert=='<')  current_state = Lt;
                    else if( convert=='>')  current_state = Gt;
                    else                    current_state = Semi;
                        
                    accept = True;
                }
                else if( convert=='=' ) current_state = Assign;/*Assign 即是完成状态又是中间状态*/
                else if( convert=='\n' || convert=='\t' || convert==' ' || convert=='\r'){
                    //keep fetching.
                    save = False;
                }
                else    current_state = Error;

                break;
            case zero/*0*/:
                if( In_range(convert,'0','7') )  current_state = Int8;
                else if( convert == 'X' )       current_state = ox;
                else if( convert == '.' )       current_state = dtf;
                else if( Isinter(convert) ){
                    current_state = Int10;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;    
                break;
            case Int10:
                if( In_range(convert,'0','9'))  current_state = Int10;
                else if ( convert=='.' )        current_state = dtf;
                else if ( Isinter(convert) ){
                    current_state = Int10;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case ox:
                if( In_range(convert,'0','9') || In_range(convert,'A','F')) current_state = Int16;
                else if( Isinter(convert) ){
                    current_state = Error;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case Int16:
                    if( In_range(convert,'0','9') || In_range(convert,'A','F')) current_state = Int16;
                    else if ( Isinter(convert) ){
                        current_state = Int16;
                        accept = True;
                        save = False;
                    }
                    else if( convert=='.')  current_state = htf;
                    else    current_state = Error;
                    break;
            case Int8:
                    if( In_range(convert,'0','7'))  current_state = Int8;
                    else if( convert=='.')          current_state = otf;
                    else if( Isinter(convert) ){
                        current_state = Int8;
                        accept = True;
                        save = False;
                    }
                    else    current_state = Error;
                    break;
            case dtf:/*Decimal to Float Middle state*/
                    if( In_range(convert,'0','9') ) current_state = f10;
                    else if( Isinter(convert) ){
                        current_state = Error;
                        accept = True;
                        save = False;
                    }
                    else    current_state = Error;
                    break;
            case f10:/*Decimal Float Accept state*/
                    if( In_range(convert,'0','9') ) current_state = f10;
                    else if( Isinter(convert) ){
                        current_state = f10;
                        accept = True;
                        save = False;
                    }
                    else    current_state = Error;
                    break;
            case htf:/*Hex to Float Middle state*/
                    if( In_range(convert,'0','9') || In_range(convert,'A','Z') )
                        current_state = f16;
                    else if( Isinter(convert) ){
                        current_state = Error;
                        accept = True;
                        save = False;
                    }
                    else    current_state = Error;
                    break;
            case f16:/*Hex Float Accept state*/
                if( In_range(convert,'0','9') || In_range(convert,'A','Z') )
                    current_state = f16;
                else if( Isinter(convert) ){
                    current_state = f16;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case otf:/*Octal to Float Middle state*/
                if( In_range(convert,'0','7') ) current_state = f8;
                else if( Isinter(convert) ){
                    current_state = Error;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case f8:/*Octal Float Accept state*/
                if( In_range(convert,'0','7') ) current_state = f8;
                else if( Isinter(convert) ){
                    current_state = f8;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case Assign:
                if( convert == '=' )    current_state = Equal;
                else    current_state = Assign;        
                
                accept = True;
                break;
            case md:
                if( convert == 'o') current_state = Do;
                else if( Isid(convert))     current_state = Id;
                else if( Isinter(convert) ) {
                    current_state = Id;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case Do:
                if( Isid(convert) ) current_state = Id;
                else if( Isinter(convert)){
                    current_state = Do;
                    accept = True;
                }
                else    current_state = Error;
                break;
            case me:
                if( convert == 'l') current_state = mel;
                else if( Isid(convert) )    current_state = Id;
                else if( Isinter(convert)){
                    current_state = Id;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case mel:
                if( convert == 's') current_state = mels;
                else if( Isid(convert) )    current_state = Id;
                else if( Isinter(convert)){
                    current_state = Id;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case mels:
                if( convert == 'e') current_state = Else;
                else if( Isid(convert) )    current_state = Id;
                else if( Isinter(convert)){
                    current_state = Id;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case Else:
                if( Isid(convert) ) current_state = Id;
                else if( Isinter(convert)){
                    current_state = Else;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case mi:
                    if( convert == 'f') current_state = If;
                    else if( Isid(convert) )    current_state = Id;
                    else if( Isinter(convert)){
                        current_state = Id;
                        accept = True;
                        save = False;
                    }
                    else    current_state = Error;
                    break;
            case If:
                if( Isid(convert) ) current_state = Id;
                else if( Isinter(convert)){
                    current_state = If;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case mt:
                if( convert == 'h') current_state = mth;
                else if( Isid(convert) )    current_state = Id;
                else if( Isinter(convert)){
                    current_state = Id;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case mth:
                if( convert == 'e') current_state = mthe;
                else if( Isid(convert) )    current_state = Id;
                else if( Isinter(convert)){
                    current_state = Id;
                    accept = True;
                }
                else    current_state = Error;
                break;
            case mthe:
                if( convert == 'n') current_state = Then;
                else if( Isid(convert) )    current_state = Id;
                else if( Isinter(convert)){
                    current_state = Id;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case Then:
                if( Isid(convert) ) current_state = Id;
                else if( Isinter(convert)){
                    current_state = Then;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case mw:
                if( convert == 'h') current_state = mwh;
                else if( Isid(convert) )    current_state = Id;
                else if( Isinter(convert)){
                    current_state = Id;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case mwh:
                if( convert == 'i') current_state = mwhi;
                else if( Isid(convert) )    current_state = Id;
                else if( Isinter(convert)){
                    current_state = Id;
                    accept = True;
                }
                else    current_state = Error;
                break;
            case mwhi:
                if( convert == 'l') current_state = mwhil;
                else if( Isid(convert) )    current_state = Id;
                else if( Isinter(convert)){
                    current_state = Id;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case mwhil:
                if( convert == 'e') current_state = While;
                else if( Isid(convert) )    current_state = Id;
                else if( Isinter(convert)){
                    current_state = Id;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case While:
                if( Isid(convert) ) current_state = Id;
                else if( Isinter(convert)){
                    current_state = While;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case Id:/*Identifier Accept state*/
                if( Isid(convert) ) current_state = Id;
                else if( Isinter(convert)){
                    current_state = Id;
                    accept = True;
                    save = False;
                }
                else    current_state = Error;
                break;
            case Error:
                if( Isinter(convert) ){
                    current_state = Error;
                    accept = True;
                    save = False;
                }
                else{
                    current_state = Error;
                }
                break;
            case End:
                return ENDFI;
            default:
                break;            
        }

        if( save == True ){
            if( string_position < Max_Va_Length-1)
                String_buffer[string_position++] = convert;
            else{
                //多出来的部分不计入
            }
        }
        else{
            if( accept == True )
                String_buffer[string_position] = '\0';
        }

    }

    printToken(current_state);
    printString();


}