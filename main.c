#include <reg51.h>


// Global Definitions

sfr LCDData = 0x90; 
sbit LCDRS = P0^1;  
sbit LCDE = P0^0; 
int isJumped = 0;
int gameOver = 0;
unsigned char LFSR = 0xAC; 
int score = 0;

unsigned char cactus[8] = {
    0x04, // 0b00100 -> Top of the cactus
    0x04, // 0b00100 -> Stem
    0x15, // 0b10101 -> Stem with branches
    0x0E, // 0b01110 -> Middle part
    0x04, // 0b00100 -> Stem
    0x04, // 0b00100 -> Stem
    0x0A, // 0b01010 -> Bottom branches
    0x00  // 0b00000 -> Empty
	};
	
	
	unsigned char trex[8] = {
    0x0C,  //  ¦¦  
    0x1E,  // ¦¦¦¦ 
    0x1A,  // ¦¦ ¦ 
    0x0C,  //  ¦¦  
    0x16,  // ¦ ¦¦ 
    0x12,  // ¦  ¦ 
    0x10,  // ¦    
    0x00   //       
};

unsigned char LCD[2][16] = {{' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',}, {2,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',}};
 
	
// Function to generate delay 
void msdelay(int ms) {
	int j = 0;
	TMOD = 0X01; 
	for(j = 0; j < ms; j++) {
		TH0 = 0XFC; 
		TL0 = 0X17; 
		TR0 = 1;  
		while(TF0 != 1); 
		TR0 = 0;   
		TF0 = 0; 
	}
}

// Function to execute command in LCD
void LCD_cmd(unsigned char command) {
	LCDData = command;
	LCDRS = 0;        
	LCDE = 1;         
	msdelay(1);      
	LCDE = 0;          
	msdelay(5);        
}

// Function to initialize the LCD Screen
void LCD_init() {
	msdelay(20);     
	LCD_cmd(0x38);    
	LCD_cmd(0x0C);     
	LCD_cmd(0x01);     
}

// Function to display character on LCD
void LCD_char(unsigned char ch) {
	LCDData = ch;   
	LCDRS = 1;       
	LCDE = 1;        
	msdelay(1);        
	LCDE = 0;          
	msdelay(5);        
}

// Function to display string on screen
void LCD_string(unsigned char *s) {
	while(*s) {       
		LCD_char(*s++); 
	}
}


// Function to display customized characters on LCD
void LCDCustomChar(unsigned char location, unsigned char *pattern) {
    int i;
    location &= 0x07; 
    LCD_cmd(0x40 | (location << 3)); // Set CGRAM address

    for (i = 0; i < 8; i++) {
        LCD_char(pattern[i]); 
    }
}


// Function to generate randombit.
unsigned char random_bit() {
	
    unsigned char gbit;
    gbit = ((LFSR >> 0) ^ (LFSR >> 1) ^ (LFSR >> 2) ^ (LFSR >> 3)) & 1;
    LFSR = (LFSR >> 1) | (gbit << 7);  // Shift right and insert feedback bit at MSB

    return gbit; 
}



// Function to display game layout
void display(){
	int i;
	//LCD_cmd(0x01);
	LCD_cmd(0x80);
	LCD_char(LCD[0][0]);
	LCD_cmd(0xC0);
	
	for(i = 0; i < 16; i++){
		LCD_char(LCD[1][i]);
		//msdelay(10);
	}
	
	
}

void gameOverDis(){
	
	unsigned char scoreStr[16];
	int i = 0;
		
	LCD_cmd(0x01);
	
	while (score > 0) {
    scoreStr[i++] = score % 10 + '0';
    score /= 10;
  } 

  scoreStr[i] = '\0';

	LCD_cmd(0x84);
	LCD_string("Game Over");
	LCD_cmd(0xC4);
	LCD_string("Score: ");
	LCD_string(scoreStr);
}



// Function to animate scree.
void shift(int *n){
	unsigned char rand_bit = random_bit();
	int i;
	
	for(i = 0; i < 15; i++){
		LCD[1][i] = LCD[1][i+1];
	}
	
	if(isJumped != 1){
		LCD[0][0] = ' ';
		LCD[1][0] = 2;
		
		if(LCD[1][1] == 3){		
			gameOver = 1;
		}
	}
	
	if(isJumped == 1){
		isJumped = 0;
	}
	
	
	if(rand_bit == 1 && *n > 5){	
			LCD[1][15] = 3;
			*n = 1;
		}else{
			LCD[1][15] = ' ';
	}

	display();
}

// Function to make Dino Jumo
void jump (){
	
	LCD[0][0] = 2;
	isJumped = 1;
}

// Function to check if key is pressed
int is_key_pressed() {
	
    P1 = 0x0F; 
		// This incomplete key checkin logic is done on purpose)
		// Is only required to indentify if key is pressed
		// There is no need to know which key is pressed
    if ((P1 & 0x0F) != 0x0F) { 
        msdelay(20); 
        while ((P1 & 0x0F) != 0x0F); 
        return 1; 
    }

    return 0; // 
}


void main() {
	
	int n = 1;
	
	
	LCD_init();
	LCDCustomChar(2, trex);
	LCDCustomChar(3, cactus);
	
	display();
	
	while(1 && gameOver != 1){
		
		if(is_key_pressed() == 1){
				jump();
		}
		
		shift(&n);
		if(gameOver == 1){
			gameOverDis();
		}
		msdelay(100);
		score = score+1;
		n = n+1;
	}
	msdelay(10000);
	
	
}
