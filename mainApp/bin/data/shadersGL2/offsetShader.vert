#version 120

varying vec2 texCoordVarying;
varying vec4 globalColor;

void main()
{
    texCoordVarying = gl_MultiTexCoord0.xy;
	gl_Position = ftransform();
}
