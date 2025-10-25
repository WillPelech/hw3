#ifndef ENTITY_H
#define ENTITY_H

#include "cs3113.h"
#include "constants.h"

enum Direction    { LEFT, UP, RIGHT, DOWN         }; 
enum EntityStatus { ACTIVE, INACTIVE              };
enum EntityType   { PLAYER, BLOCK, PLATFORM,ENEMY, NONE };

class Entity
{
private:
    Vector2 mPosition;
    Vector2 mMovement;
    Vector2 mVelocity;
    Vector2 mAcceleration;
    
    // Platform specific variables
    float mPlatformSpeed = 2.0f; 
    bool mMovingRight = true;   

    Vector2 mScale;
    Vector2 mColliderDimensions;
    
    Texture2D mTexture;
    TextureType mTextureType;
    Vector2 mSpriteSheetDimensions;
    
    std::map<Direction, std::vector<int>> mAnimationAtlas;
    std::vector<int> mAnimationIndices;
    Direction mDirection = DOWN;
    int mFrameSpeed;

    int mCurrentFrameIndex = 0;
    float mAnimationTime = 0.0f;

    bool mIsJumping = false;
    float mJumpingPower = 100.0f; 

    int mSpeed;
    float mAngle;
    float mBounciness = 0.6f; 
    int fuel_level =1000;
    bool mIsCollidingTop    = false;
    bool mIsCollidingBottom = false;
    bool mIsCollidingRight  = false;
    bool mIsCollidingLeft   = false;

    EntityStatus mEntityStatus = ACTIVE;
    EntityType   mEntityType;
    

    bool isColliding(Entity *other) const;
    void checkCollisionY(Entity **collidableEntities, int collisionCheckCount);
    void checkCollisionX(Entity **collidableEntities, int collisionCheckCount);
    void resetColliderFlags() 
    {
        mIsCollidingTop    = false;
        mIsCollidingBottom = false;
        mIsCollidingRight  = false;
        mIsCollidingLeft   = false;
    }

    void animate(float deltaTime);

public:
    static constexpr int   DEFAULT_SIZE          = 250;
    static constexpr int   DEFAULT_SPEED         = 200;
    static constexpr int   DEFAULT_FRAME_SPEED   = 14;
    static constexpr float HORIZONTAL_ACCELERATION = 500.0f; 
    static constexpr float HORIZONTAL_DAMPING = 3.0f; 
    static constexpr float MIN_BOUNCE_VELOCITY   = 50.0f;
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;
    static constexpr int fuel_decrement = 50;

    Entity();
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        EntityType entityType);
    Entity(Vector2 position, Vector2 scale, const char *textureFilepath, 
        TextureType textureType, Vector2 spriteSheetDimensions, 
        std::map<Direction, std::vector<int>> animationAtlas, 
        EntityType entityType);
    ~Entity();

    void update(float deltaTime, Entity **collidableEntities, int collisionCheckCount);
    void render();
    void normaliseMovement() { Normalise(&mMovement); }

    void jump()       { if (fuel_level > 1){mIsJumping = true;}
                        edit_fuel_level();
                     }
    void activate()   { mEntityStatus  = ACTIVE;   }
    void deactivate() { mEntityStatus  = INACTIVE; }
    void displayCollider();

    bool isActive() { return mEntityStatus == ACTIVE ? true : false; }

    void moveUp()    { mMovement.y = -1; mDirection = UP;    }
    void moveDown()  { mMovement.y =  1; mDirection = DOWN;  }
    void moveLeft()  { mMovement.x = -1; mDirection = LEFT;  }
    void moveRight() { mMovement.x =  1; mDirection = RIGHT; }

    void resetMovement() { mMovement = { 0.0f, 0.0f }; }

    Vector2     getPosition()              const { return mPosition;              }
    Vector2     getMovement()              const { return mMovement;              }
    Vector2     getVelocity()              const { return mVelocity;              }
    Vector2     getAcceleration()          const { return mAcceleration;          }
    Vector2     getScale()                 const { return mScale;                 }
    Vector2     getColliderDimensions()    const { return mScale;                 }
    Vector2     getSpriteSheetDimensions() const { return mSpriteSheetDimensions; }
    Texture2D   getTexture()               const { return mTexture;               }
    TextureType getTextureType()           const { return mTextureType;           }
    Direction   getDirection()             const { return mDirection;             }
    int         getFrameSpeed()            const { return mFrameSpeed;            }
    float       getJumpingPower()          const { return mJumpingPower;          }
    bool        isJumping()                const { return mIsJumping;             }
    int         getSpeed()                 const { return mSpeed;                 }
    float       getAngle()                 const { return mAngle;                 }
    int         get_fuel_level()             const {return fuel_level;}
    
    
    bool isCollidingTop()    const { return mIsCollidingTop;    }
    bool isCollidingBottom() const { return mIsCollidingBottom; }

    std::map<Direction, std::vector<int>> getAnimationAtlas() const { return mAnimationAtlas; }

    void setPosition(Vector2 newPosition)
        { mPosition = newPosition;                 }
    void setMovement(Vector2 newMovement)
        { mMovement = newMovement;                 }
    void setAcceleration(Vector2 newAcceleration)
        { mAcceleration = newAcceleration;         }
    void setVelocity(Vector2 newVelocity)
        { mVelocity = newVelocity;                 }
    float getBounciness() const { return mBounciness; }
    void setBounciness(float b) { mBounciness = b; }
    void setScale(Vector2 newScale)
        { mScale = newScale;                       }
    void setTexture(const char *textureFilepath)
        { mTexture = LoadTexture(textureFilepath); }
    void setColliderDimensions(Vector2 newDimensions) 
        { mColliderDimensions = newDimensions;     }
    void setSpriteSheetDimensions(Vector2 newDimensions) 
        { mSpriteSheetDimensions = newDimensions;  }
    void setSpeed(int newSpeed)
        { mSpeed  = newSpeed;                      }
    void setFrameSpeed(int newSpeed)
        { mFrameSpeed = newSpeed;                  }
    void setJumpingPower(float newJumpingPower)
        { mJumpingPower = newJumpingPower;         }
    void setAngle(float newAngle) 
        { mAngle = newAngle;                       }
    void setEntityType(EntityType entityType)
        { mEntityType = entityType;                }
    void edit_fuel_level(){
        fuel_level -= fuel_decrement;
        if (fuel_level < 0) fuel_level = 0;
    }
    void left_movement(){
        if (fuel_level > 0) mAcceleration.x = -10; 
    }
    void right_movement(){
        if (fuel_level > 0) mAcceleration.x = 10; 
    }
    void no_movement(){
       mVelocity = {0,mVelocity.y}; 
    }
    
    // Platform and Enemy movement methods
    void updatePlatformMovement() {
        if (mEntityType == PLATFORM||mEntityType == ENEMY) {
            if (mMovingRight) {
                mPosition.x += mPlatformSpeed;
                if (mPosition.x >= SCREEN_WIDTH - mScale.x/2) {
                    mMovingRight = false;
                }
            } else {
                mPosition.x -= mPlatformSpeed;
                if (mPosition.x <= mScale.x/2) {
                    mMovingRight = true;
                }
            }
        }
    }
    
    void setPlatformSpeed(float speed) { mPlatformSpeed = speed; }
};



#endif // ENTITY_CPP