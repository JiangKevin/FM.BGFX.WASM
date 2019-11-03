$input v_wpos, v_view, v_normal, v_tangent, v_bitangent, v_texcoord0// in...

/*
 * Copyright 2011-2019 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include <bgfx_shader.sh>
#include "shaderlib.sh"
#include "texture.sh"

SAMPLER2D(s_texColor,  0);
SAMPLER2D(s_texNormal, 1);

mat3 mtx3FromCols(vec3 c0, vec3 c1, vec3 c2)
{
#if BGFX_SHADER_LANGUAGE_GLSL
	return mat3(c0, c1, c2);
#else
	return transpose(mat3(c0, c1, c2));
#endif
}

void main()
{
	mat3 tbn = mtx3FromCols(v_tangent, v_bitangent, v_normal);

	vec3 normal;
	normal.xy = texture2D(s_texNormal, v_texcoord0).xy * 2.0 - 1.0;
	normal.z = sqrt(1.0 - dot(normal.xy, normal.xy) );
	vec3 view = normalize(v_view);

	vec4 color = toLinear(texture2D(s_texColor, v_texcoord0) );

	gl_FragColor.xyz = color.xyz;
	gl_FragColor.w = 1.0;
	gl_FragColor = toGamma(gl_FragColor);
}