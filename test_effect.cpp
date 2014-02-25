#include "ladspa++.h"

using namespace ladspa;

struct amplifier
{
	static constexpr port port_descriptors[] =
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
		port_descriptors
	};
	
	enum class port_names
	{
		value,
		in_1,
		out_1,
		size
	};
	
	typedef port_array<port_names, port_descriptors> ports_t;
	ports_t ports;
	
	void run(sample_size_t sample_count) const
	{
	/*	const const_buffer& pfInput = ports.get<port_names::in_1>();
		const buffer& pfOutput = ports.get<port_names::out_1>();
		const const_pointer& fGain = ports.get<port_names::value>();
		
		data* _pfOutput= pfOutput.begin();
		const data* _pfInput= pfInput.begin(); */
		
	/*	multi_iterator<ports_t,
			port_names::in_1,
			port_names::out_1>
			itr(ports, sample_count);
		
		for (sample_size_t i = 0; i < sample_count; i++) 
		 *(_pfOutput++) = *(_pfInput++) * (*fGain);*/
		
		auto container = ports.samples<
			port_names::in_1,
			port_names::out_1>(sample_count);
		
		for( auto& ptrs : container ) {
			ptrs.get<port_names::out_1>() = ptrs.get<port_names::in_1>() * (*(ports.get<port_names::value>()));
		}
 	//	for( multi_iterator<ports_t, port_names::in_1, port_names::out_1> itr = container.begin(); itr != container.end(); ++itr) { /*itr.get<port_names::in_1>();*/ }
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

#include <typeinfo>
#include <iostream>
int main()
{  
	std::cout << builder<amplifier>::get_ladspa_descriptor().Name << std::endl;
	return 0;
}

