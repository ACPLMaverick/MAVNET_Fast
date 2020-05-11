#include "QTweakSlider.h"

QTweakSlider::QTweakSlider(QWidget* a_parent /*= Q_NULLPTR*/)
	: QWidget(a_parent)
	, Hor_Layout(new QHBoxLayout(this))
	, Lbl_Name(new QLabel("Tweak Name", this))
	, SpinBox_Value(new QSpinBox(this))
	, Slider_Value(new QSlider(Qt::Orientation::Horizontal, this))
	, m_maxValue(100)
{
	LRT_QAddName(Hor_Layout);
	LRT_QAddName(Lbl_Name);
	LRT_QAddName(SpinBox_Value);
	LRT_QAddName(Slider_Value);

	Hor_Layout->addWidget(Lbl_Name);
	Hor_Layout->addWidget(SpinBox_Value);
	Hor_Layout->addWidget(Slider_Value);

	Hor_Layout->setSpacing(0);
	Hor_Layout->setMargin(0);
	Hor_Layout->setStretch(0, 27);
	Hor_Layout->setStretch(1, 20);
	Hor_Layout->setStretch(2, 53);

	SpinBox_Value->setRange(0, m_maxValue);
	Slider_Value->setRange(0, m_maxValue);

	connect(SpinBox_Value, QOverload<int>::of(&QSpinBox::valueChanged), this, &QTweakSlider::OnSpinBoxValueChanged);
	connect(Slider_Value, &QSlider::valueChanged, this, &QTweakSlider::OnSliderValueChanged);
}

void QTweakSlider::SetValue(float a_value)
{
	a_value = fmaxf(fminf(a_value, 1.0f), 0.0f);
	SpinBox_Value->setValue(static_cast<int>(a_value * m_maxValue));
}

void QTweakSlider::SetValue(int32_t a_value)
{
	a_value = a_value < 0 ? 0 : a_value;
	a_value = a_value > m_maxValue ? m_maxValue : a_value;
	SpinBox_Value->setValue(a_value);
}

int32_t QTweakSlider::GetValueInt() const
{
	return SpinBox_Value->value();
}

float QTweakSlider::GetValueFloat() const
{
	return static_cast<float>(SpinBox_Value->value()) / m_maxValue;
}

void QTweakSlider::SetName(const wchar_t * a_name)
{
	Lbl_Name->setText(QString::fromWCharArray(a_name));
}

void QTweakSlider::SetMaxValue(int a_maxValue)
{
	if (a_maxValue == m_maxValue)
	{
		return;
	}

	float currentValue = GetValueFloat();

	m_maxValue = a_maxValue;
	Slider_Value->setMaximum(m_maxValue);
	SpinBox_Value->setMaximum(m_maxValue);

	SetValue(currentValue);
}

QString QTweakSlider::GetName() const
{
	return Lbl_Name->text();
}

void QTweakSlider::OnSpinBoxValueChanged()
{
	Slider_Value->setValue(SpinBox_Value->value());
}

void QTweakSlider::OnSliderValueChanged()
{
	const int val = Slider_Value->value();
	SpinBox_Value->setValue(val);

	// I emit valueChanged signal only on this slot, cause both get always called anyway.
	const float fval = static_cast<float>(val) / m_maxValue;
	emit valueChanged_float(fval);
	emit valueChanged_int32_t(val);
}
