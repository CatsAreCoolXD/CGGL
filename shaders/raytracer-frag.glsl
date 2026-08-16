#version 430 core

out vec4 FragColor;

uniform sampler2D frameBuffer;

// Settings
uniform int raysPerPixel;
uniform int maxBounces;

uniform float blurStrength;

uniform bool enableFrameAccumulation;

uniform int frame;
uniform int randomValue;

uniform float deltaTime;
uniform float time;

uniform vec3 cameraPos;
uniform vec3 cameraLookAt;

uniform bool enableWater;
uniform float waterLevel;

// Debug stats
uniform int debugView;
uniform int debugNormalization;
int triangleTests = 0;
int boxTests = 0;

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
    vec3 min;
    int childIndex;
    vec3 max;
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
    vec3 origin, direction, invDir;
    bool underwater;
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
    return result * 2.3283064371 * pow(10.0, -10.0);
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

// Water: https://www.shadertoy.com/view/MdXyzX

// Calculates wave value and its derivative,
// for the wave direction, position in space, wave frequency and time
vec2 wavedx(vec2 position, vec2 direction, float frequency, float timeshift) {
    float x = dot(direction, position) * frequency + timeshift;
    float wave = exp(sin(x) - 1.0);
    float dx = wave * cos(x);
    return vec2(wave, -dx);
}

// Calculates waves by summing octaves of various waves with various parameters
float getwaves(vec2 position, int iterations) {
    float wavePhaseShift = length(position) * 0.1; // this is to avoid every octave having exactly the same phase everywhere
    float iter = 0.0; // this will help generating well distributed wave directions
    float frequency = 1.0; // frequency of the wave, this will change every iteration
    float timeMultiplier = 2.0; // time multiplier for the wave, this will change every iteration
    float weight = 1.0;// weight in final sum for the wave, this will change every iteration
    float sumOfValues = 0.0; // will store final sum of values
    float sumOfWeights = 0.0; // will store final sum of weights
    float dragMultiplier = 1.0;
    for(int i=0; i < iterations; i++) {
        // generate some wave direction that looks kind of random
        vec2 p = vec2(sin(iter), cos(iter));

        // calculate wave data
        vec2 res = wavedx(position, p, frequency, time * timeMultiplier + wavePhaseShift);

        // shift position around according to wave drag and derivative of the wave
        position += p * res.y * weight * dragMultiplier;

        // add the results to sums
        sumOfValues += res.x * weight;
        sumOfWeights += weight;

        // modify next octave ;
        weight = mix(weight, 0.0, 0.2);
        frequency *= 1.18;
        timeMultiplier *= 1.07;

        // add some kind of random value to make next wave look random too
        iter += 1232.399963;
    }
    // calculate and return
    return sumOfValues / sumOfWeights;
}

// Calculate normal at point by calculating the height at the pos and 2 additional points very close to pos
vec3 GetWaterNormal(vec2 pos, float e, float depth) {
    vec2 ex = vec2(e, 0);
    float H = getwaves(pos.xy, 16) * depth;
    vec3 a = vec3(pos.x, H, pos.y);
    return normalize(
            cross(
                    a - vec3(pos.x - e, getwaves(pos.xy - ex.xy, 16) * depth, pos.y),
                    a - vec3(pos.x, getwaves(pos.xy + ex.yx, 16) * depth, pos.y + e)
            )
    );
}

// Box:             https://www.shadertoy.com/view/ld23DV
void RayBoxIntersection(in Ray ray, in Box box, inout HitInfo hitInfo) {
    vec3 rd = ray.direction;
    vec3 ro = ray.origin - box.pos;

    vec3 m = sign(rd)* abs(ray.invDir);
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
    boxTests++;
    hitInfo.hit = false;

    vec3 rd = ray.direction;
    vec3 ro = ray.origin - (box.min + box.max) / 2.0;

    vec3 m = sign(rd)*abs(ray.invDir);
    vec3 n = m*ro;
    vec3 k = abs(m)*((box.max - box.min) / 2.0);

    vec3 t1 = -n - k;
    vec3 t2 = -n + k;

	float tN = max( max( t1.x, t1.y ), t1.z );
	float tF = min( min( t2.x, t2.y ), t2.z );

    if (tN > tF || tF <= 0.) {
        hitInfo.hit = false;
    } else {
        hitInfo.hit = true;
        hitInfo.dst = tN;
    }
}

