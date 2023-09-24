#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION 
#include <learnopengl/stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// Dimensiones
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(10.0f, 0.0f, 30.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

//Luz de luna
float brilloLuna = 0.3f;
//linterna encender apagar
bool encendido = false;
//auxiliares 
float aux = 10.0f; //tamaño
float aux2 = 0.0f; //radio
float aux3 = 1.0f; //radio
float aux4 = 3.0f; //velocidad

int main()
{
    // se iniciliza y configura GLWF
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Creación del objeto ventana
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Proyecto - Lincango Dennis, Nazate Christian, Vasquez Pavel", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Error al crear la ventana" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); // Crear el contexto de la ventana
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);//Llamar a la funcion en cada cambio de ventana
    glfwSetCursorPosCallback(window, mouse_callback); //posición del cursor
    glfwSetScrollCallback(window, scroll_callback); //desplazamiento

    //decirle a GLFW que capture nuestro ratón
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Inicializar glad antes de llamar a las funciones
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "La inicilizacion de glad a fallado" << std::endl;
        return -1;
    }

    // configurar el estado global de opengl
    glEnable(GL_DEPTH_TEST);

    // Construir y compilar shaders
    Shader ourShader("shaders/VS_Proyecto.vs", "shaders/FS_Proyecto.fs");
    
    // cargar models
    Model cielo("model/cielo/cielo.obj");
    Model globo("model/globo/globo.obj");
    Model ciudad("model/ciudad/ciudad.obj");
    Model luna("model/luna/luna.obj");

    ourShader.use();
    ourShader.setInt("material.diffuse", 0);
    ourShader.setInt("material.specular", 1);
    ourShader.setInt("material.emission", 2);

    camera.MovementSpeed = 10; 

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // lógica de tiempo por cuadro
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //activar el sombreador cuando se configure objetos uniformes/de dibujo
        ourShader.use();
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setFloat("material.shininess", 27.0f);//32

        // directional light ESCENA
        ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05);
        ourShader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);
        
        // point light luna
        ourShader.setVec3("pointLights[0].position", -1.3f*aux, 2.8f*aux, 0.0f*aux);
        ourShader.setVec3("pointLights[0].ambient", brilloLuna, brilloLuna, brilloLuna);
        ourShader.setVec3("pointLights[0].diffuse", brilloLuna, brilloLuna, brilloLuna);
        ourShader.setVec3("pointLights[0].specular", brilloLuna, brilloLuna, brilloLuna);
        ourShader.setFloat("pointLights[0].constant", 1.0f);
        ourShader.setFloat("pointLights[0].linear", 0.0014);
        ourShader.setFloat("pointLights[0].quadratic", 0.000007);

        // spotLight 1 Camara      
        ourShader.setVec3("spotLight[0].position", camera.Position);
        ourShader.setVec3("spotLight[0].direction", camera.Front);
        if (encendido) {
            ourShader.setVec3("spotLight[0].ambient", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight[0].diffuse", 0.0f, 0.0f, 0.0f); 
            ourShader.setVec3("spotLight[0].specular", 0.0f, 0.0f, 0.0f);
        }else {
            ourShader.setVec3("spotLight[0].ambient", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight[0].diffuse", 1.0f, 1.0f, 1.0f);
            ourShader.setVec3("spotLight[0].specular", 1.0f, 1.0f, 1.0f);
        }
        ourShader.setFloat("spotLight[0].constant", 1.0f);
        ourShader.setFloat("spotLight[0].linear", 0.001);
        ourShader.setFloat("spotLight[0].quadratic", 0.00007);
        ourShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(15.0f)));

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // renderizar el modelo cielo
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
        model = glm::scale(model, glm::vec3(0.25f)*aux);	
        ourShader.setMat4("model", model);
        cielo.Draw(ourShader);

        // renderizar el modelo ciudad
        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, 0.0f)); 
        model2 = glm::scale(model2, glm::vec3(0.005f)*aux);	
        ourShader.setMat4("model", model2);
        ciudad.Draw(ourShader);
        
        // renderizar el modelo luna
        float time1 = glfwGetTime();
        glm::mat4 model3 = glm::mat4(1.0f);
        model3 = glm::translate(model3, glm::vec3(-1.5f, 3.1f, 0.0f)*aux); 
        model3 = glm::rotate(model3, time1/10, glm::vec3(1.0f, 1.0f, 1.0f));
        model3 = glm::scale(model3, glm::vec3(0.015f)*aux);	
        ourShader.setMat4("model", model3);
        luna.Draw(ourShader);

        float radius = 0.9f;//Radio de giro
        
        for (size_t i = 0; i < 20; i++)
        {
            float time = glfwGetTime();        
            float aux5 = pow(-1,i);        
            float ejeY = 2.2f - (i * 0.1) + (sin((time + i * 30) / aux4) / 5);
            float ejeX = -aux3* aux5 * (sin((time + i*(30)) / aux4) * radius * (1 + (0.1f * i) + aux2)); 
            float ejeZ = (cos((time + i*(30)) / aux4) * radius * (1 + (0.1f * i) + aux2));
            glm::mat4 model4 = glm::mat4(1.0f);
            
            if(i==0) {
                model4 = glm::translate(model4, glm::vec3(ejeX*i, ejeY, ejeZ*i ) * aux);
            }else if (i < 21) {             
                model4 = glm::translate(model4, glm::vec3(ejeX, ejeY, ejeZ) * aux);
            }
            
            model4 = glm::rotate(model4, time/3, glm::vec3(0.0f, 10.0f, 0.0f));
            model4 = glm::rotate(model4, 86.3f, glm::vec3(1.0f, 0.0f, 0.0f)); //
            model4 = glm::scale(model4, glm::vec3(0.00003f) * aux);	
            ourShader.setMat4("model", model4);
            globo.Draw(ourShader);
        }
 
        glfwSwapBuffers(window); //Búfer de color
        glfwPollEvents();//Verifica si se activa algún evento
    }

    // Para terminar glw. Para eliminar los recursos empleados
    glfwTerminate();
    return 0;
}

// La función toma la ventana como entrada junto con una tecla. Al presionar teclas sucede una acción.
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) //brillo lunar
        if (brilloLuna <= 1.0f) {
            brilloLuna += 0.01f;
        }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        if (brilloLuna >= 0.0f) {
            brilloLuna -= 0.01f;
        }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) { //linterna
        encendido = true;
    }else {
        encendido = false;
    }

    if (camera.Position.y <= 0.7) {
        camera.Position.y = 0.7;
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) //disminuir el tamaño del radio 
        if (aux2 >= -0.5f) {
            aux2 -= 0.01f;
        }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) // aumentar el tamaño del radio
        if (aux2 <= 0.5f) {
            aux2 += 0.01f;
        }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) // sentido del giro
            aux3 = 1.0f;       
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) // sentido del giro contrario     
            aux3 = -1.0f;

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) // aumentar la velocidad
        if (aux4 >= 0.0f) {
            if (aux4 == 0.5f) //para evitar aux4=0
                aux4 -= 0.5f;
            if (aux4 >= 200.0f) //aumento
                aux4 = 6.0f;
            aux4 -= 0.5f;
        }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) // disminuir la velocidad
        if (aux4 <= 8.0f) {
            if (aux4 == -0.5f)//para evitar aux4 = 0
                aux4 += 0.5f;   
            aux4 += 0.5f;
        }
    
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) { //casi detener/corazón
        aux4 = 200.0f;
    }
    
            
        

}

// Tamaño de ventana
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Mover el mouse
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // al revés ya que las coordenadas y van de abajo hacia arriba

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// Desplazar rueda del mouse
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}