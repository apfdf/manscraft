
#version 330 core

#define BUFFER_SIZE 128

out vec4 frag_color;
in vec4 p;

uniform vec3 vertices[BUFFER_SIZE];
uniform int vertices_amount;
uniform vec3 lights[BUFFER_SIZE];
uniform int lights_amount;

uniform sampler2D tex;

void main() {

    vec3 pos = vec3(p.x, p.y, p.z);
    int index = int(p.w);
    
    vec4 def = texture(tex, vec2(p.x, p.y)); 
    float brightness = 0.2f;

    vec3 normal = normalize(cross(vertices[index*3+1] - vertices[index*3], vertices[index*3+2] - vertices[index*3]));

    for (int i = 0; i < lights_amount; i++) {

        int gets_light = 1;

        vec3 ray = lights[i] - pos;
        vec3 norm_ray = normalize(ray);

        float dist = length(ray);

        if (dot(normal, norm_ray) > 0.0f) {
            normal = -normal;
        }

        for (int j = 0; j < vertices_amount / 3; j++) {

            if (j == index) {
                continue;
            }

            vec3 A = vertices[j*3] - pos;
            vec3 B = vertices[j*3+1] - pos;
            vec3 C = vertices[j*3+2] - pos;

            vec3 triangle_normal = normalize(cross(B - A, C - A));

            float d = triangle_normal.x * A.x + triangle_normal.y * A.y + triangle_normal.z * A.z;
            float intersect_scalar = d / (triangle_normal.x * norm_ray.x + triangle_normal.y * norm_ray.y + triangle_normal.z * norm_ray.z);

            if (intersect_scalar >= 0.1f && intersect_scalar <= length(ray)) {

                vec3 intersection = norm_ray * intersect_scalar;

                float triangle_area = length(cross(B - A, C - A));

                float area_a = length(cross(B - intersection, C - intersection));
                float area_b = length(cross(C - intersection, A - intersection));
                float area_c = length(cross(A - intersection, B - intersection));
                float area_sum = area_a + area_b + area_c;

                if (abs(triangle_area - area_sum) <= 0.0001f) {
                    gets_light = 0;
                    break;
                }

            }

        }

        if (gets_light == 1) {

            vec3 tmp_normal = normal;
            if (dot(normal, norm_ray) < 0.0f) {
                tmp_normal = -normal;
            }

            brightness += (1.0f / (dist*dist)) * dot(tmp_normal, norm_ray);

        }

    }

    if (brightness == 0.2f) {
        frag_color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    } else {
        frag_color = vec4(def.r * brightness, def.g * brightness, def.b * brightness, 1.0f);
    }

}
