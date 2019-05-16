#include <cstdio>
#include <cstdint>
#include <limits>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

bool game_object_is_running = false;
int direction_move = 0;
int u_direction_move = -1;
bool fire_is_pressed = 0;
bool begin = 0;
/*Space Invaders is an arcade game that first came out in the late 70’s.
 It was originally created as a 2D top down shooter where the User_Player can
 maneuver left and right while trying to stop a large mass of alien ships
 from reaching the User_Players position. The way the player stops the oncoming
 aliens is by shooting them while moving left and right to avoid incoming
 fire from the mass of aliens. The player can earn points for each alien
 ship the player destroys. The more aliens that are destroyed, the faster
 they move and the more often they shoot back at the player. The game goes
 until the player is hit 3 times eliminating all three of the player lives
 or the User_Player eliminates all of the alien ships before they reach the player
 position. When one of these game ending scenarios happens, your score will be saved.
 */
#define GL_ERROR_CASE(glerror)\
case glerror: snprintf(error, sizeof(error), "%s", #glerror)
//debug
inline void gl_debug(const char *file, int line) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        char error[128];
        //switch for error
        
        switch (err) {
                GL_ERROR_CASE(GL_INVALID_ENUM); break;
                GL_ERROR_CASE(GL_INVALID_VALUE); break;
                GL_ERROR_CASE(GL_INVALID_OPERATION); break;
                GL_ERROR_CASE(GL_INVALID_FRAMEBUFFER_OPERATION); break;
                GL_ERROR_CASE(GL_OUT_OF_MEMORY); break;
            default: snprintf(error, sizeof(error), "%s", "UNKNOWN_ERROR"); break;
        }
        //file print
        fprintf(stderr, "%s - %s: %d\n", error, file, line);
    }
}

#undef GL_ERROR_CASE
//validate shader
void validate_shader(GLuint shader, const char *file = 0) {
    static const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    GLsizei length = 0;
    
    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
    
    if (length > 0) {
        printf("Shader %d(%s) compile error: %s\n", shader, (file ? file : ""), buffer);
    }
}
//validate program
bool validate_program(GLuint program) {
    static const GLsizei BUFFER_SIZE = 512;
    GLchar buffer[BUFFER_SIZE];
    GLsizei length = 0;
    
    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
    
    if (length > 0) {
        printf("Program %d link error: %s\n", program, buffer);
        return false;
    }
    
    return true;
}

/*
 Callbacks were used as in the homework assignments. Examples are simple
 as left/right keys move player left and right. Space bar signals for the
 player to fire. Enter starts the game and Escape Closes the window.
 */
//error on callback
void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
//callback key
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //second set of controls added
    switch (key) {
        case GLFW_KEY_ESCAPE:
            if (action == GLFW_PRESS) game_object_is_running = false;
            break;
        case GLFW_KEY_Q:
            if (action == GLFW_PRESS) game_object_is_running = false;
            break;
        case GLFW_KEY_RIGHT:
            if (action == GLFW_PRESS) direction_move += 1;
            else if (action == GLFW_RELEASE) direction_move -= 1;
            break;
        case GLFW_KEY_D:
            if (action == GLFW_PRESS) direction_move += 1;
            else if (action == GLFW_RELEASE) direction_move -= 1;
            break;
        case GLFW_KEY_LEFT:
            if (action == GLFW_PRESS) direction_move -= 1;
            else if (action == GLFW_RELEASE) direction_move += 1;
            break;
        case GLFW_KEY_A:
            if (action == GLFW_PRESS) direction_move -= 1;
            else if (action == GLFW_RELEASE) direction_move += 1;
            break;
        case GLFW_KEY_SPACE:
            if (action == GLFW_RELEASE) fire_is_pressed = true;
            break;
        case GLFW_KEY_W:
            if (action == GLFW_RELEASE) fire_is_pressed = true;
            break;
        case GLFW_KEY_ENTER:
            if (action == GLFW_PRESS) begin = true;
        default:
            break;
    }
}
//algorithm from tutorial
/* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
uint32_t xorshift32(uint32_t* rng)
{
    uint32_t x = *rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *rng = x;
    return x;
}

double random(uint32_t* rng)
{
    //return xorshift32
    return (double)xorshift32(rng) / std::numeric_limits<uint32_t>::max();
}
//struct for buffer
struct Buffer
{
    size_t width, height;
    uint32_t* data;
};
//struct for sprite
struct Sprite
{
    size_t width, height;
    uint8_t* data;
};
//struct for alien
struct Alien
{
    size_t x, y;
    uint8_t type;
};
//struct for projectile
struct Projectile
{
    size_t x, y;
    int direction;
};
//struct for player
struct User_Player
{
    size_t x, y;
    size_t live_total;
};
//struct for alien ships
struct Alien_Ship
{
    size_t x, y;
    size_t live_total;
    int direction;
};
//limit max projectiles to not overload
#define MAX_PROJECTILES 128
//struct for game
struct game_object
{
    //struct for game with width, height, remaining_aliens, num_Projectiles, aliens, Alien_Ship, User_Player, Projectiles[]
    /*size_t width, height;
     size_t remaining_aliens;
     size_t num_Projectiles;
     Alien* aliens;
     Alien_Ship Alien_Ship;
     User_Player User_Player;
     Projectile Projectiles[MAX_PROJECTILES];*/
    size_t width, height;
    size_t remaining_aliens;
    size_t num_Projectiles;
    Alien* aliens;
    Alien_Ship Alien_Ship;
    User_Player User_Player;
    Projectile Projectiles[MAX_PROJECTILES];
};
//struct for animations
struct SpriteAnimation
{
    //struct for sprite animation
    /*bool loop;
     size_t number_of_frames;
     size_t frame_length;
     size_t game_object_time;
     Sprite** frames;*/
    bool loop;
    size_t number_of_frames;
    size_t frame_length;
    size_t game_object_time;
    Sprite** frames;
};
//enum for the alien types
enum AlienType : uint8_t
{
    /*ALIEN_DEAD = 0,
     ALIEN_TYPE_A = 1,
     ALIEN_TYPE_B = 2,
     ALIEN_TYPE_C = 3*/
    ALIEN_DEAD = 0,
    ALIEN_TYPE_A = 1,
    ALIEN_TYPE_B = 2,
    ALIEN_TYPE_C = 3
};
//buffer clear
void buffer_clear(Buffer* buffer, uint32_t color)
{
    for (size_t i = 0; i < buffer->width * buffer->height; i++)
    {
        buffer->data[i] = color;
    }
}
//check for overlap
bool sprite_overlap_check(
                          const Sprite& sp_a, size_t x_a, size_t y_a,
                          const Sprite& sp_b, size_t x_b, size_t y_b
                          )
{
    //check for collision if sprites overlap, see if any of the pixels in the sprite overlap
    if (x_a < x_b + sp_b.width && x_a + sp_a.width > x_b &&
        y_a < y_b + sp_b.height && y_a + sp_a.height > y_b)
    {
        return true;
    }
    
    return false;
}

