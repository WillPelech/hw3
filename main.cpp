#include "CS3113/Entity.h"
#include "CS3113/cs3113.h"
#include "CS3113/constants.h"

// Forward declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
bool isColliding(const Vector2 *postionA, const Vector2 *scaleA,
                const Vector2 *positionB, const Vector2 *scaleB);
void renderObject(const Texture2D *texture, const Vector2 *position,
                const Vector2 *scale);

// Global Constants
constexpr int FPS = 60, SPEED = 200, SHRINK_RATE = 100;

Vector2 ORIGIN = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2},
        BIRD_BASE_SIZE = {40.0f, 40.0f}, BEAKER_BASE_SIZE = {250.0f, 250.0f};
// File paths for textures
constexpr char BIRD_FP[] = "assets/owl.png";
constexpr char BACKGROUND_FP[] = "assets/background.png";
constexpr char NEST_FP[] = "assets/nest.png";
constexpr char ENEMY_FP[] = "assets/evil_hawk.png";

enum GameState {
    PLAYING,
    WON,
    LOST
};

// Global Variables
AppStatus gAppStatus = RUNNING;
float gAngle = 0.0f, gPreviousTicks = 0.0f;
float gFuelAccumulator = 0.0f;
GameState gameState = PLAYING;

Entity *bird_entity = nullptr;
Texture2D background;
Entity *nest_platform = nullptr;
Entity *hawk_enemy_1 = nullptr;
Entity *hawk_enemy_2 = nullptr;
Entity *collidable[3];
// Function Definitions

/**
 * @brief Checks for a square collision between 2 Rectangle objects.
 *
 * @see
 * @param postionA The position of the first object
 * @param scaleA The scale of the first object
 * @param positionB The position of the second object
 * @param scaleB The scale of the second object
 * @return true if a collision is detected,
 * @return false if a collision is not detected
 */
bool isColliding(const Vector2 *postionA, const Vector2 *scaleA,
                 const Vector2 *positionB, const Vector2 *scaleB) {
  float xDistance =
      fabs(postionA->x - positionB->x) - ((scaleA->x + scaleB->x) / 2.0f);
  float yDistance =
      fabs(postionA->y - positionB->y) - ((scaleA->y + scaleB->y) / 2.0f);

  if (xDistance < 0.0f && yDistance < 0.0f)
    return true;
  return false;
}

void renderObject(const Texture2D *texture, const Vector2 *position,
                  const Vector2 *scale) {
  // Whole texture (UV coordinates)
  Rectangle textureArea = {// top-left corner
                           0.0f, 0.0f,

                           // bottom-right corner (of texture)
                           static_cast<float>(texture->width),
                           static_cast<float>(texture->height)};

  // Destination rectangle – centred on gPosition
  Rectangle destinationArea = {position->x, position->y,
                               static_cast<float>(scale->x),
                               static_cast<float>(scale->y)};

  // Origin inside the source texture (centre of the texture)
  Vector2 originOffset = {static_cast<float>(scale->x) / 2.0f,
                          static_cast<float>(scale->y) / 2.0f};

  // Render the texture on screen
  DrawTexturePro(*texture, textureArea, destinationArea, originOffset, gAngle,
                 WHITE);
}

