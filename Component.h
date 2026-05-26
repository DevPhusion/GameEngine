#pragma once
class Component
{
public:
	virtual ~Component() = default;

	bool Enabled = true;
	virtual void SetEnabled(bool enabled);
};