//draw sprite given size and color
void buffer_draw_sprite(Buffer* buffer, const Sprite& sprite, size_t x, size_t y, uint32_t color)
{
    for (size_t xi = 0; xi < sprite.width; xi++)
    {
        for (size_t yi = 0; yi < sprite.height; yi++)
        {
            if (sprite.data[yi * sprite.width + xi] &&
                (sprite.height - 1 + y - yi) < buffer->height &&
                (x + xi) < buffer->width)
            {
                buffer->data[(sprite.height - 1 + y - yi) * buffer->width + (x + xi)] = color;
            }
        }
    }
}
//buffer draw number
void buffer_draw_number(
                        Buffer* buffer,
                        const Sprite& number_spritesheet, size_t number,
                        size_t x, size_t y,
                        uint32_t color)
{
    uint8_t digits[64];
    size_t num_digits = 0;
    
    size_t current_number = number;
    do
    {
        digits[num_digits++] = current_number % 10;
        current_number = current_number / 10;
    } while (current_number > 0);
    
    size_t xp = x;
    size_t stride = number_spritesheet.width * number_spritesheet.height;
    Sprite sprite = number_spritesheet;
    for (size_t i = 0; i < num_digits; i++)
    {
        uint8_t digit = digits[num_digits - i - 1];
        sprite.data = number_spritesheet.data + digit * stride;
        buffer_draw_sprite(buffer, sprite, xp, y, color);
        xp += sprite.width + 1;
    }
}
//buffer draw text
void buffer_draw_text(
                      Buffer* buffer,
                      const Sprite& text_spritesheet,
                      const char* text,
                      size_t x, size_t y,
                      uint32_t color)
{
    size_t xp = x;
    size_t stride = text_spritesheet.width * text_spritesheet.height;
    Sprite sprite = text_spritesheet;
    for (const char* charp = text; *charp != '\0'; charp++)
    {
        char character = *charp - 32;
        if (character < 0 || character >= 65) continue;
        
        sprite.data = text_spritesheet.data + character * stride;
        buffer_draw_sprite(buffer, sprite, xp, y, color);
        xp += sprite.width + 1;
    }
}
//convert rgb to unsigned int 32
uint32_t rgb_to_uint32(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 24) | (g << 16) | (b << 8) | 255;
}

