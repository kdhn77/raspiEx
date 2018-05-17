#include <wiringPi.h>

int main (void)
{
	//wiringPi를 쓰려면wiringPiSetup()함수를 반드시먼저 선언해야 한다.
	wiringPiSetup () ;
	pinMode (0, OUTPUT) ;
	 for (;;)
	 {
		 digitalWrite (0, HIGH) ; delay (500) ;
		 digitalWrite (0,  LOW) ; delay (500) ;
	 }
	return 0 ;
}
