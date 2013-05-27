#ifndef kubaslib_queue
#define kubaslib_queue
namespace kubas
{

template <typename T, uint16_t size>
class queue {
	volatile T m [size];
	volatile uint16_t m_read;
	volatile uint16_t m_write;
	public:
	bool is_empty()
	{
		if(m_write == m_read)
			return true;
		return false;
	}
	bool is_full()
	{
		if((m_write == (m_read - 1))||((m_write == (size - 1))&&(m_read == 0)))
			return true;
		return false;
	}
	bool push (T data)
	{
		m[m_write] = data;
		if(is_full())
			return false;
		if(++m_write == size)
			m_write = 0;
		return true;
	}
	T pop ()
	{
		if(is_empty())
			return m[m_read];
		T data = m[m_read];
		if(++m_read == size)
			m_read = 0;
		return data;
	}
	T operator[](uint16_t index)
	{
		return m[index];
	}
	inline void clean()
	{
		m_read = 0;
		m_write = 0;
	}
	void copy(T *data)
	{
		for(uint16_t i = 0; i < size; ++i)
		{
			*(data + i) = m[i];
		}
	}
	void format(T data = 0)
	{
		for(uint16_t i = 0; i < size; ++i)
			m[i] = data;
		clean();
	}
	inline int state()
	{
		return m_read - m_write;
	}
};

}
#endif
