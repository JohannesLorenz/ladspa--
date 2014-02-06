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
	
	port_array<port_names, port_descriptors> ports;
	
	/*template<port_names p>
	struct buffer
	{
		buffer(const port_des* ports) {
			
		}
	};
	*/
	void run(sample_size_t sample_count) const
	{
		//data* pfInput = ports[ port_names::in_1 ];
		
		safe_ports<decltype(ports)> _ports(ports, sample_count);
		
		const const_buffer& pfInput = _ports.get<port_names::in_1>();
		const buffer& pfOutput = _ports.get<port_names::out_1>();
		const const_pointer& fGain = _ports.get<port_names::value>();
		
		//typedef port_array<port_names, port_des>::id id;
		
	/*	const_buffer pfInput = ports[ port_array<port_names, port_des>::id<port_names::in_1>() ];
		buffer pfOutput = ports[ port_array<port_names, port_des>::id<port_names::out_1>() ];
		
		const_pointer fGain = ports[ port_array<port_names, port_des>::id<port_names::value>() ];
	*/	

#if 0		
		data* pfInput = ports[ port_array<port_names, port_des>::id<port_names::in_1>() ];
		data* pfOutput = ports[ port_array<port_names, port_des>::id<port_names::out_1>() ];
		
		const data fGain = *ports[ port_array<port_names, port_des>::id<port_names::value>() ];
		
#if 0	
		data* pfInput = ports.get<port_names::out_1>();*/
		
		
		data* pfOutput = ports[port_names::out_1];
		const data fGain = *ports[port_names::value];
#endif	
		//data* pfInput = ports[_id<port_names::in_1>()];
		
	/*	buffer<port_names::in_l> pfInput2 = ports;
		buffer<port_names::out_l> pfOutput2 = ports;
		single<port_names::value> fGain = ports;
	*/	
		
		for (sample_size_t i = 0; i < sample_count; i++) 
		*(pfOutput++) = *(pfInput++) * fGain;
#endif
	}
};




/*constexpr const test make_descriptor(const struct ladspa_descriptor& l)
{
	return test { 0,0 };
}*/


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
  /*      for(auto i : builder<struct ladspa_descriptor>::port_names)
         std::cout << i << std::endl;
        return 0;*/
	
	
	std::cout << builder<amplifier>::get_ladspa_descriptor().Name << std::endl;
	
//	for(auto i : builder<amplifier>::port_names)
 //        std::cout << i << std::endl;
       
//	std::cout << typeid(port_array<amplifier::port_names, amplifier::port_des>::return_value).name() << std::endl;
	
	//std::tuple<ladspa::pointer_template<const float>> tp =
	//ladspa::pointer_template<const float>(((const float*)(& ports)->ladspa::port_array<PortNamesT, port_des_array>::operator[]<amplifier::port_names, ((const ladspa::port*)(& amplifier::port_des))>(0)), ((int)_sample_count))
	
	return 0;
	
	 
	 
//	(void) des;
	
	
}

