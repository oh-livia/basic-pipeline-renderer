// Very simple program to display OBJ files, that allows you to rotate the
// geometry around the Y axis.
//
// This program does NOT use a vertex shader to define the vertex colors.
// Instead, it computes the colors in the display callback (using Blinn/Phong)
// and passes those color values, one per vertex, to the vertex shader, which
// passes them directly to the fragment shader. This achieves what is called
// "gouraud shading".

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#endif


#include "amath.h"
#include "parse_obj.h"

int NumVertices;

typedef amath::vec4  point4;
typedef amath::vec4  color4;

float theta = 0.0;  // rotation around the Y (up) axis
float rho = 0.0;

GLuint buffers[2];
GLint matrix_loc;

point4 *vertices;
point4 *tri_normals;
point4 *normals;

// viewer's position, for lighting calculations
vec4 viewer = vec4(0., 0., 2., 0.0);

// light & material definitions, again for lighting calculations:
point4 light_position = point4(10., 10., 10., 0.0);
color4 light_ambient = color4(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse = color4(.80, .80, .80, 1.0);
color4 light_specular = color4(.50, .50, .50, 1.0);

color4 material_ambient = color4(1.0, 0.0, 1.0, 1.0);
color4 material_diffuse = color4(1.0, 0.8, 0.0, 1.0);
color4 material_specular = color4(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;

// we will copy our transformed points to here:
point4 *points; //[NumVertices];

// and we will store the colors, per face per vertex, here. since there is
// only 1 triangle, with 3 vertices, there will just be 3 here:
color4 *colors; //[NumVertices];

// a transformation matrix, for the rotation, which we will apply to every
// vertex:
float cameraZDist = 5;
vec4 cameraEye = vec4(0, 0, cameraZDist, 1);
vec4 cameraFocus = vec4(0, 0, 0, 1);
vec4 cameraUp = vec4(0, 1, 0, 0);
float perspAngle = 45.;
mat4 persp = Perspective(perspAngle, 1.0, cameraZDist - 1, cameraZDist + 1);
mat4 camera = LookAt(cameraEye, cameraFocus, cameraUp);
mat4 rotY;
mat4 rotX;

// matrix that includes all our camera transforms
// normally, you would define TWO matrices here - one projection
// matrix (as per the definition in the book, or the class notes, or
// by calling the Projection() or Frustum() calls from amath.h).
// the other matrix would be the one that defines the location/orientation
// of the camera in space (and so would transform each vertex of the
// model from "world space" to "camera space"). this matrix can be
// made by hand or by calling the amath.h function LookAt.
//
// Note that, because of the orientation of the view (along negative
// Z) and OpenGL projecting everything in clip space (the 2x2x2 cube)
// to the Z=1 face for rendering, your geometry should come out inverted
// in Z. To avoid this, you can make a transformation matrix with a
// [0 0 -1] in the z vector. Or, just call Ortho(-1,1,-1,1,-1,1)
// to make it for you.

GLuint program; //shaders


// product of components, which we will use for shading calculations:
vec4 product(vec4 a, vec4 b)
{
    return vec4(a[0]*b[0], a[1]*b[1], a[2]*b[2], a[3]*b[3]);
}


// transform a triangle's vertex data and put it into the points array.
// also, compute the lighting at each vertex, and put that into the colors
// array.
void tri(int i)
{/*
    // compute the lighting at each vertex, then set it as the color there:
    // normalize the vectors to the viewer and the light source:
    vec4 light_pos = normalize(light_position);
    
    // this should be taken out of the camera transform:
    vec4 view_pos;
    view_pos = normalize(viewer);

    vec4 half = normalize(light_pos+view_pos);
    color4 ambient_color, diffuse_color, specular_color;
    
    ambient_color = product(material_ambient, light_ambient);
    
    
    float dd = dot(light_pos, normals[3*i]);
    
    if(dd>0.0) diffuse_color = dd*product(light_diffuse, material_diffuse);
    else diffuse_color =  color4(0.0, 0.0, 0.0, 1.0);
    
    dd = dot(half, normals[3*i]);
    if(dd > 0.0) specular_color = exp(material_shininess*log(dd))*product(light_specular, material_specular);
    else specular_color = vec4(0.0, 0.0, 0.0, 1.0);
    
    
    // now transform the vertices according to the ctm transformation matrix,
    // and set the colors for each of them as well. as we are going to give
    // flat shading, we will ingore the specular component for now.
    points[3*i+0] = vertices[3*i+0];
    colors[3*i+0] = ambient_color + diffuse_color;
    
    points[3*i+1] = vertices[3*i+1];
    colors[3*i+1] = ambient_color + diffuse_color;
    
    points[3*i+2] = vertices[3*i+2];
    colors[3*i+2] = ambient_color + diffuse_color;
 */   
   
}


// initialization: set up a Vertex Array Object (VAO) and then
void init()
{
    // the projection/transformation we are using is none: just identity.
    
    
    // create a vertex array object - this defines mameory that is stored
    // directly on the GPU
    GLuint vao;
    
    // deending on which version of the mac OS you have, you may have to do this:
#ifdef __APPLE__
    glGenVertexArraysAPPLE( 1, &vao );  // give us 1 VAO:
    glBindVertexArrayAPPLE( vao );      // make it active
#else
    glGenVertexArrays( 1, &vao );   // give us 1 VAO:
    glBindVertexArray( vao );       // make it active
#endif
    
    // set up vertex buffer object - this will be memory on the GPU where
    // we are going to store our vertex data (that is currently in the "points"
    // array)
    glGenBuffers(1, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);  // make it active
    
    // specify that its part of a VAO, what its size is, and where the
    // data is located, and finally a "hint" about how we are going to use
    // the data (the driver will put it in a good memory location, hopefully)
    glBufferData(GL_ARRAY_BUFFER, (sizeof(point4)*2 + sizeof(color4))*NumVertices, NULL, GL_STATIC_DRAW);
    
    // load in these two shaders...  (note: InitShader is defined in the
    // accompanying initshader.c code).
    // the shaders themselves must be text glsl files in the same directory
    // as we are running this program:
    program = InitShader("vshader_passthrough.glsl", "fshader_passthrough.glsl");
 
    // ...and set them to be active
    glUseProgram(program);
    
    
    // this time, we are sending TWO attributes through: the position of each
    // transformed vertex, and the color we have calculated in tri().
    GLuint loc_pos, loc_color, loc_norms;
    
    loc_pos = glGetAttribLocation(program, "vPosition");
    glEnableVertexAttribArray(loc_pos);
    
    // the vPosition attribute is a series of 4-vecs of floats, starting at the
    // beginning of the buffer
    glVertexAttribPointer(loc_pos, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    loc_color = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(loc_color);

    // the vColor attribute is a series of 4-vecs of floats, starting just after
    // the points in the buffer
    glVertexAttribPointer(loc_color, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4)*NumVertices)); 

    loc_norms = glGetAttribLocation(program, "vNormal");
    glEnableVertexAttribArray(loc_norms);
    glVertexAttribPointer(loc_norms, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(point4)*NumVertices+
                                                                                sizeof(color4)*NumVertices));

    // now build & transform all the vertices and put them in the points array,
    // and their colors in the colors array:
    for (int i = 0; i < NumVertices / 3; ++i) {
        tri(i);
    }
    
    // tell the VBO to get the data from the points array and the colors array:
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(point4)*NumVertices, vertices );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(point4)*NumVertices, sizeof(color4)*NumVertices, colors );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(point4)*NumVertices + sizeof(color4)*NumVertices, 
                                        sizeof(point4)*NumVertices, normals);

    GLint loc_viewer;
    GLint loc_light_pos, loc_light_amb, loc_light_diff, loc_light_spec;
    GLint loc_mat_amb, loc_mat_diff, loc_mat_spec, loc_mat_shine;

    loc_viewer = glGetUniformLocation(program, "camEye");
    loc_light_pos = glGetUniformLocation(program, "light_pos");
    loc_light_amb = glGetUniformLocation(program, "light_amb");
    loc_light_diff = glGetUniformLocation(program, "light_diff");
    loc_light_spec = glGetUniformLocation(program, "light_spec");
    loc_mat_amb = glGetUniformLocation(program, "mat_amb");
    loc_mat_diff = glGetUniformLocation(program, "mat_diff");
    loc_mat_spec = glGetUniformLocation(program, "mat_spec");
    loc_mat_shine = glGetUniformLocation(program, "mat_shine");
 
    glUniform4f(loc_viewer, viewer[0], viewer[1], viewer[2], viewer[3]);
    glUniform4f(loc_light_pos, light_position[0], light_position[1], light_position[2], light_position[3]);
    glUniform4f(loc_light_amb, light_ambient[0], light_ambient[1], light_ambient[2], light_ambient[3]);
    glUniform4f(loc_light_diff, light_diffuse[0], light_diffuse[1], light_diffuse[2], light_diffuse[3]);
    glUniform4f(loc_light_spec, light_specular[0], light_specular[1], light_specular[2], light_specular[3]);
    glUniform4f(loc_mat_amb, material_ambient[0], material_ambient[1], material_ambient[2], material_ambient[3]);
    glUniform4f(loc_mat_diff, material_diffuse[0], material_diffuse[1], 
                    material_diffuse[2], material_diffuse[3]);
    glUniform4f(loc_mat_spec, material_specular[0], material_specular[1],
                    material_specular[2], material_specular[3]);
    glUniform1f(loc_mat_shine, material_shininess);
   
    // set the background color (white)
    glClearColor(1.0, 1.0, 1.0, 1.0); 
}



