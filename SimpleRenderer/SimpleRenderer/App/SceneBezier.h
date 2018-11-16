#pragma once

#include "Scene.h"

class DrawableLine;
class DrawableBezier;
class DrawablePoint;

class SceneBezier : public Scene
{
public:

	SceneBezier();
	virtual ~SceneBezier();

protected:

	virtual void InitResources() override;
	virtual void InitCamera() override;
	virtual void InitDrawables() override;
	virtual void UpdateLogic() override;

	void MovePlayer();
	void ComputeLineMarker();
	void ComputeBezierMarker();

	void GetClosestPoint(const glm::vec3* lines, const size_t lineNum, const glm::vec3& source, glm::vec3& outClosestPoint, glm::vec3* outSecondClosestPoint = nullptr, glm::vec3* outClosestLineVertex = nullptr);



	DrawableLine* _line;
	DrawableBezier* _bezier;
	DrawablePoint* _markerLine;
	DrawablePoint* _markerBezier;
};

