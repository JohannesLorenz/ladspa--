#include "ladspa++.h"

using namespace ladspa;

struct amplifier
{
	enum class port_names
	{
		value,
		in_1,
		out_1,
		size
	};
	
	static constexpr port_info_t port_info[] =
	{
		{ "Gain", port_types::input | port_types::control,
			{(port_hints::bounded_below 
			| port_hints::logarithmic
			| port_hints::default_1),
			0
			} },
		{ "Input", port_types::input | port_types::audio, {0} },
		{ "Output", port_types::output | port_types::audio, {0} },
		final_port
	};

	static constexpr descriptor_t descriptor =
	{
		4242, // unique id
		"amp_mono_pp", // label for lookup
		properties::hard_rt_capable, // ladspa properties
		"Mono Amplifier (ladspa++ version)", // name
		"Richard Furse (LADSPA example plugins)", // author
		strings::copyright::gpl3,
		nullptr, // implementation data
		port_info
	};
	
	typedef port_array<port_names, port_info> ports_t;
	ports_t ports;
	
	void run() const
	{
		// the classic way
/*
		const_buffer in_buffer = ports.get<port_names::in_1>();
		buffer out_buffer = ports.get<port_names::out_1>();
		
		for(std::size_t i = 0; i < in_buffer.size(); i++) {
			out_buffer[i]
				= in_buffer[i]
				* ports.get<port_names::value>();
		}*/

		// you can use C++11's range based for loops on buffers
	/*	for( auto& i : out_buffer) {
			out_buffer[i] = 0;
		}*/
		
		// the new way
		auto container = ports.samples<
			port_names::in_1,
			port_names::out_1>();
		
		for( auto& ptrs : container ) {
			ptrs.get<port_names::out_1>()
				= ptrs.get<port_names::in_1>()
				* ports.get<port_names::value>();
		}

	}
};

/*
 * to be called by ladspa
 */

void _init() {}
void _fini() {}

const LADSPA_Descriptor * 
ladspa_descriptor(plugin_index_t index) {
	return collection<amplifier>::get_ladspa_descriptor(index);
}


