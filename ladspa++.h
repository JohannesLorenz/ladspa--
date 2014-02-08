/*************************************************************************/
/* ladspa++ - A C++ wrapper for ladspa                                   */
/* Copyright (C) 2014                                                    */
/* Johannes Lorenz                                                       */
/* https://github.com/JohannesLorenz/                                    */
/*                                                                       */
/* This program is free software; you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation; either version 3 of the License, or (at */
/* your option) any later version.                                       */
/* This program is distributed in the hope that it will be useful, but   */
/* WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      */
/* General Public License for more details.                              */
/*                                                                       */
/* You should have received a copy of the GNU General Public License     */
/* along with this program; if not, write to the Free Software           */
/* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110, USA  */
/*************************************************************************/

#include <array>
#include <bitset>
#include <cassert>
#include <tuple>
#include <iostream> // TODO

#include <ladspa.h>

namespace helpers
{

//! default criterium argument, which accepts all numbers
template<int>
struct criterium_true {
	static constexpr bool value = true;
};

//! the resulting sequence type to test for
template<int ...Seq>
struct full_seq
{
};

namespace seq_helpers
{

template<int Start, int I, template<int> class Criterium, int ...Seq>
struct _seq_recurse;

//! @a cond is true => spawn @a I into seq and recurse
template<int Start, int I, template<int> class Criterium, bool cond, int ...Seq>
struct _seq_check
{
	using type = typename _seq_recurse<Start, I, Criterium, I, Seq...>::type;
};

//! @a cond is false => skip @a I in sequence and recurse
template<int Start, int I, template<int> class Criterium, int ...Seq>
struct _seq_check<Start, I, Criterium, false, Seq...>
{
	using type = typename _seq_recurse<Start, I, Criterium, Seq...>::type;
};

//! counting down, we have not reached the @a Start -> evaluate @a Criterium
template<int Start, int I, template<int> class Criterium, int ...Seq>
struct _seq
{
	using type = typename _seq_check<Start, I, Criterium, Criterium<I>::value, Seq...>::type;
};

//! counting down, we have reached the @a Start
template<int Start, template<int> class Criterium, int ...Seq>
struct _seq<Start, Start, Criterium, Seq...>
{
	using type = full_seq<Seq...>;
};

//! sequence recursion, just decrements @a I
template<int Start, int I, template<int> class Criterium, int ...Seq>
struct _seq_recurse
{
	using type = typename _seq<Start, I-1, Criterium, Seq...>::type;
};

} // namespace seq_helpers

//! creates maths like range, i.e. [Start, N]
template<int N, int Start = 1, template<int> class Criterium = criterium_true>
using math_seq = typename seq_helpers::_seq<Start-1, N, Criterium>::type;

//! creates C like range, i.e. [Start-1, N-1]. Criterium is *not* counted for i-1
template<int N, int Start = 0, template<int> class Criterium = criterium_true>
using seq = math_seq<N-1, Start, Criterium>;

// token from [1]
template<int... Is> struct old_seq {};
template<int N, int... Is> struct gen_seq : gen_seq<N-1, N-1, Is...> {};
template<int... Is> struct gen_seq<0, Is...> : old_seq<Is...> {};

//! a quick fix since array's data() member is not constexpr
template<class T, std::size_t N>
constexpr const T* get_data(const std::array<T, N>& a)
{
	return &a.front();
}

template<class EnumClass>
constexpr std::size_t enum_size() { return (std::size_t)EnumClass::size; }

//! checks whether a class has a function called "instantiate"
//! note: if we need multiple of those, inherit from a base class
template <typename T>
class has_instantiate
{ // source: [2]
	typedef char one;
	typedef long two;

