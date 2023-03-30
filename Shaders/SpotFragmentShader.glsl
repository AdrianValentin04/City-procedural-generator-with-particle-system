#version 330
 
uniform sampler2D texture_1;
uniform sampler2D texture_2;

in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 light_pos;
 
in vec2 texcoord;

layout(location = 0) out vec4 out_color;

void main()
{
	vec3 light_direction = vec3(0, -1, 0);

	// TODO : calculate the out_color using the texture2D() function
	vec4 color = texture2D(texture_1, texcoord);
	vec3 object_color = vec3(color.x, color.y, color.z);


	vec3 w_normal = normalize(world_normal);

	vec3 L = normalize( light_pos - world_position);
	vec3 V = normalize( eye_position - world_position);
	vec3 H = normalize( L + V );

	// TODO: define ambient light component
	float ambient_light = material_kd * 0.40;

	// TODO: compute diffuse light component
	float diffuse_light = max(dot(L, w_normal), 0) * material_kd;

	// TODO: compute specular light component
	float specular_light = 0;

	if (diffuse_light > 0)
	{
		specular_light =  material_ks * pow(max(dot(w_normal, H), 0), material_shininess);
	}

	// TODO: compute light

	//Spotlight
	float cut_off = radians(35);
	float spot_light = dot(-L, light_direction);
	float spot_light_limit = cos(cut_off);
	float I;
	if (spot_light > cos(cut_off))
	{
		// fragmentul este iluminat de spot, deci se calculeaza valoarea luminii conform  modelului Phong
		// se calculeaza atenuarea luminii
		float d = distance(light_pos, world_position);
		float linear_att = (spot_light - spot_light_limit) / (1 - spot_light_limit);
		float light_att_factor = pow(linear_att, 2);
		I = ambient_light + (diffuse_light + specular_light)*light_att_factor * 2;
	} else {
		I = ambient_light;
	}

	// TODO: write pixel out color
	out_color = vec4(object_color, 1) * I;
}