void initialise() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flying Bird Game");
  
  // Load background texture
  background = LoadTexture(BACKGROUND_FP);
  
  // Initialize bird
  std::map<Direction, std::vector<int>> animationAtlas{
      {DOWN, {0, 1, 2 ,3,4,5}},
      {UP, {0, 1, 2,3,4,5}},
      {LEFT, {0, 1, 2,3,4,5}},
      {RIGHT, {0, 1, 2,3,4,5}}};
  bird_entity = new Entity({-SCREEN_HEIGHT/2,-SCREEN_WIDTH/2}, BIRD_BASE_SIZE, BIRD_FP, ATLAS, {6, 9},
                           animationAtlas, PLAYER);
  
  bird_entity->setFrameSpeed(6);
  bird_entity->setBounciness(0.001f);
  
  // Initialize nest platform
  Vector2 nestPos = {static_cast<float>(GetRandomValue(100, SCREEN_WIDTH - 200)),
                     static_cast<float>(GetRandomValue(100, SCREEN_HEIGHT - 200))};
  Vector2 nestSize = {60.0f, 30.0f};
  Vector2 hawk_enemy_size = {80.0f, 50.0f};
  nest_platform = new Entity(nestPos, nestSize, NEST_FP, PLATFORM);
  nest_platform->setPlatformSpeed(2.0f);
  Vector2 hawk_enemy_1_pos = {static_cast<float>(GetRandomValue(100, SCREEN_WIDTH - 100)),
                     static_cast<float>(GetRandomValue(100, SCREEN_HEIGHT - 100))};
  hawk_enemy_1 = new Entity(hawk_enemy_1_pos,hawk_enemy_size,ENEMY_FP,ENEMY);
  Vector2 hawk_enemy_2_pos = {static_cast<float>(GetRandomValue(100, SCREEN_WIDTH - 100)),
                     static_cast<float>(GetRandomValue(100, SCREEN_HEIGHT - 100))};
  hawk_enemy_2 = new Entity(hawk_enemy_2_pos,hawk_enemy_size,ENEMY_FP,ENEMY);
  
  hawk_enemy_1->setPlatformSpeed(2.0f);
  hawk_enemy_2->setPlatformSpeed(5.0f);

  collidable[0] = nest_platform;
  collidable[1] = hawk_enemy_1;
  collidable[2] = hawk_enemy_2;

  SetTargetFPS(FPS);
}

void processInput() {
  // Only process movement input if game is still playing
  if (gameState == PLAYING) {
    // to close the game
    if (IsKeyPressed(KEY_W)){
      bird_entity->jump();
    }
    if (bird_entity) {
      bool moving = false;
      if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        if (bird_entity->get_fuel_level() > 0) {
          Vector2 acc = bird_entity->getAcceleration();
          acc.x = -Entity::HORIZONTAL_ACCELERATION;
          bird_entity->setAcceleration(acc);
          moving = true;
        }
      } else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        if (bird_entity->get_fuel_level() > 0) {
          Vector2 acc = bird_entity->getAcceleration();
          acc.x = Entity::HORIZONTAL_ACCELERATION;
          bird_entity->setAcceleration(acc);
          moving = true;
        }
      } else {
        Vector2 acc = bird_entity->getAcceleration();
        acc.x = 0.0f;
        bird_entity->setAcceleration(acc);
      }

      if (moving) {
        gFuelAccumulator += GetFrameTime();
        if (gFuelAccumulator >= 0.2f) {
          bird_entity->edit_fuel_level();
          gFuelAccumulator = 0.0f;
        }
      } else {
        gFuelAccumulator = 0.0f;
      }
    }
  }
  if (IsKeyPressed(KEY_Q) || WindowShouldClose())
    gAppStatus = TERMINATED;
}

