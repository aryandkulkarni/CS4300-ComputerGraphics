// ==================================================================
#version 330 core

// The final output color of each 'fragment' from our fragment shader.
out vec4 FragColor;

// Take in our previous texture coordinates.
in vec3 FragPos;
in vec2 v_texCoord;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

// If we have texture coordinates, they are stored in this sampler.
uniform sampler2D u_DiffuseMap; 
uniform sampler2D u_NormalMap; 

void main()
{
	// Store the texture coordinates
	vec3 normal = texture(u_NormalMap, v_texCoord).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec3 color = texture(u_DiffuseMap, v_texCoord).rgb;

    vec3 ambient_component = 0.05 * color;

    vec3 lightvector = TangentLightPos - TangentFragPos;
    vec3 lightDirection = normalize(lightvector);

    float diffuse = max(dot(normal, lightDirection), 0.0);
    vec3 diffuse_component = diffuse * color;

    vec3 reflectDirection = reflect(-lightDirection, normal);
    
    vec3 viewVector = TangentViewPos - TangentFragPos;
    vec3 viewDir = normalize(viewVector);

    float specular = pow(max(dot(viewDir, reflectDirection), 0.0), 20000.0);
    vec3 specular_component = specular * vec3(0.4);

    vec3 result = ambient_component + diffuse_component + specular_component;

    FragColor = vec4(result, 1.0);
}