void display( void )
{
 
    // clear the window (with white) and clear the z-buffer (which isn't used
    // for this example).
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

    // based on where the mouse has moved to, construct a transformation matrix:
    //ctm = persp * camera * RotateX(rho) * RotateY(theta);

    //GLint loc_ctm;
    //loc_ctm = glGetUniformLocation(program, "ctm");
    //glUniformMatrix4fv(loc_ctm, 1, 1, ctm);

    rotY = RotateY(theta);
    rotX = RotateX(rho);
    GLint loc_rotY;
    GLint loc_rotX;
    GLint loc_persp;
    GLint loc_camera;

    loc_rotY = glGetUniformLocation(program, "rotY");
    loc_rotX = glGetUniformLocation(program, "rotX");
    loc_persp = glGetUniformLocation(program, "persp");
    loc_camera = glGetUniformLocation(program, "camera");

    glUniformMatrix4fv(loc_rotY, 1, 1, rotY);
    glUniformMatrix4fv(loc_rotX, 1, 1, rotX);
    glUniformMatrix4fv(loc_persp, 1, 1, persp);
    glUniformMatrix4fv(loc_camera, 1, 1, camera);

    // draw the VAO:
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
    
    
    // move the buffer we drew into to the screen, and give us access to the one
    // that was there before:
    glutSwapBuffers();
}


