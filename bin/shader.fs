#version 330 core
out vec4 FragColor;

in vec2 ourColor;
in vec2 TexCoord;

uniform bool hide_map;

uniform vec2  player1_pos;
uniform float player1_range;
uniform vec3  player1_col;

// texture samplers
uniform sampler2D tex;

void main()
{
	vec4 hidden_colour = vec4(0.1, 0.1, 0.1, 1.0);

	if (hide_map) {
		if (distance(player1_pos, gl_FragCoord.xy) > player1_range) FragColor = hidden_colour;
		else FragColor = texture(tex, TexCoord);
	} else {
		FragColor = texture(tex, TexCoord);
	}

	if (distance(player1_pos, gl_FragCoord.xy) < 10)
		FragColor = vec4(player1_col, 1.0f);

}

