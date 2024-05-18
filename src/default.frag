
#version 330 core

#define BUFFER_SIZE 256

out vec4 frag_color;
in vec4 p;

// uniform vec3 vertices[BUFFER_SIZE];
uniform int vertices_amount;
// uniform vec3 lights[BUFFER_SIZE];
uniform int lights_amount;

uniform sampler2D tex;
// verkar fungera
uniform samplerBuffer vertices;
uniform samplerBuffer lights;


void main() {

    // float val1 = texelFetch(vertices, vertices_amount).x;
    // float val2 = texelFetch(lights, 0).x;

    // float v = texelFetch(vertices, 1).x;
    // frag_color = vec4(v, v, v, 1.0f);

    // frag_color = vec4(val1, val2, 1.0f, 1.0f);

    vec3 pos = vec3(p.x, p.y, p.z);

    int index;
    if (abs(p.w - floor(p.w)) > abs(p.w - ceil(p.w))) {
        index = int(ceil(p.w));
    } else {
        index = int(floor(p.w));
    }

    // int index = int(p.w);
    
    vec4 def = texture(tex, vec2(p.x, p.z)); 
    // vec4 def = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float brightness = 0.2f;

    vec3 tA = vec3(texelFetch(vertices, index*9).x, texelFetch(vertices, index*9+1).x, texelFetch(vertices, index*9+2).x);
    vec3 tB = vec3(texelFetch(vertices, index*9+3).x, texelFetch(vertices, index*9+4).x, texelFetch(vertices, index*9+5).x);
    vec3 tC = vec3(texelFetch(vertices, index*9+6).x, texelFetch(vertices, index*9+7).x, texelFetch(vertices, index*9+8).x);

    vec3 normal = normalize(cross(tB - tA, tC - tA));

    for (int i = 0; i < lights_amount; i++) {

        int gets_light = 1;

        vec3 ray = vec3(texelFetch(lights, i*3).x, texelFetch(lights, i*3+1).x, texelFetch(lights, i*3+2).x) - pos;
        vec3 norm_ray = normalize(ray);

        float dist = length(ray);

        if (dot(normal, norm_ray) < 0.0f) {
            normal = -normal;
        }

        for (int j = 0; j < vertices_amount / 3; j++) {

            if (j == index) {
                continue;
            }

            vec3 A = vec3(texelFetch(vertices, j*9).x, texelFetch(vertices, j*9+1).x, texelFetch(vertices, j*9+2).x) - pos;
            vec3 B = vec3(texelFetch(vertices, j*9+3).x, texelFetch(vertices, j*9+4).x, texelFetch(vertices, j*9+5).x) - pos;
            vec3 C = vec3(texelFetch(vertices, j*9+6).x, texelFetch(vertices, j*9+7).x, texelFetch(vertices, j*9+8).x) - pos;

            vec3 triangle_normal = normalize(cross(B - A, C - A));

            float d = triangle_normal.x * A.x + triangle_normal.y * A.y + triangle_normal.z * A.z;
            float intersect_scalar = d / (triangle_normal.x * norm_ray.x + triangle_normal.y * norm_ray.y + triangle_normal.z * norm_ray.z);

            if (intersect_scalar >= 0.04f && intersect_scalar <= dist) {

                vec3 intersection = norm_ray * intersect_scalar;

                float triangle_area = length(cross(B - A, C - A));

                float area_a = length(cross(B - intersection, C - intersection));
                float area_b = length(cross(C - intersection, A - intersection));
                float area_c = length(cross(A - intersection, B - intersection));
                float area_sum = area_a + area_b + area_c;

                if (abs(triangle_area - area_sum) <= 0.001f) {
                    gets_light = 0;
                    break;
                }

            }

        }

        if (gets_light == 1) {

            // vec3 tmp_normal = normal;
            // if (dot(normal, norm_ray) < 0.0f) {
            //     tmp_normal = -normal;
            // }

            brightness += (1 / (dist*dist)) * dot(normal, norm_ray);

            // brightness += (1 / (dist*dist)) * dot(normal, norm_ray);

        }

    }

    //frag_color = vec4(brightness, brightness, brightness, 1.0f);
    frag_color = vec4(def.r * brightness, def.g * brightness, def.b * brightness, 1.0f);

}
