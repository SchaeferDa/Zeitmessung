/*
 * LCD_student.h
 *
 * Created: 03-Jan-19 18:40:34
 *  Author: Ambroise Broxtermann
 */ 


#ifndef LCD_STUDENT_H_
#define LCD_STUDENT_H_


void LCD_init(void);
void set_Curser_position(unsigned char row, unsigned char column);
void LCD_print_String(char *string);
void LCD_clear_screan(void);
void LCD1602_Demo(void);


#endif /* LCD_STUDENT_H_ */