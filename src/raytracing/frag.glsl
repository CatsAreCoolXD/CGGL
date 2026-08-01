#version 430 core

out vec4 FragColor;

uniform sampler2D frameBuffer;
uniform sampler2D noiseTex;

// Settings
uniform int raysPerPixel;
uniform int maxBounces;

uniform float blurStrength;

uniform bool enableFrameAccumulation;

uniform int frame;

uniform vec3 cameraPos;
uniform vec3 cameraLookAt;

struct Material {
    vec3 color;
    vec4 emissionColor;
    float smoothness;
};

struct Sphere {
    vec3 center;
    float radius;
    int materialIndex;
};

struct Triangle {
    vec3 p1;
    vec3 p2;
    vec3 p3;
    int materialIndex;
};

struct Box {
    vec3 pos;
    vec3 size;
    int materialIndex;
};

struct Mesh {
    int triangleIndexStart;
    int triangleIndexEnd;
    int boundingBoxIndex;
};

struct BVHNode {
    vec3 pos;
    int childA;
    vec3 size;
    int childB;
    int trianglesStart;
    int trianglesEnd;
};

uniform int amountOfMaterials;
layout (std430, binding = 0) buffer materialBuffer
{
    Material materials[];
};

uniform int amountOfSpheres;
layout (std430, binding = 1) buffer sphereBuffer
{
    Sphere spheres[];
};

uniform int amountOfTriangles;
layout (std430, binding = 2) buffer triangleBuffer
{
    Triangle triangles[];
};

uniform int amountOfBoxes;
layout (std430, binding = 3) buffer boxBuffer
{
    Box boxes[];
};

uniform int amountOfMeshes;
layout (std430, binding = 4) buffer meshBuffer
{
    Mesh meshes[];
};

uniform int amountOfBVHNodes;
layout (std430, binding = 5) buffer nodeBuffer
{
    BVHNode nodes[];
};

uniform vec2 resolution;

#define INF 100000

struct Ray {
    vec3 origin, direction;
    vec3 color;
};

struct HitInfo {
    bool hit;
    float dst;
    vec3 normal;
};

struct IntersectInfo {
    bool hit;
    vec3 intersectPos;
    float dst;
    int materialIndex;
    vec3 normal;
};

float RandomValue(inout uint state)
{
	state = state * 747796405 + 2891336453;
    uint result = ((state >> ((state >> 28) + 4)) ^ state) * 277803737;
    result = (result >> 22) ^ result;
    return result / 4294967295.0;
}

float RandomValueNormalDistribution(inout uint seed){
    float theta = 2.0 * 3.1415926 * RandomValue(seed);
    float rho = sqrt(-2.0 * log(max(RandomValue(seed), 0.00001)));
    return rho * cos(theta);
}

vec3 RandomPointInsideSphere(inout uint seed){
    float x = RandomValueNormalDistribution(seed);
    float y = RandomValueNormalDistribution(seed);
    float z = RandomValueNormalDistribution(seed);
    return normalize(vec3(x, y, z));
}

vec2 RandomPointInCircle(inout uint seed){
    float angle = RandomValue(seed) * 2.0 * 3.14159;
    vec2 pointOnCircle = vec2(cos(angle), sin(angle));
    return pointOnCircle * sqrt(RandomValue(seed));
}

/* RAY-SHAPE INTERSECTION FUNCTIONS */

// Box:             https://www.shadertoy.com/view/ld23DV
void RayBoxIntersection(in Ray ray, in Box box, inout HitInfo hitInfo) {
    hitInfo.hit = false;

    vec3 rd = ray.direction;
    vec3 ro = ray.origin - box.pos;

    vec3 m = sign(rd)/max(abs(rd), 1e-8);
    vec3 n = m*ro;
    vec3 k = abs(m)*box.size;
	
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;

	float tN = max( max( t1.x, t1.y ), t1.z );
	float tF = min( min( t2.x, t2.y ), t2.z );
	
    if (tN > tF || tF <= 0.) {
        hitInfo.hit = false;
    } else {
        if (true) {
            hitInfo.hit = true;
        	hitInfo.normal = -sign(rd)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);
            hitInfo.dst = tN;
        } else if (true) { 
            hitInfo.hit = true;
        	hitInfo.normal = -sign(rd)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);
            hitInfo.dst = tF;
        } else {
            hitInfo.hit = false;
        }
    }
}