	template <typename C> static one test( decltype(&C::instantiate) ) ;
	template <typename C> static two test( ... );
public:
	enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

template <class T, template<class > class HaveClass>
using en_if_has = typename std::enable_if<HaveClass<T>::value>::type;

template <class T, template<class > class HaveClass>
using en_if_doesnt_have = typename std::enable_if<!HaveClass<T>::value>::type;

namespace foreach
{
template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), void>::type
_for_each(std::tuple<Tp...> &, FuncT) // Unused arguments are given no names.
{ }

template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I < sizeof...(Tp), void>::type
_for_each(std::tuple<Tp...>& t, FuncT& f)
{
	if(!std::get<I>(t))
	 _for_each<I + 1, FuncT, Tp...>(t, f);
}

template<std::size_t I = 0, typename FuncT, typename... Tp>
inline typename std::enable_if<I < sizeof...(Tp), void>::type
for_each(std::tuple<Tp...>& t, FuncT& f)
{
	_for_each(t,f);
}
}

} // namespace helpers

namespace ladspa
{

typedef unsigned long port_size_t;
typedef unsigned long sample_size_t;
typedef unsigned long sample_rate_t;
typedef unsigned long plugin_index_t;
typedef LADSPA_Data data;

template<class T>
class bitmask
{
	int bits;
public:
	// member functions
	constexpr int get_bits() const { return bits; }
	constexpr bool is(const bitmask<T> property) const {
		return bits & property.bits;
	}
	constexpr bitmask(int _bits) : bits(_bits) {}
	
	// related functions
	friend constexpr bitmask<T> operator|
		(const bitmask<T>& l, const bitmask<T>& r)
	{
		return bitmask<T>(l.bits|r.bits);
	}
};
/*
template <template<class > >
using en_if_is = typename std::enable_if<HaveClass<T>::value>::type;

template <class T, template<class > class HaveClass>
using en_if_doesnt_have = typename std::enable_if<!HaveClass<T>::value>::type;
*/
struct properties
{
	typedef bitmask<properties> m;
	static constexpr m realtime = LADSPA_PROPERTY_REALTIME;
	static constexpr m inplace_broken = LADSPA_PROPERTY_INPLACE_BROKEN;
	static constexpr m hard_rt_capable = LADSPA_PROPERTY_HARD_RT_CAPABLE;
};

struct port_types
{
	typedef bitmask<port_types> m;
	// ladspa forbids input + output
	static constexpr m input = LADSPA_PORT_INPUT;
	static constexpr m output = LADSPA_PORT_OUTPUT;
	// ladspa forbids audio + control
	static constexpr m control = LADSPA_PORT_CONTROL;
	static constexpr m audio = LADSPA_PORT_AUDIO;
};

struct port_hints
{
	typedef bitmask<port_hints> m;
	static constexpr m bounded_below = LADSPA_HINT_BOUNDED_BELOW;
	static constexpr m bounded_above = LADSPA_HINT_BOUNDED_ABOVE;
	static constexpr m toggled = LADSPA_HINT_TOGGLED;
	static constexpr m sample_rate = LADSPA_HINT_SAMPLE_RATE;
	static constexpr m logarithmic = LADSPA_HINT_LOGARITHMIC;
	static constexpr m integer = LADSPA_HINT_INTEGER;
	
	static constexpr m default_mask = LADSPA_HINT_DEFAULT_MASK;
	static constexpr m default_none = LADSPA_HINT_DEFAULT_NONE;
	static constexpr m default_minimum = LADSPA_HINT_DEFAULT_MINIMUM;
	static constexpr m default_low = LADSPA_HINT_DEFAULT_MIDDLE;
	static constexpr m default_high = LADSPA_HINT_DEFAULT_HIGH;
	static constexpr m default_maximum = LADSPA_HINT_DEFAULT_MAXIMUM;
	static constexpr m default_0 = LADSPA_HINT_DEFAULT_0;
	static constexpr m default_1 = LADSPA_HINT_DEFAULT_1;
	static constexpr m default_100 = LADSPA_HINT_DEFAULT_100;
	static constexpr m default_440 = LADSPA_HINT_DEFAULT_440;
};

//! class to access a whole buffer
//! can be hard copied, this will be cheap
template<class T>
class buffer_template
{
	T* _data;
	//! this is overhead for multiple equal-sized buffers
	//! however, this overhead is not much
	std::size_t _size;
public:
	buffer_template() {}
	buffer_template(T* _in_data, std::size_t _in_size)
		: _data(_in_data), _size(_in_size) {}
	
