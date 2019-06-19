#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>
#include <graphics.h>

/* *** GLOBAL DEFINITIONS *** */

char const* PATHTODRIVER="C:\\BorlandC\\Bgi";

int const _x0=100,
	  _y0=100;

unsigned char const
	  _chip_radius=12,

	  _size=30,

	  _brd_bkcolor=GREEN,
	  _brd_highlight=DARKGRAY,
	  _brd_inner=DARKGRAY,
	  _brd_outer=WHITE;

int const  _ai_delay=1000;


unsigned char _BOARD[8][8];


/* *** GRAPHIC FUNCTIONS *** */

void draw_chip(unsigned char i, unsigned char j, unsigned char color){
	setcolor(color);
	setfillstyle(SOLID_FILL,color);
	pieslice (_x0+j*30+15, _y0+i*30+15, 0, 360, _chip_radius);
	setcolor((color==BLACK)?WHITE:BLACK);
	circle (_x0+j*30+15, _y0+i*30+15, 12);
}

void draw_board(){
	unsigned char i,j; // Row and column numbers
	int x,y; // Square's upper left corner coords

	x=_x0; y=_y0;

	setfillstyle(SOLID_FILL,_brd_bkcolor);
	setcolor(_brd_inner);

	for (i=0;i<8;i++) {
	  for (j=0;j<8;j++) {
	    bar (x, y, x+_size, y+_size);
	    line (x, y, x+_size, y);
	    line (x, y, x, y+_size);
	    x+=_size;

	    _BOARD[i][j]=0;   // clearing
	  }
	  x=_x0;
	  y+=_size;
	}

	draw_chip(3,3,WHITE); _BOARD[3][3]=1;
	draw_chip(3,4,BLACK); _BOARD[3][4]=2;
	draw_chip(4,3,BLACK); _BOARD[4][3]=2;
	draw_chip(4,4,WHITE); _BOARD[4][4]=1;

	setcolor(_brd_outer);
	line (_x0, _y0, _x0+8*_size, _y0);
	line (_x0, _y0, _x0, _y0+8*_size);
	line (_x0+8*_size, _y0, _x0+8*_size, _y0+8*_size);
	line (_x0, _y0+8*_size, _x0+8*_size, _y0+8*_size);
}

void highlight(unsigned char i, unsigned char j){
	setfillstyle(INTERLEAVE_FILL,_brd_highlight);
	setcolor(_brd_outer);

	bar (_x0+j*_size, _y0+i*_size,
	     _x0+(j+1)*_size, _y0+(i+1)*_size);
	if(j==0) line(_x0, _y0+i*_size, _x0, _y0+(i+1)*_size);
	if(j==7) line(_x0+8*_size, _y0+i*_size, _x0+8*_size, _y0+(i+1)*_size);
	if(i==0) line(_x0+j*_size, _y0, _x0+(j+1)*_size, _y0);
	if(i==7) line(_x0+j*_size, _y0+8*_size, _x0+(j+1)*_size, _y0+8*_size);

	if(_BOARD[i][j]!=0) draw_chip(i,j,((_BOARD[i][j]==2)?BLACK:WHITE));
}

void undo_highlight(unsigned char i, unsigned char j){
	setfillstyle(SOLID_FILL,_brd_bkcolor);
	bar (_x0+j*_size, _y0+i*_size,
	     _x0+(j+1)*_size, _y0+(i+1)*_size);
	setcolor(_brd_inner);
	rectangle(_x0+j*_size, _y0+i*_size,
	     _x0+(j+1)*_size, _y0+(i+1)*_size);
	setcolor(_brd_outer);
	if(j==0) line(_x0, _y0+i*_size, _x0, _y0+(i+1)*_size);
	if(j==7) line(_x0+8*_size, _y0+i*_size, _x0+8*_size, _y0+(i+1)*_size);
	if(i==0) line(_x0+j*_size, _y0, _x0+(j+1)*_size, _y0);
	if(i==7) line(_x0+j*_size, _y0+8*_size, _x0+(j+1)*_size, _y0+8*_size);


	if(_BOARD[i][j]==1)	// if white
		draw_chip(i,j,WHITE);
	else if(_BOARD[i][j]==2)
		draw_chip(i,j,BLACK);
}

