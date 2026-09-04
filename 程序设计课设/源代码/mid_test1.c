void funcDeclare(int i, float f);
void funcVoid(void);
double globVar=1, globarr[9];
int main(void)
{
    int i_1=123, i_2=0x9b7c, i_3=0125;
    float f_1=.1, f_2=.3e;
    double d=.314e;
    char c_1='o';
    char s[0x19]="helloworld";
    if((( c_1 ) == ( 's' )) || (( i_1 ) != ( i_2 )))
    {
        funcDeclare(i,0.1);
        if(( c_1 ) == ( 9 ))
        {
            (( globVar ) *= ( 2 ));
        }

    }
    else
    {
        if((( d ) == ( .27 )) || (( c_1 ) > ( '9' )))
        {
            funcDeclare(i,0.1);
        }
        else
        {
            (( i_1 ) += ( 0 ));
        }

    }

    while((( i_1 ) >= ( 0 )) && (( f_1 ) < (( 1 ) + ( 3 ))))
    {
        for( (( i_1 ) = ( 0 )); (( i_1 ) <= ( c_1 )); (( i_1 ) += ( 1 )))
        {
            (( i_1 ) += ( 0 ));
            continue;
        }

        break;
    }

        return ( i_1 );

}