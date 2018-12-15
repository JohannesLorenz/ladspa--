/*************************************************************************/
/* ladspa++ - A C++ wrapper for ladspa                                   */
/* Copyright (C) 2014-2018                                               */
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
		// TODO: description
		{ "Gain",
			"Amount of multiplication to input signal.",
			port_types::input | port_types::control,
			{(port_hints::bounded_below
			| port_hints::logarithmic
			| port_hints::default_1),
			0
			} },
		port_info_common::audio_input,
		port_info_common::audio_output,
		port_info_common::final_port
	};

	static constexpr info_t info =
	{
		4242, // unique id
		"amp_mono_pp", // label for lookup
		properties::hard_rt_capable,
		"Mono Amplifier (ladspa++ version)", // name
		"Richard Furse (LADSPA example plugins)", // author
		"This effect multiplies the input signal by the Gain value"
			"and sends it out.",
		{"gain", "amplifier", "volume"},
		strings::copyright::gpl3,
		nullptr // implementation data
	};
	
	void run(port_array_t<port_names, port_info>& ports)
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
		auto container = ports.buffers<
			port_names::in_1,
			port_names::out_1>();
		
		for( auto& ptrs : container ) {
			ptrs.get<port_names::out_1>()
				= ptrs.get<port_names::in_1>()
				* ports.get<port_names::value>();
		}

	}
	
	// if you need the sample rate, you can give an arg to the ctor
	// amplifier(sample_rate_t _sample_rate) {}
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


