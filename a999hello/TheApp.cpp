#include "TheApp.h"
#include "platform.h"
#include "linmath.h"
 
static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};
 
static const char* vertex_shader_text =
"#version 320 es\n"
"precision lowp float;\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec2 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";
 
static const char* fragment_shader_text =
"#version 320 es\n"
"precision lowp float;\n"
"in vec3 color;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"    FragColor = vec4(color, 1.0);\n"
"}\n";
 
unsigned int vao_buffer, vertex_buffer, vertex_shader, fragment_shader, program;
int mvp_location, vpos_location, vcol_location;
float angle_z = 0;
 
int TheApp::getReady() {
    bExitApp = false;
 
    glGenVertexArrays(1, &vao_buffer);
    glBindVertexArray(vao_buffer);
 
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
 
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
 
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
 
    mvp_location = glGetUniformLocation(program, "MVP");
 
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");
 
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)(sizeof(float) * 2));
 
    return 1;
}
int TheApp::drawFrame() {
    myPollEvents();
 
    mat4x4 m, p, mvp;
 
    glClear(GL_COLOR_BUFFER_BIT);
    angle_z += 0.01f;
    mat4x4_identity(m);
    mat4x4_rotate_Z(m, m, angle_z);
    mat4x4_ortho(p, -screenRatio, screenRatio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);
 
    glUseProgram(program);
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);
    glDrawArrays(GL_TRIANGLES, 0, 3);
 
    mySwapBuffers();
    return 1;
}
int TheApp::cleanUp() {
    return 1;
}
int TheApp::onScreenResize(int width, int height) {
    if (screenSize[0] == width && screenSize[1] == height)
        return 0;
    screenSize[0] = width;
    screenSize[1] = height;
    screenRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
    mylog(" screen size %d x %d\n", width, height);
    return 1;
}
int TheApp::run() {
    getReady();
    while (!bExitApp) {
        drawFrame();
    }
    cleanUp();
    return 1;
}