	void assign(T* _in_data) { _data = _in_data; }
	//void set_size(std::size_t _in_size) { _size = _in_size; }
	int set_size(std::size_t _in_size) const { return _size; }
	std::size_t size() const { return size; }
	
/*	T* begin() { return _data; }
	const T* begin() const { return _data; }
	T* end() { return _data + size(); }
	const T* end() const { return _data + size(); }
	
	T* data() { return begin(); }
	const T* data() const { return begin(); }*/
	T* begin() const { return _data; }
	T* end() const { return _data + size(); }
	T* data() const { return begin(); }
};

// TODO: common base class for both?
template<class T>
class pointer_template
{
	T* _data;
public:
	pointer_template() {}
	pointer_template(T* _in_data)
		: _data(_in_data) {}
	pointer_template(T* _in_data, int )
		: pointer_template(_in_data) {}
	void assign(T* _in_data) { _data = _in_data; }
	
	const T& operator*() const { return *_data; }
	T& operator*() { return *_data; }
};

typedef buffer_template<data> buffer;
typedef buffer_template<const data> const_buffer;
typedef pointer_template<data> pointer;
typedef pointer_template<const data> const_pointer;

struct port
{
	struct range_hint_t
	{
		bitmask<port_hints> descriptor;
		data lower_bound;
		data upper_bound;
	};
		
	enum class type
	{
		name,
		descriptor,
		range_hint
	};
	
	const char* name;
	bitmask<port_types> descriptor;
	range_hint_t hint;
	
	constexpr bool is_final() const { return (name == nullptr); }
	
	template<type I> struct type_id {};
	template<type T> struct falsify_id {
		static_assert(true, "This code should never be reached");
	};
	
	// get function and overloads
	template<class T, type PT>
	T& get(type_id<PT>) const { falsify_id<PT>(); }
	constexpr const char* get(type_id<type::name>) const { return name; }
	constexpr LADSPA_PortDescriptor get(type_id<type::descriptor>) const {
		return descriptor.get_bits();
	}
	constexpr LADSPA_PortRangeHint get(type_id<type::range_hint>) const {
		return { hint.descriptor.get_bits(),
			hint.lower_bound,
			hint.upper_bound };
	}
};

//! port marking the end, recognized via the nullptr
static constexpr port final_port = { nullptr, 0, {0,0,0} };


/*
 *  Return value specialisations for port_array
 */
template<class contained_type, const bitmask<port_types>* bm, class Enable = void>
struct return_value_base_type
{
	typedef buffer_template<contained_type> type;
};

template<class contained_type, const bitmask<port_types>* bm>
struct return_value_base_type
	<contained_type, bm,
	typename std::enable_if<bm->is(port_types::control)>::type>
{
	typedef pointer_template<contained_type> type;
};

template<class base_type,
	const bitmask<port_types>* bm,
	class Enable = void>
struct return_value_access_type
{
	typedef const base_type type;
};

template<class base_type,
	const bitmask<port_types>* bm>
struct return_value_access_type
	<base_type, bm,
	typename std::enable_if<bm->is(port_types::output)>::type>
{
	typedef base_type type;
};

template<const bitmask<port_types>* bm>
using t1 = typename return_value_access_type<data, bm>::type;
template<const bitmask<port_types>* bm>
using return_value = typename return_value_base_type<t1<bm>, bm>::type;

template<class PortNamesT, const port* port_des_array>
class port_array
{
public:
	constexpr static std::size_t port_size = helpers::enum_size<PortNamesT>();
	typedef PortNamesT port_names_t;
	template<std::size_t PortName>
	struct port_return_value
	{	
		static constexpr auto arr_elem = port_des_array[PortName];
		static constexpr auto descr = arr_elem.descriptor;
		
		typedef return_value<&descr> type;
	};
	