void draw_score(){
	unsigned char
		i,j,
		black=0,white=0;
	for(i=0;i<=7;i++)
		for(j=0;j<=7;j++){
			if(_BOARD[i][j]==1) white++;
			else if(_BOARD[i][j]==2) black++;
		}

	setcolor(WHITE);
	settextstyle(0,0,1);
	char buf[100];

	sprintf(buf,"Black: %d     White: %d", black, white);

	setfillstyle(SOLID_FILL,BLACK);
	bar(_x0+40, _y0+245, _x0+250, _y0+260);
	outtextxy(_x0+40, _y0+250, buf);
}

void msg(char str[]){
	setcolor(WHITE);
	outtextxy(_x0+40, _y0-40, str);
}

void errmsg(char str[]){
	setcolor(RED);
	outtextxy(_x0+40, _y0-40, str);
}

void clrmsg(){ // clears message above the board
	setfillstyle(SOLID_FILL,BLACK);
	bar(_x0+40, _y0-40, _x0+350, _y0-25);
}

void show_turn(unsigned char player){
	char buf[256];
	setfillstyle(SOLID_FILL,BLACK);
	bar(_x0+300, _y0+50, _x0+450, _y0+65);
	setcolor(LIGHTBLUE);
	sprintf(buf,"%s player's turn.",((player==2)?"Black":"White"));
	outtextxy(_x0+300, _y0+50, buf);

	setcolor((player==2)?BLACK:WHITE);
	setfillstyle(SOLID_FILL,((player==2)?BLACK:WHITE));
	pieslice (_x0+300, _y0+20, 0, 360, _chip_radius);
	setcolor((player==2)?WHITE:BLACK);
	circle (_x0+300, _y0+20, 12);
}

/*** OTHER FUNCTIONS ***/

void chip(unsigned char i, unsigned char j, unsigned char player){
	unsigned char
	    color=((player==2)?BLACK:WHITE),
	    enemy=((player==2)?1:2);
	char v,h,vv,hh;

	// SEIZING OPPONENT'S CHIPS
	// check left
	if(j!=0 && _BOARD[i][j-1]==enemy){
		h=j-1;
		do {
			if(_BOARD[i][h]==0)
				break;
			else if(_BOARD[i][h]==enemy)
				h--;
			else if(_BOARD[i][h]==player){
				for(hh=h+1;hh<j;hh++){
					_BOARD[i][hh]=player;
					draw_chip(i,hh,color);
				}
				break;
			}
		} while(h>=0);
	}
	// check right
	if(j!=7 && _BOARD[i][j+1]==enemy){
		h=j+1;
		do {
			if(_BOARD[i][h]==0)
				break;
			else if(_BOARD[i][h]==enemy)
				h++;
			else if(_BOARD[i][h]==player){
				for(hh=h-1;hh>j;hh--){
					_BOARD[i][hh]=player;
					draw_chip(i,hh,color);
				}
				break;
			}
		} while(h<=7);
	}
	// check up
	if(i!=0 && _BOARD[i-1][j]==enemy){
		v=i-1;
		do {
			if(_BOARD[v][j]==0)
				break;
			else if(_BOARD[v][j]==enemy)
				v--;
			else if(_BOARD[v][j]==player){
				for(vv=v+1;vv<i;vv++){
					_BOARD[vv][j]=player;
					draw_chip(vv,j,color);
				}
				break;
			}
		} while(v>=0);
	}
	// check down
	if(i!=7 && _BOARD[i+1][j]==enemy){
		v=i+1;
		do {
			if(_BOARD[v][j]==0)
				break;
			else if(_BOARD[v][j]==enemy)
				v++;
			else if(_BOARD[v][j]==player){
				for(vv=v-1;vv>i;vv--){
					_BOARD[vv][j]=player;
					draw_chip(vv,j,color);
				}
				break;
			}
		} while(v<=7);
	}
	// check up-left
	if(i!=0 && j!=0 && _BOARD[i-1][j-1]==enemy){
		v=i-1;
		h=j-1;
		do {
			if(_BOARD[v][h]==0)
				break;
			else if(_BOARD[v][h]==enemy){
				v--;
				h--;
			}
			else if(_BOARD[v][h]==player){
				for(vv=v+1,hh=h+1;vv<i,hh<j;vv++,hh++){
					_BOARD[vv][hh]=player;
					draw_chip(vv,hh,color);
				}
				break;
			}
		} while(v>=0 && h>=0);
	}
	// check up-right
	if(i!=0 && j!=7 && _BOARD[i-1][j+1]==enemy){
		v=i-1;
		h=j+1;
		do {
			if(_BOARD[v][h]==0)
				break;
			else if(_BOARD[v][h]==enemy){
				v--;
				h++;
			}
			else if(_BOARD[v][h]==player){
				for(vv=v+1,hh=h-1;vv<i,hh>j;vv++,hh--){
					_BOARD[vv][hh]=player;
					draw_chip(vv,hh,color);
				}
				break;
			}
		} while(v>=0 && h<=7);
	}
	// check down-left
	if(i!=7 && j!=0 && _BOARD[i+1][j-1]==enemy){
		v=i+1;
		h=j-1;
		do {
			if(_BOARD[v][h]==0)
				break;
			else if(_BOARD[v][h]==enemy){
				v++;
				h--;
			}
			else if(_BOARD[v][h]==player){
				for(vv=v-1,hh=h+1;vv>i,hh<j;vv--,hh++){
					_BOARD[vv][hh]=player;
					draw_chip(vv,hh,color);
				}
				break;
			}
		} while(v<=7 && h>=0);
	}
	// check down-right
	if(i!=7 && j!=7 && _BOARD[i+1][j+1]==enemy){
		v=i+1;
		h=j+1;
		do {
			if(_BOARD[v][h]==0)
				break;
			else if(_BOARD[v][h]==enemy){
				v++;
				h++;
			}
			else if(_BOARD[v][h]==player){
				for(vv=v-1,hh=h-1;vv>i,hh>j;vv--,hh--){
					_BOARD[vv][hh]=player;
					draw_chip(vv,hh,color);
				}
				break;
			}
		} while(v<=7 && h<=7);
	}

	draw_chip(i,j,color);

	_BOARD[i][j]=player;
}

