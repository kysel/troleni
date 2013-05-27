#ifndef RS232NEW_BASIC
#define RS232NEW_BASIC

/*
toto:
#ifdef RS485_CTRL_PIN
RS485_CTRL_PIN::set();
#endif

nahrazeno tímto: 
 PORTD |= (1<<PD2) 

toto:
#ifdef RS485_CTRL_PIN
RS485_CTRL_PIN::clear();
#endif

nahrazeno tímto:
PORTD &= ~(1<<PD2)

*/
namespace avrlib
{

#ifndef RS232_RX_BUF
#if defined (__AVR_ATmega48__)
#define RS232_RX_BUF 32
#else
#define RS232_RX_BUF 96
#endif
#endif
#ifndef RS232_TX_BUF
#if defined (__AVR_ATmega48__)
#define RS232_TX_BUF 32
#else
#define RS232_TX_BUF 96
#endif
#endif
#ifndef DONT_USE_BOOTLOADER
#if ((!defined BOOTLOADER_WDT))
#ifndef BOOTLOADER_SEQ
#if ((defined (__AVR_ATmega16__))||(defined (__AVR_ATmega168__)))
#define BOOTLOADER_SEQ() \
	asm volatile ( \
		"ldi r31, 0x1F\n" \
		"ldi r30, 0x81\n" \
		"ijmp" \
		)
#elif defined (__AVR_ATmega48__)
#define BOOTLOADER_SEQ() \
	asm volatile ( \
		"ldi r31, 0x07\n" \
		"ldi r30, 0x81\n" \
		"ijmp" \
		);
#elif ((defined (__AVR_ATmega128__))||(defined (_AVR_IOM1284P_H_)))
#ifdef DYNAMIXEL_BOOTLOADER
#define BOOTLOADER_SEQ() \
	asm volatile ( \
		"ldi r31, 0xF0\n" \
		"ldi r30, 0x00\n" \
		"ijmp" \
		);
#else
#define BOOTLOADER_SEQ() \
	asm volatile ( \
		"ldi r31, 0xFE\n" \
		"ldi r30, 0x01\n" \
		"ijmp" \
		);
#endif
#elif defined (__AVR_ATmega32__)
#define BOOTLOADER_SEQ() \
	asm volatile ( \
		"ldi r31, 0x3F\n" \
		"ldi r30, 0x01\n" \
		"ijmp" \
		);
#elif defined (__AVR_ATmega88__)
#define BOOTLOADER_SEQ() \
	asm volatile ( \
		"ldi r31, 0x0F\n" \
		"ldi r30, 0x81\n" \
		"ijmp" \
		);
#endif
#endif
#endif
#endif
#if ((defined (__AVR_ATmega128__))||(defined (_AVR_IOM1284P_H_)))
#ifndef RS2321_RX_BUF
#define RS2321_RX_BUF 96
#endif
#ifndef RS2321_TX_BUF
#define RS2321_TX_BUF 96
#endif
#ifndef DONT_USE_BOOTLOADER
#ifndef BOOTLOADER_PORT
#define BOOTLOADER_PORT 0
#endif
#endif
#endif
#ifndef endl
#define endl "\n\r"
#endif
#ifndef RESET_SEQ
#define RESET_SEQ() \
	asm volatile ( \
		"ldi r31, 0x00\n" \
		"ldi r30, 0x00\n" \
		"ijmp" \
		)
#endif
class rs232_t {

#ifndef RS232_EXTERN_RX_INT
	queue <char, RS232_RX_BUF> m_rx;
#endif
	queue <char, RS232_TX_BUF> m_tx;

public:

#ifndef RS232_EXTERN_RX_INT
	void data_in(char ch)
	{
		m_rx.push(ch);
	}
#endif

	bool data_out(char &ch)
	{
		if(m_tx.empty())
			return false;
		ch = m_tx.pop();
		return true;
	}