	template<typename T> struct falsify : std::false_type { }; // TODO: other falsify is wrong
	template<class T> struct _return_value { static_assert(falsify<T>::value, "This should not be instantiated."); };
	template<int ...Is>
	struct _return_value<helpers::full_seq<Is...>>
	{
		typedef std::tuple<typename port_return_value<Is>::type...> type;
	};
	typedef typename _return_value<typename helpers::seq<port_size>>::type
	return_value;
private:
#ifdef SIMPLE
	std::array<data*, helpers::enum_size<PortNamesT>()> storage;
#else
	return_value storage;
	
	struct caller
	{
		void (&callback)(port_array&, data*);
	};
	
	template<class T>
	static constexpr typename std::array<caller, port_size> init_callers(T)
	{
		static_assert(falsify<T>::value, "This should not be instantiated.");
	}

	template<int ...Is>
	static constexpr typename std::array<caller, port_size> init_callers(helpers::full_seq<Is...>)
	{
		return {{port_array::set_static<Is>...}};
	}
	
	static constexpr typename std::array<caller, port_size> callers
		= init_callers(typename helpers::seq<port_size>{});
	
#endif
	static bool in_range_cond(int id)
	{
		return id >= 0 && id < helpers::enum_size<PortNamesT>();
	}
	
#ifdef SIMPLE
	void set_internal(int id, data* d) {
		assert(in_range_cond(id));

		storage[id] = d;
	}
#else
	template<int id>
	void set_internal(data* d) {
		assert(in_range_cond(id));
		std::get<id>(storage).assign(d);
	}
#endif
public:
	void set(int id, data* d) {
#ifdef SIMPLE
		set_internal(id, d);
#else
		callers[id].callback(*this, d);
#endif
	}

	template<int id>
	static void set_static(port_array& p, data* d) {
		std::cout << "set static" << std::endl;
		p.set_internal<id>(d);
	}
	
	template<PortNamesT PortName>
	struct id
	{
		//typedef PortName port_name;
	};
	
public:
#ifdef SIMPLE
	data* operator[](std::size_t id) const
	{
		return storage[id];
	}
	data* operator[](port_names_t id) const
	{	
		return storage[(std::size_t)id];
	}
	
/*	struct caller
	{
		const LADSPA_Descriptor& (&callback)();
	};

	static constexpr std::array<caller, sizeof...(Args)> init_callers()
	{
		return {{builder<Args>::get_ladspa_descriptor...}};
	}
	
	static constexpr std::array<caller, sizeof...(Args)> callers
		= init_callers();*/
	
#else
public:
	template<std::size_t id>
	const typename port_return_value<id>::type& get() const {
		return std::get<id>(storage);
	}
	template<port_names_t id>
	const typename port_return_value<(std::size_t)id>::type& get() const {
		return get<(std::size_t)id>();
	}
#endif
};

#ifndef SIMPLE
template<class PortNamesT, const port* port_des_array>
constexpr typename std::array<typename port_array<PortNamesT, port_des_array>::caller, 
	port_array<PortNamesT, port_des_array>::port_size>
	port_array<PortNamesT, port_des_array>::callers;
#endif

//! class which the user will have to fill in
struct descriptor_t
{
	unsigned long unique_id;
	const char * label;
	bitmask<properties> plugin_properties;
	const char * name;
	const char * maker;
	const char * copyright;
	void * implementationData;
	const ladspa::port* arr;
};

// returns size of the port array
static constexpr port_size_t get_port_size(const ladspa::port* arr) {
	return arr->is_final() ? 0 : get_port_size(arr + 1) + 1;
}

// TODO: capitalization
/**
 * The builder class
 * @brief Builds all things ladspa needs for you parameters
 * 
 * You provide parameters via @a Plugin. They will be converted into a ladspa
 * descriptor. This includes descripting data and function pointers. The
 * latter means that this class also redirects the callbacks to @a Plugin. So
 * you have to provide some stuff for @a Plugin (note that the names must be
 * the same):
 * 
 * static constexpr descriptor_t descriptor;
 * static constexpr port[...]
 * TODO: rewrite that // TODO: other file
 * port_array<port_names, static constexpr port[...]> ports; // port_names is an enum, port des
 * LADSPA_Handle* instantiate(const struct _LADSPA_Descriptor * _descriptor, sample_rate_t _sample_rate) // optional
 * void sample_size_t sample_count()
 * 
 */
template<class Plugin>
class builder
{
	static constexpr const descriptor_t& instance = Plugin::descriptor;
	static constexpr port_size_t port_size = get_port_size(instance.arr);
//	typedef port_array<typename Plugin::port_names,
//		static_cast<const ladspa::port*>(instance.arr)> port_array_t;
	
