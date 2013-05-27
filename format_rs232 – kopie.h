#ifndef KUBAS_FORMAT
#define KUBAS_FORMAT

#ifndef KUBAS_DEFAULT_STRING_SIZE
#define KUBAS_DEFAULT_STRING_SIZE 32
#endif

#ifndef KUBAS_NUMBER_2_STRING_CHARSET
#define KUBAS_NUMBER_2_STRING_CHARSET "0123456789ABCDEF"
#endif

#ifndef NULL
#define NULL 0
#endif

namespace avrlib {

template <typename Integer>
bool number2string (Integer n, char *s, uint8_t align = 0,
	uint8_t base = 10, char fill = ' ',
	uint8_t division = KUBAS_DEFAULT_STRING_SIZE, char separator = ' ',
	bool Minus = true)
{
	char charset[] = KUBAS_NUMBER_2_STRING_CHARSET;
	char buf[KUBAS_DEFAULT_STRING_SIZE];
	uint8_t i = 0;
	bool negative = false;
	uint8_t division_counter = division;
	if(n == 0)
		buf[i++] = charset[0];
	else
	{
		if(n < 0)
		{
			negative = true;
			n = -n;
		}
		for(; n != 0; n /= base)
		{
			if(i == (KUBAS_DEFAULT_STRING_SIZE-2))
				return false;
			buf[i++] = charset[n % base];
			if(--division_counter == 0)
			{
				buf[i++] = separator;
				division_counter = division;
			}
		}
	}
	if(Minus && negative)
		buf[i++] = '-';
	++division_counter;
//	if(align%2 != 0)
//		--align;
	while(i < align)
	{
		if(--division_counter != 0)
			buf[i++] = fill;
		else
		{
			buf[i++] = separator;
			division_counter = division+1;
		}
	}
	if(!Minus)
	{
		if(negative)
			buf[--i] = '-';
		else
			if(fill == '0')
				buf[--i] = ' ';
			else
				buf[--i] = fill;
		++i;
	}
	uint8_t j = 0;
	for(--i; i != 255; --i)
		s[j++] = buf[i];
	s[j] = NULL;
	return true;
}

#ifdef KUBAS_STRING
template <typename Integer>
bool number2string (Integer n, string &s, uint8_t align = 0,
	uint8_t base = 10, char fill = ' ',
	uint8_t division = KUBAS_DEFAULT_STRING_SIZE, char separator = ' ',
	bool Minus = true)
{
	char charset[] = KUBAS_NUMBER_2_STRING_CHARSET;
	char buf[KUBAS_DEFAULT_STRING_SIZE];
	uint8_t i = 0;
	s.clear();
	bool negative = false;
	uint8_t division_counter = division;
	if(n == 0)
		buf[i++] = charset[0];
	else
	{
		if(n < 0)
		{
			negative = true;
			n = -n;
		}
		for(; n != 0; n /= base)
		{
			if(i == (KUBAS_DEFAULT_STRING_SIZE-2))
				return false;
			buf[i++] = charset[n % base];
			if(--division_counter == 0)
			{
				buf[i++] = separator;
				division_counter = division;
			}
		}
	}
	if(Minus && negative)
		buf[i++] = '-';
	++division_counter;
	while(i < align)
	{
		if(--division_counter != 0)
			buf[i++] = fill;
		else
		{
			buf[i++] = separator;
			division_counter = division+1;
		}
	}
	if(!Minus)
	{
		if(negative)
			buf[--i] = '-';
		else
			if(fill == '0')
				buf[--i] = ' ';
			else
				buf[--i] = fill;
		++i;
	}
	for(--i; i != 255; --i)
		s += buf[i];
	return true;
}

template <typename Integer>
bool string2number(const string &s, Integer &res)
{
	bool isNegative = s[0]=='-';
	Integer n = 0;
	for(uint8_t i = isNegative?1:0; i != s.length(); ++i)
	{
		if(s[i]<'0' || s[i]>'9')
			return false;
		n = n*10+s[i]-'0';
	}
	if(isNegative)
		n = -n;
	res = n;
	return true;
}
#endif

class format_t {

	uint8_t Align;
	uint8_t Base;
	char Fill;
	uint8_t Division;
	char Separator;
	bool Minus;//close to number (true) or before align chars (false)
#ifdef KUBAS_STRING
#ifndef RS232_EXTERN_RX_INT
	bool cin_isFirst;
	string cin_buf;
	bool cin_isGood;
#endif
#endif
	//uint32_t getTimeout;

public:

	format_t()
	{
#ifndef RS232_EXTERN_RX_INT
#ifdef KUBAS_STRING
		cin_isFirst = true;
		cin_isGood = true;
		cin_buf.clear();
#endif
#endif
        Align = 0;
		Base = 10;
		Fill = ' ';
		Minus = true;
		Division = KUBAS_DEFAULT_STRING_SIZE;
		Separator = ' ';
		//getTimeout = 0;
	}

