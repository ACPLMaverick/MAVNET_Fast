#include "InputBinding.h"



InputBinding::InputBinding()
	: Serializable(k_propertyName)
	, m_generator()
{
}

InputBinding::InputBinding(GamepadButtons source, InputActionKey destination)
	: Serializable(k_propertyName)
	, m_generator()
{
	m_sources.Get().push_back(source);
	m_destinations.Get().push_back(destination);

	m_generator.ResolveMode(*this);
}

InputBinding::InputBinding(const Sources & sources, const Destinations & destinations)
	: Serializable(k_propertyName)
	, m_generator()
{
	m_sources.Get() = sources;
	m_destinations.Get() = destinations;

	m_generator.ResolveMode(*this);
}

InputBinding::InputBinding(const InputBinding& other)
	: m_sources(other.m_sources)
	, m_destinations(other.m_destinations)
	, m_generator(other.m_generator)
{
}

InputBinding::InputBinding(InputBinding&& other)
	: m_sources(std::move(other.m_sources))
	, m_destinations(std::move(other.m_destinations))
	, m_generator(other.m_generator)
{
}

InputBinding::~InputBinding()
{
}

InputBinding& InputBinding::operator=(const InputBinding& other)
{
	m_sources = other.m_sources;
	m_destinations = other.m_destinations;
	m_generator = other.m_generator;

	return *this;
}

InputBinding& InputBinding::operator=(InputBinding&& other)
{
	m_sources = std::move(other.m_sources);
	m_destinations = std::move(other.m_destinations);
	m_generator = other.m_generator;

	return *this;
}

void InputBinding::SetSource(GamepadButtons a_source)
{
	m_sources.Get().clear();
	m_sources.Get().push_back(a_source);

	m_generator.ResolveMode(*this);
}

void InputBinding::SetDestination(InputActionKey a_destination)
{
	m_destinations.Get().clear();
	m_destinations.Get().push_back(a_destination);

	m_generator.ResolveMode(*this);
}

void InputBinding::SetSources(const Sources & a_sources)
{
	m_sources = a_sources;
	std::sort(m_sources.Get().begin(), m_sources.Get().end());
	m_generator.ResolveMode(*this);
}

void InputBinding::SetDestinations(const Destinations & a_destinations)
{
	m_destinations = a_destinations;
	std::sort(m_destinations.Get().begin(), m_destinations.Get().end());
	m_generator.ResolveMode(*this);
}

void InputBinding::SetData(const Sources& a_sources, const Destinations& a_destinations)
{
	m_sources = a_sources;
	m_destinations = a_destinations;

	std::sort(m_sources.Get().begin(), m_sources.Get().end());
	std::sort(m_destinations.Get().begin(), m_destinations.Get().end());

	m_generator.ResolveMode(*this);
}

void InputBinding::GenerateActions(const GamepadState& a_gamepadState, const GamepadConfig& a_gamepadConfig, std::vector<InputAction>& a_outActions)
{
	m_generator.GenerateActions(*this, a_gamepadState, a_gamepadConfig, a_outActions);
}

const FilePath InputBinding::GetFilePath_Internal()
{
	return FilePath(L"", L"inputBinding");
}

const char* InputBinding::k_propertyName = "InputBinding";
const int32_t InputBinding::k_mouseMovementMultiplier = 20;
const int32_t k_mouseScrollMultiplier = 5;