	/*
	 * This shifts the arrays
	 */
	template<port::type PT, int N, class T, int... Is>
	static constexpr auto _get_elem(helpers::old_seq<Is...>, T* lhs)
	-> std::array<decltype(lhs[0].get(port::type_id<PT>())), N>
	{
		return {{lhs[Is].get(port::type_id<PT>())...}};
	}

	template<port::type PT, int N, class T>
	static constexpr auto get_elem(T* lhs)
	-> decltype( _get_elem<PT, N>(helpers::gen_seq<N>{}, lhs) )
	{
		return _get_elem<PT, N>(helpers::gen_seq<N>{}, lhs);
	}
	
	/*
	 * These functions are the ladspa callbacks
	 */
	//! This version lets the user choose a constructor
	template<class _Plugin, helpers::en_if_has<
		_Plugin, helpers::has_instantiate>* = nullptr>
	static LADSPA_Handle _instantiate(
		const struct _LADSPA_Descriptor * _descriptor,
		sample_rate_t _sample_rate) {
		return (void*)_Plugin::instantiate(_descriptor, _sample_rate);
	}
	
	//! This simply allocates a new object without parameters
	template<class _Plugin, helpers::en_if_doesnt_have
		<_Plugin, helpers::has_instantiate>* = nullptr>
	static LADSPA_Handle _instantiate(
		const struct _LADSPA_Descriptor *, sample_rate_t ) {
		return new _Plugin;
	}
	
	static void _cleanup(LADSPA_Handle _instance) {
		delete static_cast<Plugin*>(_instance);
	}
	
	static void 
	_connect_port(LADSPA_Handle _instance,
		port_size_t _port,
		data * _data_location)
	{
		static_cast<Plugin*>(_instance)
			->ports.set(_port, _data_location);
	}
	
	template<class ...Args> static void do_nothing(Args...) {}
	
	template<int ...Is>
	static void set_sizes(Plugin*& pl,
		sample_size_t _sample_count,
		helpers::full_seq<Is...>)
	{
		// please do not remove this line, it is really necessary!
		do_nothing(pl->ports.template get<Is>().set_size(_sample_count)...);
		
//		auto& ports = static_cast<Plugin*>(_instance)->ports;
		
	/*	typedef decltype(static_cast<Plugin*>(_instance)->ports) port_array_t;
		
		typename port_array_t::return_value tp
			(	// constructors for the save buffers/pointers
				// they have the pointers as argument, and their lenght
				// (which will be ignored for pointer_template)
				typename port_array_t::template port_return_value<Is>::type(ports[Is], _sample_count)...
			);*/
	//	set_sizes();
		
	//	static_cast<Plugin*>(_instance)->run(_sample_count);
	}
	
	template<int i>
	struct criterium_is_buffer
	{
		constexpr static bool value = instance.arr[i].descriptor.is(port_types::audio);
	};
	
	//void _activate(LADSPA_Handle Instance);
	static void _run(LADSPA_Handle _instance,
		sample_size_t _sample_count)
	{
		Plugin* instance = static_cast<Plugin*>(_instance);
		
		set_sizes(instance, _sample_count, helpers::seq<port_size, 0, criterium_is_buffer>{});
	//	_run_with_seq(_instance, _sample_count, helpers::gen_seq<port_size>{});
		instance->run(_sample_count); // todo: remove sample_count
	}
	
