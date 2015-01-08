
in vec3 inPosition;
in vec4 inTexCoord;
out vec4 varTexCoord;

void main(void)
{
	varTexCoord = vec4(inTexCoord);
        gl_Position = vec4(inPosition.xyz, 1.0);
}
