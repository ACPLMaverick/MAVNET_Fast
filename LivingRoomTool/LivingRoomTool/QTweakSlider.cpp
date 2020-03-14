#include "QTweakSlider.h"

QTweakSlider::QTweakSlider(QWidget* a_parent /*= Q_NULLPTR*/)
	: QWidget(a_parent)
	, Hor_Layout(new QHBoxLayout(this))
	, Lbl_Name(new QLabel("Tweak Name", this))
	, SpinBox_Value(new QSpinBox(this))
	, Slider_Value(new QSlider(Qt::Orientation::Horizontal, this))
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

	SpinBox_Value->setRange(0, 100);
	Slider_Value->setRange(0, 100);

	connect(SpinBox_Value, QOverload<int>::of(&QSpinBox::valueChanged), this, &QTweakSlider::OnSpinBoxValueChanged);
	connect(Slider_Value, &QSlider::valueChanged, this, &QTweakSlider::OnSliderValueChanged);
}

void QTweakSlider::SetValue(float a_value)
{
	a_value = fmaxf(fminf(a_value, 1.0f), 0.0f);
	SpinBox_Value->setValue(static_cast<int>(a_value * 100.0f));
}

float QTweakSlider::GetValue() const
{
	return static_cast<float>(SpinBox_Value->value()) / 100.0f;
}

void QTweakSlider::SetName(const wchar_t * a_name)
{
	Lbl_Name->setText(QString::fromWCharArray(a_name));
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
	const float fval = static_cast<float>(val) / 100.0f;
	emit valueChanged(fval);
}