	static constexpr std::array<LADSPA_PortDescriptor, port_size>
		port_descriptors
		= get_elem<port::type::descriptor, port_size>(instance.arr);
	static constexpr std::array<const char*, port_size>
		port_names
		= get_elem<port::type::name, port_size>(instance.arr);
	static constexpr std::array<LADSPA_PortRangeHint, port_size>
		port_range_hints
		= get_elem<port::type::range_hint, port_size>(instance.arr);
	static constexpr LADSPA_Descriptor descriptor_for_ladspa =
	{
		instance.unique_id,
		instance.label,
		instance.plugin_properties.get_bits(),
		instance.name,
		instance.maker,
		instance.copyright,
		port_size,
		helpers::get_data(port_descriptors),
		helpers::get_data(port_names),
		helpers::get_data(port_range_hints),
		instance.implementationData,
		_instantiate<Plugin>,
		_connect_port,
		nullptr, //_activate,
		_run,
		nullptr, // run_adding
		nullptr, // set_run_adding_gain
		nullptr, // deactivate
		_cleanup
	};
public:
	static const LADSPA_Descriptor& get_ladspa_descriptor()
	{ return descriptor_for_ladspa; }
};

template<class port_array_t>
class safe_ports
{
private:
	const typename port_array_t::return_value port_tuple;
	
	template<int ...Is>
	safe_ports(const port_array_t& ports, int _sample_count, helpers::full_seq<Is...>)
	: port_tuple(	// constructors for the save buffers/pointers
				// they have the pointers as argument, and their lenght
				// (which will be ignored for pointer_template)
				typename port_array_t::template port_return_value<Is>::type(ports[Is], _sample_count)...
			)
	{
	}
	
	template<std::size_t id>
	auto get() -> decltype( std::get<id>(port_tuple) ) {
		return std::get<id>(port_tuple);
	}
	
public:
	safe_ports(const port_array_t& ports, int _sample_count)
	: safe_ports(ports, _sample_count, typename helpers::seq<helpers::enum_size<typename port_array_t::port_names_t>()>())
	{}

	template<typename port_array_t::port_names_t id>
	auto get() -> decltype( this->get<(std::size_t)id>() ) {
		return get<(std::size_t)id>();
	}
};

/*
 * Static member definitions
 */
template<class Plugin>
	constexpr std::array<LADSPA_PortDescriptor,
	builder<Plugin>::port_size>
	builder<Plugin>::port_descriptors;
template<class Plugin>
	constexpr std::array<const char*, builder<Plugin>::port_size>
	builder<Plugin>::port_names;
template<class Plugin>
	constexpr std::array<LADSPA_PortRangeHint, builder<Plugin>::port_size>
	builder<Plugin>::port_range_hints;

template<class Plugin>
	constexpr LADSPA_Descriptor builder<Plugin>::descriptor_for_ladspa;

namespace strings
{
	namespace copyright
	{
		constexpr const char* none = "None";
		constexpr const char* gpl2 = "GPL v2";
		constexpr const char* gpl3 = "GPL v3";
	}
	
}

//! This is for a collection of your plugin types
//! it helps you to select the correct descriptor at runtime
template<class ...Args>
class collection
{
	struct caller
	{
		const LADSPA_Descriptor& (&callback)();
	};

	static constexpr std::array<caller, sizeof...(Args)> init_callers()
	{
		return {{builder<Args>::get_ladspa_descriptor...}};
	}
	
	static constexpr std::array<caller, sizeof...(Args)> callers
		= init_callers();
public:
	//! Returns the requested descriptor
	//! or nullptr if the index is out of range.
	static const LADSPA_Descriptor* get_ladspa_descriptor(
		plugin_index_t index)
	{
		return (index < 0 || index >= sizeof...(Args))
			? nullptr
			: &callers[index].callback();
	}
};

template<class ...Args>
constexpr std::array<typename collection<Args...>::caller, sizeof...(Args)>
	collection<Args...>::callers;

struct correctness_checker
{
	// TODO: different IDs, In Out, broken inplace etc
};

}

// sources:
// [1] http://stackoverflow.com/questions/17173697/
//     find-out-in-c-if-binary-number-is-prefix-of-another
// [2] http://stackoverflow.com/questions/257288/
//     is-it-possible-to-write-a-c-template
//     -to-check-for-a-functions-existence[]