// use this motionfunc to demonstrate rotation - it adjusts "theta" based
// on how the mouse has moved. Theta is then used the the display callback
// to generate the transformation, ctm, that is applied
// to all the vertices before they are displayed:
void mouse_move_rotate (int x, int y)
{
    
    static int lastx = 0;// keep track of where the mouse was last:
    static int lasty = 0;
    
    int amntX = x - lastx; 
    if (amntX != 0) {
        theta +=  amntX;
        if (theta > 360.0 ) theta -= 360.0;
        if (theta < 0.0 ) theta += 360.0;
        
        lastx = x;
    }
    
    int amntY = y - lasty;
    if (amntY != 0) {
        rho += amntY;
        if (rho > 85.0) rho = 85.0;
        if (rho < -85.0) rho = -85.0;
            
        lasty = y;
    }

    // force the display routine to be called as soon as possible:
    glutPostRedisplay();
    
}


// the keyboard callback, called whenever the user types something with the
// regular keys.
void mykey(unsigned char key, int mousex, int mousey)
{
    if(key=='q'||key=='Q') exit(0);
    
    // and r resets the view:
    if (key =='r') {
        theta = 0;
    }
    
    if (key == 'z')
    {
        cameraZDist -= 0.5;
        if (cameraZDist < 2)
            cameraZDist = 2;
        cameraEye = vec4(0, 0, cameraZDist, 1);
        camera = LookAt(cameraEye, cameraFocus, cameraUp);
        persp = Perspective(perspAngle, 1.0, cameraZDist - 1, cameraZDist + 1);
        glutPostRedisplay();
    }
    
    if (key == 'x')
    {
        cameraZDist += 0.5;
        if (cameraZDist  > 50)
            cameraZDist = 50;
        cameraEye = vec4(0, 0, cameraZDist, 1);
        camera = LookAt(cameraEye, cameraFocus, cameraUp);
        persp = Perspective(perspAngle, 1.0, cameraZDist - 1, cameraZDist + 1);
        glutPostRedisplay();
    }

    // call the display() callback as soon as possible:
    glutPostRedisplay();

}