unsigned char check(unsigned char player, unsigned char i, unsigned char j){
	// Check if a chip can be placed
	int v,h;
	unsigned char enemy=((player==2)?1:2);

	if(_BOARD[i][j]!=0) return 0; // if trying to place chip over another
	// check left
	if(j!=0 && _BOARD[i][j-1]==enemy){
		h=j-1;
		do {
			if(_BOARD[i][h]==0)
				break;
			else if(_BOARD[i][h]==enemy)
				h--;
			else if(_BOARD[i][h]==player)
				return 1;
		} while(h>=0);
	}
	// check right
	if(j!=7 && _BOARD[i][j+1]==enemy){
		h=j+1;
		do {
			if(_BOARD[i][h]==0)
				break;
			else if(_BOARD[i][h]==enemy)
				h++;
			else if(_BOARD[i][h]==player)
				return 1;
		} while(h<=7);
	}
	// check up
	if(i!=0 && _BOARD[i-1][j]==enemy){
		v=i-1;
		do {
			if(_BOARD[v][j]==0)
				break;
			else if(_BOARD[v][j]==enemy)
				v--;
			else if(_BOARD[v][j]==player)
				return 1;
		} while(v>=0);
	}
	// check down
	if(i!=7 && _BOARD[i+1][j]==enemy){
		v=i+1;
		do {
			if(_BOARD[v][j]==0)
				break;
			else if(_BOARD[v][j]==enemy)
				v++;
			else if(_BOARD[v][j]==player)
				return 1;
		} while(v<=7);
	}
	// check up-left
	if(i!=0 && j!=0 && _BOARD[i-1][j-1]==enemy){
		v=i-1;
		h=j-1;
		do {
			if(_BOARD[v][h]==0)
				break;
			else if(_BOARD[v][h]==enemy){
				v--;
				h--;
			}
			else if(_BOARD[v][h]==player)
				return 1;
		} while(v>=0 && h>=0);
	}
	// check up-right
	if(i!=0 && j!=7 && _BOARD[i-1][j+1]==enemy){
		v=i-1;
		h=j+1;
		do {
			if(_BOARD[v][h]==0)
				break;
			else if(_BOARD[v][h]==enemy){
				v--;
				h++;
			}
			else if(_BOARD[v][h]==player)
				return 1;
		} while(v>=0 && h<=7);
	}
	// check down-left
	if(i!=7 && j!=0 && _BOARD[i+1][j-1]==enemy){
		v=i+1;
		h=j-1;
		do {
			if(_BOARD[v][h]==0)
				break;
			else if(_BOARD[v][h]==enemy){
				v++;
				h--;
			}
			else if(_BOARD[v][h]==player)
				return 1;
		} while(v<=7 && h>=0);
	}
	// check down-right
	if(i!=7 && j!=7 && _BOARD[i+1][j+1]==enemy){
		v=i+1;
		h=j+1;
		do {
			if(_BOARD[v][h]==0)
				break;
			else if(_BOARD[v][h]==enemy){
				v++;
				h++;
			}
			else if(_BOARD[v][h]==player)
				return 1;
		} while(v<=7 && h<=7);
	}
	return 0;
}
unsigned char can_go(unsigned char player){
	/* A place for algorithm of
	finding whether the player
	defined by parameter 'players'
	can make his turn (returns 1)
	or not (returns 0). */
	unsigned char i,j,
	    enemy=((player==2)?1:2);
        char v,h;

	for (i=0;i<=7;i++)
		for (j=0;j<=7;j++)

	if(_BOARD[i][j]==player){
		// check left
		if(j!=0 && _BOARD[i][j-1]==enemy){
			h=j-1;
			do {
				if(_BOARD[i][h]==player)
					break;
				else if(_BOARD[i][h]==enemy)
					h--;
				else if(_BOARD[i][h]==0)
					return 1; // can go left ;-)
			} while(h>=0);
		}
		// check right
		if(j!=7 && _BOARD[i][j+1]==enemy){
			h=j+1;
			do {
				if(_BOARD[i][h]==player)
					break;
				else if(_BOARD[i][h]==enemy)
					h++;
				else if(_BOARD[i][h]==0)
					return 1; // can go right
			} while(h<=7);
		}
		// check up
		if(i!=0 && _BOARD[i-1][j]==enemy){
			v=i-1;
			do {
				if(_BOARD[v][j]==player)
					break;
				else if(_BOARD[v][j]==enemy)
					v--;
				else if(_BOARD[v][j]==0)
					return 1; // can go up
			} while(v>=0);
		}
		// check down
		if(i!=7 && _BOARD[i+1][j]==enemy){
			v=i+1;
			do {
				if(_BOARD[v][j]==player)
					break;
				else if(_BOARD[v][j]==enemy)
					v++;
				else if(_BOARD[v][j]==0)
					return 1; // can go down
			} while(v<=7);
		}
		// check up-left
		if(i!=0 && j!=0 && _BOARD[i-1][j-1]==enemy){
			v=i-1;
			h=j-1;
			do {
				if(_BOARD[v][h]==player)
					break;
				else if(_BOARD[v][h]==enemy){
					v--;
					h--;
				}
				else if(_BOARD[v][h]==0)
					return 1; // can go up-left
			} while(v>=0 && h>=0);
		}
		// check up-right
		if(i!=0 && j!=7 && _BOARD[i-1][j+1]==enemy){
			v=i-1;
			h=j+1;
			do {
				if(_BOARD[v][h]==player)
					break;
				else if(_BOARD[v][h]==enemy){
					v--;
					h++;
				}
				else if(_BOARD[v][h]==0)
					return 1; // can go up-right
			} while(v>=0 && h<=7);
		}
		// check down-left
		if(i!=7 && j!=0 && _BOARD[i+1][j-1]==enemy){
			v=i+1;
			h=j-1;
			do {
				if(_BOARD[v][h]==player)
					break;
				else if(_BOARD[v][h]==enemy){
					v++;
					h--;
				}
				else if(_BOARD[v][h]==0)
					return 1; // can go down-left
			} while(v<=7 && h>=0);
		}
		// check down-right
		if(i!=7 && j!=7 && _BOARD[i+1][j+1]==enemy){
			v=i+1;
			h=j+1;
			do {
				if(_BOARD[v][h]==player)
					break;
				else if(_BOARD[v][h]==enemy){
					v++;
					h++;
				}
				else if(_BOARD[v][h]==0)
					return 1; // can go down-right
			} while(v<=7 && h<=7);
		}
	}
	return 0; // if no chances for player, return 0 = he can't go.
}