	inline bool is_send()
	{
		return m_tx.empty();
	}

#if ((defined (__AVR_ATmega16__))||(defined (__AVR_ATmega32__)))
	uint32_t init(uint32_t speed)
	{
		UCSRA = (1<<U2X);
		UCSRB = ((1<<RXCIE)|(1<<RXEN)|(1<<TXEN));
		UCSRC = ((1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1));
		speed = (((F_CPU/(float(8*speed)))-1)-((F_CPU/(8*speed))-1))<0.5?((F_CPU/(8*speed))-1):((F_CPU/(8*speed)));
		UBRRH = ((speed&0xFF00)>>8);
		UBRRL = (speed&0x00FF);
		
				/* rs485 */	UCSRB |= (1<<TXCIE);
				
#ifdef RS485_CTRL_PIN
		RS485_CTRL_PIN::output(true);
		RS485_CTRL_PIN::clear();
		UCSRB |= (1<<TXCIE);
#endif
 PORTD &= ~(1<<PD2); /* Shození RS485 */

		return F_CPU/(float)(8*(((UBRRH<<8)|UBRRL)+1));
	}
	void send_char(char data)
	{
		while(!m_tx.push(data)) {}
 PORTD |= (1<<PD2); /* Nahození RS485 */
		UCSRB |= (1<<UDRIE);
	}
	
	bool is_recv()
	{
		return !m_rx.empty();
	}
	
	void send_char_immediately(char data)
	{
 PORTD |= (1<<PD2); /* Nahození RS485 */
		UCSRA |= (1<<TXC);
		UDR = data;
		while((UCSRA & (1<<TXC)) == 0){}
 PORTD &= ~(1<<PD2); /* Shození RS485 */

	}
#elif ((defined _AVR_IOMX8_H_)||(defined (__AVR_ATmega128__))||(defined (_AVR_IOM1284P_H_)))
//#if _AVR_IOMX8_H_ == 1
	uint32_t init(uint32_t speed)
	{
#if ((defined _AVR_IOMX8_H_)&&(defined BOOTLOADER_EXTERN_WDT))
		if((MCUSR & (1<<WDRF))!=0)
		{
			MCUSR = 0;
			BOOTLOADER_SEQ();
		}
		else
			MCUSR = 0;
#endif
		UCSR0A = (1<<U2X0);
		UCSR0B = ((1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0));
		UCSR0C = ((1<<UCSZ00)|(1<<UCSZ01));
		speed = (((F_CPU/(float(8*speed)))-1)-((F_CPU/(8*speed))-1))<0.5?((F_CPU/(8*speed))-1):((F_CPU/(8*speed)));
		UBRR0H = ((speed&0xFF00)>>8);
		UBRR0L = (speed&0x00FF);
//		numbersSpaces = 1;
#ifdef RS485_CTRL_PIN
		RS485_CTRL_PIN::output(true);
		RS485_CTRL_PIN::clear();
		UCSR0B |= (1<<TXCIE0);
#endif
		return F_CPU/((float)(8*(((UBRR0H<<8)|UBRR0L)+1)));
	}
	void send_char(char data)
	{
		while(!m_tx.push(data)) {}
		UCSR0B |= (1<<UDRIE0);
	}

	void send_char_immediately(char data)
	{
#ifdef RS485_CTRL_PIN
		RS485_CTRL_PIN::set();
#endif
		UCSR0A |= (1<<TXC0);
		UDR0 = data;
		while((UCSR0A & (1<<TXC0)) == 0){}
#ifdef RS485_CTRL_PIN
		RS485_CTRL_PIN::clear();
#endif
	}
#endif
	void wait()
	{
		while(!m_tx.empty()) {}
	}

	void clear()
	{
		m_rx.clear();
	}
#ifndef RS232_EXTERN_RX_INT
	bool peek(char & data)
	{
		if(m_rx.empty())
			return false;
		data = m_rx.pop();
		return true;
	}