void load_obj_file (char *filename)
{
    
    std::vector<int> tri_ids;
    std::vector<float> tri_verts;
    
    read_wavefront_file(filename, tri_ids, tri_verts);
    
    NumVertices = tri_ids.size();
    
    vertices    = new point4[NumVertices];
    tri_normals = new point4[NumVertices];
    normals     = new point4[NumVertices];
    points      = new point4[NumVertices];
    colors      = new color4[NumVertices];

    // tri_ids is a list of the vertex indices for each triangle, so the first
    // triangle uses up the first 3 indices, etc.
    for (int k = 0; k < tri_ids.size() / 3; ++k) {
        
        vertices[3*k][0] = tri_verts[3*tri_ids[3*k]];
        vertices[3*k][1] = tri_verts[3*tri_ids[3*k]+1];
        vertices[3*k][2] = tri_verts[3*tri_ids[3*k]+2];
        vertices[3*k][3] = 1.;
        
        vertices[3*k+1][0] = tri_verts[3*tri_ids[3*k+1]];
        vertices[3*k+1][1] = tri_verts[3*tri_ids[3*k+1]+1];
        vertices[3*k+1][2] = tri_verts[3*tri_ids[3*k+1]+2];
        vertices[3*k+1][3] = 1.;

        vertices[3*k+2][0] = tri_verts[3*tri_ids[3*k+2]];
        vertices[3*k+2][1] = tri_verts[3*tri_ids[3*k+2]+1];
        vertices[3*k+2][2] = tri_verts[3*tri_ids[3*k+2]+2];
        vertices[3*k+2][3] = 1.;

        vec3 n = normalize(cross(vertices[3*k+1] - vertices[3*k+0],
                              vertices[3*k+2] - vertices[3*k+1]));

        tri_normals[tri_ids[3*k]] = n;
        tri_normals[tri_ids[3*k+1]] = n;
        tri_normals[tri_ids[3*k+2]] = n;
    }

    for (int i = 0; i < NumVertices; i++) {

        normals[i] = tri_normals[tri_ids[i]];

    }  
}

int main(int argc, char** argv)
{
    
    // initialize glut, and set the display modes
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    
    // give us a window in which to display, and set its title:
    glutInitWindowSize(512, 512);
    glutCreateWindow("rotate OBJ file");
    
    // for displaying things, here is the callback specification:
    glutDisplayFunc(display);
    
    // when the mouse is moved, call this function!
    // you can change this to mouse_move_translate to see how it works
    glutMotionFunc(mouse_move_rotate);
 
    // for any keyboard activity, here is the callback:
    glutKeyboardFunc(mykey);
    
#ifndef __APPLE__
    // initialize the extension manager: sometimes needed, sometimes not!
    glewInit();
#endif

    // read the obj file and put it into the points array:
    load_obj_file (argv[1]);

    // call the init() function, defined above, to initialize all the gl buffers:
    init();
    
    // enable the z-buffer for hidden surface removel:
    glEnable(GL_DEPTH_TEST);

    // once we call this, we no longer have control except through the callbacks:
    glutMainLoop();
    return 0;
}
