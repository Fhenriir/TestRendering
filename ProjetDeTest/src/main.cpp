#include "opengl-framework/opengl-framework.hpp" // Inclue la librairie qui va nous servir à faire du rendu
#include "glm/ext/matrix_clip_space.hpp"

int main()
{
    // Initialisation
    gl::init("TPs de Rendering"); // On crée une fenêtre et on choisit son nom
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_ONE);
    gl::maximize_window(); // On peut la maximiser si on veut
    auto camera = gl::Camera{};
    gl::set_events_callbacks({ camera.events_callbacks() });
    gl::set_events_callbacks({camera.events_callbacks()});
    glm::mat4 const view_matrix = camera.view_matrix();

    auto const shader = gl::Shader{{
        .vertex   = gl::ShaderSource::File{"res/vertex.vert"},
        .fragment = gl::ShaderSource::File{"res/fragment.frag"},
    }};

    auto const triangle_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0 /*Index de l'attribut dans le shader, on en reparle juste après*/}},
            .data   = {
                -0.5f, 0.5f, // Position2D du 1er sommet
                0.5f, -0.5f, // Position2D du 2ème sommet
                -0.5f, -0.5f  // Position2D du 3ème sommet
            },
        }},
    }};

    auto const triangle_mesh2 = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0 /*Index de l'attribut dans le shader, on en reparle juste après*/}},
            .data   = {
                -0.5f, 0.5f, // Position2D du 1er sommet
                0.5f, -0.5f, // Position2D du 2ème sommet
                0.5f, 0.5f  // Position2D du 3ème sommet
            },
        }},
    }};

    auto const rectangle_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0}},
            .data   = {
                -0.3f, -0.3f, // Position2D du 1er sommet
                +0.3f, -0.3f, // Position2D du 2ème sommet
                +0.3f, +0.3f, // Position2D du 3ème sommet
                -0.3f, +0.3f  // Position2D du 4ème sommet
            },
        }},
        .index_buffer   = {
            0, 1, 2, // Indices du premier triangle : on utilise le 1er, 2ème et 3ème sommet
            0, 2, 3  // Indices du deuxième triangle : on utilise le 1er, 3ème et 4ème sommet
        },
    }};

    auto const background_mesh = gl::Mesh{{
        .vertex_buffers = {{
            .layout = {gl::VertexAttribute::Position2D{0}},
            .data = {
                -2.f, -2.f, // Position2D du 1er sommet
                +2.f, -2.f, // Position2D du 2ème sommet
                +2.f, +2.f, // Position2D du 3ème sommet
                -2.f, +2.f  // Position2D du 4ème sommet
            },
        }},
        .index_buffer = {
            0, 1, 2, // Indices du premier triangle : on utilise le 1er, 2ème et 3ème sommet
            0, 2, 3  // Indices du deuxième triangle : on utilise le 1er, 3ème et 4ème sommet
        },
    }};

    //glClearColor(0.f, 0.f, 1.f, 1.f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
    //glClear(GL_COLOR_BUFFER_BIT); // Exécute concrètement l'action d'appliquer sur tout l'écran la couleur choisie au-dessus

    while (gl::window_is_open())
    {
        glClearColor(0.f, 0.f, 1.f, 0.1f); // Choisis la couleur à utiliser. Les paramètres sont R, G, B, A avec des valeurs qui vont de 0 à 1
        glClear(GL_COLOR_BUFFER_BIT); // Exécute concrètement l'action d'appliquer sur tout l'écran la couleur choisie au-dessus
        glm::mat4 const view_matrix = camera.view_matrix(); // On récupère la matrice de vue de la caméra
        glm::mat4 const projection_matrix = glm::infinitePerspective(1.f /*field of view in radians*/, gl::framebuffer_aspect_ratio() /*aspect ratio*/, 0.001f /*near plane*/);
        shader.bind(); // On a besoin qu'un shader soit bind (i.e. "actif") avant de draw(). On en reparle dans la section d'après.
        float _baseTime = gl::time_in_seconds();
        shader.set_uniform("viewMatrix", projection_matrix * view_matrix);
        shader.set_uniform("time", _baseTime);
        shader.set_uniform("alpha", 1.f);
        shader.set_uniform("color",glm::vec4(0.9,0.9,0,1));
        rectangle_mesh.draw(); // C'est ce qu'on appelle un "draw call" : on envoie l'instruction à la carte graphique de dessiner notre mesh.
    }
}