unsigned char game_pvp(){
	char
		key,
		buf[256];
	unsigned char i=0,j=0,
	    current=2; // 2 if its black turn, 1 if white

	draw_board();
	show_turn(current);
	draw_score();

	// Help
	setcolor(LIGHTGRAY);
	outtextxy(_x0+300, _y0+70, "Key controls: ");
	outtextxy(_x0+300, _y0+85, "WHITE: ");
	outtextxy(_x0+300, _y0+100, "Up - W");
	outtextxy(_x0+300, _y0+115, "Down - S");
	outtextxy(_x0+300, _y0+130, "Left - A");
	outtextxy(_x0+300, _y0+145, "Right - D");
	outtextxy(_x0+300, _y0+160, "Action - <Space>");
	outtextxy(_x0+300, _y0+180, "BLACK: ");
	outtextxy(_x0+300, _y0+195, "Up - Up Arrow");
	outtextxy(_x0+300, _y0+210, "Down - Down Arrow");
	outtextxy(_x0+300, _y0+225, "Left - Left Arrow");
	outtextxy(_x0+300, _y0+240, "Right - Right Arrow");
	outtextxy(_x0+300, _y0+255, "Action - <Enter>");


	do{
	  key=getch();
	  if(current==2){
	   switch(key){
	    case 80: // Down
	      undo_highlight(i,j);
	      i=(i==7)?0:i+1;
	      highlight(i,j);
	    break;
	    case 72: // Up
	      undo_highlight(i,j);
	      i=(i==0)?7:i-1;
	      highlight(i,j);
	    break;
	    case 77: // Right
	      undo_highlight(i,j);
	      j=(j==7)?0:j+1;
	      highlight(i,j);
	    break;
	    case 75: // Left
	      undo_highlight(i,j);
	      j=(j==0)?7:j-1;
	      highlight(i,j);
	    break;
	    case 13: // <Enter> (laying a chip)
	      clrmsg();

	      if(check(current,i,j)){
		chip(i,j,current);
		highlight(i,j);
		draw_score();

		current=(current==1)?2:1;  // change turn
		if(!can_go(current)){
		  sprintf(buf,"Player %s can't make any moves.",((current==2)?"BLACK":"WHITE"));
		  msg(buf);

		  current=(current==1)?2:1;
		  if(!can_go(current))
		    return 1;         // Game over.
		}
		show_turn(current);
	      }
	      else
		errmsg("Can't place a chip here!");
	      break;
	     case 27: // <Escape>
	      return 0;  // Forced exit.
	    }
	   } else if(current==1){
	    switch(key){
	     case 's': // Down
	       undo_highlight(i,j);
	       i=(i==7)?0:i+1;
	       highlight(i,j);
	     break;
	     case 'w': // Up
	       undo_highlight(i,j);
	       i=(i==0)?7:i-1;
	       highlight(i,j);
	     break;
	     case 'd': // Right
	       undo_highlight(i,j);
	       j=(j==7)?0:j+1;
	       highlight(i,j);
	     break;
	     case 'a': // Left
	       undo_highlight(i,j);
	       j=(j==0)?7:j-1;
	       highlight(i,j);
	     break;
	     case ' ': // <SPACE> (laying a chip)
	       clrmsg();

	       if(check(current,i,j)){
		chip(i,j,current);
		highlight(i,j);
		draw_score();

		current=(current==1)?2:1;  // change turn
		if(!can_go(current)){
		  sprintf(buf,"Player %s can't make any moves.",((current==2)?"BLACK":"WHITE"));
		  msg(buf);

		  current=(current==1)?2:1;
		  if(!can_go(current))
		    return 1;         // Game over.
		}
		show_turn(current);
	       }
	       else
		errmsg("Can't place a chip here!");
	     break;
	     case 27: // <Escape>
	      return 0;  // Forced exit.
	    }
	   }
	} while(1);

}

