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

public:
	// Set value is clamped between 0-1.
	void SetValue(float value);
	void SetValue(int32_t value);
	// Retrieved value is in range of 0-1.
	float GetValueFloat() const;
	int32_t GetValueInt() const;

public:
	void SetName(const wchar_t* name);
	void SetMaxValue(int maxValue);
	QString GetName() const;

signals:
	void valueChanged_float(float newValue);
	void valueChanged_int32_t(int32_t newValue);

private:

	void OnSpinBoxValueChanged();
	void OnSliderValueChanged();

	QHBoxLayout* Hor_Layout;

	QLabel* Lbl_Name;
	QSpinBox* SpinBox_Value;
	QSlider* Slider_Value;

	int m_maxValue;
};

QT_END_NAMESPACE
