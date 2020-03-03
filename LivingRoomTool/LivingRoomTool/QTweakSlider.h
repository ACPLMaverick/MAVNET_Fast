#pragma once
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QSlider>

#include "Common.h"

QT_BEGIN_NAMESPACE

class QTweakSlider :
	public QWidget
{
	Q_OBJECT

public:

	QTweakSlider(QWidget* parent = Q_NULLPTR);

	// Set value is clamped between 0-1.
	void SetValue(float value);
	// Retrieved value is in range of 0-1.
	float GetValue() const;

	void SetName(const wchar_t* name);
	QString GetName() const;

private:

	void OnSpinBoxValueChanged();
	void OnSliderValueChanged();

	QHBoxLayout* Hor_Layout;

	QLabel* Lbl_Name;
	QSpinBox* SpinBox_Value;
	QSlider* Slider_Value;
};

QT_END_NAMESPACE