void RayBoxIntersection(in Ray ray, in BVHNode box, inout HitInfo hitInfo) {
    hitInfo.hit = false;

    vec3 rd = ray.direction;
    vec3 ro = ray.origin - box.pos;

    vec3 m = sign(rd)/max(abs(rd), 1e-8);
    vec3 n = m*ro;
    vec3 k = abs(m)*box.size;
	
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;

	float tN = max( max( t1.x, t1.y ), t1.z );
	float tF = min( min( t2.x, t2.y ), t2.z );
	
    if (tN > tF || tF <= 0.) {
        hitInfo.hit = false;
    } else {
        if (true) {
            hitInfo.hit = true;
        	hitInfo.normal = -sign(rd)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);
            hitInfo.dst = tN;
        } else if (true) { 
            hitInfo.hit = true;
        	hitInfo.normal = -sign(rd)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);
            hitInfo.dst = tF;
        } else {
            hitInfo.hit = false;
        }
    }
}

// Triangle:        https://www.shadertoy.com/view/MlGcDz
void RayTriangleIntersection(in Ray r, in Triangle triangle, inout HitInfo hitInfo) {
    vec3 v0 = triangle.p1;
    vec3 v1 = triangle.p2;
    vec3 v2 = triangle.p3;

    vec3 v1v0 = v1 - v0;
    vec3 v2v0 = v2 - v0;
    vec3 rov0 = r.origin - v0;

    vec3  n = cross( v1v0, v2v0 );
    vec3  q = cross( rov0, r.direction );
    float d = 1.0/dot( r.direction, n );
    float u = d*dot( -q, v2v0 );
    float v = d*dot(  q, v1v0 );
    float t = d*dot( -n, rov0 );

    hitInfo.hit = false;

    if( u<0. || v<0. || (u+v)>1.) {
        hitInfo.hit = false;
    } else if (t > 0.){
        hitInfo.hit = true;
        hitInfo.dst = t;
        hitInfo.normal = normalize(n);
    }
}

void RaySphereIntersection(in Sphere sphere, in Ray r, inout HitInfo hitInfo)
{
    hitInfo.hit = false;

    vec3 oc = sphere.center - r.origin;
    float b = -2.0 * dot(r.direction, oc);
    float c = dot(oc, oc) - sphere.radius*sphere.radius;
    float discriminant = b*b - 4*c;

    // No solution when d < 0 (ray misses sphere)
    if (discriminant >= 0)
    {
        float s = sqrt(discriminant);
        // Distance to nearest intersection point (from quadratic formula)
        float dstNear = max(0, (-b - s) / 2);
        float dstFar = (-b + s) / 2;

        // Ignore intersections that occur behind the ray
        if (dstFar >= 0)
        {
            hitInfo.hit = true;
            bool isInside = dstNear == 0;
            hitInfo.dst = isInside ? dstFar : dstNear;
        }
    }
}

/* SCENE INTERSECTION FUNCTIONS */

void TestTriangles(in Ray ray, int start, int end, inout IntersectInfo result){
    result.hit = false;
    result.dst = INF;
    for (int i = start; i < end; i++){
        HitInfo info;
        RayTriangleIntersection(ray, triangles[i], info);
        if (info.hit && info.dst < result.dst){
            result.hit = true;
            result.intersectPos = ray.origin + ray.direction * info.dst;
            result.dst = info.dst;

            result.normal = info.normal;

            result.materialIndex = triangles[i].materialIndex;
        }
    }
}

void RayBVHIntersection(in Ray ray, inout HitInfo result, int nodeIndex){
    RayBoxIntersection(ray, nodes[nodeIndex], result);
}

void TestMeshes(in Ray ray, inout IntersectInfo result){
    result.hit = false;
    result.dst = INF;
    for (int i = 0; i < amountOfMeshes; i++){
        HitInfo boundingBoxHitInfo;
        RayBVHIntersection(ray, boundingBoxHitInfo, meshes[i].boundingBoxIndex);
        if (boundingBoxHitInfo.hit){
            TestTriangles(ray, nodes[meshes[i].boundingBoxIndex].trianglesStart, nodes[meshes[i].boundingBoxIndex].trianglesEnd, result);
        }
    }
}

