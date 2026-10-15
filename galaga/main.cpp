#include "DrawContext.h"
#include "GameEngine.h"
#include "Player.h"

const bool gUseCollisionDemo = true;

class Ball : public CMPUT350::CollisionObject
{
public:
	Ball(CMPUT350::Point2D location, int radius) : CollisionObject(), mRadius(radius), mLoc(location)
	{
		mInCollision = mHasCollisionPoint = false;
		mShapes.push_back(CMPUT350::Shape(CMPUT350::Circle(0, 0, radius)));
		mBounds = CMPUT350::Rect(CMPUT350::Point2D(-mRadius, -mRadius), 2*mRadius, 2*mRadius); // Initialize with dummy values
		
	}
	
	void Update(CMPUT350::GameContext *context) override
	{
		mInCollision = mHasCollisionPoint = false;
	}
	
	void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) override
	{
		mInCollision = true;
		mHasCollisionPoint = false;
	}
	
	void CollisionEnter(const std::shared_ptr<CollisionObject> &obj, const CMPUT350::Point2D &collisionPoint) override
	{
		mInCollision = true;
		mHasCollisionPoint = true;
		mCollisionPoint = collisionPoint;
	}
	
	const CMPUT350::Rect &GetBounds() override
	{
		return mBounds;
	}
	
	bool IsStatic() const  { return false; }
	
	const std::vector<CMPUT350::Shape> &GetShapes() override
	{
		return mShapes;
	}
	
	bool IsAlive() const override
	{
		return true;
	}
	
	CMPUT350::Point2D GetLocation() const override
	{
		return mLoc;
	}
	
	float GetRotation() const override
	{
		return 0;
	}
	
	void RenderForeground(CMPUT350::GameContext *context) override
	{
		if (mInCollision)
		{
			context->ScreenContext->DrawCircle({0, 0}, mRadius, CMPUT350::Colors::red);
		}
		else {
			context->ScreenContext->DrawCircle({0, 0}, mRadius, CMPUT350::Colors::blue);
		}
		if (mHasCollisionPoint){
			context->ScreenContext->SetContextRotation(0);
			context->ScreenContext->SetContextOffset(CMPUT350::Point2D(0,0));

			context->ScreenContext->DrawCircle(mCollisionPoint, 5, CMPUT350::Colors::yellow);
		}
	}
private:
	int mRadius;
	CMPUT350::Point2D mLoc, mCollisionPoint;
	bool mInCollision;
	bool mHasCollisionPoint;
	std::vector<CMPUT350::Shape> mShapes;
	CMPUT350::Rect mBounds;
};

class Line : public CMPUT350::CollisionObject
{
public:
	Line(CMPUT350::Point2D from, CMPUT350::Point2D to) : CollisionObject(), mFrom(from), mTo(to), mRotation(0)
	{
		mInCollision = mHasCollisionPoint = false;
		mShapes.push_back(CMPUT350::Shape(CMPUT350::Line({0, 0}, mTo-mFrom)));
		mBounds = CMPUT350::Rect({0, 0}, mTo-mFrom);
	}
	
	void Update(CMPUT350::GameContext *context) override
	{
		mRotation = (mRotation+3)%360;
		mInCollision = mHasCollisionPoint = false;
	}
	
	void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) override
	{
		mInCollision = true;
		mHasCollisionPoint = false;
	}
	
	void CollisionEnter(const std::shared_ptr<CollisionObject> &obj, const CMPUT350::Point2D &collisionPoint) override
	{
		mInCollision = true;
		mHasCollisionPoint = true;
		mCollisionPoint = collisionPoint;
	}
	
	const CMPUT350::Rect &GetBounds() override
	{
		return mBounds;
	}
	
	bool IsStatic() const  { return false; }
	
	const std::vector<CMPUT350::Shape> &GetShapes() override
	{
		return mShapes;
	}
	
	bool IsAlive() const override
	{
		return true;
	}
	
	CMPUT350::Point2D GetLocation() const override
	{
		return mFrom;
	}
	
	float GetRotation() const override
	{
		return mRotation*3.14159265358979323846f/180.0f;
	}
	
	void RenderForeground(CMPUT350::GameContext *context) override
	{
		if (mInCollision)
		{
			context->ScreenContext->DrawLine({0, 0}, mTo-mFrom, 2.0f, CMPUT350::Colors::red);
			
		}
		else {
			context->ScreenContext->DrawLine({0, 0}, mTo-mFrom, 2.0f, CMPUT350::Colors::blue);
		}
		if (mHasCollisionPoint){
			context->ScreenContext->DrawCircle(mCollisionPoint, 3, CMPUT350::Colors::yellow);
		}

	}
private:
	int mRadius;
	CMPUT350::Point2D mFrom, mTo, mCollisionPoint;
	bool mInCollision;
	bool mHasCollisionPoint;
	int mRotation;
	std::vector<CMPUT350::Shape> mShapes;
	CMPUT350::Rect mBounds;
};

