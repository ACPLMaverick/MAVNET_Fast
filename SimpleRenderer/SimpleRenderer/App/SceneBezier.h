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

	void ComputeBezierPoints(const std::vector<glm::vec3>& linePoints, std::vector<glm::vec3>& outBezier);
	void MovePlayer();
	void ComputeLineMarker();
	void ComputeBezierMarker();

	DrawableLine* _line;
	DrawableBezier* _bezier;
	DrawablePoint* _markerLine;
	DrawablePoint* _markerBezier;
};

