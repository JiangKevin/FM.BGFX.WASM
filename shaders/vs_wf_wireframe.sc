$input a_position, a_color1
$output v_view, v_bc

/*
 * Copyright 2016 Dario Manesku. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include <bgfx_shader.sh>
#include "uniforms.sh"
#include "shaderlib.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0) );

	v_view = u_camPos - mul(u_model[0], vec4(a_position, 1.0) ).xyz;
	v_bc = a_color1;
}