class Rect : public CMPUT350::CollisionObject
{
public:
	Rect(CMPUT350::Point2D from, CMPUT350::Point2D to) : CollisionObject(), mFrom(from), mTo(to)
	{
		mInCollision = mHasCollisionPoint = false;
		mShapes.push_back(CMPUT350::Shape(CMPUT350::Rect({0, 0}, mTo-mFrom)));
		mBounds = CMPUT350::Rect({0, 0}, mTo-mFrom); // Initialize with dummy values
	}
	
	void Update(CMPUT350::GameContext *context) override
	{
		mRotation -= 0.05f;
		if (mRotation < 0)
			mRotation += 2.0f*3.14159265358979323846f;
		mInCollision = mHasCollisionPoint = false;
	}
	
	void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) override
	{
		mInCollision = true;
		mHasCollisionPoint = false;
	}
	
	void CollisionEnter(const std::shared_ptr<CollisionObject> &obj, const CMPUT350::Point2D &collisionPoint) override
	{
		mInCollision = true;
		mHasCollisionPoint = true;
		mCollisionPoint = collisionPoint;
	}
	
	const CMPUT350::Rect &GetBounds() override
	{
		return mBounds;
	}
	
	bool IsStatic() const  { return false; }
	
	const std::vector<CMPUT350::Shape> &GetShapes() override
	{
		return mShapes;
	}
	
	bool IsAlive() const override
	{
		return true;
	}
	
	CMPUT350::Point2D GetLocation() const override
	{
		return mFrom;
	}
	
	float GetRotation() const override
	{
		return mRotation;
	}
	
	void RenderForeground(CMPUT350::GameContext *context) override
	{

		if (mInCollision)
		{
			context->ScreenContext->DrawRect(CMPUT350::Rect({0, 0}, mTo-mFrom), CMPUT350::Colors::red);
		}
		else {
			context->ScreenContext->DrawRect(CMPUT350::Rect({0, 0}, mTo-mFrom), CMPUT350::Colors::blue);
		}
		if (mHasCollisionPoint);
		context->ScreenContext->DrawCircle(mCollisionPoint, 3, CMPUT350::Colors::yellow);

	}
private:
	int mRadius;
	CMPUT350::Point2D mFrom, mTo, mCollisionPoint;
	CMPUT350::Rect mBounds;
	bool mInCollision;
	bool mHasCollisionPoint;
	float mRotation;
	std::vector<CMPUT350::Shape> mShapes;
};

int main()
{
	if (gUseCollisionDemo)
	{
		CMPUT350::GameEngine engine(512, 512, "Collision Demo");

		
		
		engine.AddGameObject(std::make_shared<Ball>(CMPUT350::Point2D(100, 100), 25));
		engine.AddGameObject(std::make_shared<Ball>(CMPUT350::Point2D(150, 100), 25));

		engine.AddGameObject(std::make_shared<Ball>(CMPUT350::Point2D(300, 100), 25));
		engine.AddGameObject(std::make_shared<Rect>(CMPUT350::Point2D(350, 90), CMPUT350::Point2D(400, 105)));
		engine.AddGameObject(std::make_shared<Ball>(CMPUT350::Point2D(360, 70), 5));

		engine.AddGameObject(std::make_shared<Ball>(CMPUT350::Point2D(400, 400), 25));
		engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(350, 350), CMPUT350::Point2D(390, 350)));
		engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(75, 350), CMPUT350::Point2D(100, 375)));
		engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(100, 350), CMPUT350::Point2D(75, 375)));

		engine.AddGameObject(std::make_shared<Rect>(CMPUT350::Point2D(400-25, 400-125), CMPUT350::Point2D(400+50, 400-100+20)));
		engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(350, 350-100), CMPUT350::Point2D(390, 350-100)));

		engine.AddGameObject(std::make_shared<Rect>(CMPUT350::Point2D(256-25, 256-25), CMPUT350::Point2D(256+25, 256+25)));
		engine.AddGameObject(std::make_shared<Rect>(CMPUT350::Point2D(200, 200), CMPUT350::Point2D(225, 225)));
		
		engine.AddGameObject(std::make_shared<Ball>(CMPUT350::Point2D(250, 400), 25));
		engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(250, 450), CMPUT350::Point2D(260, 400)));
		

		//engine.AddGameObject(std::make_shared<Ball>(CMPUT350::Point2D(175, 400), 25));
		//engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(175, 415), CMPUT350::Point2D(175, 440)));
		
		//engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(30, 30), CMPUT350::Point2D(50, 30)));
		//engine.AddGameObject(std::make_shared<Line>(CMPUT350::Point2D(40, 30), CMPUT350::Point2D(60, 30)));

		
		engine.Run();
	}
	else {
		CMPUT350::GameEngine engine(768, 1024, "Galaga");
		//auto game = std::make_shared<Galaga>(768, 1024);
		//engine.AddGameObject(game);
		engine.Run();
	}
    return 0;
}