unsigned char game_pvpc(unsigned char ai){
	int key;
	unsigned char i=0,j=0,
		current=2; // 2 if its black turn, 1 if white
	char buf[256];

	draw_board();
	show_turn(current);
	draw_score();

	// Help
	setcolor(LIGHTGRAY);
	outtextxy(_x0+300, _y0+70, "Key controls: ");
	outtextxy(_x0+300, _y0+90, "Up - Up Arrow");
	outtextxy(_x0+300, _y0+105, "Down - Down Arrow");
	outtextxy(_x0+300, _y0+120, "Left - Left Arrow");
	outtextxy(_x0+300, _y0+135, "Right - Right Arrow");
	outtextxy(_x0+300, _y0+150, "Action - <Enter>");

	randomize();

	while(1){
	  if(ai==current){
		if(can_go(current)){
			clrmsg();
			do{
				undo_highlight(i,j);

				// Aiming for corners...
				if(check(current,0,0)){
					i=0;
					j=0;
				} else if(check(current,0,7)){
					i=0;
					j=7;
				} else if(check(current,7,0)){
					i=7;
					j=0;
				} else if(check(current,7,7)){
					i=7;
					j=7;
				} else {
					do{
						i=random(8);
						j=random(8);
					} while(!check(current,i,j));
				}

				delay(_ai_delay);
				chip(i,j,current);
				highlight(i,j);
				draw_score();

				current=(current==1)?2:1;  // change turn
				if(!can_go(current)){
					sprintf(buf,"Player %s can't make any moves.",((current==2)?"BLACK":"WHITE"));
					msg(buf);
					current=(current==1)?2:1;
					if(!can_go(current))
						return 1; // Game over.
				}
				show_turn(current);
				break;
			} while(1);
		}
	  } else {
		clrmsg();
		do{
		  key=getch();
		  switch(key){
		    case 80: // Down
		      undo_highlight(i,j);
		      i=(i==7)?0:i+1;
		      highlight(i,j);
		    break;
		    case 72: // Up
		      undo_highlight(i,j);
		      i=(i==0)?7:i-1;
		      highlight(i,j);
		    break;
		    case 77: // Right
		      undo_highlight(i,j);
		      j=(j==7)?0:j+1;
		      highlight(i,j);
		    break;
		    case 75: // Left
		      undo_highlight(i,j);
		      j=(j==0)?7:j-1;
		      highlight(i,j);
		    break;
		    case 13: // <Enter> (laying a chip)
		      clrmsg();

		      if(check(current,i,j)){
			chip(i,j,current);
			highlight(i,j);
			draw_score();

			current=(current==1)?2:1;  // change turn
			if(!can_go(current)){
			  sprintf(buf,"Player %s can't make any moves.",((current==2)?"BLACK":"WHITE"));
			  msg(buf);

			  current=(current==1)?2:1;
			  if(!can_go(current))
			    return 1;         // Game over.
			}
			show_turn(current);
		      }
		      else
			errmsg("Can't place a chip here!");
		    break;
		    case 27: // <Escape>
		      return 0;  // Forced exit.
		  }
		} while(current!=ai);
	  }
	}
}

