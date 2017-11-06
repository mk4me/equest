attribute vec4 weight;
attribute vec4 index;
attribute float numBones;

varying vec4 pos;
varying vec4 rawpos;

uniform mat4 boneMatrices[33];
uniform mat4 boneSpaceMatrices[33];
uniform vec4 color;
uniform vec4 lightPos;


varying vec3 normal;
varying vec3 viewvec;



void main(void)
{
    gl_Position = vec4 (0, 0, 0, 0);
    ivec4 ibonesidx = ivec4(index);
    for (int i = 0; i < 4; i++)
    {
        gl_Position += boneMatrices[int(index[i])] * boneSpaceMatrices[int(index[i])] * gl_Vertex * weight[i];
    }

    gl_Position = gl_ModelViewProjectionMatrix * gl_Position;

    normal = gl_NormalMatrix * gl_Normal;
    viewvec = gl_NormalMatrix * vec3 (gl_Vertex);
	
	gl_FrontColor = vec4(weight.x, weight.y, weight.z, weight.w);
}