	char get()
	{
		char data;
		while(!peek(data)) {}
		return data;
	}
#endif	

}; rs232_t rs232;

#if ((defined (__AVR_ATmega16__))||(defined (__AVR_ATmega32__)))
#ifndef RS232_EXTERN_RX_INT
ISR(USART_RXC_vect)
{
	char data = UDR;
	if((UCSRA & (1<<FE)) == (1<<FE))
		return;
	rs232.data_in(data);
#ifndef DONT_USE_BOOTLOADER
	static const unsigned char bootSeq[] = { 0x74, 0x7E, 0x7A, 0x33 };
	static uint8_t state = 0;
	if (data == bootSeq[state])
	{
		if (++state == 4)
		{
#ifndef BOOTLOADER_WDT
			DDRA = 0;
			DDRB = 0;
			DDRB = 0;
			DDRD = 0;
			UCSRA = 0;
			UCSRB = 0;
			UCSRC = 0;
			UBRRL = 0;
			UBRRH = 0;
			BOOTLOADER_SEQ();
#else
			WDTCR |= (1<<WDE);
#endif
		}
	}
	else
		state = 0;
#endif
}
#endif RS232_EXTERN_RX_INT

ISR(USART_UDRE_vect)
{
	char data;
	if(rs232.data_out(data))
		UDR = data;
	else
		UCSRB &= ~(1<<UDRIE);
}
//#ifdef RS485_CTRL_PIN
ISR(USART_TXC_vect)
{	
	//RS485_CTRL_PIN::clear();
	PORTD &= ~(1<<PD2); /* Shození RS485 */
}
//#endif RS485_CTRL_PIN
/*#elif ((defined _AVR_IOMX8_H_)||(defined(__AVR_ATmega128__))||(defined (_AVR_IOM1284P_H_)))
//#if _AVR_IOMX8_H_ == 1
#ifndef RS232_EXTERN_RX_INT
#if ((defined(__AVR_ATmega128__))||(defined (_AVR_IOM1284P_H_)))
ISR(USART0_RX_vect)
#else
ISR(USART_RX_vect)
#endif
{
	char data = UDR0;
	if((UCSR0A & (1<<FE0)) == (1<<FE0))
		return;
	rs232.data_in(data);
#ifndef DONT_USE_BOOTLOADER
#if ((((defined(__AVR_ATmega128__))||(defined (_AVR_IOM1284P_H_))))&&(BOOTLOADER_PORT == 0)||(defined _AVR_IOMX8_H_))
	static const unsigned char bootSeq[] = { 0x74, 0x7E, 0x7A, 0x33 };
	static uint8_t state = 0;
	if (data == bootSeq[state])
	{
		if (++state == 4)
		{
//			rs232.sendImediatly("wdt\n\r");
			
#if ((!defined(BOOTLOADER_WDT)) && (!defined(BOOTLOADER_EXTERN_WDT)))
//			DDRA = 0;
			DDRB = 0;
			DDRC = 0;
			DDRD = 0;
			PORTB = 0;
			PORTC = 0;
			PORTD = 0;
//			TCCR0A = 0;
//			TCCR0B = 0;
//			TCCR1A = 0;
//			TCCR1B = 0;
//			TCCR2A = 0;
//			TCCR2B = 0;
			UCSR0A = 0;
			UCSR0B = 0;
			UCSR0C = 0;
			UBRR0L = 0;
			UBRR0H = 0;
			BOOTLOADER_SEQ();
#else
#if (defined _AVR_IOMX8_H_)
			WDTCSR = (1<<WDE);
#else
			WDTCR |= (1<<WDE);
#endif
#endif
		}
	}
	else
		state = 0;
#endif
#endif
}
#endif//RS232_EXTERN_RX_INT*/
/*#if ((defined(__AVR_ATmega128__))||(defined (_AVR_IOM1284P_H_)))
ISR(USART0_UDRE_vect)
#else
ISR(USART_UDRE_vect)
#endif
{
	char data;
	if(rs232.data_out(data))
		UDR0 = data;
	else
		UCSR0B &= ~(1<<UDRIE0);
}*/
#ifdef RS485_CTRL_PIN
#if ((defined(__AVR_ATmega128__))||(defined (_AVR_IOM1284P_H_)))
ISR(USART0_TX_vect)
#else
ISR(USART_TXC_vect)
#endif
{
	RS485_CTRL_PIN::clear();
}
#endif
#endif

#if ((defined (__AVR_ATmega128__))||(defined (_AVR_IOM1284P_H_)))
#ifndef OTHER_RS2321
class rs2321_t {

	queue <char, RS2321_RX_BUF> m_rx;
	queue <char, RS2321_TX_BUF> m_tx;
	uint8_t numbersSpaces;

public:

	void data_in(char ch)
	{
		m_rx.push(ch);
	}

	bool data_out(char &ch)
	{
		if(m_tx.empty())
			return false;
		ch = m_tx.pop();
		return true;
	}

	inline bool is_send()
	{
		return m_tx.empty();
	}
	
	bool is_recv()
	{
		return m_rx.empty();
	}