unsigned char whoswinner(){
	// counts the amount of chips on the board
	// and returns the winner's id or 0 if draw
	unsigned char
		i,j,
		black=0,white=0;
	for(i=0;i<=7;i++)
		for(j=0;j<=7;j++){
			if(_BOARD[i][j]==1) white++;
			else if(_BOARD[i][j]==2) black++;
		}
	return ( (black>white) ? 2 : ((white>black)?1:3) );
}

/* *** MENU FUNCTIONS * ***/

unsigned char pos=1; // current position in menu

void draw_menu(){
	setcolor(RED);
	settextstyle(DEFAULT_FONT,HORIZ_DIR,4);
	outtextxy(20, 20, "OTHELLO");

	setcolor(BLUE);
	settextstyle(SANS_SERIF_FONT,HORIZ_DIR,1);
	outtextxy(150, 40, "peBepCu ;-)");

	setcolor(WHITE);
	settextstyle(DEFAULT_FONT,HORIZ_DIR,2);
	outtextxy(70, 100, "P1 vs. P2");

	setcolor(LIGHTGRAY);
	outtextxy(70, 150, "Play with computer");
	outtextxy(70, 200, "Exit game");
}

void menu_down(){
	switch(pos){
		case 3:
			setcolor(LIGHTGRAY);
			outtextxy(70, 200, "Exit game");

			pos=1;
			setcolor(WHITE);
			outtextxy(70, 100, "P1 vs. P2");
		break;
		case 1:
			setcolor(LIGHTGRAY);
			outtextxy(70, 100, "P1 vs. P2");

			pos=2;
			setcolor(WHITE);
			outtextxy(70, 150, "Play with computer");
		break;
		case 2:
			setcolor(LIGHTGRAY);
			outtextxy(70, 150, "Play with computer");

			pos=3;
			setcolor(WHITE);
			outtextxy(70, 200, "Exit game");
		break;
	}
}

