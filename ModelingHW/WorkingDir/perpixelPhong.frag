#version 130

in vec3 Position;
in vec3 Normal;

uniform vec3 Kd; 	
uniform vec3 Ld; 		
uniform float Shininess;
uniform vec3 LightDirection; 		
uniform mat4 ViewMatrix;

out vec4 Color0;

void main( void )
{
	vec4 LightDir_eye = ViewMatrix * vec4(LightDirection,0.0);
	vec3 n = normalize( Normal );
	vec3 s = normalize( vec3(LightDir_eye));
	vec3 v = normalize(vec3(-Position));
	vec3 r = reflect( -s, n );
	vec3 LightIntensity = Kd*0.1 + Ld *( Kd * max(dot(s, n),0.0) + pow(max(dot(r,v),0.0),Shininess) );

	Color0 = vec4(LightIntensity, 1.0);
}
