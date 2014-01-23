#ifndef SINGLETONT_H
#define SINGLETONT_H

template <class SingletonClass>
class SingletonT
{
public:
	static SingletonClass* instance()
	{
		if (!m_instance)
		{
			m_instance = new SingletonClass;
		}
		return m_instance;
	}
	static void free()
	{
		if (m_instance)
		{
			delete m_instance;
            m_instance = 0;
		}
    }

protected:
    SingletonT()
    {}

private:
    static SingletonClass *m_instance;
};

template <class SingletonClass>
SingletonClass* SingletonT<SingletonClass>::m_instance = 0;

#endif // SINGLETONT_H