void update() {
  float ticks =(float)GetTime();
  float deltaTime =ticks - gPreviousTicks;
  gPreviousTicks = ticks;
  
  // Only update movement if game is still playing
  if (gameState == PLAYING) {
    if (nest_platform) {
      nest_platform->update(deltaTime, nullptr, 0);
    }
    if (hawk_enemy_1){
      hawk_enemy_1->update(deltaTime,nullptr,0);
    } 
    if (hawk_enemy_2){
      hawk_enemy_2->update(deltaTime,nullptr,0);
    } 
    
    if (bird_entity) {
      bird_entity->update(deltaTime, collidable, 3); 

      Vector2 pos = bird_entity->getPosition();
      Vector2 vel = bird_entity->getVelocity();

      float halfW = bird_entity->getScale().x / 2.0f;
      if (pos.x - halfW < 0) {
        pos.x = halfW;
        vel.x = -vel.x * bird_entity->getBounciness();
      } else if (pos.x + halfW > SCREEN_WIDTH) {
        pos.x = SCREEN_WIDTH - halfW;
        vel.x = -vel.x * bird_entity->getBounciness();
      }

      float halfH = bird_entity->getScale().y / 2.0f;
      if (pos.y - halfH < 0) {
        pos.y = halfH;
        vel.y = -vel.y * bird_entity->getBounciness();
      } else if (pos.y + halfH > SCREEN_HEIGHT) {
        gameState = LOST;
        pos.y = SCREEN_HEIGHT - halfH;
        vel.y = 0; 
        vel.x = 0; 
      }

      bird_entity->setPosition(pos);
      bird_entity->setVelocity(vel);

      if (gameState == PLAYING && nest_platform) {
        Vector2 birdScale = bird_entity->getScale();
        Vector2 nestPos = nest_platform->getPosition();
        Vector2 nestScale = nest_platform->getScale();
        Vector2 enemy1Pos = hawk_enemy_1->getPosition();
        Vector2 enemy1Scale = hawk_enemy_1->getScale();
        Vector2 enemy2Pos = hawk_enemy_2->getPosition();
        Vector2 enemy2Scale = hawk_enemy_2->getScale();
        
        Vector2 expandedNestScale = nestScale;
        Vector2 expandedEnemy1Scale = enemy1Scale;
        Vector2 expandedEnemy2Scale = enemy2Scale;
        expandedNestScale.x *= 1.1f;  
        expandedNestScale.y *= 1.1f; 
        expandedEnemy1Scale.x *= 1.1f; 
        expandedEnemy1Scale.y *= 1.1f;
        expandedEnemy2Scale.x *= 1.1f;
        expandedEnemy2Scale.y *= 1.1f;
        
        if (isColliding(&pos, &birdScale, &nestPos, &expandedNestScale) &&
            vel.y >= 0) {
          gameState = WON;
          bird_entity->setVelocity({0, 0});
          bird_entity->setAcceleration({0, 0});
        }
        if (isColliding(&pos, &birdScale, &enemy1Pos, &expandedEnemy1Scale)) {
          gameState = LOST;
          bird_entity->setVelocity({0, 0});
          bird_entity->setAcceleration({0, 0});
        } 
        if (isColliding(&pos, &birdScale, &enemy2Pos, &expandedEnemy2Scale)) {
          gameState = LOST;
          bird_entity->setVelocity({0, 0});
          bird_entity->setAcceleration({0, 0});}
        // }
        // if (bird_entity->get_fuel_level() <= 0 && 
        //     pos.y > nestPos.y + nestScale.y) {
        //   gameState = LOST;
        // }
      }
    }
  }
}

void render() {
  BeginDrawing();
  ClearBackground(RAYWHITE);
  
  DrawTexturePro(background,
                 (Rectangle){0, 0, (float)background.width, (float)background.height},
                 (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT},
                 (Vector2){0, 0}, 0.0f, WHITE);
  
  if (nest_platform)
    nest_platform->render();
  
  if (hawk_enemy_1)
    hawk_enemy_1->render();
  if (hawk_enemy_2)
    hawk_enemy_2->render();
  
  if (bird_entity)
    bird_entity->render();

  if (bird_entity) {
    char fuelText[32];
    snprintf(fuelText, sizeof(fuelText), "Fuel: %d", bird_entity->get_fuel_level());
    int fuelTextWidth = MeasureText(fuelText, 20);
    DrawText(fuelText, SCREEN_WIDTH - fuelTextWidth - 10, 10, 20, BLACK);
  }

  const char* message = "";
  Color messageColor = WHITE;
  if (gameState == WON) {
    message = "Game Won!";
    messageColor = GREEN;
  } else if (gameState == LOST) {
    message = "Game Lost!";
    messageColor = RED;
  }
  
  if (gameState != PLAYING) {
    int textWidth = MeasureText(message, 40);
    DrawText(message, SCREEN_WIDTH/2 - textWidth/2, SCREEN_HEIGHT/2 - 20, 40, messageColor);
  }

  EndDrawing();
}

void shutdown() {
  if (bird_entity) {
    delete bird_entity;
    bird_entity = nullptr;
  }
  if (nest_platform) {
    delete nest_platform;
    nest_platform = nullptr;
  }
  UnloadTexture(background);
  CloseWindow();
}

int main(void) {
  initialise();

  while (gAppStatus == RUNNING) {
    processInput();
    update();
    render();
  }

  shutdown();

  return 0;
}
