#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ENTER      10
#define ESC        27

/* Arrow key scan code has ESC, 91, key, enter */
#define ARROW_UP   65
#define ARROW_DOWN 66

/* page up and down scan code has ESC, 91, key, unknow key, enter */
#define PAGE_UP    53
#define PAGE_DOWN  54

/* home and end key scan code contains ESC, 79, key, and enter */
#define HOME       72
#define END        70

#define SORT_ORDER 111 /* 'o' to reverse sort order */
#define SORT_KEY   107 /* 'k' to change sort key    */

static int
get_key_event(void)
{                                                                                                     
	int i; 
	int rc;                                                                                       

	rc = getchar();                                                                               

	if (rc == ENTER) {                                                                            
		return ENTER;
	} else if (rc == ESC) {
		/* read 91 */                                                                         
		rc = getchar();                                                                       

		if (rc == 91) {
			/* read arrow key */                                                          
			rc = getchar();                                                               
			switch (rc) {                                                                 
			case ARROW_DOWN:                                                              
			case ARROW_UP:                                                                
				/* read enter */                                                      
				getchar();                                                            
				break;
			case PAGE_UP:                                                                 
			case PAGE_DOWN:                                                               
				/* read enter */                                                      
				getchar();                                                            
				getchar();                                                            
			}                                                                             
		} else if (rc == 79) {
			rc = getchar();                                                               
			switch (rc) {                                                                 
			case HOME:                                                                    
				getchar();
				break;                                                                
			case END:                                                                     
				getchar();                                                            
				break;
			}                                                                             
		}

		return rc;
	} else if (rc == SORT_KEY || rc == SORT_ORDER) {
		getchar();
		return (rc);
	} else {
		printf("rc = %d\n", rc);
	}

	return (0);                                                                                   
}

int main()
{
	int rc;

	while (1) {
		rc = get_key_event();
		switch (rc) {
		case ENTER:
			printf("ENTER\n");
			break;
		case ARROW_UP:
			printf("ARROW_UP\n");
			break;
		case ARROW_DOWN:
			printf("ARROW_DOWN\n");
			break;
		case PAGE_UP:
			printf("PAGE_UP\n");
			break;
		case PAGE_DOWN:
			printf("PAGE_DOWN\n");
			break;
		case HOME:
			printf("HOME\n");
			break;
		case END:
			printf("END\n");
			break;
		case SORT_KEY:
			printf("SORT_KEY\n");
			break;
		case SORT_ORDER:
			printf("SORT_ORDER\n");
			break;
		}
	}
}
