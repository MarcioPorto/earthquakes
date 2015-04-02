/*
  Comp 394-01 S15 
  Assignment #3 Earthquakes Display
  Marcio Porto and Charles Park
*/

#include "Earth.h"
#include "config.h"

using namespace std;

Earth::Earth(shared_ptr<Camera> camera, shared_ptr<Light> light) {
    defaultCamera = camera;
    this->light = light;

  // Note: TEXTURE_PATH is set in config.h
    tex = Texture::fromFile(TEXTURE_PATH);
    setupGeometry();
    setupShader();
}

void Earth::setupGeometry() {
    // TODO: Write your own code here to tesselate and create texture coordinates for the Earth 
    // and then save the data to VertexBuffers, AttributeArrays, and an IndexStream.  The tesselation
    // should be based on the STACKS and SLICES parameters.
    Array<Vector3> cpuVerts;
    Array<Vector3> cpuNorms;
    Array<Vector2> cpuTexCoords;
    Array<int> cpuIndices;

    const int STACKS = 400;
    const int SLICES = 800;
    
    for(double lat=0; lat<pi(); lat+=pi()/STACKS){
        for(double lon=0; lon<=2*pi(); lon+=pi()/SLICES){
            Vector3 location = getPosition(lat,lon);
            cpuVerts.append(location);
            cpuNorms.append(location.unit());
            cpuTexCoords.append(Vector2((((SLICES/pi())*(lon))/(2*SLICES)),1-(((STACKS/pi())*(lat))/(STACKS))));
        }
    }
    for(double lon=0; lon<=2*pi(); lon+=pi()/SLICES){
        Vector3 location = getPosition(pi(),lon);
        cpuVerts.append(location);
        cpuNorms.append(location.unit());
        cpuTexCoords.append(Vector2((((SLICES/pi())*(lon))/(2*SLICES)),1-(((STACKS/pi())*(pi()))/(STACKS))));
    }
    
    int count = 0;
    int x, y, z;
    for(int i=0; i<STACKS; i++){
        for(int j=0; j<SLICES*2*2; j++){
            if(count % (SLICES*2*2) == 0){
                x = (i+1)*SLICES*2;
                y = x - SLICES*2;
                z = y+1;
                cpuIndices.append(x,y,z);
                count += 1;
            }
            else if((count % (SLICES*2*2)) == (SLICES*2*2-2)){
                x += 1;
                y = x-SLICES*2;
                z = i*SLICES*2;
                cpuIndices.append(x,y,z);
                count += 1;
            }
            else if((count % (SLICES*2*2)) == (SLICES*2*2-1)){
                y = i*SLICES*2;
                z = (i+1)*SLICES*2;
                cpuIndices.append(x,y,z);
                count += 1;
            }
            else {
                if(count % 2 == 1){
                    y += 1;
                    z = x+1;
                    cpuIndices.append(x,y,z);
                    count += 1;

                }
                else{
                    x += 1;
                    z = y+1;
                    cpuIndices.append(x,y,z);
                    count += 1;
                }
            }
        }
    }
    // Allocates space on the gpu to store my data
    vbuffer = VertexBuffer::create((sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector2)) * cpuVerts.size() +
        sizeof(long)*cpuIndices.size());
    debugAssertGLOk();

    // Actually uploads my cpu data to the gpu
    gpuVerts = AttributeArray(cpuVerts, vbuffer);
    debugAssertGLOk();
    gpuNorms = AttributeArray(cpuNorms, vbuffer);
    debugAssertGLOk();
    gpuTexCoords = AttributeArray(cpuTexCoords, vbuffer);
    gpuIndices = IndexStream(cpuIndices, vbuffer);
    debugAssertGLOk();




}

Vector3 Earth::getPosition(double latitude, double longitude) {
    // TODO: Given a latitude and longitude as input, return the corresponding 3D x,y,z position 
    // on your Earth geometry
    //latitude = toRadians(latitude);
    //longitude = toRadians(longitude);

    Vector3 location = Vector3(cos(longitude)*cos(latitude-pi()/2),sin(latitude-pi()/2),-sin(longitude)*cos(latitude-pi()/2));

    return location;

}

void Earth::setupShader() {
    G3D::String vertexShaderPath = DATA_DIR+"gouraud-shader.vrt";
    G3D::String fragmentShaderPath = DATA_DIR+"gouraud-shader.pix";

    debugAssert(FileSystem::exists(vertexShaderPath)); // If this asserts false, then your DATA_DIR is not set correctly in config.h
    debugAssert(FileSystem::exists(fragmentShaderPath));
    shader = Shader::fromFiles(vertexShaderPath, fragmentShaderPath);
}

void Earth::configureShaderArgs(RenderDevice* rd) {
    Args args;
    args.enableG3DArgs(true);
    args.setUniform("wsLight", light->position().xyz().direction());
    args.setUniform("lightColor", light->color);
    args.setUniform("wsEyePosition", defaultCamera->frame().translation);
    args.setUniform("textureSampler", tex, Sampler::video());
    
    //TODO: Set the attribute arrays and primitive types to draw the geometry
    //      that you created in setupGeometry().
    
    args.setAttributeArray("vertex", gpuVerts);
    args.setAttributeArray("normal", gpuNorms);
    args.setAttributeArray("texCoord0", gpuTexCoords);

    args.setIndexStream(gpuIndices);
    args.setPrimitiveType(PrimitiveType::TRIANGLES);

    rd->apply(shader, args);
}

void Earth::draw(RenderDevice *rd) {
  rd->pushState();
  rd->setCullFace(CullFace::NONE);

  configureShaderArgs(rd);

  rd->popState();
}