// Triangle:        https://www.shadertoy.com/view/MlGcDz
void RayTriangleIntersection(in Ray r, in Triangle triangle, inout HitInfo hitInfo) {
    triangleTests++;
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
        float dstNear = max(0, (-b - s) * 0.5);
        float dstFar = (-b + s) * 0.5;

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

bool RayBVHIntersection(in Ray ray, in BVHNode node, inout HitInfo result){
    RayBoxIntersection(ray, node, result);
    return result.hit;
}

// Thanks to Sebastian Lague for the iterative approach!
void TestMeshes(in Ray ray, inout IntersectInfo result){
    for (int i = 0; i < amountOfMeshes; i++){
        int nodeStack[64]; // The array size is the maximum depth of the BVH
        int stackIndex = 0;
        nodeStack[stackIndex++] = meshes[i].boundingBoxIndex;
        while (stackIndex > 0){
            BVHNode node = nodes[nodeStack[--stackIndex]];

            if (node.childIndex == 0) { // Leaf node, has no children
                TestTriangles(ray, node.trianglesStart, node.trianglesEnd, result);
            } else { // Test children
                HitInfo hitChildA;
                hitChildA.hit = false;
                hitChildA.dst = INF;
                HitInfo hitChildB;
                hitChildB.hit = false;
                hitChildB.dst = INF;

                RayBVHIntersection(ray, nodes[node.childIndex + 0], hitChildA);
                RayBVHIntersection(ray, nodes[node.childIndex + 1], hitChildB);

                if (hitChildA.dst < hitChildB.dst){
                    if (hitChildB.hit && hitChildB.dst < result.dst)
                        nodeStack[stackIndex++] = node.childIndex + 1;
                    if (hitChildA.hit && hitChildA.dst < result.dst)
                        nodeStack[stackIndex++] = node.childIndex + 0;
                } else {
                    if (hitChildA.hit && hitChildA.dst < result.dst)
                        nodeStack[stackIndex++] = node.childIndex + 0;
                    if (hitChildB.hit && hitChildB.dst < result.dst)
                        nodeStack[stackIndex++] = node.childIndex + 1;
                }
            }
        }
    }
}

void TestSpheres(in Ray ray, inout IntersectInfo result){
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

bool TestWater(in Ray ray, inout IntersectInfo result){
    // Ray-plane intersection
    vec3 planeNormal = vec3(0.,1.,0.);
    float planeDist = ray.origin.y - 1.;
    float a = dot(ray.direction, planeNormal);
    float d = -(dot(ray.origin, planeNormal)+planeDist)/a;
    if (a <= 0.) {
        vec3 pos = ray.origin;
        const float depth = 1.0;
        for (int i = 0; i < 64; i++){
            float waterHeight = getwaves(pos.xz, 16) * depth - depth;
            if (waterHeight + 0.01 > pos.y){
                float dst = distance(pos, ray.origin);
                if (dst >= result.dst) return false;
                result.hit = true;
                result.dst = dst;
                result.intersectPos = pos;
                result.normal = GetWaterNormal(pos.xz, 0.01, depth);

                return true;
            }
            pos += ray.direction * (pos.y - waterHeight);
        }
    }
    return false;
}

bool hitWater = false;
bool tracingUnderWater = false;
Ray refractedRay;

vec3 GetSkyColor(vec3 rd){
    if (tracingUnderWater) return vec3(0.);
    float a = 0.7*(rd.y + 1.0);
    return (1.0-a)*vec3(1.0, 1.0, 1.0) + a*vec3(0.5, 0.7, 1.0);
}

#define ABSORPTION vec3(0.45, 0.10, 0.015)
#define OUTSCATTER vec3(0.015, 0.01, 0.003)
#define INSCATTER vec3(0.002, 0.02, 0.12)

vec3 RayTrace(in Ray r, inout uint seed){
    vec3 incomingLight = vec3(0.);
    vec3 color = vec3(1.);
    Ray rayStack[12];
    int stackIndex = 0;
    rayStack[stackIndex++] = r;
    int maxRays = 2;
    int rays = 1;
    bool currentRayIsWaterReflection = false;
    float fresnel;
    while (stackIndex > 0){
        Ray ray = rayStack[--stackIndex];
        currentRayIsWaterReflection = false;
        for (int bounce = 0; bounce <= maxBounces; bounce++){
            IntersectInfo result;
            result.hit = false;
            result.dst = INF;
            TestSpheres(ray, result);
            TestMeshes(ray, result);

            if (enableWater && !ray.underwater && TestWater(ray, result)){
                if (rays < maxRays){
                    refractedRay.origin = result.intersectPos;
                    refractedRay.direction = refract(ray.direction, result.normal, 1.0 / 1.333);
                    refractedRay.invDir = 1.0 / refractedRay.direction;
                    refractedRay.underwater = true;

                    rayStack[stackIndex++] = refractedRay;
                    rays++;
                }

                const float WATER_IOR = 1.333;

                float cosTheta = clamp(-dot(ray.direction, result.normal), 0.0, 1.0);
                float F0 = pow((1.0 - WATER_IOR) / (1.0 + WATER_IOR), 2.0);

                fresnel = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);

                ray.origin = result.intersectPos;
                ray.direction = reflect(ray.direction, result.normal);
                ray.invDir = 1.0 / ray.direction;

                currentRayIsWaterReflection = true;

                continue;
            }

            if (result.hit){
                // If the ray hit a light source, add it's light to the ray. Also add the color of the material to the sum.
                vec3 emittedLight = materials[result.materialIndex].emissionColor.rgb * materials[result.materialIndex].emissionColor.a;
                vec3 materialColor = materials[result.materialIndex].color;
                if (ray.underwater){
                    float depth = distance(result.intersectPos, ray.origin);
                    color *= exp(-(ABSORPTION + OUTSCATTER) * depth) * (1. - fresnel);
                }
                if (currentRayIsWaterReflection) emittedLight *= fresnel;
                incomingLight += color * emittedLight;
                color *= materialColor;

                // Stop tracing if the light is already really low.
                float p = max(color.r, max(color.g, color.b));
                //if (bounce > 5 && RandomValue(seed) < p) break;

                if (dot(result.normal, ray.direction) > 0) result.normal *= -1.0; // Normal must be wrong, so correct it

                // Apply a small epsilon to make sure the ray doesn't hit the same object again
                ray.origin = result.intersectPos + result.normal * 0.001;

                // Depending on the smoothness, bounce the ray back into the scene randomly or reflect it if the material is smooth.
                vec3 randomDirection = normalize(result.normal + RandomPointInsideSphere(seed));
                float smoothness = materials[result.materialIndex].smoothness;
                if (smoothness == 0.0) ray.direction = randomDirection;
                else ray.direction = mix(randomDirection, reflect(ray.direction, result.normal), smoothness);
                ray.invDir = 1.0 / ray.direction;
            } else {
                vec3 skyColor = GetSkyColor(ray.direction);
                if (ray.underwater) incomingLight += (INSCATTER) * (1. / (1. - fresnel)) * 2;
                else incomingLight += skyColor * color;
                break;
            }
        }
    }
    
    return incomingLight / rays;
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
    uint seed = uint(index + (frame + randomValue) * index);

    vec3 colorSum = vec3(0.);
    for (int r = 0; r < raysPerPixel; r++) {
        Ray ray;
        ray.origin = ro;
        ray.underwater = false;

        vec2 jitter = RandomPointInCircle(seed) * blurStrength / resolution.x;
        vec3 jitteredViewpoint = i + r * jitter.x + u * jitter.y;

        ray.direction = normalize(jitteredViewpoint - ro);
        ray.invDir = 1.0 / ray.direction;

        colorSum += RayTrace(ray, seed);
    }

    vec3 col = colorSum / (raysPerPixel + int(hitWater));
    vec3 previousCol = texture(frameBuffer, texCoords).rgb;
    vec3 avg = (previousCol * frame + col) / (frame + 1);

    float triangleDebug = float(triangleTests) / float(debugNormalization);
    float boxDebug = float(boxTests) / float(debugNormalization);

    if (debugView == 1) col = vec3(triangleDebug);
    if (debugView == 2) col = vec3(boxDebug);
    if (debugView == 3) col = vec3(boxDebug, 0., triangleDebug);

    if (!enableFrameAccumulation) return col;

    if (frame == 0) return col;
    return avg;
}

void main(){
    FragColor = vec4(GetPixelColor(), 1.0);
}