	void send_char(const char ch)
	{
		rs232.send_char(ch);
	}
	void send(const char *str)
	{
		for(; *str != 0; ++str)
			rs232.send_char(*str);
	}
	void send_char_immediately(const char ch)
	{
		rs232.send_char_immediately(ch);
	}
	void send_immediately(const char *str)
	{
		for(; *str != 0; ++str)
			rs232.send_char_immediately(*str);
	}

	template <typename Integer>
	void send_number_immediately (Integer n, uint8_t base = 0)
	{
		char buf[KUBAS_DEFAULT_STRING_SIZE];
		number2string(n, buf, Align, base==0?Base:base, Fill, Division, Separator, Minus);
		send_immediately(buf);
	}

	void align(uint8_t n) { Align = n; }
	void base (uint8_t n) { Base = n;  }
	void fill (char n)    { Fill = n;  }
	void division (uint8_t n) { Division = n;  }
	void separator (char n)    { Separator = n;  }
	uint8_t align() { return Align; }
	uint8_t base()  { return Base;  }
	char    fill()  { return Fill;  }
	uint8_t division()  { return Division;  }
	char    separator()  { return Separator;  }
	void minus(bool m) { Minus = m; }
	bool minus() { return Minus; }
//********
	format_t &operator << (const char &ch)
	{
		rs232.send_char(ch);
		return *this;
	}
	format_t &operator << (const char *str)
	{
		for(; *str != 0; ++str)
			rs232.send_char(*str);
		return *this;
	}
	template <typename Integer>
	format_t &operator << (Integer n)
	{
		char buf[KUBAS_DEFAULT_STRING_SIZE];
		number2string(n, buf, Align, Base, Fill, Division, Separator, Minus);
		send(buf);
		return *this;
	}
	format_t &operator << (const bool &n)
	{
		send(n?"true":"false");
		return *this;
	}
#ifdef KUBAS_STRING
	format_t &operator << (const string &str)
	{
		for(uint8_t i = 0; i != str.length(); ++i)
			rs232.send_char(str[i]);
		return *this;
	}
#endif
//input
#ifndef RS232_EXTERN_RX_INT
	format_t &operator >> (char &ch)
	{
		uint8_t i = 0;
		char temp = 0;
		while(temp != '\r')
		{
			temp = rs232.get();
			if(i == 0)
				ch = temp;
			if(temp != 8)
				++i;
			else
				if(i != 0)
					--i;
		}
		return *this;
	}
#ifdef KUBAS_STRING
	format_t &operator >> (string &str)
	{
		if(cin_isFirst)
		{
			char ch = rs232.get();
			cin_buf.clear();
			while(ch != '\r')
			{
				if(ch == 8)
				{
					if(cin_buf.length() != 0)
					{
						rs232.send_char(ch);
						cin_buf.erase(cin_buf.length()-1, 1);
					}
					else
						rs232.send_char('\a');
				}
				else
				{
					cin_buf += ch;
					if(cin_buf.length() == cin_buf.max_size())
						rs232.send_char('\a');
					else
						rs232.send_char(ch);
				}
				ch = rs232.get();
			}
			send(endl);
			cin_isFirst = false;
		}
		uint8_t i = cin_buf.find(' ');
		if (i == ((uint8_t)npos))
			cin_isFirst = true;
		str = cin_buf.substr(0, i);
		cin_buf.erase(0, i + 1);
		return *this;
	}
	format_t &operator >> (uint8_t & n)
	{
		string s;
		if(!(*this>>s)||!string2number(s, n))
			cin_isGood = false;
		return *this;
	}
	format_t &operator >> (int8_t & n)
	{
		string s;
		if(!(*this>>s)||!string2number(s, n))
			cin_isGood = false;
		return *this;
	}
	format_t &operator >> (uint16_t & n)
	{
		string s;
		if(!(*this>>s)||!string2number(s, n))
			cin_isGood = false;
		return *this;
	}
	format_t &operator >> (int16_t & n)
	{
		string s;
		if(!(*this>>s)||!string2number(s, n))
			cin_isGood = false;
		return *this;
	}
	format_t &operator >> (uint32_t & n)
	{
		string s;
		if(!(*this>>s)||!string2number(s, n))
			cin_isGood = false;
		return *this;
	}
	format_t &operator >> (int32_t & n)
	{
		string s;
		if(!(*this>>s)||!string2number(s, n))
			cin_isGood = false;
		return *this;
	}

	operator void const *() const { return cin_isGood?this:0; }

	void clear()
	{
		cin_isGood = true;
		cin_buf.clear();
		cin_isFirst = true;
	}

	bool is_complete() { return cin_isFirst; }

#endif

	bool peek(char &ch) { return rs232.peek(ch); }

	char get() { return rs232.get(); }
#endif

	void wait() { rs232.wait(); }
};

}

#endif
