// CodeReclaimers, LLC, Colbert GA 30628
// Copyright (c) 2016
// Distributed under the MIT License

#include "BSplineCurveInteractiveWindow.h"

int main(int, char const*[])
{
#if defined(_DEBUG)
    LogReporter reporter(
        "LogReport.txt",
        Logger::Listener::LISTEN_FOR_ALL,
        Logger::Listener::LISTEN_FOR_ALL,
        Logger::Listener::LISTEN_FOR_ALL,
        Logger::Listener::LISTEN_FOR_ALL);
#endif

    Window::Parameters parameters(L"BSplineCurveInteractiveWindow", 0, 0, 512, 512);
    auto window = TheWindowSystem.Create<BSplineCurveInteractiveWindow>(parameters);
	TheWindowSystem.MessagePump(window, TheWindowSystem.NO_IDLE_LOOP);
    TheWindowSystem.Destroy<BSplineCurveInteractiveWindow>(window);
    return 0;
}

BSplineCurveInteractiveWindow::BSplineCurveInteractiveWindow(Parameters& parameters)
    :
    Window2(parameters),
	mControlRadius(3),
	mSelectedControl(-1)
{
	std::vector<Vector2<float>> samples;
	auto const twoPi = static_cast<float>(GTE_C_TWO_PI);
	for (auto i = -100; i <= 100; ++i)
	{
		auto const x = i / 100.0f;
		auto const y = 0.3f * sin(twoPi * x);
		samples.push_back({ x, y });
	}
	mSpline = std::make_shared<BSplineCurveFit<float>>(2, static_cast<int>(samples.size()),
		reinterpret_cast<float const*>(&samples[0]), 3, 8);
}

void BSplineCurveInteractiveWindow::DrawSpline()
{
	auto const numSamples = 100;
	float multiplier = 1.0f / (numSamples - 1.0f);
	std::vector<Vector2<float>> samples;
	for (unsigned int i = 0; i < numSamples; ++i)
	{
		auto const t = multiplier * i;
		Vector2<float> p;
		mSpline->GetPosition(t, &p[0]);
		samples.push_back(p);
	}

	for (size_t i = 1; i < samples.size(); ++i)
	{
		auto const s0 = ToScreenCoord(samples[i - 1]);
		auto const s1 = ToScreenCoord(samples[i]);
		DrawLine(s0[0], s0[1], s1[0], s1[1], 0XA000A0);
	}

	auto const controls = reinterpret_cast<Vector2<float> const*>(mSpline->GetControlData());
	auto const numControls = mSpline->GetNumControls();
	for (int i = 0; i < numControls; ++i)
	{
		auto const c1 = ToScreenCoord(controls[i]);
		if (i > 0)
		{
			auto const c0 = ToScreenCoord(controls[i - 1]);
			DrawLine(c0[0], c0[1], c1[0], c1[1], 0X00A0A0);
		}

		DrawThickPixel(c1[0], c1[1], mControlRadius, 0x0000FF);
	}
}

void BSplineCurveInteractiveWindow::OnDisplay()
{
	ClearScreen(0xFFFFFFFF);

	auto const topLeft = FromScreenCoord({ 0, 0 });
	auto const bottomRight = FromScreenCoord({ mXSize, mYSize });

	for (int i = -200; i <= 200; i += 10)
	{
		auto color = 0xF0F0F0;
		if (i % 100 == 0)
		{
			color = 0xFFD0D0;
		}
		auto const x = i / 100.0f;
		auto const p0 = ToScreenCoord({ x, topLeft[1] });
		auto const p1 = ToScreenCoord({ x, bottomRight[1] });
		DrawLine(p0[0], p0[1], p1[0], p1[1], color);
	}

	for (int i = -200; i <= 200; i += 10)
	{
		auto color = 0xF0F0F0;
		if (i % 100 == 0)
		{
			color = 0xFFD0D0;
		}
		auto const y = i / 100.0f;
		auto const p0 = ToScreenCoord({ topLeft[0], y });
		auto const p1 = ToScreenCoord({ bottomRight[0], y });
		DrawLine(p0[0], p0[1], p1[0], p1[1], color);
	}

	DrawSpline();
	mScreenTextureNeedsUpdate = true;
	Window2::OnDisplay();
}

bool BSplineCurveInteractiveWindow::OnMouseClick(int button, int state, int x, int y, unsigned int modifiers)
{
	if (MOUSE_DOWN == state)
	{
		mSelectedControl = -1;
		auto const controls = reinterpret_cast<Vector2<float> const*>(mSpline->GetControlData());
		auto const numControls = mSpline->GetNumControls();
		for (int i = 0; i < numControls; ++i)
		{
			auto const c1 = ToScreenCoord(controls[i]);
			auto const d = c1 - Vector2<int>{x, y};
			if (std::abs(d[0]) <= mControlRadius && std::abs(d[1]) <= mControlRadius)
			{
				mSelectedControl = i;
				break;
			}
		}
		OnDisplay();
	}
	else if (MOUSE_UP == state)
	{
		mSelectedControl = -1;
		OnDisplay();
	}

	return Window2::OnMouseClick(button, state, x, y, modifiers);
}

bool BSplineCurveInteractiveWindow::OnMouseMotion(int button, int x, int y, unsigned int modifiers)
{
	if (mSelectedControl >= 0)
	{
		auto const controls = (Vector2<float>*)(mSpline->GetControlData());
		controls[mSelectedControl] = FromScreenCoord({ x, y });
		OnDisplay();
	}

	return Window2::OnMouseMotion(button, x, y, modifiers);
}

Vector2<int> BSplineCurveInteractiveWindow::ToScreenCoord(Vector2<float> const& pt) const
{
	auto const scale = std::min(mXSize, mYSize) * 0.3f;
	auto const x = static_cast<int>(round(0.5f * mXSize + scale * pt[0]));
	auto const y = static_cast<int>(round(0.5f * mYSize - scale * pt[1]));

	return{ x, y };
}

Vector2<float> BSplineCurveInteractiveWindow::FromScreenCoord(Vector2<int> const& pt) const
{
	auto const scale = std::min(mXSize, mYSize) * 0.3f;
	auto const x = (pt[0] - mXSize / 2) / scale;
	auto const y = -(pt[1] - mYSize / 2) / scale;

	return{ x, y };
}