void menu_up(){
	switch(pos){
		case 2:
			setcolor(LIGHTGRAY);
			outtextxy(70, 150, "Play with computer");

			pos=1;
			setcolor(WHITE);
			outtextxy(70, 100, "P1 vs. P2");
		break;
		case 3:
			setcolor(LIGHTGRAY);
			outtextxy(70, 200, "Exit game");

			pos=2;
			setcolor(WHITE);
			outtextxy(70, 150, "Play with computer");
		break;
		case 1:
			setcolor(LIGHTGRAY);
			outtextxy(70, 100, "P1 vs. P2");

			pos=3;
			setcolor(WHITE);
			outtextxy(70, 200, "Exit game");
		break;
	}
}

void main(){
	clrscr();

	/* graphics initialization */

	/* request auto detection */
	int gdriver = DETECT, gmode, errorcode;

	/* initialize graphics mode */
	initgraph(&gdriver, &gmode, PATHTODRIVER);

	/* read result of initialization */
	errorcode = graphresult();

	if (errorcode != grOk) { /* an error occurred */
		printf("Graphics error: %s\n", grapherrormsg(errorcode));
		printf("Press any key to halt:");
		getch();
		return;
	}
	/* end of initialization */

	unsigned char
		winner,
		ai,
		pos2=1;

	char
		key,key2;


	draw_menu();

	do{
		key=getch();
		switch(key){
			case 80:
				menu_down();
				break;
			case 72:
				menu_up();
				break;
			case 13:
				switch(pos){
					case 1:
						clrscr();
						cleardevice();
						printf("Press Esc to return to the main menu.");
                        settextstyle(DEFAULT_FONT,HORIZ_DIR,1);
						switch(game_pvp()){
							case 1: // if game is played to the end
							    clrmsg();
							    switch(whoswinner()){
								case 1:
									msg("WHITE WIN!");
									break;
								case 2:
									msg("BLACK WIN!");
									break;
								case 3:
									msg("DRAW!");
									break;
							    }
							    getch();
							    cleardevice();
							    break;
							  case 0:
							    cleardevice();
							    break;
						}
						draw_menu();
					break;
					case 2:
						setcolor(BLUE);
						settextstyle(DEFAULT_FONT,HORIZ_DIR,2);
						outtextxy(100, 300, "Select your color: ");

						setcolor(WHITE);
						setfillstyle(SOLID_FILL,BLACK);
						bar(200, 350, 299, 365);
						outtextxy(200, 350, "BLACK");

						setcolor(BLACK);
						setfillstyle(SOLID_FILL,WHITE);
						bar(300, 350, 399, 365);
						outtextxy(300, 350, "WHITE");

						do{
							key2=getch();
							switch(key2){
								case 77: //right
								if(pos2!=1){
									pos2=1;
									setcolor(WHITE);
									setfillstyle(SOLID_FILL,BLACK);
									bar(200, 350, 299, 365);
									outtextxy(200, 350, "BLACK");

									setcolor(BLACK);
									setfillstyle(SOLID_FILL,WHITE);
									bar(300, 350, 399, 365);
									outtextxy(300, 350, "WHITE");
								}
								break;
								case 75: //left
								if(pos2!=2){
									pos2=2;
									setcolor(BLACK);
									setfillstyle(SOLID_FILL,WHITE);
									bar(200, 350, 299, 365);
									outtextxy(200, 350, "BLACK");

									setcolor(WHITE);
									setfillstyle(SOLID_FILL,BLACK);
									bar(300, 350, 399, 365);
									outtextxy(300, 350, "WHITE");
								}
								break;
								case 13: //enter
									ai=((pos2==2)?1:2);
									clrscr();
									cleardevice();
									settextstyle(DEFAULT_FONT,HORIZ_DIR,1);
									printf("Press Esc to return to the main menu.");
									switch(game_pvpc(ai)){
										case 1: // if game is played to the end
										    clrmsg();
										    switch(whoswinner()){
											case 1:
												msg("WHITE WIN!");
												break;
											case 2:
												msg("BLACK WIN!");
												break;
											case 3:
												msg("DRAW!");
												break;
										    }
										    getch();
										    cleardevice();
										    key2=27;
										    break;
										  case 0:
										    cleardevice();
                                                                                    key2=27;
										    break;
									}
								break;
							}
						} while(key2!=27);
						setfillstyle(SOLID_FILL,BLACK);
						bar(100, 300, 400, 365);
						draw_menu();
                                                pos=1;
						break;
					case 3:
						closegraph();
						return;
				}
			break;
		}
	} while(key!=27);

	closegraph();
}
