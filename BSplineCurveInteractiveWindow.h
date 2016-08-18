// CodeReclaimers, LLC, Colbert GA 30628
// Copyright (c) 2016
// Distributed under the MIT License

#pragma once

#include <GTEngine.h>
using namespace gte;

class BSplineCurveInteractiveWindow : public Window2
{
public:
    BSplineCurveInteractiveWindow(Parameters& parameters);

	virtual void OnDisplay() override;
	virtual bool OnMouseClick(int button, int state, int x, int y, unsigned int modifiers) override;
	virtual bool OnMouseMotion(int button, int x, int y, unsigned int modifiers) override;

private:
	void DrawSpline();

	Vector2<int> ToScreenCoord(Vector2<float> const& pt) const;
	Vector2<float> FromScreenCoord(Vector2<int> const& pt) const;

	std::shared_ptr<BSplineCurveFit<float>> mSpline;
	int mControlRadius;
	int mSelectedControl;
};