void TestSpheres(in Ray ray, inout IntersectInfo result){
    result.hit = false;
    result.dst = INF;
    for (int i = 0; i < amountOfSpheres; i++){
        HitInfo info;
        RaySphereIntersection(spheres[i], ray, info);
        if (info.hit && info.dst < result.dst) {
            result.hit = true;
            result.intersectPos = ray.origin + ray.direction * info.dst;
            result.dst = info.dst;

            vec3 d = result.intersectPos - spheres[i].center;
            result.normal = normalize(d);

            result.materialIndex = spheres[i].materialIndex;
        }
    }
}

vec3 GetSkyColor(vec3 rd){
    return vec3(0.);
    float a = 0.5*(rd.y + 1.0);
    return (1.0-a)*vec3(1.0, 1.0, 1.0) + a*vec3(0.5, 0.7, 1.0);
}

vec3 RayTrace(in Ray ray, inout uint seed){
    vec3 incomingLight = vec3(0.);
    vec3 color = vec3(1.);
    for (int bounce = 0; bounce <= maxBounces; bounce++){
        IntersectInfo resultSpheres;
        IntersectInfo resultMeshes;
        TestSpheres(ray, resultSpheres);
        TestMeshes(ray, resultMeshes);

        IntersectInfo result = (resultSpheres.dst < resultMeshes.dst && resultSpheres.hit) ? resultSpheres : resultMeshes;
        if (result.hit){
            vec3 emittedLight = materials[result.materialIndex].emissionColor.rgb * materials[result.materialIndex].emissionColor.a;
            incomingLight += color * emittedLight;
            color *= materials[result.materialIndex].color;

            if (dot(result.normal, ray.direction) > 0) result.normal *= -1.0; // Normal must be wrong, so correct it

            ray.origin = result.intersectPos + result.normal * 0.001;

            vec3 randomDirection = normalize(result.normal + RandomPointInsideSphere(seed));
            ray.direction = mix(randomDirection, reflect(ray.direction, result.normal), materials[result.materialIndex].smoothness);
        } else {
            if (bounce == 0) incomingLight = GetSkyColor(ray.direction);
            break;
        }
    }
    
    return incomingLight;
}

vec3 GetPixelColor(){
    // Thanks to myself 6 months ago for creating this camera code
    vec2 uv = (gl_FragCoord.xy-.5*resolution)/resolution.y;

    float camZoom = 1.; // Distance from the camera to the viewport
    
    vec3 ro = cameraPos;
    
    vec3 f = normalize(cameraLookAt-ro);
    vec3 r = cross(vec3(0., 1., 0.), f);
    vec3 u = cross(f, r);
    
    vec3 c = ro+f*camZoom;
    vec3 i = c + uv.x*r + uv.y*u;
    vec3 dir = i - ro;
    vec3 rd = normalize(dir);

    vec2 texCoords = gl_FragCoord.xy / resolution;
    float index = gl_FragCoord.x + gl_FragCoord.y * resolution.x;
    float maxIndex = resolution.x + resolution.y * resolution.x;
    uint seed = uint(index + frame * index);

    vec3 colorSum = vec3(0.);
    for (int r = 0; r < raysPerPixel; r++) {
        Ray ray;
        ray.origin = ro;

        vec2 jitter = RandomPointInCircle(seed) * blurStrength / resolution.x;
        vec3 jitteredViewpoint = i + r * jitter.x + u * jitter.y;

        ray.direction = normalize(jitteredViewpoint - ro);

        colorSum += RayTrace(ray, seed);
    }

    vec3 col = colorSum / raysPerPixel;
    vec3 previousCol = texture(frameBuffer, texCoords).rgb;
    vec3 avg = (previousCol * frame + col) / (frame + 1);

    if (!enableFrameAccumulation) return col;

    if (frame == 0) return col;
    return avg;
}

void main(){
    FragColor = vec4(GetPixelColor(), 1.0);
}