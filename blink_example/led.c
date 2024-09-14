//tutorial from low level learning at https://www.youtube.com/watch?v=j4xw8QomkXs
#include <avr/io.h>
#include <util/delay.h>
//don't know why I have to add below to get rid of red sguiggles, but works without it
// #include <avr/io43u32x.h>
// #include <avr/io90pwm161.h>


int main(void)
{
    //set PORTB5 as an output
    DDRB = DDRB | (1<<DDB5);

    while(1)
    {
        //set PORT B5
        PORTB = PORTB | (1 << PORTB5);

        //wait 
        _delay_ms(1000);//does no operations until 1000ms has elapsed

        //unset PORTB5
        PORTB = PORTB & ~(1 << PORTB5);

        //wait some more
        _delay_ms(2000);
    }



}