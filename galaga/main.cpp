#include <random>
#include "GameEngine.h"
#include "MathUtil.h"
#include "Player.h"
#include "Enemy.h"
#include "Stars.h"

class Ball : public CMPUT350::CollisionObject
{
public:
    Ball(int radius) : CollisionObject(), mRadius(radius), mNumCollisions(0), mLoc(radius, radius), mSpeed(3, 3),
                       mSpeedNext(3, 3), mFrameCollisions(0)
    {
        std::uniform_int_distribution<int> xRand(mRadius, 1024 - mRadius);
        std::uniform_int_distribution<int> yRand(mRadius, 768 - mRadius);
        std::uniform_int_distribution<int> speed(1, 5);

        // Initialize ball with a random position and speed
        mLoc.x = xRand(gen);
        mLoc.y = yRand(gen);
        mSpeed.x = speed(gen); // Random speed between 1 and 5
        mSpeed.y = speed(gen); // Random speed between 1 and 5
    }

    float GetRotation() const override {
        return 0;
    }

    CMPUT350::Point2D GetLocation() const override {
        return CMPUT350::Point2D(0,0);

    }

    void Update(CMPUT350::GameContext* context) override
    {
        // Update ball position or state
        mLoc += mSpeedNext;
        mSpeed = mSpeedNext;
        if (mLoc.x > 1024 - mRadius || mLoc.x < mRadius)
        {
            mSpeed.x = -mSpeed.x;
            if (mSpeed.x > 0)
                mSpeed.x++;
        }
        if (mLoc.y > 768 - mRadius || mLoc.y < mRadius)
        {
            mSpeed.y = -mSpeed.y;
            if (mSpeed.y > 0)
                mSpeed.y++;
        }
        if (mNumCollisions % 10 == 8)
        {
            std::uniform_int_distribution<int> size(50, 150);
            context->mEngineView->AddGameObject(std::make_shared<Ball>(size(gen)));
            mNumCollisions++;
        }
        mSpeedNext = mSpeed;
    }

    void LateUpdate(CMPUT350::GameContext* context) override
    {
        if (mFrameCollisions > 0)
            mNumCollisions++;
        mFrameCollisions = 0;
    }

    void CollisionEnter(const std::shared_ptr<CollisionObject>& obj) override
    {
        std::shared_ptr<Ball> ball = std::dynamic_pointer_cast<Ball>(obj);
        // Only collisions with other Balls
        if (ball != nullptr)
        {
            CMPUT350::Point2D b1 = mLoc;
            CMPUT350::Point2D b2 = ball->mLoc;
            if (b1.Distance(b2) <= ball->mRadius + mRadius) // Are the balls within radius
            {
                mFrameCollisions++;
                CMPUT350::Point2D normal = b1 - b2;
                normal /= normal.Distance(CMPUT350::Point2D(0, 0));
                mCollisionPoint = normal * (-mRadius);
                float m1 = 3.14f * mRadius * mRadius; // mass of this ball
                float m2 = 3.14f * ball->mRadius * ball->mRadius; // mass of other ball
                CMPUT350::Point2D v1prime = mSpeed - (2 * m2 / (m1 + m2)) * ((normal * (mSpeed - ball->mSpeed)) *
                    normal);
                mSpeedNext = v1prime;
            }
        }
    }

    const std::vector<CMPUT350::Shape>& GetShapes() override
    {
        // The ball reports its location as (0,0) and draws itself in world space, so its shape carries the
        // real centre and the engine's offset leaves it alone
        mShapes.clear();
        mShapes.push_back(CMPUT350::Shape(CMPUT350::Circle(mLoc, (float)mRadius)));
        return mShapes;
    }

    const CMPUT350::Rect& GetBounds() const override
    {
        // A static here would be shared by every Ball, so each ball keeps its own box
        mBounds = CMPUT350::Rect(mLoc - mRadius, 2 * mRadius, 2 * mRadius);
        return mBounds;
    }

    bool IsAlive() const override
    {
        return mNumCollisions < 10;
    }

    void RenderForeground(CMPUT350::GameContext* context) override
    {
        context->ScreenContext->DrawCircle(mLoc, mRadius, CMPUT350::Colors::red);
        context->ScreenContext->DrawCenteredText("Circle" + std::to_string(mNumCollisions), mRadius / 6.0f, mLoc,
                                                 CMPUT350::Colors::white);
        context->ScreenContext->DrawLine(mLoc, mLoc + mCollisionPoint, mRadius * 0.1f, CMPUT350::Colors::blue);
    }

private:
    static std::random_device rd;
    static std::mt19937 gen;

    int mRadius, mNumCollisions, mFrameCollisions;
    CMPUT350::Point2D mLoc, mSpeed, mSpeedNext, mCollisionPoint;
    mutable CMPUT350::Rect mBounds;
    std::vector<CMPUT350::Shape> mShapes;
};

// Shared randoms across all balls
// These are the actual objeects associated with the class. Normally these
// would be in a C++ file
std::random_device Ball::rd;
std::mt19937 Ball::gen(rd());

int main()
{
    // The ball sample is the engine's own test, so the galaga scene is what runs by default
    bool runBallSample = false;

    if (runBallSample)
    {
        CMPUT350::GameEngine engine(1024, 768, "Ball Simulation");
        engine.AddGameObject(std::make_shared<Ball>(100));
        engine.AddGameObject(std::make_shared<Ball>(50));
        engine.Run();
    }
    else
    {
        CMPUT350::GameEngine engine(768, 1024, "Galaga");
        auto player = std::make_shared<Player>(CMPUT350::Point2D(768 / 2, 900));
        engine.AddGameObject(player);
        engine.AddGameObject(std::make_shared<Stars>(250, CMPUT350::Rect(0, 0, 768, 1024)));
        // 40 enemies evenly spread across the top. One row would be 1600px of enemy in a 768px window,
        // so they form a grid: 8 columns 96 apart, 5 rows 60 apart, none of them overlapping.
        for (int row = 0; row < 5; row++)
        {
            for (int col = 0; col < 8; col++)
            {
                engine.AddGameObject(std::make_shared<Enemy>(CMPUT350::Point2D(48 + col * 96, 80 + row * 60)));
            }
        }
        engine.Run();
    }
    return 0;
}
