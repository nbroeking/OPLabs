#include <types.h>

namespace os {

INTERFACE Runnable {
public:
	inline virtual ~Runnable(){};
	virtual void run() = 0;
};

template <typename T>
class FunctionRunner : public Runnable {
public:
	FunctionRunner(void(*func)(T&arg), const T& arg) {
		m_function = func;
		m_arg = arg;
	}

	void run() OVERRIDE {
		m_function( m_arg );
	}
private:
	T m_arg;
	void (*m_function)(T& arg);
};

}
