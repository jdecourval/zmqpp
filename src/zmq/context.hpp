/*
 *  Created on: 9 Aug 2011
 *      Author: Ben Gray (@benjamg)
 */

#ifndef CPPZMQ_CONTEXT_HPP_
#define CPPZMQ_CONTEXT_HPP_

#include <cassert>

#include <zmq.h>

#include "exception.hpp"

namespace zmq
{

/**
 * \class context
 * \brief The context class represents internal zmq context and io threads.
 *
 * By default the context class will create one thread, however this can be
 * overridden in the constructor.
 *
 * The context class is the only object that can be considered thread safe.
 *
 * This class is c++0x move supporting and cannot be copied.
 */
class context
{
public:
	/**
	 * \brief Initialise the 0mq context.
	 *
	 * If only inproc is used then the context may be created with zero threads.
	 * Any inproc endpoint using sockets must be created using the same context.
	 *
	 * The context is thread safe an may be used anywhere in your application,
	 * however there is no requirement (other than inproc restrictions) for you
	 * to do this.
	 *
	 * \param threads an integer argument for the number of required threads. Defaults to 1
	 */
	context(int const& threads = 1)
		: _context(nullptr)
	{
		_context = zmq_init(threads);

		if (nullptr == _context)
		{
			throw zmq_internal_exception();
		}
	}

	/**
	 * \brief closes the 0mq context.
	 *
	 * Any sockets using this context will throw exceptions if they are still
	 * in use
	 */
	~context()
	{
		if (nullptr != _context)
		{

#ifndef NDEBUG // unused assert variable in release
			int result = zmq_term(_context);
			assert(0 == result);
#else
			zmq_term(_context);
#endif // NDEBUG

			_context = nullptr;
		}
	}

	/**
	 * \brief Move supporting constructor
	 *
	 * allows zero-copy move semantics to be used with this class
	 *
	 * \param source a rvalue instance of the object who's internals we wish to steal
	 */
	context(context&& source)
	{
		_context = source._context;
		source._context = nullptr;
	}

	/**
	 * \brief Move supporting operator
	 *
	 * allows zero-copy move semantics to be used with this class
	 *
	 * \param source a rvalue instance of the object who's internals we wish to steal
	 */
	void operator=(context&& source)
	{
		_context = source._context;
		source._context = nullptr;
	}

	/**
	 * \brief Validity checking of the context
	 *
	 * Checks if the underlying 0mq context for this instance is valid.
	 *
	 * Contexts should always be valid unless people are doing 'fun' things with
	 * std::move.
	 *
	 * \return boolean true if the object is valid.
	 */
	operator bool() const
	{
		return nullptr != _context;
	}

	/**
	 * DON'T USE: Access to the raw 0mq context
	 *
	 * \return void pointer to the underlying 0mq context
	 */
	operator void*() const
	{
		return _context;
	}

private:
	void* _context;

	// No copy - private and not implemented
	context(context const&);
	void operator=(context const&);
};

}

#endif /* CPPZMQ_CONTEXT_HPP_ */