//START MAIN
/////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[])
{
 //set buffer size for height and width
    const size_t buffer_width = 224;
    const size_t buffer_height = 256;
    
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) return -1;
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(2 * buffer_width, 2 * buffer_height, "Space Invaders 4610", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    
    glfwSetKeyCallback(window, key_callback);
    
    glfwMakeContextCurrent(window);
    //error checking
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "Error initializing GLEW.\n");
        glfwTerminate();
        return -1;
    }
    //gl version
    int glVersion[2] = { -1, 1 };
    glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);
    
    gl_debug(__FILE__, __LINE__);
    //for console window
    printf("Using OpenGL: %d.%d\n", glVersion[0], glVersion[1]);
    printf("Renderer used: %s\n", glGetString(GL_RENDERER));
    printf("Shading Language: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    glfwSwapInterval(1);
    
    glClearColor(1.0, 0.0, 0.0, 1.0);
    
    // Create graphics buffer
    Buffer buffer;
    buffer.width = buffer_width;
    buffer.height = buffer_height;
    buffer.data = new uint32_t[buffer.width * buffer.height];
    
    buffer_clear(&buffer, 0);
    
    // Create texture for presenting buffer to OpenGL
    GLuint buffer_texture;
    glGenTextures(1, &buffer_texture);
    glBindTexture(GL_TEXTURE_2D, buffer_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, buffer.width, buffer.height, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, buffer.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    
    // Create vao for generating fullscreen triangle
    GLuint fullscreen_triangle_vao;
    glGenVertexArrays(1, &fullscreen_triangle_vao);
    
    
    // Create shader for displaying buffer
    static const char* fragment_shader =
    "\n"
    "#version 330\n"
    "\n"
    "uniform sampler2D buffer;\n"
    "noperspective in vec2 TexCoord;\n"
    "\n"
    "out vec3 outColor;\n"
    "\n"
    "void main(void){\n"
    "    outColor = texture(buffer, TexCoord).rgb;\n"
    "}\n";
    
    static const char* vertex_shader =
    "\n"
    "#version 330\n"
    "\n"
    "noperspective out vec2 TexCoord;\n"
    "\n"
    "void main(void){\n"
    "\n"
    "    TexCoord.x = (gl_VertexID == 2)? 2.0: 0.0;\n"
    "    TexCoord.y = (gl_VertexID == 1)? 2.0: 0.0;\n"
    "    \n"
    "    gl_Position = vec4(2.0 * TexCoord - 1.0, 0.0, 1.0);\n"
    "}\n";
    
    GLuint shader_id = glCreateProgram();
    
    {
        //Create vertex shader
        GLuint shader_vp = glCreateShader(GL_VERTEX_SHADER);
        
        glShaderSource(shader_vp, 1, &vertex_shader, 0);
        glCompileShader(shader_vp);
        validate_shader(shader_vp, vertex_shader);
        glAttachShader(shader_id, shader_vp);
        
        glDeleteShader(shader_vp);
    }
    
    {
        //Create fragment shader
        GLuint shader_fp = glCreateShader(GL_FRAGMENT_SHADER);
        
        glShaderSource(shader_fp, 1, &fragment_shader, 0);
        glCompileShader(shader_fp);
        validate_shader(shader_fp, fragment_shader);
        glAttachShader(shader_id, shader_fp);
        
        glDeleteShader(shader_fp);
    }
    
    glLinkProgram(shader_id);
    //make sure shader id is validated
    if (!validate_program(shader_id)) {
        fprintf(stderr, "Error while validating shader.\n");
        glfwTerminate();
        glDeleteVertexArrays(1, &fullscreen_triangle_vao);
        delete[] buffer.data;
        return -1;
    }
    
    glUseProgram(shader_id);
    
    GLint location = glGetUniformLocation(shader_id, "buffer");
    glUniform1i(location, 0);
    
    
    //OpenGL setup
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    
    glBindVertexArray(fullscreen_triangle_vao);
    
    /*
     we created sprites where the sprites are represented as a bitmap.
     We illustrate the sprite in with a map of 8x8,11x8,11x7,13x8,12x8
     (3 alien ships,1 ship explosion,1 player ship)
     These models were taken from and example that gave a tutorial on creating basic sprites
     */
    
    // Prepare game_object
    Sprite alien_sprites[7];
    //Alien_Ship sprite 1a
    alien_sprites[0].width = 8;
    alien_sprites[0].height = 8;
    alien_sprites[0].data = new uint8_t[64]
    {
        0,0,0,1,1,0,0,0, // ...@@...
        0,0,1,1,1,1,0,0, // ..@@@@..
        0,1,1,1,1,1,1,0, // .@@@@@@.
        1,1,0,1,1,0,1,1, // @@.@@.@@
        1,1,1,1,1,1,1,1, // @@@@@@@@
        0,1,0,1,1,0,1,0, // .@.@@.@.
        1,0,0,0,0,0,0,1, // @......@
        0,1,0,0,0,0,1,0  // .@....@.
    };
    //Alien_Ship sprite 1b
    alien_sprites[1].width = 8;
    alien_sprites[1].height = 8;
    alien_sprites[1].data = new uint8_t[64]
    {
        0,0,0,1,1,0,0,0, // ...@@...
        0,0,1,1,1,1,0,0, // ..@@@@..
        0,1,1,1,1,1,1,0, // .@@@@@@.
        1,1,0,1,1,0,1,1, // @@.@@.@@
        1,1,1,1,1,1,1,1, // @@@@@@@@
        0,0,1,0,0,1,0,0, // ..@..@..
        0,1,0,1,1,0,1,0, // .@.@@.@.
        1,0,1,0,0,1,0,1  // @.@..@.@
    };
    //Alien_Ship sprite 2a
    alien_sprites[2].width = 11;
    alien_sprites[2].height = 8;
    alien_sprites[2].data = new uint8_t[88]
    {
        0,0,1,0,0,0,0,0,1,0,0, // ..@.....@..
        0,0,0,1,0,0,0,1,0,0,0, // ...@...@...
        0,0,1,1,1,1,1,1,1,0,0, // ..@@@@@@@..
        0,1,1,0,1,1,1,0,1,1,0, // .@@.@@@.@@.
        1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@
        1,0,1,1,1,1,1,1,1,0,1, // @.@@@@@@@.@
        1,0,1,0,0,0,0,0,1,0,1, // @.@.....@.@
        0,0,0,1,1,0,1,1,0,0,0  // ...@@.@@...
    };
    //Alien_Ship sprite 2b
    alien_sprites[3].width = 11;
    alien_sprites[3].height = 8;
    alien_sprites[3].data = new uint8_t[88]
    {
        0,0,1,0,0,0,0,0,1,0,0, // ..@.....@..
        1,0,0,1,0,0,0,1,0,0,1, // @..@...@..@
        1,0,1,1,1,1,1,1,1,0,1, // @.@@@@@@@.@
        1,1,1,0,1,1,1,0,1,1,1, // @@@.@@@.@@@
        1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@
        0,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@.
        0,0,1,0,0,0,0,0,1,0,0, // ..@.....@..
        0,1,0,0,0,0,0,0,0,1,0  // .@.......@.
    };
    //Alien_Ship sprite 3a
    alien_sprites[4].width = 12;
    alien_sprites[4].height = 8;
    alien_sprites[4].data = new uint8_t[96]
    {
        0,0,0,0,1,1,1,1,0,0,0,0, // ....@@@@....
        0,1,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@@.
        1,1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@@
        1,1,1,0,0,1,1,0,0,1,1,1, // @@@..@@..@@@
        1,1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@@
        0,0,0,1,1,0,0,1,1,0,0,0, // ...@@..@@...
        0,0,1,1,0,1,1,0,1,1,0,0, // ..@@.@@.@@..
        1,1,0,0,0,0,0,0,0,0,1,1  // @@........@@
    };
    
    //Alien_Ship sprite 3b
    alien_sprites[5].width = 12;
    alien_sprites[5].height = 8;
    alien_sprites[5].data = new uint8_t[96]
    {
        0,0,0,0,1,1,1,1,0,0,0,0, // ....@@@@....
        0,1,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@@.
        1,1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@@
        1,1,1,0,0,1,1,0,0,1,1,1, // @@@..@@..@@@
        1,1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@@
        0,0,1,1,1,0,0,1,1,1,0,0, // ..@@@..@@@..
        0,1,1,0,0,1,1,0,0,1,1,0, // .@@..@@..@@.
        0,0,1,1,0,0,0,0,1,1,0,0  // ..@@....@@..
    };
    //Alien_Ship sprite mother ship
    Sprite Alien_Ship_sprite;
    Alien_Ship_sprite.width = 12;
    Alien_Ship_sprite.height = 8;
    Alien_Ship_sprite.data = new uint8_t[96]
    {
        0,0,0,0,0,1,1,0,0,0,0,0, // .....@@.....
        0,0,0,1,1,1,1,1,1,0,0,0, // ...@@@@@@...
        0,1,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@@.
        1,1,0,0,1,0,0,1,0,0,1,1, // @@..@..@..@@
        1,1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@@
        1,1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@@
        0,1,1,1,0,1,1,0,1,1,1,0, // .@@@.@@.@@@.
        0,0,1,0,0,0,0,0,0,1,0,0  // ..@......@..
    };
    
    //alien death sprite
    Sprite alien_death_sprite;
    alien_death_sprite.width = 13;
    alien_death_sprite.height = 7;
    alien_death_sprite.data = new uint8_t[91]
    {
        0,1,0,0,1,0,0,0,1,0,0,1,0, // .@..@...@..@.
        0,0,1,0,0,1,0,1,0,0,1,0,0, // ..@..@.@..@..
        0,0,0,1,0,0,0,0,0,1,0,0,0, // ...@.....@...
        1,1,0,0,0,0,0,0,0,0,0,1,1, // @@.........@@
        0,0,0,1,0,0,0,0,0,1,0,0,0, // ...@.....@...
        0,0,1,0,0,1,0,1,0,0,1,0,0, // ..@..@.@..@..
        0,1,0,0,1,0,0,0,1,0,0,1,0  // .@..@...@..@.
    };
    //User_Player sprite
    Sprite User_Player_sprite;
    User_Player_sprite.width = 11;
    User_Player_sprite.height = 7;
    User_Player_sprite.data = new uint8_t[77]
    {
        0,0,0,0,0,1,0,0,0,0,0, // .....@.....
        0,0,0,0,1,1,1,0,0,0,0, // ....@@@....
        0,0,0,0,1,1,1,0,0,0,0, // ....@@@....
        0,1,1,1,1,1,1,1,1,1,0, // .@@@@@@@@@.
        1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@
        1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@
        1,1,1,1,1,1,1,1,1,1,1, // @@@@@@@@@@@
    };
    //if sprite is dead, it no longer renders.
    Sprite dead;
    dead.width = 1;
    dead.height = 1;
    dead.data = new uint8_t[1]
    {
        0
    };
    
    //sprite spreadsheet. sprits render on 1's with 0 being empty space
    Sprite text_spritesheet;
    text_spritesheet.width = 5;
    text_spritesheet.height = 7;
    text_spritesheet.data = new uint8_t[65 * 35]
    {
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
        0,1,0,1,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,1,0,1,0,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,1,1,1,1,1,0,1,0,1,0,0,1,0,1,0,
        0,0,1,0,0,0,1,1,1,0,1,0,1,0,0,0,1,1,1,0,0,0,1,0,1,0,1,1,1,0,0,0,1,0,0,
        1,1,0,1,0,1,1,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,1,1,0,1,0,1,1,
        0,1,1,0,0,1,0,0,1,0,1,0,0,1,0,0,1,1,0,0,1,0,0,1,0,1,0,0,0,1,0,1,1,1,1,
        0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,
        1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,
        0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,0,0,1,0,0,0,1,1,1,0,1,0,1,0,1,0,0,1,0,0,
        0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,1,1,1,1,1,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,
        0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,
        
        0,1,1,1,0,1,0,0,0,1,1,0,0,1,1,1,0,1,0,1,1,1,0,0,1,1,0,0,0,1,0,1,1,1,0,
        0,0,1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,1,1,0,
        0,1,1,1,0,1,0,0,0,1,0,0,0,0,1,0,0,1,1,0,0,1,0,0,0,1,0,0,0,0,1,1,1,1,1,
        1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
        0,0,0,1,0,0,0,1,1,0,0,1,0,1,0,1,0,0,1,0,1,1,1,1,1,0,0,0,1,0,0,0,0,1,0,
        1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
        0,1,1,1,0,1,0,0,0,1,1,0,0,0,0,1,1,1,1,0,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
        1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,
        0,1,1,1,0,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
        0,1,1,1,0,1,0,0,0,1,1,0,0,0,1,0,1,1,1,1,0,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
        
        0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,
        0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,
        0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
        1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,
        0,1,1,1,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,
        0,1,1,1,0,1,0,0,0,1,1,0,1,0,1,1,1,0,1,1,1,0,1,0,0,1,0,0,0,1,0,1,1,1,0,
        
        0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,
        1,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,1,1,1,0,
        0,1,1,1,0,1,0,0,0,1,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,0,1,1,1,0,
        1,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,0,
        1,1,1,1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,
        1,1,1,1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,1,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,
        0,1,1,1,0,1,0,0,0,1,1,0,0,0,0,1,0,1,1,1,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
        1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,
        0,1,1,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,1,1,1,0,
        0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
        1,0,0,0,1,1,0,0,1,0,1,0,1,0,0,1,1,0,0,0,1,0,1,0,0,1,0,0,1,0,1,0,0,0,1,
        1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,1,1,1,1,
        1,0,0,0,1,1,1,0,1,1,1,0,1,0,1,1,0,1,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,
        1,0,0,0,1,1,0,0,0,1,1,1,0,0,1,1,0,1,0,1,1,0,0,1,1,1,0,0,0,1,1,0,0,0,1,
        0,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
        1,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,1,1,1,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,
        0,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,1,0,1,1,0,0,1,1,0,1,1,1,1,
        1,1,1,1,0,1,0,0,0,1,1,0,0,0,1,1,1,1,1,0,1,0,1,0,0,1,0,0,1,0,1,0,0,0,1,
        0,1,1,1,0,1,0,0,0,1,1,0,0,0,0,0,1,1,1,0,1,0,0,0,1,0,0,0,0,1,0,1,1,1,0,
        1,1,1,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,
        1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,0,1,1,1,0,
        1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,
        1,0,0,0,1,1,0,0,0,1,1,0,0,0,1,1,0,1,0,1,1,0,1,0,1,1,1,0,1,1,1,0,0,0,1,
        1,0,0,0,1,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,1,0,0,0,1,
        1,0,0,0,1,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,
        1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,1,1,1,1,
        
        0,0,0,1,1,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,1,
        0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0,
        1,1,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,1,1,0,0,0,
        0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,
        0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    //sprite number spritesheet
    Sprite number_spritesheet = text_spritesheet;
    number_spritesheet.data += 16 * 35;
    
    //sprite for User_Player Projectile
    Sprite User_Player_Projectile_sprite;
    User_Player_Projectile_sprite.width = 1;
    User_Player_Projectile_sprite.height = 3;
    User_Player_Projectile_sprite.data = new uint8_t[3]
    {
        1, 1, 1
    };
    
    //sprite for alien Projectile
    Sprite alien_Projectile_sprite[2];
    alien_Projectile_sprite[0].width = 3;
    alien_Projectile_sprite[0].height = 7;
    alien_Projectile_sprite[0].data = new uint8_t[21]
    {
        0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,
    };
    
    alien_Projectile_sprite[1].width = 3;
    alien_Projectile_sprite[1].height = 7;
    alien_Projectile_sprite[1].data = new uint8_t[21]
    {
        0,1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,1,0,1,0,
    };
    
    //sprite animation for alien Projectile
    SpriteAnimation alien_Projectile_animation;
    alien_Projectile_animation.loop = true;
    alien_Projectile_animation.number_of_frames = 2;
    alien_Projectile_animation.frame_length = 5;
    alien_Projectile_animation.game_object_time = 0;
    
    alien_Projectile_animation.frames = new Sprite*[2];
    alien_Projectile_animation.frames[0] = &alien_Projectile_sprite[0];
    alien_Projectile_animation.frames[1] = &alien_Projectile_sprite[1];
    //alien animation for aliens 1-3, a and b
    SpriteAnimation alien_animation[3];
    
    size_t alien_update_frequency = 120;
    
    for (size_t i = 0; i < 3; i++)
    {
        alien_animation[i].loop = true;
        alien_animation[i].number_of_frames = 2;
        alien_animation[i].frame_length = alien_update_frequency;
        alien_animation[i].game_object_time = 0;
        
        alien_animation[i].frames = new Sprite*[2];
        alien_animation[i].frames[0] = &alien_sprites[2 * i];
        alien_animation[i].frames[1] = &alien_sprites[2 * i + 1];
    }
    
    //game_object create and set game window dimensions
    game_object game_object;
    game_object.width = buffer_width;
    game_object.height = buffer_height;
    game_object.num_Projectiles = 0;
    //number of aliens
    game_object.remaining_aliens = 55;
    game_object.aliens = new Alien[game_object.remaining_aliens];
    
    //Define User_Player start location and lives
    game_object.User_Player.x = 112 - 5;
    game_object.User_Player.y = 32;
    game_object.User_Player.live_total = 3;
    
    //Define Alien_Ship start location and lives
    game_object.Alien_Ship.x = game_object.width - Alien_Ship_sprite.width;
    game_object.Alien_Ship.y = 215;
    game_object.Alien_Ship.live_total = 3;
    
    //Define alien swarm positions
    size_t alien_swarm_position = 24;
    size_t alien_swarm_max_position = game_object.width - 16 * 11 - 3;
    
    size_t aliens_killed = 0;
    size_t alien_update_game_object_timer = 0;
    bool should_change_speed = false;
    
    //Create table of aliens for drawing later
    for (size_t xi = 0; xi < 11; xi++)
    {
        for (size_t yi = 0; yi < 5; yi++)
        {
            Alien& alien = game_object.aliens[xi * 5 + yi];
            alien.type = (5 - yi) / 2 + 1;
            
            const Sprite& sprite = alien_sprites[2 * (alien.type - 1)];
            
            alien.x = 16 * xi + alien_swarm_position + (alien_death_sprite.width - sprite.width) / 2;
            alien.y = 17 * yi + 128;
        }
    }
    
    //death counters
    uint8_t* death_counters = new uint8_t[game_object.remaining_aliens];
    for (size_t i = 0; i < game_object.remaining_aliens; i++)
    {
        death_counters[i] = 10;
    }
    
    uint32_t clear_color = rgb_to_uint32(0, 0, 0);
    uint32_t rng = 13;
    //set move directions
    int alien_direction_move = 4;
    
    int Alien_Ship_direction_move = 4;
    //set score and credits
    size_t score = 0;
    size_t credits = 0;
    //start game running
    game_object_is_running = true;
    //set player move direction
    int User_Player_direction_move = 0;
    while (!glfwWindowShouldClose(window) && game_object_is_running)
    {
        buffer_clear(&buffer, clear_color);
        //if game hasent started, go to start screen
        if (begin == false && game_object.User_Player.live_total != 0) {
            score = 0;
            game_object.User_Player.live_total = 3;
            buffer_draw_text(&buffer, text_spritesheet, "PRESS ENTER TO BEGIN", game_object.width / 2 - 60, game_object.height / 2, rgb_to_uint32(255, 255, 255));
            
            glTexSubImage2D(
                            GL_TEXTURE_2D, 0, 0, 0,
                            buffer.width, buffer.height,
                            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
                            buffer.data
                            );
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            
            glfwSwapBuffers(window);
            
        }
        else if (begin == true && game_object.User_Player.live_total == 0) {
            //game over from dying
            //when the game is over
            //show game over and score screen
            buffer_draw_text(&buffer, text_spritesheet, "GAME OVER", game_object.width / 2 - 30, game_object.height / 2, rgb_to_uint32(255, 255, 255));
            buffer_draw_text(&buffer, text_spritesheet, "SCORE", 4, game_object.height - text_spritesheet.height - 7, rgb_to_uint32(255, 255, 255));
            buffer_draw_number(&buffer, number_spritesheet, score, 4 + 2 * number_spritesheet.width, game_object.height - 2 * number_spritesheet.height - 12, rgb_to_uint32(128, 0, 0));
            
            glTexSubImage2D(
                            GL_TEXTURE_2D, 0, 0, 0,
                            buffer.width, buffer.height,
                            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
                            buffer.data
                            );
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            
            glfwSwapBuffers(window);
            glfwPollEvents();
            continue;
        }
        else {
            
            // Draw the game, show score and credit
            buffer_draw_text(&buffer, text_spritesheet, "SCORE", 4, game_object.height - text_spritesheet.height - 7, rgb_to_uint32(255, 255, 255));
            buffer_draw_number(&buffer, number_spritesheet, score, 4 + 2 * number_spritesheet.width, game_object.height - 2 * number_spritesheet.height - 12, rgb_to_uint32(255, 255, 255));
            
            {
                char credit_text[16];
                sprintf(credit_text, "CREDIT %02lu", credits);
                buffer_draw_text(&buffer, text_spritesheet, credit_text, 164, 7, rgb_to_uint32(128, 0, 0));
            }
            
            //Draw player health indicator
            buffer_draw_number(&buffer, number_spritesheet, game_object.User_Player.live_total, 4, 7, rgb_to_uint32(255, 255, 255));
            size_t xp = 11 + number_spritesheet.width;
            for (size_t i = 0; i < game_object.User_Player.live_total - 1; i++)
            {
                buffer_draw_sprite(&buffer, User_Player_sprite, xp, 7, rgb_to_uint32(50, 90, 255));
                xp += User_Player_sprite.width + 2;
            }
            //Draw the Alien_ship
            if (game_object.Alien_Ship.live_total == 0)
            {
                buffer_draw_sprite(&buffer, alien_death_sprite, game_object.Alien_Ship.x, game_object.Alien_Ship.y, rgb_to_uint32(180, 0, 0));
                game_object.Alien_Ship.x -= (alien_death_sprite.width - Alien_Ship_sprite.width) / 2;
                buffer_draw_sprite(&buffer, alien_death_sprite, game_object.Alien_Ship.x, game_object.Alien_Ship.y, rgb_to_uint32(0, 0, 0));
            }
            //When Alien_ship is not dead
            else
            {
                buffer_draw_sprite(&buffer, Alien_Ship_sprite, game_object.Alien_Ship.x, game_object.Alien_Ship.y, rgb_to_uint32(0, 128, 0));
            }
            //Draw other aliens in swarm
            for (size_t i = 0; i < game_object.width; i++)
            {
                buffer.data[game_object.width * 16 + i] = rgb_to_uint32(255, 255, 255);
            }
            
            for (size_t ai = 0; ai < game_object.remaining_aliens; ai++)
            {
                if (death_counters[ai] == 0) continue;
                
                const Alien& alien = game_object.aliens[ai];
                //if alien is dead, draw death sprite
                if (alien.type == ALIEN_DEAD)
                {
                    buffer_draw_sprite(&buffer, alien_death_sprite, alien.x, alien.y, rgb_to_uint32(180, 0, 0));
                }
                //otherwise, draw regular aliens
                else
                {
                    const SpriteAnimation& animation = alien_animation[alien.type - 1];
                    size_t current_frame = animation.game_object_time / animation.frame_length;
                    const Sprite& sprite = *animation.frames[current_frame];
                    buffer_draw_sprite(&buffer, sprite, alien.x, alien.y, rgb_to_uint32(0, 128, 0));
                }
            }
            //Generate and draw projectiles
            for (size_t bi = 0; bi < game_object.num_Projectiles; bi++)
            {
                const Projectile& Projectile = game_object.Projectiles[bi];
                const Sprite* sprite;
                if (Projectile.direction > 0) sprite = &User_Player_Projectile_sprite;
                else
                {
                    size_t cf = alien_Projectile_animation.game_object_time / alien_Projectile_animation.frame_length;
                    sprite = &alien_Projectile_sprite[cf];
                }
                buffer_draw_sprite(&buffer, *sprite, Projectile.x, Projectile.y, rgb_to_uint32(0, 200, 0));
            }
            buffer_draw_sprite(&buffer, User_Player_sprite, game_object.User_Player.x, game_object.User_Player.y, rgb_to_uint32(50, 90, 255));
            
            glTexSubImage2D(
                            GL_TEXTURE_2D, 0, 0, 0,
                            buffer.width, buffer.height,
                            GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,
                            buffer.data
                            );
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            
            glfwSwapBuffers(window);
            
            // Simulate Projectiles
            for (size_t bi = 0; bi < game_object.num_Projectiles; bi++)
            {
                game_object.Projectiles[bi].y += game_object.Projectiles[bi].direction;
                if (game_object.Projectiles[bi].y >= game_object.height || game_object.Projectiles[bi].y < User_Player_Projectile_sprite.height)
                {
                    game_object.Projectiles[bi] = game_object.Projectiles[game_object.num_Projectiles - 1];
                    --game_object.num_Projectiles;
                    continue;
                }
                
                //Check if Alien Projectile hits player
                if (game_object.Projectiles[bi].direction < 0)
                {
                    bool overlap = sprite_overlap_check(
                                                        alien_Projectile_sprite[0], game_object.Projectiles[bi].x, game_object.Projectiles[bi].y,
                                                        User_Player_sprite, game_object.User_Player.x, game_object.User_Player.y
                                                        );
                    
                    if (overlap)
                    {
                        --game_object.User_Player.live_total;
                        game_object.Projectiles[bi] = game_object.Projectiles[game_object.num_Projectiles - 1];
                        --game_object.num_Projectiles;
                        
                        //perhaps we need to check if the game_object is over or not.
                        break;
                    }
                }
                // User_Player Projectile
                else
                {
                    // Check if User_Player Projectile hits an alien Projectile
                    for (size_t bj = 0; bj < game_object.num_Projectiles; bj++)
                    {
                        if (bi == bj) continue;
                        //Check for overlap using defined function
                        bool overlap = sprite_overlap_check(
                                                            User_Player_Projectile_sprite, game_object.Projectiles[bi].x, game_object.Projectiles[bi].y,
                                                            alien_Projectile_sprite[0], game_object.Projectiles[bj].x, game_object.Projectiles[bj].y
                                                            );
                        
                        if (overlap)
                        {
                            if (bj == game_object.num_Projectiles - 1)
                            {
                                game_object.Projectiles[bi] = game_object.Projectiles[game_object.num_Projectiles - 2];
                            }
                            else if (bi == game_object.num_Projectiles - 1)
                            {
                                game_object.Projectiles[bj] = game_object.Projectiles[game_object.num_Projectiles - 2];
                            }
                            else
                            {
                                game_object.Projectiles[(bi < bj) ? bi : bj] = game_object.Projectiles[game_object.num_Projectiles - 1];
                                game_object.Projectiles[(bi < bj) ? bj : bi] = game_object.Projectiles[game_object.num_Projectiles - 2];
                            }
                            game_object.num_Projectiles -= 2;
                            break;
                        }
                    }
                    
                    // Check if player hits alien
                    for (size_t ai = 0; ai < game_object.remaining_aliens; ai++)
                    {
                        const Alien& alien = game_object.aliens[ai];
                        if (alien.type == ALIEN_DEAD) continue;
                        
                        const SpriteAnimation& animation = alien_animation[alien.type - 1];
                        size_t current_frame = animation.game_object_time / animation.frame_length;
                        const Sprite& alien_sprite = *animation.frames[current_frame];
                        bool overlap = sprite_overlap_check(
                                                            User_Player_Projectile_sprite, game_object.Projectiles[bi].x, game_object.Projectiles[bi].y,
                                                            alien_sprite, alien.x, alien.y
                                                            );
                        
                        if (overlap)
                        {
                            score += 10 * (4 - game_object.aliens[ai].type);
                            game_object.aliens[ai].type = ALIEN_DEAD;
                            
                            game_object.aliens[ai].x -= (alien_death_sprite.width - alien_sprite.width) / 2;
                            game_object.Projectiles[bi] = game_object.Projectiles[game_object.num_Projectiles - 1];
                            --game_object.num_Projectiles;
                            ++aliens_killed;
                            
                            if (aliens_killed % 15 == 0) should_change_speed = true;
                            
                            break;
                        }
                    }
                    
                    //Alien_Ship hit by checking overlap then checking overlap of pixel positions
                    bool overlap = sprite_overlap_check(User_Player_Projectile_sprite, game_object.Projectiles[bi].x, game_object.Projectiles[bi].y, Alien_Ship_sprite, game_object.Alien_Ship.x, game_object.Alien_Ship.y);
                    if (game_object.Alien_Ship.live_total == 0) continue;
                    if (overlap) {
                        score += 50;
                        game_object.Projectiles[bi] = game_object.Projectiles[game_object.num_Projectiles - 1];
                        --game_object.num_Projectiles;
                        --game_object.Alien_Ship.live_total;
                        break;
                    }
                }
            }
            // Simulate aliens, if there are less aliens, they increase in speed and begin to fire more frequently
            if (should_change_speed)
            {
                should_change_speed = false;
                alien_update_frequency /= 2;
                for (size_t i = 0; i < 3; i++)
                {
                    alien_animation[i].frame_length = alien_update_frequency;
                }
            }
            
            // Update death counters, the higher the death counter is, the more the aliens fire and the faster they move and 
            //change frames for the animation
            for (size_t ai = 0; ai < game_object.remaining_aliens; ai++)
            {
                const Alien& alien = game_object.aliens[ai];
                if (alien.type == ALIEN_DEAD && death_counters[ai])
                {
                    --death_counters[ai];
                }
            }
            
            if (alien_update_game_object_timer >= alien_update_frequency)
            {
                alien_update_game_object_timer = 0;
                
                if ((int)alien_swarm_position + alien_direction_move < 0)
                {
                    alien_direction_move *= -1;
                    
                    for (size_t ai = 0; ai < game_object.remaining_aliens; ai++)
                    {
                        Alien& alien = game_object.aliens[ai];
                        alien.y -= 8;
                    }
                }
                else if (alien_swarm_position > alien_swarm_max_position - alien_direction_move)
                {
                    alien_direction_move *= -1;
                }
                alien_swarm_position += alien_direction_move;
                
                for (size_t ai = 0; ai < game_object.remaining_aliens; ai++)
                {
                    Alien& alien = game_object.aliens[ai];
                    alien.x += alien_direction_move;
                }
                
                if (aliens_killed < game_object.remaining_aliens)
                {
                    size_t rai = game_object.remaining_aliens * random(&rng);
                    while (game_object.aliens[rai].type == ALIEN_DEAD)
                    {
                        rai = game_object.remaining_aliens * random(&rng);
                    }
                    const Sprite& alien_sprite = *alien_animation[game_object.aliens[rai].type - 1].frames[0];
                    game_object.Projectiles[game_object.num_Projectiles].x = game_object.aliens[rai].x + alien_sprite.width / 2;
                    game_object.Projectiles[game_object.num_Projectiles].y = game_object.aliens[rai].y - alien_Projectile_sprite[0].height;
                    game_object.Projectiles[game_object.num_Projectiles].direction = -2;
                    ++game_object.num_Projectiles;
                }
            }
            
            // Update animations
            //this goes with the speed, if the speed is increased from less aliens left alive
            //the aliens will move faster, change animation quicker and fire more freqently.
            for (size_t i = 0; i < 3; i++)
            {
                ++alien_animation[i].game_object_time;
                if (alien_animation[i].game_object_time >= alien_animation[i].number_of_frames * alien_animation[i].frame_length)
                {
                    alien_animation[i].game_object_time = 0;
                }
            }
            ++alien_Projectile_animation.game_object_time;
            if (alien_Projectile_animation.game_object_time >= alien_Projectile_animation.number_of_frames * alien_Projectile_animation.frame_length)
            {
                alien_Projectile_animation.game_object_time = 0;
            }
            
            ++alien_update_game_object_timer;
            
            // Simulate User_Player
            //this goes with the speed, if the speed is increased from less aliens left alive
            //the aliens will move faster, change animation quicker and fire more freqently.
            User_Player_direction_move = 2 * direction_move;
            
            if (User_Player_direction_move != 0)
            {
                if (game_object.User_Player.x + User_Player_sprite.width + User_Player_direction_move >= game_object.width)
                {
                    game_object.User_Player.x = game_object.width - User_Player_sprite.width;
                }
                else if ((int)game_object.User_Player.x + User_Player_direction_move <= 0)
                {
                    game_object.User_Player.x = 0;
                }
                else game_object.User_Player.x += User_Player_direction_move;
            }
            
            Alien_Ship_direction_move = u_direction_move;
            //Simulate Alien_ship movement
            //this goes with the speed, if the speed is increased from less aliens left alive
            //the aliens will move faster, change animation quicker and fire more freqently.
            if (Alien_Ship_direction_move != 0)
            {
                if (game_object.Alien_Ship.x + Alien_Ship_sprite.width + Alien_Ship_direction_move >= game_object.width)
                {
                    game_object.Alien_Ship.x = game_object.width - Alien_Ship_sprite.width;
                    u_direction_move *= -1;
                }
                else if ((int)game_object.Alien_Ship.x + Alien_Ship_direction_move <= 0)
                {
                    game_object.Alien_Ship.x = 0;
                    u_direction_move *= -1;
                }
                else
                {
                    game_object.Alien_Ship.x += Alien_Ship_direction_move;
                    Alien_Ship_direction_move -= 1;
                }
            }
            
            if (aliens_killed < game_object.remaining_aliens)
            {
                size_t ai = 0;
                while (game_object.aliens[ai].type == ALIEN_DEAD) ai++;
                const Sprite& sprite = alien_sprites[2 * (game_object.aliens[ai].type - 1)];
                size_t pos = game_object.aliens[ai].x - (alien_death_sprite.width - sprite.width) / 2;
                if (pos > alien_swarm_position) alien_swarm_position = pos;
                
                ai = game_object.remaining_aliens - 1;
                while (game_object.aliens[ai].type == ALIEN_DEAD) ai--;
                pos = game_object.width - game_object.aliens[ai].x - 13 + pos;
                if (pos > alien_swarm_max_position) alien_swarm_max_position = pos;
            }
            //Reset swarm when all aliens are killed
            //this is where we could implement more level design if more time was given. 
            //the next level is almost the same as the first level.
            else
            {
                alien_update_frequency = 120;
                alien_swarm_position = 24;
                
                aliens_killed = 0;
                alien_update_game_object_timer = 0;
                
                alien_direction_move = 4;
                
                for (size_t xi = 0; xi < 11; xi++)
                {
                    for (size_t yi = 0; yi < 5; yi++)
                    {
                        size_t ai = xi * 5 + yi;
                        
                        death_counters[ai] = 10;
                        
                        Alien& alien = game_object.aliens[ai];
                        alien.type = (5 - yi) / 2 + 1;
                        
                        const Sprite& sprite = alien_sprites[2 * (alien.type - 1)];
                        
                        alien.x = 16 * xi + alien_swarm_position + (alien_death_sprite.width - sprite.width) / 2;
                        alien.y = 17 * yi + 128;
                    }
                }
            }
            
            // Process events such as fire, max mojectiles, player movement
            if (fire_is_pressed && game_object.num_Projectiles < MAX_PROJECTILES)
            {
                game_object.Projectiles[game_object.num_Projectiles].x = game_object.User_Player.x + User_Player_sprite.width / 2;
                game_object.Projectiles[game_object.num_Projectiles].y = game_object.User_Player.y + User_Player_sprite.height;
                game_object.Projectiles[game_object.num_Projectiles].direction = 2;
                ++game_object.num_Projectiles;
            }
            fire_is_pressed = false;
        }
        glfwPollEvents();
    }
    //clear buffer and memory when closing game
    glfwDestroyWindow(window);
    glfwTerminate();
    
    glDeleteVertexArrays(1, &fullscreen_triangle_vao);
    
    for (size_t i = 0; i < 6; i++)
    {
        delete[] alien_sprites[i].data;
    }
    
    delete[] text_spritesheet.data;
    delete[] alien_death_sprite.data;
    delete[] User_Player_Projectile_sprite.data;
    delete[] alien_Projectile_sprite[0].data;
    delete[] alien_Projectile_sprite[1].data;
    delete[] alien_Projectile_animation.frames;
    delete[] Alien_Ship_sprite.data;
    
    for (size_t i = 0; i < 3; i++)
    {
        delete[] alien_animation[i].frames;
    }
    delete[] buffer.data;
    delete[] game_object.aliens;
    delete[] death_counters;
    
    return 0;
 //END
}
