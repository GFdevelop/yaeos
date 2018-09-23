// versione ricorsiva
void printint_(int a){
	int b = a%10;
    a = a/10;
    if (a>0) printint_(a);
    if (b==0) {tprint("0");} else if (b==1) {tprint("1");}
    else if (b==2) {tprint("2");} else if (b==3) {tprint("3");}
    else if (b==4) {tprint("4");} else if (b==5) {tprint("5");}
    else if (b==6) {tprint("6");} else if (b==7) {tprint("7");}
    else if (b==8) {tprint("8");} else if (b==9) {tprint("9");}
}
void printint(int a){
	if (a<0) {tprint("-"); a = -a;}
    printint_(a);
	tprint("\n");
}

// versione iterativa
/*
void printint(int a){
    int finish = 0;
    if (a<0){ tprint("-"); a = -a;}
    int b = a, c=0, s = 0;
    while (b!=0) { b = (b - b%10)/10; c += 1;}
    b = a;
    while (!finish){
        for (int i=1; i<c; i++) { b = (b - b%10)/10;} c -= 1;
        if (b==0) {tprint("0");} else if (b==1) {tprint("1");}
        else if (b==2) {tprint("2");} else if (b==3) {tprint("3");}
        else if (b==4) {tprint("4");} else if (b==5) {tprint("5");}
        else if (b==6) {tprint("6");} else if (b==7) {tprint("7");}
        else if (b==8) {tprint("8");} else if (b==9) {tprint("9");}
        if (c==0) {finish = !finish;}
        s += b * pow (10 ,c);
        b = a - s;
    }
    printf("\n");
}
*/
void printchar(char * a){
	char b;
	for (int i = 0; i < sizeof(a); i++) {
		b = a[i];
		if (b=='a') {tprint("a");} else if (b=='b') {tprint("b");} else if (b=='c') {tprint("c");}
		else if (b=='d') {tprint("d");} else if (b=='e') {tprint("e");} else if (b=='f') {tprint("f");}
		else if (b=='g') {tprint("g");} else if (b=='h') {tprint("h");} else if (b=='i') {tprint("i");}
		else if (b=='m') {tprint("m");} else if (b=='n') {tprint("n");} else if (b=='o') {tprint("o");}
		else if (b=='p') {tprint("p");} else if (b=='q') {tprint("q");} else if (b=='r') {tprint("r");}
		else if (b=='s') {tprint("s");} else if (b=='t') {tprint("t");} else if (b=='u') {tprint("u");}
		else if (b=='v') {tprint("v");} else if (b=='z') {tprint("z");} else if (b=='x') {tprint("x");}
		else if (b=='y') {tprint("y");} else if (b=='w') {tprint("w");}
		else if (b=='A') {tprint("A");} else if (b=='B') {tprint("B");} else if (b=='C') {tprint("C");}
		else if (b=='D') {tprint("D");} else if (b=='E') {tprint("E");} else if (b=='F') {tprint("F");}
		else if (b=='G') {tprint("G");} else if (b=='H') {tprint("H");} else if (b=='I') {tprint("I");}
		else if (b=='M') {tprint("M");} else if (b=='N') {tprint("N");} else if (b=='O') {tprint("O");}
		else if (b=='P') {tprint("P");} else if (b=='Q') {tprint("Q");} else if (b=='R') {tprint("R");}
		else if (b=='S') {tprint("S");} else if (b=='T') {tprint("T");} else if (b=='U') {tprint("U");}
		else if (b=='V') {tprint("V");} else if (b=='Z') {tprint("Z");} else if (b=='X') {tprint("X");}
		else if (b=='Y') {tprint("Y");} else if (b=='W') {tprint("W");}
		else if (b=='0') {tprint("0");} else if (b=='1') {tprint("1");} else if (b=='2') {tprint("2");}
		else if (b=='3') {tprint("3");} else if (b=='4') {tprint("4");} else if (b=='5') {tprint("5");}
		else if (b=='6') {tprint("6");} else if (b=='7') {tprint("7");} else if (b=='8') {tprint("8");}
		else if (b=='9') {tprint("9");}
		else if (b=='\n'){tprint("\n");}else if (b=='-') {tprint("-");}
		else {tprint("unknowChar");}
	}
}