	uint32_t init(uint32_t speed)
	{
		UCSR1A = (1<<U2X1);
		UCSR1B = ((1<<RXCIE1)|(1<<RXEN1)|(1<<TXEN1));
		UCSR1C = ((1<<UCSZ10)|(1<<UCSZ11));
		speed = (((F_CPU/(float(8*speed)))-1)-((F_CPU/(8*speed))-1))<0.5?((F_CPU/(8*speed))-1):((F_CPU/(8*speed)));
		UBRR1H = ((speed&0xFF00)>>8);
		UBRR1L = (speed&0x00FF);
		numbersSpaces = 1;
#ifdef RS4851_CTRL_PIN
		RS4851_CTRL_PIN::output(true);
		RS4851_CTRL_PIN::clear();
		UCSR1B |= (1<<TXCIE1);
#endif
		return F_CPU/((float)(8*(((UBRR1H<<8)|UBRR1L)+1)));
	}
	void send_char(char data)
	{
#ifdef RS4851_CTRL_PIN
		RS4851_CTRL_PIN::set();
#endif
		//rs232.send_char(data);
		while(!m_tx.push(data)) {}
		UCSR1B |= (1<<UDRIE1);
	}
	
	void send_char_immediately(char data)
	{
#ifdef RS4851_CTRL_PIN
		RS4851_CTRL_PIN::set();
#endif
		UCSR1A |= (1<<TXC1);
		UDR1 = data;
		while((UCSR1A & (1<<TXC1)) == 0){}
#ifdef RS4851_CTRL_PIN
		RS4851_CTRL_PIN::clear();
#endif
	}
	void wait()
	{
		while(!m_tx.empty()) {}
	}

	bool peek(char & data)
	{
		if(m_rx.empty())
			return false;
		data = m_rx.pop();
		return true;
	}

	char get()
	{
		char data;
		while(!peek(data)) {}
		return data;
	}

}; rs2321_t rs2321;

ISR(USART1_RX_vect)
{
	char data = UDR1;
	if((UCSR1A & (1<<FE1)) == (1<<FE1))
		return;
	rs2321.data_in(data);
#ifndef DONT_USE_BOOTLOADER
#if BOOTLOADER_PORT == 1
	static const unsigned char bootSeq[] = { 0x74, 0x7E, 0x7A, 0x33 };
	static uint8_t state = 0;
	if (data == bootSeq[state])
	{
		if (++state == 4)
		{
#ifndef BOOTLOADER_WDT
			UCSR1A = 0;
			UCSR1B = 0;
			UCSR1C = 0;
			UBRR1L = 0;
			UBRR1H = 0;
			BOOTLOADER_SEQ();
#else
			WDTCR |= (1<<WDE);
#endif
		}
	}
	else
		state = 0;
#endif
#endif
}
ISR(USART1_UDRE_vect)
{
	char data;
	if(rs2321.data_out(data))
		UDR1 = data;
	else
		UCSR1B &= ~(1<<UDRIE1);
}
#ifdef RS4851_CTRL_PIN
ISR(USART1_TX_vect)
{
	RS4851_CTRL_PIN::clear();
}
#endif //RS4851_CTRL_PIN
#endif
#endif

}//end of namespace

#endif

/*void rs232_decimal(double number)
{
	char characters [38];
	uint8_t ch = 0;
	for(ch = 0; ch < 38; ++ch)
	{
		characters[ch] = '0';
	}
	ch = 0;
	if(number == 0)
	{
		rs232_sendChar('0');
		return;
	}
	else if(number < 0)
	{
		characters[ch] = '-';
		++ch;
		number = -number;
	}
	int32_t number1 = ((int32_t)number);
	for(int32_t i = 1000000000;i >= 1; i = (i / 10))
	{
		if(number1 >= i)
		{
			characters[ch] = ('0' + ((number1 / i) % 10));
			++ch;
		}
	}
	number -= number1;
	characters[ch] = '.';
	++ch;
	uint8_t stop = (ch + 6);
	for(uint8_t i = 0; i < 24; ++i)
	{
		number = number * 10;
		characters[ch] = ((char)(number + '0'));
		if(number >= 1)
		{
			//stop = ch;
		}
		number -= (characters[ch] - '0');
		++ch;
	}
	for(uint8_t s = 0; s < stop; ++s)
	{
		rs232_sendChar(characters[s]);
	}
}

void rs232_up(uint8_t i)
{
	for(uint8_t j = 0; i > j; ++j)
	{
		rs232_sendChar(27);
		rs232_sendChar(91);
		rs232_sendChar(65);
		rs232_sendChar(0);
	}
}
*/
