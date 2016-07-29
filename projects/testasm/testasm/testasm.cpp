/* Some embedded simulators crash unless the file has .bss and .data sections. 
   We provide both by introducing two dummy variables below. */

static volatile int s_SimFix1, s_SimFix2 = 1;

int TestFunc(int a, int b)
{
	volatile char sz[] = "Hello, world!";	/* Hover the mouse over sz to see its value in Visual Studio */
	asm("nop");	/*Put a breakpoint here to test debugging*/
	return a + b;
}

int main()
{
	s_SimFix1 = s_SimFix2 = 0;

	TestFunc(1, 2);
	return 0;
}